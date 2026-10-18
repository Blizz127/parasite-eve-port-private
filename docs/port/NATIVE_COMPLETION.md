# Native completion: what still is not compiled host code

Owner goal (2026-10-17, direct): work to 100% native completion, recode anything
that still uses an interpreter, and find and fix bugs and mismatches.

Branch `claude/native-complete-20261017` (worktree `build/wt-native-20261017`).
Base: the Decomp Bot lineage `cursor/pe-native-only-akao-6f51` 52bade3d (Day-5
route) merged with `claude/pe-db9ca12b-private-test1` db9ca12b (the owner's
controller/dev-menu test build), merge commit dce6091b.

Nothing here is published, pushed or installed. No game ran on the devbox.
Verification is builds, unit tests and differential oracles. Every number
below comes from a command named next to it. Matched C, behaviour (NON_MATCHING)
C, hand ports and generated TUs are counted separately; none of this is a
"% native" or a playable-slice claim.

## What "interpreter" means in this port

The shipping native-only binary has **no CPU interpreter and no CPU emulation**:

- `pc_port/CMakeLists.txt` links the R3000A interpreter
  (`tests/pe_akao_interp_oracle.c`) only into the test executables
  `pe-native-tests`, `pe-leaf-oracle-tests` and `pe-akao-tick-oracle-tests`.
- `pc_port/platform/pe_audio_driver.c`: the AKAO sound driver runs the matched
  C. "The old in-runtime R3000A interpreter moved to the test-only oracle."
- `pc_port/src/port_main.c`: the boundary policy defaults to `stop` and prints
  `[NATIVE] boundary policy=stop (no MIPS interpreter, no CPU emulation in this
  binary)`.

What is still not native is code the port has **no host C for**. Each case
ends the game for the player (exit 0, no core) when it is reached:

| Kind | Mechanism | Owner-visible effect |
|---|---|---|
| Boundary | `PE_D_COMP_BOUNDARY*`, `PE_Decomp_Boundary*`, `PE_GuestCall` with no registry entry | `CPU_BOUNDARY/REFUSED`, `[STOP] reason=unresolved-boundary` |
| Stub | `Bootstrap_ReturnVoid` on a normal path | stop (strict) |
| Shim | `[DAY1_SHIM]` skip | silent skip under the record policy; stop under the native-only policy (one exception, below) |
| Disc swap | `func_80069B08` is a native hand port with a host tray; it stops only when no `--disc2-image` is configured. `package_runtime.sh` ships `data/disc2.bin`. | stop without a Disc 2 image |

Both discs carry a byte-identical EXE (`452fb033...`) and PE.IMG
(`146c0ce7...`). Only the `FMV1/` and `FMV2/` movies differ, so one code census
covers both discs.

## Census (2026-10-18)

**Room code**: `PE_DISC1_BIN=<disc 1> python3 pc_port/tools/pe_all_rooms_native_census.py`.
Registry `zz_dispatch.c`: 5743 thunks. 185 code rooms, 9103 (room x body)
rows, 2952 unique (vma, size, FNV) bodies.

| | session start | 2026-10-18 06:10 |
|---|--:|--:|
| native-generated bodies | 8855 | 9073 (87 via a data head) |
| not-native bodies (all statically reachable) | 217 | 28 |
| direct-addr-guarded (hand port bound by address only) | 31 | 0 |
| native-direct (hand port bound by fingerprint) | 0 | 2 |
| unique not-native bodies | 136 (183,492 bytes) | 28 (68,192 bytes) |

A body resolves only by fingerprint, as `PE_GuestCode_Resolve` does. Rooms are
never declared resident in production, and closure-equal duplicates count once.

The 28 bodies with no C anywhere are effect and particle callbacks in late
rooms:

| room | bodies |
|---|--:|
| m0318i | 8 |
| m0432i | 9 |
| m0429i | 8 |
| m0404i | 1 |
| m0256i | 1 |
| m0273i | 1 |

The vendor (khasinski) tree also has them as asm. The two native-direct bodies
are m0005i 8018F614/8018FDC4: hand ports the effect dispatcher binds by
fingerprint (efffd7cb).

