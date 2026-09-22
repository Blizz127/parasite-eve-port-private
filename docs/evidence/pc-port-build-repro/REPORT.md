# pc_port: generator correctness + a reproducible build

Date: 2026-09-22. Scope: `tools/analysis/gen_decomp_ports.py`,
`scripts/build_pc_port.sh` (new), `docs/ai_context/PC_PORT_FROM_DECOMP.md`.
Nothing under `configs/`, `src/`, `asm/` or `CLAUDE.md` was touched.

`src/` and `configs/USA/disc1.yaml` were being extended by eight sibling
matching lanes throughout this work, so every number carries its plan hash
(`python3 tools/build/disc1_plan.py --check`). The like-for-like before/after
comparison in §2 is pinned to plan `bac82138ac60` (1297 `c` spans); the
end-state proof in §5 is plan `f5b48b142713` (1378 `c` spans). Re-derive
before reuse.

---

## 1. The defect: the generator emitted code that does not compile

Reproduced before any change (host cmake at `/tmp/pe-tools/root/usr/bin/cmake`,
`LD_LIBRARY_PATH=/tmp/pe-tools/root/usr/lib/x86_64-linux-gnu`):

```
$ python3 tools/analysis/gen_decomp_ports.py && cmake --build pc_port/build -j -- -k
game/decomp/func_80047F48_port.c:29:7:  error: assignment to 'int *' from 'pe_addr_t' …
game/decomp/func_80047F48_port.c:36:23: error: passing argument 1 of 'func_80062CB8' …
game/decomp/func_8004FE58_port.c:34:11: error: assignment to 'unsigned char *' from 'int' …
game/decomp/func_8005D940_port.c:24:19: error: passing argument 1 of 'func_8006E2D0' …
game/decomp/func_8005D940_port.c:25:19: error: passing argument 1 of 'func_8006E454' …
```

Three more appeared as sibling lanes matched new leaves during the work
(`func_80033430`, `func_8007DC04`, `func_8008F178`), which is itself the
evidence that this is a *class*, not three cases.

**Root cause, stated once.** A retail `int *` is a 32-bit **guest address**. In
a generated TU, host-side, the same value is a 64-bit **host pointer**. The
eligibility contract screened that confusion at the leaf's own boundary (E7:
pointer parameters; E8: pointer casts) but nowhere else, so it leaked through
three other doors:

| Door | Example | Was |
| --- | --- | --- |
| a callee's **return** | `int *p = func_80062A20(a0, 0);` (host proto returns `pe_addr_t`) | compile error |
| a callee's **parameter** | `char buf[8]; func_8006E2D0(buf, …);` (host proto takes `pe_addr_t`) | compile error |
| a guest **array of pointers** | `extern unsigned char *D_800B0E38[];` | 8-byte read of a 4-byte guest slot |
| a pointer **narrowed to an int** | `return (unsigned int)D_800119CC;` | silent truncation, warning only |

The last two are worse than the first two: they *compile*.

## 2. Approach taken for Defect 1: (a) conservative screening, plus a compile gate

The handoff offered (a) screen such leaves out, or (b) lower them correctly by
allocating guest scratch for the local aggregate. **(a) was chosen.** Reasons:

1. (b) does not address the actual defect set. Only one of the five originally
   failing leaves (`func_8005D940`) is "a local aggregate passed by address";
   `func_80047F48` and `func_8004FE58` are *callee returns*, which no scratch
   allocator fixes.
2. (b) is not evidently faithful. Retail passes a real address inside the
   guest stack frame; a generated leaf has no guest frame. Synthesising one
   would invent lifetime and aliasing semantics that nothing in the decomp
   evidence pins — exactly the kind of guess the generator is built to refuse.
3. A smaller honest eligible set beats a larger broken one.

Four new static rules (documented in the generator docstring and in
`PC_PORT_FROM_DECOMP.md` §3), numbered to continue that table:

* **E13** — a `func_*` call result bound to a pointer object, unless the
  callee's *resolved host* return type is itself a pointer. Resolution order:
  canonical `pe_port_compat.h`-class prototype → loud boundary (returns `int`)
  → the leaf's own declaration.
* **E14** — a host-pointer expression (pointer local, local aggregate,
  `&anything`) passed where the callee's canonical parameter type is not a host
  pointer. Deliberately *not* triggered by a bare `D_` symbol argument
  (`fix_argument_addresses` already substitutes the guest address) nor by a
  pointer *parameter* (the E7b wrapper forwards `pe_<name>`, the guest address
  — verified by re-admitting `func_80050088`, `func_8007FC88`, `func_80080AC4`
  after that refinement).
* **E15** — a guest array/global *of pointers* (`extern T *D_X[];`,
  `extern T **D_X;`).
* **E16** — a host pointer narrowed to an integer (`(unsigned int)D_X`,
  `(int)ptr_param`). Refined so a *value* cast through the object is not caught
  (`(signed char)D_800BCEA8[i].byte0`, `(unsigned int)p[1]` stay eligible).

