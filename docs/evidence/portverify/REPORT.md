# portverify — hand ports vs matched decomp (2026-09-23/24)

**Question:** `tools/analysis/gen_decomp_ports.py` reported 449 *conflicts*:
matched leaves whose generated TU would collide with an existing hand-written
pc_port definition. For each one, does the hand port behave like the matched C?

**Method:** `tools/analysis/portverify/run.sh`. It renders each conflict's
generated TU without writing `pc_port/game/decomp`, renames the function to
`pv_<name>`, and compiles it exactly as pc_port compiles decomp TUs (`-std=gnu11
-O0 -funsigned-char`). It links that against the pc_port runtime library, which
supplies the hand definition. Each trial forks both versions from identical
seeded guest RAM, scratchpad and arguments. The seeds use 12 RAM modes: zero,
random, pointer-dense, small bytes/words, 0xF8–0xFF, and mixed; argument pools
cover edge values, NULL and valid guest pointers. The harness compares all 2 MiB
of RAM, the scratchpad and the return value (typed by the matched C). A
difference is only a *candidate*: each one below was confirmed against retail
with `tools/analysis/pe_dis.sh` before anything changed.

## Result

| | count |
| --- | --- |
| conflicts at start | 449 |
| harnessed (compiled + linked renamed) | 413 |
| hand ports retired → generated TU (now `ported-from-decomp`) | **273** (5390 words), list: `switched_leaves.txt` |
| confirmed hand-port bugs fixed | **9 leaves** + 1 systemic (char signedness) |
| generator soundness bugs found | 5 (2 fixed in the generator, 1 fixed in CMake, 2 reported) |

Coverage (`python3 tools/analysis/pc_port_coverage.py`):
before `matched=1834 from_decomp=475 hand=1278 stub=21 absent=60`;
after `matched=1902 from_decomp=756 hand=1122 stub=22 absent=2`. The absent
drop and the matched rise come from sibling lanes. The 273 retirements are
this lane's.

Suite: `scripts/build_pc_port.sh --build-dir build/fanout/portverify/pcbuild` →
`Results: 1544 run, 1544 passed, 0 failed, 0 skipped`, and
`gen_decomp_ports: verified 756 port TU(s) up to date; 0 drifted`.

## Retail-accuracy bugs fixed (hand port ≠ matched C = retail)

| leaf | hand port did | retail / matched C | evidence | fix |
| --- | --- | --- | --- | --- |
| `func_800661EC` | mask `& 0xFFF0` (drops bits 31..16); `a3==8` → 1 else 9 | `li v0,-16` (`~0xF`); `bne a3,8` → `ori 1`, fall-through `ori 9` | 0x80066220, 0x8006624C | retired; tests BTL64, DECOMPPTR_field_forwarder, PV_661EC |
| `func_800144FC` | state ≥ 0x3C yields (returns 0, rewinds 0xC) | `sltiu 60`/`beqz` → `j …; li v0,1` (completes) | 0x80014520, 0x80014658 | retired; BTL95, PV_144FC |
| `func_80019B08` | actor bit 8 set → wait, clear → complete | bit 8 set → clear 0x20, return 1; clear → wait | 0x80019B30–0x80019B4C | retired; SEW8, PV_19B08 |
| `func_8004F464` | boundary call, `D_8009D008` never cleared | call, then `D_8009D008 = 0` | src/func_8004F464.c | retired; PV_4F464 |
| `func_800528F0` | 2 self-XOR passes (and the test "oracle" too) | 3 passes (six xor loops 0x800529EC–0x80052B10) | pe_dis 0x800528F0 | retired; oracle + constants in 528F0/6A9E4 |
| `func_8006B35C` | one store to `0x80094488` | zero `+6` of each of 4 eight-byte records (`0x8009448E/96/9E/A6`) | src/func_8006B35C.c | hand fixed (kept for its null-`w14C` host guard); PV_6B35C |
| `func_8006BE4C` | `(CE3 + sign) >> 1` | `(CE3 - 10) / 2` (`addiu v1,a0,-10`) | 0x8006BE94 | retired; BTL127 gate, PV_6BE4C |
| `func_80067B74` | early-return stub (body missing) | fade step: colour walk, counter, phase 0x400→0x800 | src/func_80067B74.c, `break 7` at 0x80067BB0 | hand adapter written; PV_67B74 |
| `func_80067D18` | early-return stub | colour hold + 0xC000 phase advance | src/func_80067D18.c | hand adapter written; PV_67D18 |
| all decomp TUs using plain `char` (63, e.g. `func_80052BCC`) | compiled with x86 **signed** `char`: `c &= 0xFF; if (c == 0xFF)` never true | era cc1 `char` is unsigned (`lbu`) | retail oracle INV8 case 80 (`func_80052BCC(0x8015FB80, 0x8015FC00)`) | `pc_port/CMakeLists.txt`: `-funsigned-char` (`/J` on MSVC) for `game/decomp` + `game/decomp_hand`; PV_52BCC |