From the previous census (47) to this one, seven bodies became generated
NON_MATCHING TUs: m0429i 801982A4 (4dba7922, after that census) and room
batch 1 (m0318i 80192718/801955E4, m0350i 80196F2C, m0428i 801928CC, m0429i
80192878, m0432i 801962FC). The earlier statement here that the 47 already
excluded 801982A4 was wrong; the census listed it as not-native.

Batch 2 (after that census: m0318i 80192A7C/80193638, m0432i 801957CC) took it
from 40 to 37.
Batch 4 (m0318i 80194164/8019646C/80192ED4/8019326C, m0404i 80193A6C) took it
from 37 to 32.
Batch 5 (m0318i 80196820/80195904/80195190, m0404i 801935E0) took it from 32
to 28.

**EXE** (`gen_decomp_ports.py --e21`): eligible 1130 verbatim, E21 lowered
21, 112 matched leaves that a hand port defines. Five uncompilable verbatim
TUs remain: 80072F64, 80073A44, 8007A244, 80080E34 and 80081A7C. All are in
the Psy-Q band, where the port's own `PE_` models are the implementation.

The 12 asm-only EXE effect functions (no C on any lineage, reached through
effect descriptor tables in the EXE data) are NON_MATCHING C now:
- 800C3324, 800C689C, 800D5A00, 800D8388, 800C83A0, 800C499C (249e76f38);
- 800C9398, 800D6514, 800DC058, 800D5290, 800DE0A8, 800CAE0C (batch 3).

All 12 came from workflow wf_b3938761: a decomp agent wrote each one, an
independent verifier confirmed it, and the main session re-ran the proof.
800C3324 is the 3D textured-quad packet builder that the weapon-effect
callbacks 800CC644/800CC878 (the 0x800E0D08 table) reached through a STOP
boundary. With batch 3, 1040 + 6 = 1046 EXE TUs.

**Subsystem overlays**:

| overlay | TUs | note |
|---|--:|---|
| ovl_0457 | 27/27 | credits, CREDITS.XA |
| ovl_0700 | 45/46 | |
| ovl_03D2 | 26/27 | |
| ovl_03C9 | 18/18 | |
| ovl_03C5 | 8/9 | |

Every skipped leaf has a native hand port.

## Fixed in this branch

Boundaries and dispatch:

- **0a2adc74** (seven stale boundaries; the target had host C):
  - Items -> Use for item kinds 2, 4-6 and 12-14
  - Equipment top menu rows 2/3
  - Equipment tool cancel, window, confirm and back
  - effect `func_800DEA30` mode 1, now rotated by the actor matrix.
- **bc80e0ce**: seven callback dispatchers try the registry before stopping:
  - menu cell predicate
  - inventory sort item and compare
  - actor contact
  - CD, SDK and card-hook indirect calls.

  `func_800CF5B0` and `func_800DB25C` are called with their matched C's
  arguments.
- **69ec7e0e**:
  - Equipment confirm calls the generated `func_8005A318`; it was a boundary.
  - `func_800D5898` and `func_800DAB98` call the generated `func_800D27FC`.
- **efffd7cb**: the effect dispatcher (`func_800D413C_port.c`) bound m0005i's
  hand ports by address alone. Other rooms put different code at those addresses:
  - m0113i 8018F018, the pool callback of its 0xA48 effect
  - m0412i 8018F614
  - m0205i/m0243i/m0406i/m0410i 8018FB84
  - m0256i 80190A6C

  For those rooms, Eve's charge-effect code would have run on the wrong data.
  Each binding now requires the m0005i body's FNV. The Eve charge, flare and
  beam retail tests now place those code bytes from PE.IMG, as retail has
  them resident.

Native bodies that replaced boundaries:

- **ending credits** (docs/evidence/native-credits-20261018/REPORT.md):
  - `func_8001220C` reached the credits main loop `0x801235DC` through a
    `psx_compat.h` Bootstrap stub. It now calls through the registry, which
    binds the generated ovl_0457 TU by the fingerprint of the bytes
    `func_8006F044` just reloaded.
  - The credits' libpress calls (DecDCTBufSize, DecDCTvlcSize, DecDCToutSync)
    run on host replacements (`platform/pe_libpress.c`).
  - Subsystem overlays and E21 EXE leaves now apply host replacements, as
    rooms did.
  - printf (`func_80071A74`) has a host replacement. room_m0318i
    `func_80192560` had stopped on its actor-not-found debug print.
  - The `func_8004DCA4` nonzero mode (field-VM menu op) follows the matched C
    and is checked against 8 new retail cases.
  - The decode differential (31 pictures x 3 bounds, 558 resumed calls) is the
    first run of the `func_8010C89C` hand port's resume path.

- **d0c4075e**: GTE command engine for all 22 documented COP2 commands. The
  `gte-native-hardware` ctest has 1100 real-console cases and 231741 checks,
  with 0 failures.
- **97b02098**: font-page initializer `80038D74` and animation decoders
  `80039B74/D24/ED4`.
- **de55f637**: m0086i 8018F734 (NON_MATCHING).
- **60bc8623 / 69ec7e0e**: E21 lowering of matched EXE leaves no hand port
  defines, 21 of them. They include field-VM 0x83/0xB6, `8005A318`,
  `800D27FC`, `8006B4F8` (room loader; reached only via `8003F074` through the
  registry), `800CF5B0` and `800DB25C`.

Room and overlay leaves:

- **9bafd689 / 7cb8ed02**: 59 + 5 byte-matched leaves harvested from other
  lineages, each `EXACT_REBUILD_GATE=PASS`.
- **0b71115e**: 15 functions carved after rooms' data heads (gates PASS).
- **c71599ad / 69ec7e0e**: lowering of bitfields, `__builtin_memcpy`,
  `func_`-named data, integer inline asm, file-local static helpers and dead
  trailing parameters. This unlocked 11 room bodies and 16 subsystem-overlay
  bodies.
- **4dba7922** m0429i 801982A4 (NON_MATCHING): near twin of m0269i
  8019070C. `e21_leaf.py` passes 91 directed seeds, covering 401/412 words; a
  supplementary spec covers the 11 default arms.

Lowering bugs found by the differential:

- **60bc8623**: narrow prototype arguments on jalr. m0429i 80191928 passed
  0x801063EC where retail passes 0x63EC.
- **6c802c38**: call-containing arguments are evaluated in source order. In
  m0418i 80190914, GetTPage runs before GetClut.
- **a1eb0f38**: narrow prototype arguments on *direct* calls. In m0429i
  801982A4 and its twin m0269i 8019070C, `func_800CFB7C(void *, short, void *)`
  received 0x0007A24A where retail passes 0xFFFFA24A.

Tools:

- **ada567f6 / 4850836a**:
  - The e21 oracle GTE models FLAG and the 44-bit MAC, and runs every other
    command on the port's hardware-checked engine.
  - Harness extent, seeding and mock fixes.
  - `e21_leaf.py` proves one room leaf privately, with `--nonmatching`,
    `--inputs` and `--uncovered`.
- **08cf6870 / d6fcf88b**: the census data-head rule, matching how retail
  addresses those words.

## e21 differential sweep (`e21_diff.py --with-calls --check-gte`)

Before the oracle fixes: 1716 ok, 113 MISMATCH, 41 skipped, 1 nocompile. Of
the 113:

| count | cause | status |
|--:|---|---|
| 96 | oracle FLAG / 44-bit MAC | oracle fixed |
| 8 | harness extents, mocks, seeding | harness fixed |
| 1 | real argument-order bug | fixed in 6c802c38 |
| 8 | retail reads uninitialized stack | residual, below |

The 41 skipped TUs now run their unmodelled GTE commands on the native engine.

Sweep 3 (2026-10-18 00:32-01:01 UTC, `e21_diff.py --with-calls --check-gte
--jobs 4 --verbose`): 1711 ok, 8 MISMATCH, 139 nocompile, 31 skipped.

- The 8 mismatches are exactly the stack-residue set below: the 0x4A0 body in
  m0065i/m0087i/m0391i/m0393i/m0399i/m0401i, and the 0x3FC CompMatrix body in
  m0034i/m0174i.