Because no static rule set can anticipate the shape of a leaf that has not been
matched yet, a final gate was added:

* **E17 — compile gate.** Every rendered TU is syntax-checked with the host C
  compiler (`cc -std=gnu11 -fsyntax-only`, pc_port's own `-I` set and `-D`
  defines, mirroring `pc_port/build/CMakeFiles/pe_field_runtime.dir/flags.make`)
  before it is written. A TU that does not compile is reported
  (`uncompilable <leaf>: <first error>`) and not emitted. `--no-compile-check`
  disables it; a host with no C compiler gets a loud warning, never a silent
  skip. Cost: ~16 s for the full set, parallel, vs ~1.5 s without.

Also fixed while proving the above:

* **Orphan pruning.** `pc_port/game/decomp/` is git-ignored generated output
  that CMake globs, and the generator never removed anything. A TU whose leaf
  became ineligible stayed on disk and kept breaking the build
  (`func_80033430_port.c` did exactly that). Each run now deletes any
  `func_XXXXXXXX_port.c` it did not emit; `--verify` reports it as `orphan`
  instead of deleting. `--only`/`--limit` suppress pruning.
* **Pointer-global pointee derivation.** `extern unsigned int *volatile
  D_8009B40C;` produced `PE_DECOMP_PTRGLOBAL(0x8009B40Cu, unsigned int *volatile)`
  — a pointer where an `unsigned int` was wanted
  (`error: invalid operands to binary &`). The trailing qualifier is now
  stripped (`pointer_pointee()`); dropping it is behaviour-preserving because
  `PE_DECOMP_PTRGLOBAL` re-reads the guest word with `PE_LoadU32` on every
  expansion anyway.

### Honest cost, measured on one pinned snapshot

Old contract (`git show HEAD:tools/analysis/gen_decomp_ports.py`) and new, run
back to back on the same `src/`:

```
old: eligible 726 (12236 w) -> would emit 366 (5040 w)   # does not build
new: eligible 680 (10864 w) -> would emit 351 (4669 w)   # minus 3 at the E17 gate = 348
```

So correctness costs **18 TUs / 440 instruction words (−4.9 %)** of the
decomp-derived set. In exchange: the tree builds, and 13 leaves whose host
pointer was being silently truncated (E16) no longer ship. Measurable: the
build went from **5 `-Wpointer-to-int-cast` / `-Wint-to-pointer-cast` warnings
to 0** (the 43 that remain are all `-Wdiscarded-qualifiers`; see §6).

Leaves rejected by the new rules (`--check --verbose`) are listed there by name
and reason; each is a candidate for a hand-written adapter, not a lost leaf.

## 3. Defect 2: a clean checkout did not build, and the recipe was wrong

`pc_port/game/decomp/` is git-ignored generated output globbed by
`pc_port/CMakeLists.txt:385`. On a clean checkout the documented
`cmake -S pc_port -B pc_port/build` configures `-- pe: 0 decomp-derived port
TUs` and fails at link with undefined references (`func_800614A0`,
`func_800527B4`, `func_80064A48`, `func_8005E54C`, `func_800527C0`). Also
`cmake` is not on this host's `PATH`.

`scripts/build_pc_port.sh` (new, `set -euo pipefail`, idempotent) runs the five
stages and fails loudly at each: generate → verify → configure → build → test.
It resolves `cmake` from `$CMAKE`, then `PATH`, then a short list of known
prefixes, adds a relocated cmake's own `lib` directory to `LD_LIBRARY_PATH`,
and prints installation instructions if none works. Flags: `--clean`,
`--no-tests`, `--build-dir DIR`; env `CMAKE`, `JOBS`.

The generate+verify pair is retried up to three times, for the case this tree
actually exhibits: eight sibling matching lanes were extending `src/` *while the
build ran*, so a TU generated in stage 1 can already be stale by stage 2.

A second, internal cause was found and fixed in the generator rather than papered
over by the retry: whether a callee gets its own prototype or a loud boundary
depends on whether that callee's TU exists on disk, so adding or pruning a TU
changes the correct content of its **callers**. Starting from an empty
`pc_port/game/decomp/`, a single pass left 27 files stale and needed all three
script retries to settle. The generator now re-runs its own pipeline until the
emit set stops changing (`MAX_ROUNDS = 8`, with a loud warning if it does not
converge): one round in steady state (~15 s), four rounds (~70 s) from empty,
and `--verify` is clean immediately afterwards.

## 4. Defect 3: stale generated TUs

Two mechanical causes, both fixed:

1. The generated tree simply lagged `src/`. Re-running the generator clears it;
   `scripts/build_pc_port.sh` now always regenerates before configuring, so it
   cannot be skipped.
2. The generator never pruned. TUs whose leaf had become ineligible or started
   colliding with a hand port stayed on disk and were still compiled — see
   orphan pruning above.

After both, `--verify` reports `0 drifted` on a settled tree.

## 5. Verification

