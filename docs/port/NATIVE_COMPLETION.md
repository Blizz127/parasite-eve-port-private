# Native completion: what still is not compiled host code

Owner goal (2026-10-17, direct): work to 100% native completion, recode anything
that still uses an interpreter, and find and fix bugs and mismatches.

Branch `claude/native-complete-20261017` (worktree `build/wt-native-20261017`).
Base: the Decomp Bot lineage `cursor/pe-native-only-akao-6f51` 52bade3d (Day-5
route) merged with `claude/pe-db9ca12b-private-test1` db9ca12b (the owner's
controller/dev-menu test build), merge commit dce6091b.

Nothing here is published, pushed or installed. No game run happens on the
devbox; verification is builds, unit tests and differential oracles.

## What "interpreter" means in this port

The shipping native-only binary has **no CPU interpreter and no CPU emulation**:

- `pc_port/CMakeLists.txt` links the R3000A interpreter
  (`tests/pe_akao_interp_oracle.c`) only into the test executables
  `pe-native-tests`, `pe-leaf-oracle-tests` and `pe-akao-tick-oracle-tests`.
- `pc_port/platform/pe_audio_driver.c`: the AKAO sound driver runs the matched
  C; "the old in-runtime R3000A interpreter moved to the test-only oracle".
- `pc_port/src/port_main.c`: boundary policy defaults to `stop` and prints
  `[NATIVE] boundary policy=stop (no MIPS interpreter, no CPU emulation in this
  binary)`.

What is still not native is code the port has **no host C for**. Each kind
ends the game for the player (exit 0, no core) when it is reached:

| Kind | Mechanism | Owner-visible effect |
|---|---|---|
| Boundary | `PE_D_COMP_BOUNDARY*`, `PE_Decomp_Boundary*`, `PE_GuestCall` with no registry entry | `CPU_BOUNDARY/REFUSED`, `[STOP] reason=unresolved-boundary` |
| Stub | `Bootstrap_ReturnVoid` on a normal path | stop (strict) |
| Shim | `[DAY1_SHIM]` skip | silent skip in record policy; stop in the native-only policy |
| GTE | `PE_GTE_Command` `abort()` on an unimplemented cop2 command; `PE_GTE_MVMVA` silently returns for LLM/LCM, V1/V2, BK/FC | crash, or wrong geometry/colour with no error |
| Field VM | an opcode handler with no native body | `[VM] unported script opcode`, stop |
| Disc swap | `func_80069B08` is a stub (KNOWN_DIVERGENCES #28) | Disc 2 movies are read from Disc 1 |

Both discs carry a byte-identical EXE (`452fb033...`) and PE.IMG
(`146c0ce7308bf9fdc2ba5a84230e198db0663f3b`, 206,213,120 bytes); only the
`FMV1/` and `FMV2/` movie files differ. One code census covers both discs.

## Census (2026-10-17)

Separate numbers, never one "% native" (docs: separate coverage reporting).

**Room code** (`PE_DISC1_BIN=<disc 1> python3 -I pc_port/tools/pe_all_rooms_native_census.py`,
registry `pc_port/game/decomp_ovl/_dispatch/zz_dispatch.c` sha256 42c50d58...,
5531 entries, 75 tables, overlay entries with fingerprint 0: none):

| | bodies (room x body) | statically reachable |
|---|--:|--:|
| native-generated | 8855 | 4340 |
| not-native | 217 | 177 |
| direct-addr-guarded (hand binding by address only; not counted native) | 31 | 24 |

185 code rooms; 2952 unique (vma, size, FNV) bodies; **136 unique not-native
bodies (183,492 bytes)**, of which 10 have a masked-body C twin in another
room. Rooms are never declared resident in production, so a body resolves
only by fingerprint (closure-equal duplicates count once, as in
`PE_GuestCode_Resolve`).

**Matched room C on other lineages.** 90 overlay C files exist on other refs
and not here. About 59 close census gaps, e.g. m0023i 8018F004/8018F3C8/8018F710/
8018FC14, the shared 0xA48 effect callback (m0012i 8018F20C and 9 twins),
m0418i x7, m0269i x5 and m0428i x4. The other 21 are byte-matched
versions of this lineage's NON_MATCHING ports. The plan is in the session
scratchpad (`harvest_plan.json`).

**EXE** (`tools/analysis/pc_port_coverage.py`): matched 2253; generated from
decomp 1013; hand translated 1119; stub 37; absent 84. "Absent" includes the
Psy-Q libgte helpers the port implements under `PE_` names, so it overcounts.
EXE generator: eligible 1130, ineligible 1123. The top reasons are pointer
models (pointer-to-pointer 153, struct with pointer members 147, inline asm
123), which the room path's E21 guest-memory lowering handles.

**GTE** (read-only census of every cop2 command word): the game issues 22
variants. No room code issues a command outside `PE_GTE_Command`. The 6
unimplemented variants (INTPL, SQR lm1, DCPL, DPCT, and the only LCM/IR/BK
MVMVA `0x4A4DA412` in LightColor `func_80079034`) are in Psy-Q libgte helpers
with no static reference. The exception is `func_80078194`, whose reachable
path is emulated natively (`PE_NormalizeVectorRetail`).

**Explicit boundary/stub/shim sites** (103 lines in production code):
CALL-GAP 29, DISPATCH 20, TRAP 19 (retail-faithful divide/stack guards),
WAIT/IO 11, OTHER 11, infrastructure 13. The native-only binary stops on
boundaries, so 5 of the 6 `[DAY1_SHIM]` paths are unreachable there; the Eve
flash substitution (`m0023i_effect_port.c`, KNOWN_DIVERGENCES #17) runs
silently.

## Fixed in this branch

- **Seven stale boundaries** (0a2adc74): the target had host C but the call
  site still stopped the game. Items -> Use for item kinds 2, 4-6 and 12-14;
  Equipment top menu rows 2/3; the Equipment tool mode (cancel, tool window,
  confirm, back); effect `func_800DEA30` mode 1, whose particle velocity is now
  rotated by the actor matrix as in retail. Verification: rewritten against the
  matched C by reading; the native suite passes and `PA_ovl2_DEA30` checks the
  rotation. **No test drives the Items/Equipment branches yet**; a directed
  retail differential of `func_80057834`/`func_800466C0` is still to do.
- **e21 oracle** (c0dc1e9f, dd4151ce): merged the 10-15 tightening (prologue-only
  register-save exclusion, 12 stack arguments, 32-byte pointer snapshots,
  per-callee extents, GTE state compare). A first-branch delay-slot spill now
  counts as a save; this cleared the two false MISMATCHes it caused (m0188i
  80195A14, m0273i 801981A4). The sweep finds 660 ok, 28 skipped and 0
  unexplained mismatches over all current candidates.

## Gates on this branch (2026-10-17)

- `pe-native-tests` (isolated HOME, Disc 1 configured): 1657 run, 1657 passed,
  0 failed, 0 skipped.
- `retail_data_guard.py --strict --exe ... --peimg ...`: OK, 20540 files.
- `psyq_port_guard.py --binary build/pcport/parasite-eve-port --require-exe`:
  PASS relative to the allowlist, 976 offenders. This is unchanged and is not a
  clearance; the public-binary hold stands.
- No game run, runner job, package or publication.

## Work packages

1. Integrate the 2026-10-15 source packets (`docs/evidence/native-*-20261015/`)
   by source, regenerating the registry instead of applying staged edits:
   complete native GTE commands plus the matching lowering policy; m0086i
   8018F004/8018F734/801900CC and m0089i 801924F8; the 80190574 return fix;
   80078004 LZCS/LZCR; AVSZ3/AVSZ4 FLAG; field-VM opcodes 0x83/0xB6; font-page
   initializer 80038D74; animation decoders 80039B74/80039D24/80039ED4.
2. Room census and conversion of reachable not-native bodies, twins first.
3. Differential sweep: e21 over every generated room TU (today only TUs without
   calls, plus NON_MATCHING leaves, are tested).
4. EXE: replace hand ports and close stubs/absent leaves from matched C.
5. Disc swap (#28).