- The 139 nocompiles were a harness bug from ada567f6. Its mock-stub
  parameter heuristic read `unsigned code` (type `unsigned`, name `code`) as
  an unnamed two-word type and emitted `unsigned code p1`. So those 139 TUs
  were not tested in sweep 3. `e21_diff.param_is_named` now treats a last word
  as a type only when it is a C type keyword or a `_t` typedef.

Sweep 4 (2026-10-18 02:38-03:10 UTC): the generated tree of 3c103eb4e (room
batch 1 included), with the f91561717 harness fix:

    e21_diff: MISMATCH: 8
    e21_diff: ok: 1853
    e21_diff: skipped: 34

All 1895 room candidates ran, with 0 nocompile. The 139 TUs sweep 3 could not
compile all pass. The 8 mismatches are the same stack-residue TUs.

Sweep 5 (2026-10-18 04:30-05:04 UTC, all 2012 candidates on the bfdf402af
tree: 1898 room, 81 subsystem-overlay and 33 EXE TUs):

    e21_diff: MISMATCH: 11
    e21_diff: nocompile: 7
    e21_diff: ok: 1954
    e21_diff: skipped: 40

- The 8 room mismatches are the stack-residue set again.
- The other 10 findings were all subsystem-overlay TUs swept for the first
  time, and every one was a harness artifact, fixed in 3af74726a:
  - a parameter named `a_t`;
  - a mock symbol duplicated by `pe_gte.c`;
  - callee outputs the mocks did not write (VectorNormal, RotTrans,
    Akao_Calc3DPan, ovl_0700 8018F55C);
  - stack-pointer logging at boundary calls.
- Each of the 10 TUs then matched, 9 of them with every word covered. No
  subsystem-overlay or EXE TU showed a port difference.

Subsystem overlays had never been swept before this. The sweep took only room TUs until
495b7c1c4, which adds the 81 E21 TUs of ovl_03C5/03C9/03D2/0457/0700.
`--only ovl_0457` gives 15 ok, 3 skipped, 0 MISMATCH. The three skipped
(func_8012562C, func_80125744, func_80125894) are CD-status busy-waits that
no seeded mock exits; they need directed mock return values, which the
harness does not have yet.

## Host compile semantics (verbatim TUs)

1016 of the 2923 room TUs, and most generated EXE TUs, are verbatim lowerings,
not E21. The leaf's C body is copied and guest data is reached through typed
host pointers (`PE_DECOMP_SCALAR` / `PE_DECOMP_ARRAY` / `PE_Translate`). The
e21 sweep takes only E21 TUs (`candidates()` needs the "E21 guest-memory"
marker), so no sweep has compared these TUs with retail. Static checks on the
2026-10-18 tree:

- **`long` was 64-bit (bug, fixed in the generator).** gcc-2.7.2-psx is
  ILP32. ovl_03C5 `80121A00` and ovl_03D2 `801922F4` define
  `D_800BCDC8 PE_DECOMP_ARRAY(0x800BCDC8u, DRAWENV)` with a DRAWENV that ends
  in `unsigned long dr_env[16]`. On x86-64 that struct is 160 bytes instead of
  retail's 0x5C. `D_800BCDC8[i] = D_801D13AC[i]` for i = 0, 1 then copies at
  the wrong stride and runs past the two entries, over `D_800BCE80` (both
  DISPENVs) and 96 more bytes of EXE globals. Both TUs are fingerprint-bound
  in the dispatch registry (0CA880D5 and DB91F7BC). A scan of the EXE and
  all 515 PE.IMG blobs found:
  - one jal to 80121A00 and none to 801922F4;
  - no aligned word holding either address;
  - no `lui` + `addiu`/`ori` pair (within 24 instructions) building either.

  The one jal is in `func_8003F3C4` (EXE 8003F534), whose hand port calls the
  hand-ported `func_80121A00` by name. So no static reference reaches either
  generated TU. `gen_decomp_ports.era_ilp32` now spells `long` as `int`
  (outside `long long`) in every verbatim plan; the E21 lowering already typed
  `long` as 4 bytes. A scan of every verbatim struct that overlays guest
  memory found no other host-width member (no pointer members; E18 rejects
  those).