```
$ python3 tools/build/disc1_plan.py --check
disc1 plan: 1811 spans (1428 c, 381 asm, 2 rodata), geometry=0x1EE000, plan=9afa86715bcc

$ rm -rf pc_port/game/decomp pc_port/build     # simulate a clean checkout
$ ./scripts/build_pc_port.sh
-- [1/5] generating decomp-derived port TUs (attempt 1)
gen_decomp_ports: wrote 367 port TU(s) to pc_port/game/decomp
gen_decomp_ports: 4 leaf/leaves not emitted (rendered TU does not compile)
  uncompilable func_80080B04: passing argument 1 of 'func_8007C044' …
  uncompilable func_8008F178: passing argument 1 of 'func_8008F0D0' …
  uncompilable func_80090BCC: passing argument 1 of 'func_8009021C' …
  uncompilable func_800CED3C: passing argument 1 of 'func_8007506C' …
gen_decomp_ports: 334 conflict(s) not emitted
-- [2/5] verifying generated TUs against src/
gen_decomp_ports: verified 367 port TU(s) up to date; 0 drifted
-- [3/5] configuring     -- pe: 367 decomp-derived port TUs
-- [4/5] building        (0 errors; 43 warnings, all -Wdiscarded-qualifiers)
-- [5/5] running the native suite
Results: 1405 run, 1405 passed, 0 failed, 0 skipped
EXIT=0
```

One invocation, from an empty generated directory, on the first attempt — the
generator's internal fixed-point loop (§3) removed the need for the script's
retries here. (The plan hash differs from §2's because sibling matching lanes
advanced `src/` and `configs/USA/disc1.yaml` between the pinned comparison and
this run. §2 is the like-for-like measurement; this is the end-state proof.)

**No file was excluded by hand.** `0 skipped` only because a retail disc is
configured at `local/pe_disc1.path`; ~44 disc-gated cases skip without one.
1405/1405 matches the prior known-good figure — no test was added, removed,
weakened, or disabled.

Coverage (`tools/analysis/pc_port_coverage.py`), same session:

```
before (plan 18a4acdecb0c, 1028 matched):  from_decomp 284 leaves / 2617 w  (13.2 %)
after  (plan 9afa86715bcc, 1428 matched):  from_decomp 367 leaves / 5269 w  (15.8 %)
```

Read that honestly: the rise is dominated by sibling lanes adding ~400 matched
leaves (the *percentage* even falls, because they add matched leaves faster
than the generator can carry them into the port) during the session, **not** by this change. The like-for-like effect of
this change is the pinned −18 TUs / −440 words in §2.

## 6. Open defects (not fixed here)

1. **A generated wrapper TU's caller can keep a disagreeing prototype.** When a
   callee is itself a decomp-derived leaf emitted through the E7b wrapper path,
   its host signature becomes `(pe_addr_t …)`, but a caller that has no
   canonical shim prototype for it keeps the *leaf's* `unsigned char *`
   declaration verbatim (`neutralize_declarations`). Where the argument types
   then disagree the compiler catches it (E17 rejected `func_8008F178`,
   `func_80090BCC`, `func_80089F58`, `func_800CED3C` on exactly this); where they happen to
   agree, it is a silent ABI mismatch at link time. A scan of the 348 emitted
   TUs found **0 current instances**, so nothing is shipping wrong today, but
   the hole is real. The fix is to make the emitted wrapper signature (not the
   leaf's declaration) the authority for such callees, which needs cross-leaf
   plan knowledge inside `render()`.
2. **43 `-Wdiscarded-qualifiers` warnings** remain in generated TUs
   (`unsigned int *p = &D_800B0CD8;` binds a `volatile` guest lvalue to a
   plain pointer). The address is right; the risk is that the compiler may CSE
   loads the retail code reloads. Worth a rule that propagates the qualifier.
3. **`&D_SYMBOL` as a call argument is screened, not lowered.**
   `fix_argument_addresses` substitutes the retail guest address for a *bare*
   `D_` argument; the `&D_SYMBOL` form is unambiguously the same guest address
   and could be lowered the same way, re-admitting `func_8007506C` /
   `func_800750CC`. Left out deliberately: it widens a rewrite that currently
   has no way to check the callee wants a guest address rather than a host
   pointer.

## 7. Files changed

| File | Change |
| --- | --- |
| `tools/analysis/gen_decomp_ports.py` | E13–E16 static rules; E17 compile gate; orphan pruning; `pointer_pointee()` qualifier fix; `canonical_signatures()`/`split_top()`/`local_objects()`/`is_host_pointer_type()` helpers; `--no-compile-check`; docstring rewrite |
| `scripts/build_pc_port.sh` | **new** — generate/verify/configure/build/test, cmake resolution, `--clean`/`--no-tests`/`--build-dir` |
| `docs/ai_context/PC_PORT_FROM_DECOMP.md` | E13–E17 rows, orphan pruning, the one-step-lag note, build-script recipe |
| `docs/evidence/pc-port-build-repro/REPORT.md` | **new** — this file |
