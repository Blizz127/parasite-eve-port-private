# Porting matched decompiled C into `pc_port/`

This document is the contract for **deriving** the native `pc_port/` implementation
from the verified matching decompilation in `src/`. It is the "how a new matched
leaf gets ported" recipe referenced by the generated TUs' provenance banners and
by `tools/analysis/gen_decomp_ports.py`.

Read `CLAUDE.md` first. The hard rules there still hold: never invent decompiled C,
every claim needs evidence, `src/` is the era-exactness authority and is **never
written to** by this mechanism.

---

## 1. The mechanism: generated derived TUs

Three candidate mechanisms were considered:

| Option | Verdict | Why |
| --- | --- | --- |
| **Shared source** — compile `src/*.c` natively with a forced-include shim | rejected | `src/` carries era-only artifacts (`register X asm("$6")` pins, `asm volatile` fences, Psy-Q prototypes). A macro layer cannot redefine `asm` pins or `register` keywords portably, and the shim would have to be kept in exact lockstep with 700+ leaves; any drift silently changes host codegen with no link-time signal. |
| **Derived port TUs** — generate `pc_port/game/decomp/func_XXXX_port.c` from the leaf | **chosen** | `src/` stays untouched and authoritative; the host adaptation is explicit, reviewable, and regenerable; it matches the existing `*_port.c` convention already used throughout `pc_port/`; and provenance is stamped into every file. |
| **Hybrid** — mechanical for pure-logic, hand for hardware | chosen *inside* the derived mechanism | The generator is conservative: pure-logic leaves are derived verbatim; hardware/pointer/asm-touching leaves are *reported*, not guessed, and need a hand-written adapter. |

`tools/analysis/gen_decomp_ports.py` is the mechanism. It:

1. reads the matched `c` span list from `configs/USA/disc1.yaml` (read-only — never edited);
2. reads each `src/func_XXXXXXXX.c`;
3. applies an explicit **eligibility contract** (below);
4. copies the leaf's body **verbatim** into `pc_port/game/decomp/func_XXXXXXXX_port.c`,
   wrapping it in only a provenance banner, a `#include "pe_guest_decomp.h"`,
   generated data-symbol macros, and generated loud-boundary shims;
5. never modifies `src/`, the manifest, or any sibling-owned file.

Run it:

```bash
python3 tools/analysis/gen_decomp_ports.py            # (re)generate
python3 tools/analysis/gen_decomp_ports.py --check    # totals + skip histogram
python3 tools/analysis/gen_decomp_ports.py --check --verbose  # every skip + reason
python3 tools/analysis/gen_decomp_ports.py --verify   # exit 1 if a TU drifted
python3 tools/analysis/gen_decomp_ports.py --only func_80123456   # one leaf
python3 tools/analysis/gen_decomp_ports.py --no-compile-check    # skip the E17 gate
```

The whole build — generate, verify, configure, build, run the native suite — is
one script:

```bash
scripts/build_pc_port.sh              # generate + configure + build + test
scripts/build_pc_port.sh --clean      # remove the build dir first
scripts/build_pc_port.sh --no-tests   # stop after the build
CMAKE=/path/to/cmake scripts/build_pc_port.sh   # cmake not on PATH
```

Use it rather than a bare `cmake -S pc_port -B pc_port/build`: the generated
directory is git-ignored, so on a clean checkout the bare cmake configures
`0 decomp-derived port TUs` and then **fails at link** with undefined references
to the leaves those TUs provide.

It is **idempotent**: re-running writes only files whose content changed, so
`--verify` is a clean drift gate after the matching decomp advances. Note that
one run may do several internal rounds: whether a callee gets its own prototype
or a loud boundary depends on whether that callee's TU exists in
`pc_port/game/decomp/`, so adding or pruning a TU changes the correct content of
its *callers*. The generator therefore re-runs its own pipeline until the emit
set stops changing (one round in steady state, ~15 s; four rounds and ~70 s from
an empty directory), and only then is `--verify` clean. `scripts/build_pc_port.sh`
still retries generate+verify up to three times, because a `src/` that sibling
matching lanes are extending *while the build runs* looks the same from outside. The build
globs `pc_port/game/decomp/*_port.c` (`CMakeLists.txt`, `file(GLOB CONFIGURE_DEPENDS)`),
so a newly generated TU joins the library with no build-file edit.

---

## 2. Host adaptation: what the shim does

`pc_port/include/pe_guest_decomp.h` is the only header a generated TU needs. Every
platform difference the leaf relies on is absorbed by one of these macros:

### Data symbols (`D_800XXXXX`)

A matched leaf addresses fixed absolute data. The generated TU declares one macro
per declared symbol, chosen by the declaration's shape:

| Leaf declaration | Generated macro | Host semantics |
| --- | --- | --- |
| `extern int D_8009CF0C;` | `PE_DECOMP_SCALAR(0x8009CF0Cu, int)` | a `volatile` lvalue in the one 2 MiB guest-RAM authority (`PE_Translate`), so repeated reads are not CSE'd away exactly as retail reloads them |
| `extern short D_8009589C[];` | `PE_DECOMP_ARRAY(0x8009589Cu, short)` | a base pointer into guest RAM; the leaf indexes it |
| `extern unsigned char *D_8009D048;` | `PE_DECOMP_PTRGLOBAL(0x8009D048u, unsigned char)` | loads the 32-bit guest address stored in the slot and translates it — retail's `lui`/`lw` base |

The macro name is the **data symbol**, so the leaf's body text is byte-for-byte
unchanged. `PE_DECOMP_SCALAR` includes `sizeof(type)` in the bounds check;
`PE_DECOMP_ARRAY` probes one byte because an `extern T name[];` has no known
extent (tests pin the retail extents). A pointer global holding an
out-of-range value aborts loudly in `PE_Translate` — the intended
"unported subsystem" signal, never a silent zero; a stored 0 becomes a host
NULL so the leaf's own `if (p)` test runs (see "Guest NULL stays NULL").

If the pc_port shim headers already declare a macro at that address with a
different host type (`#define D_8009D048 PE_GUEST_U32(...)`), the generator emits
`#undef D_8009D048` first so the **matched leaf's own declaration wins**.

### Callees pc_port has not implemented: loud boundaries

A derived leaf may call a retail function pc_port has not ported yet. That callee
cannot be a real `extern` (there is no host prototype) and must not be quietly
zeroed. The generator emits, per unresolved callee, a macro that redirects the
call site into the argument-forwarding boundary shim:

```c
/* boundary: func_8004E704 has no pc_port implementation yet */
#define func_8004E704(...) \
    PE_D_COMP_BOUNDARY1("func_8004E704", 0x8004E704u, __VA_ARGS__)
```

`PE_Decomp_Boundary()` records the symbol in the boundary registry **and** pushes
the guest argument vector (arity 0–4) into the established
`Bootstrap_ReturnInt4Indirect` log, so a test can assert both *which* edge was
unresolved and *what arguments the matched leaf actually passed*. Existing
hand-written ports use that same boundary log; derived ports join it rather than
inventing a parallel one. The generated arity comes from the balanced-paren
argument text of the call site, so a short call yields zero, never a syntax error.

Crucially, a boundary callee keeps the leaf's own declaration **verbatim**
(removing it would let the generated boundary macro rewrite the prototype into
nonsense). This is asserted by the generated-TU build plus
`test_DECOMP_boundary_argument_vector`.

### Declarations the generator rewrites

`neutralize_declarations()` replaces, with a `/* shimmed by pe_guest_decomp.h: X */`
comment:

- `extern` declarations containing a `D_` data symbol (the generated macro owns it);
- prototypes of callees that are loud boundaries (the boundary macro owns them);
- prototypes of callees with a **canonical `pe_port_compat.h`/`pe_sdk.h` prototype**,
  where the leaf's redeclaration could clash on signedness or pointer-ness with the
  canonical host type. The matched leaf stays authoritative for the **call site**;
  the host prototype comes from the canonical header.

Everything else — `typedef`s, `struct`s, prototypes of other decomp-derived
leaves — stays byte-for-byte.

### Constructs deliberately *not* auto-ported

There is no host model for these, so the generator reports the leaf instead of
guessing (the eligibility contract, §3):

- memory-mapped hardware and BIOS space (`0x1F8xxxxx`, `0xA0xxxxxx`, `0xB0xxxxxx`);
- GTE/COP2;
- BIOS trampolines (`jr $t2`) and `syscall`;
- `register X asm("$N")` pins and `asm volatile` fences;
- guest pointers held in host pointers where the pointer is **not** a direct
  address-valued parameter (pointer returns, callee results dereferenced,
  pointer casts of non-`D_` operands, function-address values, pointer-to-pointer
  parameters, and a pointer parameter *stored into guest RAM* — see E7b).

Everything else with a plain address-valued pointer parameter is adapted by the
**pointer-parameter host adapter** (E7b below), so those leaves are now derived
rather than reported.

The listed categories still need a **hand-written adapter** in `pc_port/` (as they
always have), and the coverage tool keeps counting them as `absent` /
`hand-translated` until that adapter lands.

---

## 3. Eligibility contract (what the generator will derive)

A leaf is derived only if **all** of these hold. Everything else is skipped with
a printed reason (`--check --verbose`) — never silently mis-ported.