- **Strict aliasing and signed overflow.** Production compiles decomp TUs at
  `-O3` (Release), while the era compiler had no type-based aliasing and
  retail's addu/subu wrap. Compiling every decomp TU at the production flags
  with and without the two options (4093 TUs, no compile failures): 88
  compile differently, 16 because of `-fno-strict-aliasing` and 73 because of
  `-fwrapv` (one because of both).
  The aliasing ones that were read (for example the libspu-style
  `func_80085F74`: `*(unsigned int *)a0` and `*(short *)(a0 + 8)`) only
  reschedule loads. The wrapv ones fold comparisons such as
  `0x1400 - r > 0x1000` (`func_80019C4C`) on the assumption that nothing
  overflows. Decomp TUs are now built with `-fno-strict-aliasing -fwrapv`
  (pc_port/CMakeLists.txt). The e21 harness compiles at `-O1` unless
  `E21_CFLAGS` (the TU) and `E21_GTE_CFLAGS` (pe_gte.c and pe_gte_commands.c,
  plain `-O3` in production) are set.
- **Unsequenced calls.** 13 verbatim statements have the shape
  `r = func_800C251C(o, ...) | func_800C2758(o, ...)`, where C leaves the
  order unspecified. Retail calls func_800C251C first (for example
  `func_800C7D54`: jal 800C251C at 800C7D6C, then jal 800C2758 at
  800C7D88). The relocations in all 13 host objects show the same order
  under GCC -O3. This is a latent hazard for another compiler, not a live bug.
  The 60 E21 groups are rand() (`func_80071A54`) beside the pure sin/cos table
  getters `func_80077DC4` / `80077CF4`, where order cannot change a result.
  (Scanner: siblings in one full-expression, `&&`, `||`, `?:`, `,` and `;`
  treated as sequence points.)

## Boundary-site census

`python3 tools/analysis/boundary_site_census.py` lists every Bootstrap /
boundary call site in production pc_port code, including calls of file-local
wrappers such as `menu_boundary("name")`. On 2026-10-18 it found 228 sites.
60 name a function exactly, and 28 of those name one that has host C. Each of
the 28 was read, and none is stale:

- a jump-table or callback target other than the retail default;
- the bootstrap-disc test fixture in `func_800698D4`;
- a no-disc or no-MDEC condition;
- retail `BREAK 7` divide traps;
- the memory-card boundary test mode.

`func_8004C608`'s "unported help selection" default is unreachable: the hand
port has explicit cases for every id 0..60.

`pc_port_coverage.py`'s "stub" bucket (36 leaves) is not a list of non-native
code. It counts any body that has a bootstrap marker on any path, and most of
those are hand ports with one guarded error path.

## Known residuals (documented, not hidden)

- **Retail stack residue.** `RotMatrix` (80079754) sets only a matrix's 3x3.
  The code then reads that matrix's translation from stack the call never
  wrote:
  - the 0x4A0 body in m0065i/m0087i/m0391i/m0393i/m0399i/m0401i loads it as
    TR;
  - m0034i/m0174i `gte_CompMatrix(&o->m, &m20, ...)` uses `m20.t`, so
    `o->m.t` follows the residue.

  Native residue differs, so effect placement can differ. A fix needs recorded
  retail stack contents (a runner `--recorded` snapshot), not zero-fill. This
  is the same class as KNOWN_DIVERGENCES #17.