Regression tests: `pc_port/tests/test_portverify.h` (`test_PV_all`), plus the
updated existing tests named above. Every one of those old assertions encoded
the hand bug.

## Generator soundness bugs (gen_decomp_ports.py)

1. **E7b pointer adapter translated eagerly:** `PE_Translate(pe_p, 1)` aborted
   on a guest NULL before the leaf's own `if (a0 != 0)` could run
   (`func_80080998`, route test B54KY). **Fixed:** NULL stays NULL.
2. **Wrapper pointer-global slots translated eagerly:** a null slot the leaf only
   compares aborted (`func_800136C0`: `o != D_8009D254`, new-game route).
   **Fixed:** same null-tolerant form.
3. **Plain `char` signedness:** see the table above. **Fixed in CMake.**
4. **`(T **)` casts pass E9/E15:** an 8-byte host-pointer load from 4-byte guest
   RAM. This is live in `pc_port/game/decomp/func_80030640_port.c:43`
   (`inner = *(unsigned int **)(rec + 0x68)`), and also shows up in the conflict
   TUs `func_80067B74`, `func_80067D18`, `func_80051684` and `func_800524D0`.
   `func_80017EC4` has an int→host-pointer cast of a parameter and of a scalar
   `D_`. **Reported to the lead, not fixed**; `audit_casts.py` flags the shape.
5. **KUSEG guest pointers:** `PE_Translate` accepts only KSEG0 RAM, but retail
   data holds KUSEG aliases (`0x00xxxxxx`); the hand ports emulate the mirror
   (`menu_ram`, `sort_ram`, `pe_menu_ram`, `func_8001A680_command_cut`). A
   generated TU cannot. **Reported:** a platform-level mirror in `PE_Translate`
   would let those leaves switch.

## Why some equivalent-looking leaves stay hand ports

Each of these was switched, failed the suite or a route, and was reverted with
`revert.py`.

- **Retail v0 residual** (void matched leaf, but callers or tests read the `$v0`
  that the tail callee left): `func_80086464`, `func_80087090`, `func_80086608`,
  `func_800614AC`, `func_800824F0`, `func_80052594`, `func_800868F0`,
  `func_80086A28`.
- **Host glue** (stop-epoch / `ShouldStop` early exits, CD MMIO through
  `PE_CdStoreU8`): `func_80036F7C`, `func_8003E91C`, `func_80042228`,
  `func_80042928`, `func_8004CDAC`, `func_8005C1EC`, `func_80069660`,
  `func_8007A2A4`, `func_8007B964`, `func_8007BAC0`.
- **KUSEG aliasing:** `func_8005F1A0`, `func_8006346C`, `func_800724F4`,
  `func_80064C20`, plus `func_80013300`, `func_80017AE8`, `func_80022210`. The
  last three call `func_8001A680_command_cut`; the generated TU would reach the
  non-mirroring `func_8001A680` adapter.
- **Not switchable today:** 25 TUs that don't compile renamed; 5 whose callees
  are file-static in pc_port; 13 with loud boundaries where the hand port
  implements the callee; 9 unsafe-cast TUs; 8 static hand definitions;
  hardware adapters (`func_80073E10` IRQ mask, `func_80076B98` GPU DMA,
  `func_8007D15C` SPU IRQ, `func_800811E4`/`func_800800F4` CD timing).

## Known false-positive classes (for future runs)

- Pointer-dense seeds make a `count` global negative, so the host-compiled
  decomp's `base + count` pointer arithmetic doesn't wrap like 32-bit retail
  (`func_80053B48`, `func_80053E6C`). The hand port is *more* retail-like there.
- A hand port that reads retail `.rodata` from guest RAM where the matched C has
  local arrays (`func_800323C8`).
- Boundary policy: the hand port stops the port at an unported callee, while the
  generated TU records the boundary and continues (`func_80042798`,
  `func_80042A10`).
- Harness blind spot: narrow input gates (`func_8006BE4C` passed 64 random
  trials while buggy). Small-value modes 8–11 were added afterwards, and
  equivalence verdicts remain evidence, not proof.

## Reproduce

```bash
export TMPDIR=$PWD/build/tmp
tools/analysis/portverify/run.sh --lib 192      # work dir: build/portverify
grep ^DIVERGE build/portverify/sweep.txt
```

## Round 2 (2026-09-24): KUSEG mirror + remaining conflicts

- **`(T **)` audit:** `audit_casts.py` flags 0 of the live generated TUs after
  portabsent's E9b/E9 rules. `func_80030640` is now a hand adapter
  (`decomp_hand/e9_guest_ptr_port.c`).