| # | Rule |
| --- | --- |
| E1 | It is a matched `c` span in `configs/USA/disc1.yaml`. |
| E2 | No `asm`/`__asm__` with a real instruction. |
| E2b | Era codegen hints are removed, not rejected: a register pin `register T x asm("$N")` and an **empty-template** asm fence (`asm volatile("" : "=r"(x) : "0"(x))`, `asm("" ::: "memory")`) compute nothing. Exception: a pin *without* an initialiser that is read before it is written captures a live retail register — rejected (`pinned register X read before written`). Any non-empty template (even `"nop"`) still falls under E2. |
| E3 | No hardware/BIOS-space 32-bit literal (`0x1F8xxxxx`/`0xA0xxxxxx`/`0xB0xxxxxx`). Ordinary masks stay verbatim. |
| E4 | The file defines exactly one `func_*` with a body, and that name is the leaf. (A bare prototype is not a definition — forward declarations of callees must not disqualify a leaf.) |
| E5 | No indirect call through a local pointer. |
| E6 | No `&func_*` and no bare `func_*` used as a **value** — 32-bit code pointers are not host pointers. Two shapes are rewritten to the retail VMA instead of rejected (`rewrite_code_addresses`): an integer cast `(unsigned int)func_X` / `(int)&func_X` becomes `(unsigned int)0xXu`, and a bare `func_X` passed as a whole call argument whose host parameter takes a guest address (a boundary, or a canonical/generated non-pointer parameter) becomes `(pe_addr_t)0xXu` (the hand-adapter convention). A `void *` store, a pointer-global assignment or a real host function-pointer parameter is still reported. |
| E7 | No **function**-pointer return type and no recursive pointer-returning leaf. An **object**-pointer return is adapted: the verbatim leaf is emitted as `static T *pe_host_func_X(...)` and the exported `pe_addr_t func_X(...)` returns `PE_HostToGuest()` of its result (a host pointer into guest RAM → its KSEG0 address, scratchpad → `0x1F800000+off`, NULL → 0, anything else aborts loudly — `pc_port/platform/pe_guest_ram.c`). |
| E7b | A pointer-typed parameter is adapted by the guest-address host adapter **only** if it is address-valued: no pointer-to-pointer parameter, no `&param`, no *storing* the parameter into guest RAM (`dst->field = p` would write a 64-bit host pointer into a 32-bit guest slot), no `(T *)p` store-through-dereference cast, no recursive self-call, and no non-primitive pointee type. Every call site then forwards the guest address (`pe_p`), while memory access goes through the translated `host_p` pointer. |
| E7c | A **guest-pointer vector** parameter `T **args` (T primitive) — a field-VM opcode handler's operand vector — is adapted when every use is a *dereferenced* element: `*args[k]`, `**args`, `**(args + k)`, `*(T2 *)args[k]`. Each element becomes `PE_DECOMP_PTRGLOBAL(pe_args + 4u * (k), T)` (the retail `lw` of the 32-bit element, then the access through it). A bare element passed on (`f(args[2])`), the vector used as a value, or any other `**` in the leaf is reported. |
| E8 | No callee result indexed/dereferenced (would be a 32-bit guest pointer). |
| E9 | Every `(T *)` cast operand must evaluate to a **host pointer into guest RAM** (`is_pointer_expr`): `&lvalue`, an array / pointer-global `D_`, a pointer parameter, a local or parameter whose own declaration is pointer- or array-typed, or pointer arithmetic on one of those. An indexed / dereferenced / member / call *value* (`(unsigned int *)base[0x28]`, `(T *)*arg++`, `func_X() + 0x24`), a scalar `D_` (`(unsigned char *)(D_8009D2F0 + 0xF)`) or an int local holding an address is a 32-bit guest address, not a host pointer, and is rejected. An E7c `(T *)args[k]` element stays accepted. (Until 2026-09-23 the rule accepted any operand that *mentioned* a name assigned from a `D_` symbol, which let int locals and scalars through: func_80019FE0, func_80018D50/DD4 and func_8004006C were emitted and dereferenced guest words as host pointers.) |
| E9b | No `(T **)` cast. `*(unsigned int **)(rec + 0x68)` reads an 8-byte host pointer out of 4-byte guest RAM; the old cast regex never matched a double star, so func_80030640 was emitted this way. Such leaves need a hand adapter (`decomp_hand/e9_guest_ptr_port.c`). |
| E10 | No pointer-pointer store into a `D_` symbol. |
| E11 | Every called `func_*` either has a known pc_port definition (linkable) or becomes a loud boundary; an existing definition with a **canonical prototype must accept the same arity** as the matched call site (otherwise the host would reinterpret arguments → reported). |
| E12 | No assignment to a pointer-global slot (the translate-and-dereference macro cannot represent storing a 32-bit guest address). |
| E13 | No `func_*` call result bound to a pointer object. A retail callee returning `int *` returns a 32-bit **guest address**; host-side it is either a canonical `pe_addr_t` prototype or a loud boundary returning `int`, so `int *p = func_X();` is not representable. Rejected unless the callee's resolved host return type is itself a pointer. |
| E14 | No host-pointer expression passed to a callee parameter that is not a host pointer. A local aggregate (`char buf[8]`), a pointer local, or `&local` is *host* stack memory; a canonical prototype taking `pe_addr_t` wants a guest address, and a generated leaf has no guest frame. (A bare `D_` symbol argument is not this case — see the rewrite below; a pointer *parameter* is not either — E7b forwards `pe_<name>`.) |
| E15 | No guest array/global **of pointers** (`extern T *D_XXXX[];`, `extern T **D_XXXX;`). The slot holds a 32-bit guest address; typing the element as a host pointer reads 8 bytes on a 64-bit host. One level (`extern T *D_XXXX;`) is fine — `PE_DECOMP_PTRGLOBAL` loads the 32-bit slot and translates it. |
| E16 | No host pointer narrowed to an integer (`(unsigned int)D_XXXX`, `(int)ptr_param`, and — since 2026-09-23 — a parenthesised pointer expression `(unsigned int)(tbl + k)`, which the identifier-only regex missed). Retail's value is a 32-bit guest address; the host accessor is a 64-bit pointer, so the cast truncates it. |
| E17 | **Compile gate.** Every rendered TU is syntax-checked with the host C compiler (pc_port's own `-I` set and `-D` defines) before it is written. A TU that does not compile is reported and not emitted. `--no-compile-check` disables it; a host with no C compiler gets a loud warning, never a silent skip. |
| E18 | No leaf-defined `struct` with a pointer member. It describes guest RAM whose pointer fields are 32-bit guest addresses; the host compiler would lay it out with 8-byte pointers, misplacing that field and every field after it. This compiles cleanly and is silently wrong, which is how `func_80017A24`, `func_80017C8C` and `func_800C4E50` were emitted before this rule. Such leaves need a hand adapter that reads the pointer fields with `PE_LoadU32`. |
| E19 | A relational compare between two pointers (`dst >= src`) compares *host* addresses. Two operands in the one guest-RAM allocation (RAM array symbols, or locals assigned only from them) order exactly like their KSEG0 guest addresses and keep the compare; a compare whose operands are exactly known guest values (a pointer global's stored word, an unmodified pointer parameter's `pe_` value, an array's address) is rewritten to compare those guest values; anything else (a modified parameter, a pointer loaded from RAM) is reported — a scratchpad pointer or a KUSEG/KSEG1 alias would compare in a different order than on the PS1 (portverify finding 9, func_80072334). |

E13–E16 are four angles on one fact: **a 32-bit guest address and a 64-bit host
pointer are not the same object.** E15 and E16 matter most, because those two
shapes *compile* (with only a `-Wpointer-to-int-cast`-class warning) and are
silently wrong at run time. E17 exists because no static rule set can anticipate
every shape a newly matched leaf takes — it keeps `pc_port/game/decomp/`
buildable by construction.

**Orphan pruning.** The generator owns every `func_XXXXXXXX_port.c` in
`pc_port/game/decomp/`, and CMake globs that directory, so a TU left behind
after its leaf became ineligible (or started colliding with a hand port) would
still be compiled. Each run removes any `*_port.c` it did not just emit;
`--verify` reports such a file as `orphan` instead of deleting it. `--only` and
`--limit` suppress pruning, since they deliberately restrict the emit set.

### Guest 0: NULL only where the leaf null-tests (2026-09-24)

`PE_Translate` folds KUSEG `0x00000000..0x001FFFFF` (and KSEG1) onto KSEG0
(`PE_RamCanonical`), exactly like the PS1, so a guest 0 is valid low RAM and
retail code that dereferences it reads low RAM. The generator therefore maps a
guest 0 to a host NULL **only** for a pointer parameter or pointer global that
the leaf itself null-tests (`if (p)`, `p != 0`, `0 == p`, `!p`, `p &&`, `p ||`,
`p ?`, also through a plain copy `q = p;` — `null_tested()`): those get
`PE_DecompTranslateOrNull` / `PE_DECOMP_PTRGLOBAL_NULLABLE`, so the leaf's own
test sees null (func_80080998's guard). Every other pointer uses plain
`PE_Translate` / `PE_DECOMP_PTRGLOBAL`, i.e. the mirror. (A blanket NULL
mapping made 71 generated TUs segfault where retail reads low RAM —
portverify.) Unit tests: `NullTestedRuleTests`.

### Hardware registers: one MMIO table (2026-09-24)

`pc_port/platform/pe_mmio.{h,c}` is the single table of modelled I/O
registers (I_STAT/I_MASK, DPCR/DICR, RCNT1 count/mode 0x107, RCNT2
count/mode/target — list in `pe_mmio.h`). `PE_Load*`/`PE_Store*` route any
0x1F801xxx (and KSEG0/KSEG1 alias) access through it: a modelled register
reaches its device model in program order, anything else aborts loudly. A
generated TU that reaches hardware through a pointer global
(`PE_DECOMP_PTRGLOBAL`, e.g. the generated SetRCnt through
`D_8009B7D0 = 0x1F801100`) gets a host pointer into a device-synced shadow of
the modelled block; its writes are committed at the next translate, value
access or device tick (documented limits in `pe_mmio.h`: a same-value
rewrite is not seen, batched writes apply in table order, pointer reads
don't clear RCNT reached flags, an unmodelled slot reads the poison 0xDE and
aborts on write). BIOS A-function string/memory routines are
`pe_bios_string.c` (psx-spx semantics quoted per function).

### Pointer returns keep the root's segment (2026-09-24)

The pointer-return adapter converts the leaf's host result with
`PE_HostToGuest` (KSEG0). When the leaf has exactly one pointer root (one
pointer global or E7b pointer parameter), its guest value is read before the
call and `PE_DecompReturnSegment` re-applies a KUSEG/KSEG1 segment: with
`D_800E2248 = 0x65`, func_800C2B10 returns `0x6D` as retail does, not
`0x8000006D`. With several roots the result stays KSEG0 (documented alias
loss).

### Bare data symbols inside parentheses (fixed 2026-09-23)

`fix_argument_addresses` used to turn *every* bare `D_X` in a `(`/`,` context
into the address literal, whatever its declaration. That is right for an array
(it decays to its address) but wrong for the other two kinds, and it compiled
without a warning:

- a **scalar** is its *value*: `func_80073E10(D_80095884)` (func_80077404)
  passed `0x80095884` instead of the saved mask, and `if (D_8009D0E8)`
  (func_8005E518) became an always-true constant. Scalars are now left alone.
- a **pointer global** passes the guest pointer *stored in* the slot, as
  retail's `lui`/`lw` does: `func_800726F4(D_800BCDA8)` (func_800403C8) now
  emits `(pe_addr_t)PE_LoadU32(0x800BCDA8u)`, not the slot address.

52 generated TUs changed; unit tests `CallAddressArgTests.test_bare_*`.
The shared call-argument walker (`rewrite_call_args`) also now shifts an
enclosing call's end offset when an inner argument is rewritten (nested calls
were previously split on stale offsets; `CodeAddressTests.test_nested_call_offsets_stay_correct`).

### One deterministic rewrite: guest addresses as arguments

When a bare `D_XXXXXXXX` appears as a call argument, the generated macro would
expand to a **host** pointer, but the host callee expects `pe_addr_t`. The
generator rewrites only that position:

```c
func_800C2414(arg0, D_800E0824);   /*  = func_800C2414(arg0, (pe_addr_t)0x800E0824u);  */
```

The argument *semantics* (the retail guest address) are preserved exactly; the
rewrite is positional and mechanical, and it is the only textual change the
generator makes inside a leaf body.

### Host signatures of generated callees

A generated TU is itself the host authority for its function's signature.
`derived_signatures()` reads every generated definition back and treats it like a
canonical header prototype: E11 arity, the E13/E14 host-pointer checks, and the
caller's own declaration. The caller's retail prototype stays verbatim when its
types already agree. When they disagree (an E7b callee takes `pe_addr_t` where the
retail prototype says `unsigned char *`, or `void` vs `int`), it is replaced by a
`/* host prototypes of decomp-derived callees */` block. Before this, such a caller
compiled against its own prototype and linked against a different definition.
That hid six defects: zero-argument calls to one- and two-argument callees
(retail passes a live `$a0`/`$a1`) and a host stack pointer handed to a `pe_addr_t`
callee. Those callers are now reported (`call arity …`, `host pointer … passed`).

### Guest addresses in call arguments (extended)

`fix_argument_addresses` also rewrites a *whole* call argument of the form `&D_X`,
`&D_X[k]`, or array / pointer-global `D_X + k` to its retail guest address
(`(pe_addr_t)(0xXu + (uint32_t)(k) * sizeof(T))`, or
`PE_LoadU32(0xXu) + …` for a pointer global). It applies only when the callee's
host parameter at that position takes a guest address: a loud boundary, or a
canonical / generated signature with a non-pointer parameter. Sub-expressions such
as `*(T *)(D_X + 4)` are never touched, and neither is a scalar `D_X + k`, which is
a value. Previously a boundary recorded the 64-bit *host* pointer of `&D_X` as the
guest argument (`func_800809E0`, `func_80073BBC`, `func_80082DBC`). Unit tests:
`tools/analysis/test_gen_decomp_ports.py`.

### One generated host adapter: address-valued pointer parameters (E7b)

Roughly a third of the remaining ineligible leaves take a plain address-valued
pointer parameter — the shape that dominated the first eligibility pass. Those
are now derived with a **host adapter** instead of being reported, because the
guest address *is* the whole interface and the body can stay verbatim.

The retail leaf:

```c
typedef struct { short *first; short *second; unsigned short *third; } Arguments;
int func_80017C8C(Arguments *arg0) {
    func_800661EC(*arg0->first, *arg0->second, *arg0->third, 8);
    return 1;
}
```

becomes (provenance banner elided):

```c
int func_80017C8C(pe_addr_t pe_arg0)
{
    Arguments *host_arg0 = (Arguments *)PE_Translate(pe_arg0, 1);
    /* verbatim body (src/func_80017C8C.c) */
    func_800661EC(*host_arg0->first, *host_arg0->second, *host_arg0->third, 8);
    return 1;
}
```

The rules that keep it honest:

- **Only the convention adapts.** The signature takes `pe_addr_t`, a `host_<p>`
  local is `PE_Translate`d from it, and the body text is otherwise the matched C.
  Control flow, arithmetic, bit constants and signedness are untouched.
- **Memory access** goes through `host_<p>`; a call site hands the callee the
  *guest address* (`pe_<p>`), because a canonical pc_port prototype or a boundary
  records a guest argument, not a host pointer.
- **`D_` symbols and struct access** are textually rebound to their `host_`
  accessors, with a `(?<![.>])` guard so a struct *member* of the same name is
  never mistaken for the parameter.
- **The adapter refuses shapes where the guest/host distinction leaks.** A
  pointer-to-pointer parameter, `&param`, storing the parameter into guest RAM,
  a store through a `(T *)param` cast, a recursive self-call, or a
  non-primitive pointee type all fall back to "report, do not guess".
- Verified by `pc_port/tests/test_decomp_ptr_params.h`, which pins both the
  memory effect of the dereferenced body and the *guest address* forwarded to a
  boundary (`test_DECOMPPTR_boundary_guest_addresses`).

---

## 3b. Overlay leaves (2026-09-24)

`src/overlays/<ovl>/func_X.c` holds ~2500 matched overlay leaves (5 subsystem
overlays + room chunk 2s). Rooms, `ovl_03D2` and `ovl_0700` share the
0x8018EFxx arena, so one `func_801xxxxx` name can be *different code* in
different overlays (346 such names) — an overlay leaf can never be a plain
`func_X` symbol.

### Step 1 — namespaced generated TUs (built)

`tools/analysis/gen_overlay_ports.py` (run by `scripts/build_pc_port.sh`
next to `gen_decomp_ports.py`, with its own `--verify`) writes
`pc_port/game/decomp_ovl/<ovl>/func_X_port.c`:

- the body is the matched leaf verbatim; a block of rename macros after the
  shim include (`#define func_X <ovl>__func_X`, for the leaf and every
  same-overlay callee that is also emitted) makes the host symbol
  `<ovl>__func_X`, so two overlays' leaves of the same name link side by side;
- a same-overlay callee without a port is a loud boundary labelled
  `"<ovl>:func_Y"`; it is never bound to a plain-named pc_port definition
  (that may implement another overlay's code at the same address);
- an EXE callee binds exactly as for EXE leaves; a callee in another
  overlay's window is a loud boundary `"overlay:func_Y"`;
- eligibility E1–E18, the E17 compile gate and the host adaptation are
  `gen_decomp_ports.py`'s, unchanged. Banner: `decomp-source-overlay:
  <ovl>/func_X` (the EXE scans ignore `decomp_ovl/`).
- Order: subsystem overlays (`ovl_0700`, `ovl_0457`, `ovl_03D2`, `ovl_03C9`,
  `ovl_03C5`) first; rooms after, deduplicated by body (step 4).
- Tests: `tools/analysis/test_gen_overlay_ports.py`; native
  `PORTABSENT_overlay_namespaced` calls leaves by their namespaced symbols.

### Step 2 — coverage by (overlay, name) (built)

`python3 tools/analysis/pc_port_coverage.py --overlays` buckets every overlay
leaf keyed by (overlay, name): ported-from-decomp (a step-1 TU), hand-translated
/ stub (a `<ovl>__func_X` definition, or a plain `func_X` whose name exists in
exactly one overlay), **ambiguous** (a plain `func_X` for a name several
overlays share — never credited; the report lists them for annotation as
`<ovl>__func_X`), absent.

### Step 3 — residency + guest-code dispatch (built 2026-09-24)

Goal: a call through a guest code pointer (callback slot, handler word,
function table, `D_8009566C->f`, overlay behaviour tables) dispatches to the
same code retail would jump to, instead of the ad-hoc `fn == 0x8018…` chains
and the generator's blanket "indirect call through local" rejection.

1. **Residency registry** — `PE_Overlay_SetResident(arena, ovl)` /
   `PE_Overlay_Resident(arena)` (`pc_port/platform/pe_overlay.{h,c}`). The
   port's overlay loaders call it after placing an image or room chunk in
   guest RAM. Arenas come from `configs/USA/overlays/manifest.yaml` (fixed
   load addresses; 0x8018EFxx shared). **Verification against manifest
   bytes:** the generator emits, per overlay, a small signature table (the
   first N words of its text window, taken from the matched leaves' own
   extracted bytes at build time — never committed disc data; if the blob is
   not present locally the table is empty and verification is skipped with a
   visible warning). `PE_Overlay_Verify(arena)` compares guest RAM against it;
   a mismatch clears residency and makes dispatch loud rather than wrong.
2. **Per-overlay dispatch tables** (generated with the step-1 TUs):
   `{vma, thunk}` for every emitted leaf of that overlay, where a thunk has
   the uniform shape `int thunk(uintptr_t a0..a3)` and casts to the leaf's
   host parameters (all parameters are integers or `pe_addr_t` after E7b; a
   leaf with any other parameter type gets no thunk). An EXE table covers
   generated EXE leaves and hand ports with canonical integer signatures.
3. **`PE_GuestCode_Resolve(addr)`** — EXE window → EXE table; an overlay
   window → the resident overlay for that arena → its table; otherwise (or no
   resident overlay, or no thunk) → `NULL`. **`PE_GuestCall(addr, n,
   a0..a3)`** calls the thunk or, on `NULL`, records a loud boundary carrying
   the address (the existing boundary log), returning 0 — never a guess.
4. **Generator:** "indirect call through local" becomes derivable: a call
   through a guest code pointer (`f(args)` where `f` is loaded from guest RAM,
   `(*(T (**)())p)(args)`, `D_fnptr(args)`) is rewritten to
   `PE_GuestCall(<the 32-bit guest word>, …)`; the pointer's *value* is read
   as `PE_LoadU32` of its guest slot, never through a host function-pointer
   type. Leaves where the fn-pointer value escapes otherwise (stored,
   compared, returned) stay reported.
5. **Migration:** existing ad-hoc dispatchers (`PE_EffectCallback` chains,
   `PE_Callback_Bind`) can later route through `PE_GuestCode_Resolve`
   (portverify owns those sites). Tests: residency set/verify/mismatch,
   resolve per arena with two overlays sharing one arena, thunk argument
   forwarding, unresolved address → loud boundary with the address.

**As built.** Runtime `pc_port/platform/pe_guestcode.{h,c}`
(`PE_GuestCall`, `PE_GuestCode_Resolve`, `PE_Overlay_SetResident[Verified]`,
SHA-1 over guest RAM against the committed config hash). The registry
`pc_port/game/decomp_ovl/_dispatch/zz_dispatch.c` is generated by
`gen_overlay_ports.py` (one uniform thunk per function with an all-integer
host signature: generated EXE leaves, non-static canonical-header ports, and
every emitted overlay leaf; per-leaf FNV-1a fingerprints of the retail bytes
from the locally extracted blobs; compile-gated) and is linked **only** into
`parasite-eve-port` and `pe-native-tests`, which call
`PE_GuestCode_InstallGenerated()` at startup — in the static library it would
drag every thunk's object into every small test executable. Without a
registry every guest call is a loud boundary. Residency is cleared by
`PE_Sdk_ResetState`. Generator: `rewrite_guest_calls()` turns every
function-pointer declarator into `pe_addr_t`, function-pointer casts into
integer casts, and calls through them into
`PE_GuestCall("@<vma>:<callee>", addr, n, a0..a3)`; `&D_X`-style arguments
become guest addresses, host-pointer arguments and >4 arguments are
reported. The E5 "indirect call through local" rejection class is gone
(113 EXE leaves; most were already hand-ported and are now conflicts for
review). Also fixed on the way: `call_args()` no longer treats prototypes as
zero-argument calls, and `pc_port_definitions()` ignores `static`
definitions (not linkable). Tests: `GuestCallTests`, `CallArgsTests`,
native `PORTABSENT_guestcode`.

### Step 4 — room dedupe (built 2026-09-24)

`gen_overlay_ports.py` (default run, or `--rooms`) groups the room leaves by
a key = sha1(normalised body) refined, to a fixed point, by the keys of the
same-room callees it calls (`room_groups`): an identical body calling a
same-named but *different* room function is a different function, and a
same-room callee that is not a matched leaf is room-specific (conservative —
such leaves never merge). Each group is emitted once as
`pc_port/game/decomp_ovl/rooms/func_X__<key8>_port.c` with host symbol
`room_<key8>__func_X`, analysed in its canonical (first) member room; every
member room's rename macros and dispatch table point at that symbol, and the
banner carries one `decomp-source-overlay: <room>/func_X` line per member
(so `--overlays` coverage credits every member). 66 rooms, 2402 room leaves,
2006 distinct bodies; 862 group TUs emitted. Tests: `RoomDedupeTests`,
native `PORTABSENT_room_dedupe_dispatch` (a deduplicated room leaf dispatched
per resident room, itself calling through `o->fn`).

## 4. Per-leaf recipe (adding a port)

1. **Confirm the leaf is matched.** `python3 tools/build/disc1_plan.py --check`
   must list `[<offset>, c, func_XXXXXXXX]` in `configs/USA/disc1.yaml`, and
   `docs/evidence/func-XXXXXXXX/REPORT.md` must exist.
2. **Regenerate.** `python3 tools/analysis/gen_decomp_ports.py`. If the leaf is
   eligible you get `pc_port/game/decomp/func_XXXXXXXX_port.c`; if not,
   `--check --verbose` prints the reason and you write a hand adapter instead.
3. **Build.** `scripts/build_pc_port.sh` (generate + verify + configure + build
   + test). Since E17 the generator refuses to emit a TU that does not compile,
   so a leaf that silently disappears from `pc_port/game/decomp/` is reported as
   `uncompilable <leaf>: <first error>` — that is an eligibility gap: fix the
   *generator rule*, never the generated file (generated files are overwritten
   on the next run and must never be hand-edited).
4. **Test.** Add or extend a case in `pc_port/tests/test_decomp_ports.h` (plain
   leaves) or `pc_port/tests/test_decomp_ptr_params.h` (E7b adapter leaves),
   pinning the retail behavior — especially quirks: exact bit constants
   (`0x20000000` vs `0x80000000`), signedness/narrowing, write/no-write guards,
   search-and-break order, double dereference, mirrored indexing. Call
   `ResetTestState()` first, seed guest RAM with `PE_StoreU32`/`PE_StoreU8`/
   `PE_StoreU16`, and read back with `PE_LoadU32`/`PE_LoadU8`/`PE_LoadU16`.
5. **Verify + document.**
   `python3 tools/analysis/gen_decomp_ports.py --verify` must report `0 drifted`.
   Refresh the coverage numbers with
   `python3 tools/analysis/pc_port_coverage.py`.
6. **Never edit `src/`.** If the leaf looks wrong, that is a decomp issue owned by
   the matching lane; report it, do not patch `src/`.

### Disagreements with an existing hand port

Where a pre-existing hand-written `*_port.c` and the matched C disagree, **the
matched C wins**, and the discrepancy is recorded here and in the test that pins
the matched behavior. The generator never *replaces* an existing definition on
its own — a leaf whose `func_*` is already defined elsewhere is reported as a
**conflict** and skipped, so overriding a hand port is a deliberate, reviewed act.

**portverify (2026-09-24)** is that reviewed act for the existing conflicts.
`tools/analysis/portverify/run.sh` differentially executes each generated TU
against the hand definition. Record: `docs/evidence/portverify/REPORT.md`.
273 hand ports were retired: each hand definition was replaced by a one-line
`/* func_X: ported from the matching decomp … */` marker, and its canonical
header prototype was aligned to the matched signature. Nine hand-port bugs were
confirmed against retail and fixed; regressions are in
`pc_port/tests/test_portverify.h`. Keep a hand port when it carries host glue
(stop-epoch/`ShouldStop`, MMIO), emulates the KUSEG RAM mirror (`x < 0x200000 ?
x | 0x80000000 : x`), or returns a retail `$v0` residual that callers read, where
the matched C is `void`. Rules learned:

- **Plain `char` is unsigned** in the era compiler (`lbu`; `lb` needs
  `signed char`). `pc_port/CMakeLists.txt` compiles every `game/decomp` and
  `game/decomp_hand` TU with `-funsigned-char` (`/J` on MSVC). Without it,
  `src/func_80052BCC.c` missed its 0xFF terminator against the retail INV8 oracle.
- **A guest NULL stays a host NULL** in the E7b adapter and in wrapper
  pointer-global slots, so a matched leaf's own `if (p != 0)` test runs instead
  of `PE_Translate(0)` aborting first. A non-null address is still bounds-checked.

---

## 5. Coverage / gap tool

`tools/analysis/pc_port_coverage.py` maps every matched `c` span to its pc_port
counterpart and classifies it into exactly one bucket:

- **`ported-from-decomp`** — a `pc_port/game/decomp/*_port.c` with a
  `decomp-source:` provenance banner naming the matched leaf (structurally detected);
- **`hand-translated`** — a real pc_port definition written independently of `src/`;
- **`stub`** — every definition *body* of the leaf contains a bootstrap marker
  (`BOOTSTRAP_RET`, `Bootstrap_Return*`). Judged per body, not per file: a real
  port sharing a file with some other callee's boundary stub is
  `hand-translated`. Prototypes/`extern` declarations are not definitions;
  `static inline` bodies in `pc_port/**/*.h` are. (The earlier file-level rule
  mislabelled 246 real hand ports as stubs; regression test
  `tools/analysis/test_pc_port_coverage.py`.)
- **`absent`** — no pc_port definition at all.

```bash
python3 tools/analysis/pc_port_coverage.py            # totals + one summary line
python3 tools/analysis/pc_port_coverage.py --json     # machine-readable
python3 tools/analysis/pc_port_coverage.py --list-bucket ported-from-decomp
python3 tools/analysis/pc_port_coverage.py --deps func_80030640   # dependency surface
```

Output is stable and diffable (sorted sections, deterministic one-line summary).
It re-reads the YAML and `src/` on every run, so it stays correct as matching
advances. Exit status is 0 for a successful report; low coverage is not an error.

---

## 6. Honest limits

- **Not every matched leaf is auto-portable.** The eligibility contract is
  conservative on purpose. Pointer-heavy, asm-bearing, and hardware-touching
  leaves need hand adapters; the generator reports them rather than guessing.
- **A derived port proves behavior, not cycle-exactness.** It reproduces the
  matched **C semantics**; it is not a matching leaf and does not add to the
  matched-C count (`pc_port/` is explicitly not a matching leaf per `CLAUDE.md`).
- **A boundary is a loud stub, not a port.** Where a derived leaf calls an
  unported callee, the call is recorded and returns 0. The edge is enumerable and
  testable, but the subsystem is still not implemented — the coverage tool keeps
  counting the callee as `absent` until its own adapter lands.
- **`--verify` is the drift gate.** If the matching lane edits a `src/` leaf, the
  generated TU is stale until the generator re-runs; `--verify` exits 1 and lists
  the stale files.