- **m0023i flash `[DAY1_SHIM]`** (`m0023i_effect_port.c`, KNOWN_DIVERGENCES
  #17) is the one shim that substitutes silently in production. Retail reads
  the stack residue of the previous mode-2 call.
- **m0005i hand ports versus generated TUs.** 8018F018, 8018F330 and 8018FB84
  now have matched C (harvested), but the effect dispatcher still sends
  m0005i's own bytes to the hand ports. Those hand ports are checked against
  retail by the ATK25/ATK26/ATK29 retail-case tests (Open, below).
- `func_8003F074` -> `func_8006B4F8` (room loader) is generated and reached only
  by a guest jalr. The production room transition uses the hand cuts in
  `func_8006B4F8_port.c`.

## Gates on this branch

- `pe-native-tests` (isolated HOME, Disc 1 configured, built in /dev/shm from
  the regeneration with room batch 5, 2026-10-18 06:07 UTC): 1661/1661, 0
  skipped. The four `credits_*` tests and 8 more NAM9 retail cases are new
  since 4dba7922.
- ctests `park-m0086-full`, `park-m0089-full`, `gte-native-fixed`,
  `gte-native-hardware`, and the two dev-menu ctests: 6/6 pass.
- `scripts/run_native_oracle_harnesses.sh`:
  - font 54/54
  - animation 117/117
  - field VM 693/693
- `retail_data_guard.py --strict` on every commit's files: OK.
- `psyq_port_guard.py --binary parasite-eve-port --binary pe-native-tests`
  (regenerations of f91561717, 249e76f38, bfdf402af and room batch 5): PASS relative to the
  allowlist, 1421 entries, unchanged. This is not a public-binary clearance.
- Not run on the devbox, by rule:
  - the Day-1 route regression;
  - the movies-on Day-2 check;
  - any game run;
  - the owner device gate.

## Open

- The 28 remaining no-C room bodies (m0318i 8, m0432i 9, m0429i 8, m0404i 1,
  m0256i 1, m0273i 1). The room workflow (wf_9660d38d) is working through them.
- Intra-overlay boundaries in generated subsystem-overlay TUs. These calls go
  to a non-emitted leaf of the same overlay that only a hand port implements:
  - ovl_03D2 `80191B64`, from 801924F8 and 80192934;
  - ovl_03C5 `80121270`, from 80121C04 and 80122040;
  - ovl_0700 `8018F344`, `80193B5C` and `80197BA0`.

  They are dormant on the native path today:
  - The ovl_03D2 chain is reached in retail from the title loop
    (`func_801909B4`, jal 80190660 at 80190D74 and 80192CE8 at 80190D84).
    The port runs the title loop as a hand port, which calls the hand-ported
    `func_80190660` and no `func_80192CE8` at all.
  - ovl_03C5 80121C04/80122040 are reached from the EXE (jal at 80014FA4 and
    8003F520), whose ports call the hand-ported movie player by name.
  - The ovl_0700 TUs have no generated caller.

  If any of these generated TUs is ever dispatched through the registry, the
  boundary would stop it.
- 40 swept TUs ran 0 seeds in sweep 5 ("skipped"): every one of the 6
  default seeds faulted in the oracle or spun on a mocked callee. 34 are
  room bodies, also skipped in sweep 4. `E21_FAULTS=1` prints why each seed
  was dropped. The earlier reading here, that the seeds fault on the room's
  own data, does not hold for m0348i 8018F240. Its loads chase pointers
  through the argument (`lw v1,8(a0)` then `lw v1,0x238(v1)`) and through
  mocked returns (`lw v0,0(v0)` after `jal 800C2B28`). A small seeded word
  there (`0000023A` = 2 + 0x238) faults, and a chain of about six loads
  survives about one random image in eight. The opt-in seed extension
  (`E21_MIN_OK=4`, 8c6a2fbf7) alone gives `ok, 4 seeds (seed range extended
  to 21), retail coverage 200/200 words`. A sweep of all 40 with
  `--min-ok 4 --max-seeds 512` is running. Bodies that still run no seed need
  a directed spec, as `func_8006B4F8` (the room loader) got: with
  `r(0x8006E6A8)=0;r(0x8006E7E8)=0` it runs 7 seeds, 267/540 words, ok.
- `func_800339A0` with style >= 4 stays a loud boundary. Retail reads the
  stack above a 16-byte local there.
- The retail stack-residue residuals and m0023i's `[DAY1_SHIM]` (above).
- A directed retail differential of the Items/Equipment paths
  (`func_80057834`, `func_800466C0`, `func_8005A318`).
- m0005i: hand ports or generated TUs. The hand ports that the effect
  dispatcher binds for m0005i's own bytes (8018F018, 8018F330, 8018FB84)
  have retail-case tests generated from the original Disc 1 code:
  ATK25 charge (`retail_eve_charge_cases.h`), ATK26 flare and ATK29 beam.
  The generated TUs are the e21-checked alternative. No change is needed
  for native coverage; switching is a cleanup choice.