- **RAM mirror:** `pc_port/platform/pe_guest_ram.c` `translate_impl` folds KUSEG
  `0x00000000–0x001FFFFF` and KSEG1 `0xA0000000–0xA01FFFFF` onto KSEG0 through
  `PE_RamCanonical()`. Only the mapped 2 MiB is folded, and a range may not cross
  the segment end. `PE_AddressIsRam`/`PE_RangeIsRam` keep their KSEG0-only
  meaning for their 110 callers. Tests: `PV_ram_mirror`, which covers the aliases,
  the last word, a crossing word (aborts) and `0x00200000` (aborts). The two tests
  that required address 0 to abort (`53968_…`, `53B48_…`) now assert the
  retail semantics instead: address 0 reads low RAM, identical to `0x80000000`.
- **Mirror leaves switched:** `func_8006346C`, `func_800724F4`, `func_80013300`,
  `func_80017AE8`, `func_80022210`. `func_8005F1A0` and `func_80064C20` stay hand
  ports: retail dereferences guest address 0 there, and the generator maps guest
  0 to host NULL (finding 6 below).
- **Re-sweep of every remaining conflict** (192 trials, mirror on): 61 EQUIV,
  93 ABORTDIFF, 18 DIVERGE, 1 INCONCL (down from 23).
  - 26 more leaves switched: EQUIV, or ABORTDIFF where only the hand port aborts,
    after auto-excluding host glue, helper calls, v0 residuals and unsafe shapes.
  - Every new DIVERGE was classified: CD adapter `func_80080D5C`; argument-position
    convention `func_800CE3B4` (equivalent under `argmap.json`);
    `func_800C2B10`/`func_800C2B28`, see finding 7.
  - 71 of the ABORTDIFFs are generated-TU SIGSEGVs from the guest 0 → host NULL
    rule.
- **Generator findings routed to portabsent:**
  - (6) Map NULL only for names the leaf null-tests, and mirror-translate
    otherwise. This would unblock about 70 more switches.
  - (7) `PE_HostToGuest` canonicalizes KUSEG aliases in pointer returns: retail
    returns `0x6D`, the generated TU `0x8000006D`.
- **Totals:** 304 hand ports retired (6317 words). Coverage
  `matched=1943 from_decomp=793 hand=1123 stub=25 absent=2`. Suite
  `Results: 1555 run, 1555 passed, 0 failed, 0 skipped`.

## Rounds 3–4 (2026-09-24): after portabsent's null_tested rule and guest-call dispatch

- **Round 3** (null_tested rule, finding 6): generated-TU SIGSEGVs dropped
  from 71 to 17. 12 hand ports retired, including `func_8005F1A0` and
  `func_80064C20`. Their generated TUs now pass the live ATK24 route test.
  `func_800CE2B4`/`func_800CE3B4` take the matched `(a0, a1, a2=data)`; the
  weapon-callback dispatcher passes `(slot, rec, data)`.
- **Round 4** (portabsent: `PE_GuestCall` dispatch, static-definition
  handling): 256 conflicts. The harness now links the guest-code registry
  (`game/decomp_ovl/_dispatch/zz_dispatch.c`) plus a `Trace_Direct` sink, and
  maps leaf-local typedefs to guest words. 21 more hand ports retired.
- **New retail-accuracy bug fixed:** the hand port of `func_80052F70` compared
  the item cap unsigned; retail is `slti v1,v1,51` (0x80052F94, signed) and
  reloads the byte after each call. The hand port is kept, because it reads
  pc_port's host-side `D_8009D018`, and fixed. Regression
  `PV_52F70_signed_cap`.
- **Still open for portabsent:** finding (7) misses pointer roots reached
  through a local copy. `func_80078C94` and `func_80072334` return
  `0x80000000` for a destination of 0, where retail returns 0.
- **Classified, kept as hand ports:**
  - boundaries in the generated TU: 3B708, 405A4, 409B4, 75B4C, 80164, 3AF14
  - stop/controller glue: 8003EB04, 8005C498, 80082974
  - CD adapters: 8007C13C, 8007C214, 80080D5C
  - SDK callback models in `pe_libetc.c`: 73C94, 73CC4, 73CF4, 73D24, 73D58
  - callback-dispatch policy: 80042B6C
  - host-side `D_8009D048` authority: 80054CF8

## Round 5 (2026-09-24)

- 12 more hand ports retired (total **349**); `func_80078C94` became EQUIV once
  portabsent's copy-root segment fix landed. It and three other EQUIV
  `decomp_hand` adapters were retired by portabsent.
- Generator finding 9: relational compares of host pointers rooted in
  different guest regions don't preserve guest order. `func_80072334`
  (dst=0x1F800258 scratchpad, src=0) returned dst+3 where retail returns dst.
  portabsent landed it as rule E19; `func_80072334` stays a hand port.
- The harness masks the `decomp_hand` guest scratch windows 0x801FF500–0x801FFD7F.
- Suite `Results: 1569 run, 1569 passed, 0 failed, 0 skipped`; coverage
  `matched=1959 from_decomp=849 hand=1083 stub=27 absent=0`.
