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
Registry `zz_dispatch.c`: 5771 thunks; 5775 after the four EXE callbacks of "Dangling callbacks" (regen24; the room census is unchanged). 185 code rooms, 9103 (room x body)
rows, 2952 unique (vma, size, FNV) bodies.

| | session start | 2026-10-18 13:10 |
|---|--:|--:|
| native-generated bodies | 8855 | 9101 (87 via a data head) |
| not-native bodies (all statically reachable) | 217 | 0 |
| direct-addr-guarded (hand port bound by address only) | 31 | 0 |
| native-direct (hand port bound by fingerprint) | 0 | 2 |
| unique not-native bodies | 136 (183,492 bytes) | 0 |

A body resolves only by fingerprint, as `PE_GuestCode_Resolve` does. Rooms are
never declared resident in production, and closure-equal duplicates count once.

"0 not-native" means that every room body has compiled host code. It does not
mean that no room body can stop: a native body can still call something with
no native code. The known case was room_m0017i `func_8018F0F0`, which called
`func_8003A088` through a generated boundary. The port had that function only
under a cut name, and the leaf's GTE asm does not lower. Since regen25 it is a
direct call; see "Dangling callbacks". The 38 remaining generated boundary
sites (`boundary_site_census.py --generated`) are the dormant ones listed under
"Generated-TU boundaries".

No body is left without C. The last 47 were effect and particle callbacks of the
late rooms. 4dba7922 covered m0429i 801982A4 and the room batches 1-12 the other
46. All are NON_MATCHING, from workflows wf_441734e8 and wf_9660d38d: each leaf
was decompiled by one agent, confirmed by an independent verifier (or, for
m0318i 80196E4C, by the main session after a harness fix) and re-proved by
the main session.

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
Batch 6 (m0432i 8019702C, m0429i 80196748/801972EC/80196CCC, m0318i
80197CBC/801944E8) took it from 28 to 22.
Batch 7 (m0429i 801960C4, m0318i 80198268/80197618/80194AAC, m0432i
8019485C) took it from 22 to 17.
Batch 8 (m0432i 80193940/80195B40, m0318i 80195D20/80196E4C, m0273i
80194E6C) took it from 17 to 12.
Batch 9 (m0432i 8019408C/80194F60/80196554, m0429i 80198914/801978EC,
m0318i 801988F8) took it from 12 to 6.
Batch 10 (m0432i 80198348/8019753C, m0429i 8019549C) took it from 6 to 3.
Batch 11 (m0404i 80192540) took it from 3 to 2.
Batch 12 (m0256i 801940B0, m0429i 80192F9C) took it from 2 to 0.

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
  - Erratum: until f452c3594 (E23) the credits frame flip, op 6 of table 1,
    passed PutDispEnv/DrawOTagEnv byte values instead of env addresses. No
    credits test covered op 6. See the E23 entry below and the erratum in
    the credits report.
  - Erratum 2: the credits' CD-XA arms spin forever on every provider. Since
    e27fc416a, main's arm stops on `credits_cd_xa_service` before entering, as it did
    before this branch (see "Reachable gap: the ending credits need CD-XA").

Dangling callbacks (see "Dangling callbacks"):

- **d3e872a2a**: the item-list update 0x800494AC (E21, after
  `guest_lower.LOWER_REASONS` gained "address of function") and the shop
  update 0x8004A9A0 (NON_MATCHING: retail's `$a1 = D_8009CFDC` to
  `func_8005BBE4` made explicit). Both used to be `PE_MenuInputCallback`
  stops; `PV_494AC_item_list_confirm_runs_native` and
  `PV_4A9A0_slot_detail_input_runs_native` showed the stops and now pass.
- **f1a1b1c74**: the rising-sparks particle callback 0x800DD9E4 and field
  effect type 16's main 0x800DB6BC, both NON_MATCHING with
  lowering-only type edits. Both used to stop in `PE_EffectCallback`. The e21
  harness gained `func_800D1DEC`'s argument extents (pos 6 bytes, col 3
  bytes, from its retail reads).

Host providers:

- **54fff9664, DsRead busy word.** The synchronous host read `func_8006E6D4`
  (`platform/pe_libcd.c`) wrote 1 to `0x8009B6D4` as "retail post-issue
  state". Retail `func_80080E34` stores that 1 at `0x8009B6CC` (`li v0,1` /
  `sw v0,8(s0)` at 0x80080F38, with `s0 = 0x8009B6C4` from 0x80080F10).
  Both completion paths clear it in `func_80081268`, inside the same
  callback that drops the pending count `D_8009B6B4` to 0. No retail EXE
  code references `0x8009B6D4` (no `-18732` displacement in the
  disassembly), and no overlay assembly references any `D_8009B6xx`.
  - The completed host read now writes `0x8009B6CC = 0` and leaves
    `0x8009B6D4` alone.
  - The five tests that asserted the stray write now assert the busy word.
  - Effect today: none observable. Nothing read `0x8009B6D4`, and
    `0x8009B6CC` was already 0. The write was a transcription of the first
    `s0` base (`0x8009B6CC`, used for the -32/-28/-24 stores) applied to the
    final `sw v0,8(s0)`, after `s0` had been reloaded.

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

Sweep 6 (2026-10-18 08:44-09:37 UTC, regen14/15 tree): the native side built
with the shipped flags (`E21_CFLAGS="-O3 -fno-strict-aliasing -fwrapv"
E21_GTE_CFLAGS=-O3`) instead of -O1:

    e21_diff: MISMATCH: 8
    e21_diff: ok: 1983
    e21_diff: skipped: 36

No nocompile and no error. The 8 mismatches are the stack-residue set (the
same TU names as sweep 5). The 36 skipped are from the 40 above; they run
only with the seed extension or their directed specs. The first sweep-6
attempt crashed after 62 minutes on the seed-extension name clash, fixed in
9f64651f3.

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

### Verbatim room TUs against retail (`tools/analysis/e21_verbatim.py`)

The new harness compiles a verbatim TU the way production does: real
pc_port headers, `-O3 -fno-strict-aliasing -fwrapv -funsigned-char`. It
links the TU against:
- `PE_Translate` and friends over its own RAM;
- a logging mock for every undefined `func_X` callee, built from its
  `gcc -aux-info` prototype;
- the native GTE.

Retail runs in e21_diff's oracle on the same seeded RAM, and the results are
compared with e21_diff's rules. A callee whose host prototype flips the
leaf's 32-bit return signedness gets mock returns with bit 31 clear, the
range where both types agree. The result names it. For example,
`func_800C6CE0` is `unsigned int` in the leaves and `int` in
pe_port_compat.h; it returns 0..5.

All 1016 verbatim room TUs (2026-10-18 07:16-07:45 UTC, regen14 tree):

    e21_verbatim.py --seeds 6 --jobs 3 --check-gte --verbose
    e21_verbatim: MISMATCH: 3
    e21_verbatim: ok: 1004
    e21_verbatim: skipped: 9

The three mismatches were each read. All three are latent, with no
divergence for the values the game passes:

- m0429i `80192B10`, seed 3: `call 8001AA78 a0 00000000 vs 80000000`. The
  seeded actor pointer `D_8009D254` was NULL. `PE_DECOMP_PTRGLOBAL`
  translates guest 0 to RAM's base, so the TU's writes through it land on
  the same physical words retail writes (KUSEG 0x1D8...). The round trip
  `PE_HostToGuest(d)` hands the callee the KSEG0 alias 0x80000000 of that
  NULL. `func_8001AA78` dereferences it at once (`o->f98`), the same physical
  RAM.
- m0418i `80193480`, seed 0: `RAM 0x8015BEA8 retail 00 native 01`. The leaf
  declares `int func_80071A54()` (rand); pe_port_compat.h declares it
  `unsigned int`. `func_80071A54() % 256 < t` is a signed compare in retail
  and an unsigned one in the TU. A negative threshold `t` (a random word
  here) stores 1 instead of 0. `t` is a 0..255 percentage in play.
- m0432i `80199048`, seed 4: `v0 801994F4 vs 001994F4`. The pointer-return
  adapter moves the result into the segment of the root pointer
  `D_8009D254` (`PE_DecompReturnSegment`). This leaf returns the constant
  `&D_801994F4`, which retail returns as 0x801994F4 whatever the root's
  segment. They differ only when the actor pointer is a KUSEG address; the
  game's is KSEG0.

The 9 skipped TUs (`func_8018FDF8`, `801906EC`, `80190A7C`, `80190E98`,
`801929F8`, `801932EC`, `80193F70`, `801942F0`, `80194440`) are one family:
the slot setter `Slot *p = &D_X[a1]`. A seeded pointer-sized `a1` puts every
store outside RAM, so they ran no seed even with 96 (`E21_FAULTS=1`: "bad
address 81xxxxxx" on every seed). With the directed specs `a0=0,1; a1=0,1,2,3`
in nonmatching.yaml (a0 up to 4 for 801906EC's switch):

    e21_verbatim.py --set rooms --seeds 24 --jobs 3 --check-gte --verbose --only <the 9>
    e21_verbatim: ok: 9          (24 seeds each)

So every verbatim room TU now runs against retail. The only room mismatch
left is the m0429i 80192B10 NULL alias, a harness artifact.

The EXE's and the subsystem overlays' verbatim TUs (1013 + 41,
2026-10-18 07:49-08:36 UTC, same tree; run with the bit-31 clamp then on):

    e21_verbatim.py --set exe,ovl --seeds 6 --jobs 2 --check-gte --verbose
    e21_verbatim: MISMATCH: 17
    e21_verbatim: ok: 902
    e21_verbatim: skipped: 78
    e21_verbatim: unsupported: 57

Every mismatch was read. None diverges for the values the game passes:

- 4, NULL alias (`a0 00000000 vs 80000000`): 80018004, 80018164, 80020D50,
  8002F300. These are the 80192B10 case above.
- 12, 32-bit address wrap: retail's address arithmetic wraps mod 2^32 and the
  host pointer model does not. Nine native SIGSEGVs (800428D4, 80047040,
  80050AD8, 80050F10, 8005DC28, 80065D44, 80065DCC, 80076B44) and three
  `PE_HostToGuest` aborts (80018A9C, 800C2B10, 800C2B28). Two examples:
  - 80076B44 `D_800A3348[a0]` with a seeded a0 of 0x80123456. In retail that
    is KUSEG 0x001C679E, inside RAM. In the port it is the host array
    pointer plus 0x80123456.
  - 80065D44 `q += *(unsigned int *)(p + 0x1C)` with a seeded "negative"
    offset.

  In play these are small table indices and positive block offsets. One is
  plausible in play: `func_800428D4` writes record `D_800A1860 - 1` (an
  unsigned index). It is a menu callback (`func_80050580` registers it with
  `func_80042B50`). If it ever ran with `D_800A1860 == 0`, retail would write
  the record before the table (0x800A0ABD) and the port would fault.
- 1, narrow return (800184EC): the leaf declares `signed char
  func_8002FAF8()`, the host `int`. The real callee returns 0, 1 or 2.
- 1, out-of-range local table index (800323C8): a seeded `signed char a0`
  indexes the 18-byte local tables beyond their end, so each side reads its
  own stack. In play a0 is 0..8.

57 TUs were unsupported:
- 20 call a loud boundary (next item);
- 10 call the host shims `HostFB_VSync` / `HostFB_SetDispMask`;
- the rest declare callees with TU-local types (S64, RECT), which the
  generated mocks did not see.

The harness now handles the last two groups. The mock file includes the real
headers plus the TU's own typedefs, and the headers' one-line inline
wrappers (`func_80073A44(m) { return HostFB_VSync(m); }`) map a host call
back to its retail address. Re-running the 57:

    e21_verbatim: MISMATCH: 1     (ovl_0457 801247F4: the E23 bug above)
    e21_verbatim: ok: 20
    e21_verbatim: skipped: 1
    e21_verbatim: unsupported: 35

The 35 left are the 20 dormant boundary TUs and 15 whose callee or entry
takes a struct by value (`S64`), which register mocks cannot express.

The 78 skipped TUs need directed specs. Narrow returns, parameter-width
drift (a K&R leaf passing a full `int` where the host prototype narrows to
`short`; both harnesses mask narrow parameters to their width) and
out-of-range indices are not closed by any generator change. They are
recorded here as residuals.

**E23: multi-dimensional array externs (live bug in the native credits).**
ovl_0457 `func_801247F4` is the credits frame flip, entry 6 of op table 1
at 0x80125B48 (`... 80124704, 801247F4, 80124034 ...`). The credits main loop
dispatches it through the registry every frame of that table. Its leaf
declares `extern unsigned char D_800BCE80[][20]` (DISPENV rows) and
`D_800BCDC8[][92]` (DRAWENV rows). Retail passes the row addresses,
0x800BCE80 + 20k and 0x800BCDC8 + 92k, to PutDispEnv (`func_800755F0`) and
DrawOTagEnv (`func_800754E4`). The verbatim lowering flattened both to the
one-dimensional `PE_DECOMP_ARRAY(addr, unsigned char)`, so `D_800BCDC8[k]`
was the byte at offset k. The object code shows it
(`movzbl (%rbx,%rax,1),%edi` before `call func_800755F0`): both env
arguments were byte values, which are addresses in the low-RAM vector area.
This was found by reading the one verbatim mismatch in ovl_0457 (a native
SIGSEGV on a seeded frame index). Rule E23 now refuses verbatim lowering
for a multi-dimensional array extern, and such a leaf takes the E21
lowering, which computes guest addresses. No other verbatim TU declares
one: a scan of every verbatim TU's leaf source finds only these two
externs. ovl_0457 `func_80123CAC` (`D_80125B1C[][4]`) was already E21.
`gen_overlay_ports.py --overlay ovl_0457 --verify` reports exactly this one
TU drifting.

**E22 call-site casts (generator fix, regen15).** The generator replaces a
leaf's own callee declaration with the host prototype. Where the two 32-bit
return types differ only in signedness, every value-used call is now cast
back to the leaf's type (`((unsigned int)/* E22 leaf type */func_800C6CE0(o))`).
A callee the leaf calls without a declaration counts as C89 implicit `int`.
Counted from regen16's markers, regen15 put casts in 47 EXE TUs, 55 room TUs
and 1 ovl_0457 TU. One more EXE TU, `func_80077294`, got only the note: its
one call is a statement, so nothing was cast. The E22 commit says "48 EXE
and 56 room, casts only"; these are the exact figures. The note is now
written only when a cast is inserted. Beyond these, regen15 changed the 5
new batch-7 TUs and one segment-adapter fix. Cases:
- the rand `func_80071A54` (`int` in the leaves, `unsigned int` on the host);
- `func_800C6CE0` (`unsigned int` / `int`);
- `func_8003708C` (`int` / `uint32_t`);
- `func_80062A20` (`int` / `pe_addr_t`).

The pointer-return adapter no longer moves a constant `&D_X` result into a
root pointer's segment (m0432i 80199048). The inline `/* E22 leaf type */`
marker was added after regen15, whose TUs carry the same casts without it.

The check: all three verbatim sets on the regen15 tree, with the bit-31 clamp
off (2026-10-18 09:16-10:16 UTC):

    e21_verbatim.py --set rooms,exe,ovl --seeds 6 --jobs 3 --check-gte --verbose
    e21_verbatim: MISMATCH: 18
    e21_verbatim: ok: 1908
    e21_verbatim: skipped: 87
    e21_verbatim: unsupported: 57

The clamped runs before E22 gave 1906 ok and 20 MISMATCH. m0418i
80193480 (the rand compare) and m0432i 80199048 (the segment adapter) now
pass. m0065i 8018F054 and the other func_800C6CE0 callers pass with
unclamped returns. No new mismatch appeared; the 18 left are the latent
classes listed above.

**Generated-TU boundaries.** The verbatim path does not apply host
replacements. 21 generated EXE TUs bind a callee with no host C to a loud
boundary, `#define func_X(...) PE_D_COMP_BOUNDARYn("func_X", ...)`: printf
(`func_80071A74`) in 6 of them, and libspu/libcd-band internals
(`func_8007E1F4`, `8007E22C`, `8007BDDC`, ...) in the rest.
`boundary_site_census.py` skipped `#define` lines and so missed them; it now
counts each binding. `--generated` gave 39 sites (it was 8): the 8 overlay
sites above plus 31 bindings in these 21 TUs. Since regen25 it gives 38:
room_m0017i `func_8018F0F0`'s call of `func_8003A088` is now a direct call
(see "Dangling callbacks"), and the other 7 overlay sites are the dormant
intra-overlay boundaries in Open. None of the 21 is reached:
- a reverse call-graph walk over pc_port finds no hand, platform or src
  caller, only other generated Psy-Q-band TUs that nothing reaches;
- the EXE and the 515 PE.IMG blobs hold no aligned word with any of their
  addresses and no `lui` + `addiu`/`ori` pair building one.

In retail they are library-internal jal chains, which the port replaces with
its hand ports and `PE_` models.

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

**Full triage of all 228 sites** (workflow wf_8fa94741, 2026-10-18 13:35-17:10
UTC):
- Ten classifier agents took 23 sites each. Each one read the site, the
  condition that reaches it and the callers.
- Every site a classifier called reachable or unknown then went to an
  independent verifier, 69 checks in all.
- One extra row is the install site of a callback whose stop is another
  row, so there are 229 rows.

| bucket | rows |
|---|--:|
| dormant (no production path sets the condition) | 70 |
| error-defensive (retail error or trap path, or a state shipped data never produces) | 68 |
| test-fixture (only a test or a test-only mode reaches it) | 44 |
| unknown | 41 |
| no disc or device | 3 |
| reachable-unported | 3 |

Verifier verdicts: 36 not reachable, 3 reachable-unported, 30 unknown.

The 3 reachable rows are two defects, both fixed today:
- `cd_stream_port.c:927` (`CD_command_wait`), the credits' CD-XA arms. The
  credits now stop before entering (e27fc416a); see "Reachable gap".
- `func_80063E0C_port.c:54` (`PE_MenuInputCallback`), plus its install row
  `port6_port.c:52`: the item-list and shop callbacks 0x800494AC and
  0x8004A9A0, now native (d3e872a2a); see "Dangling callbacks".

The 41 unknown rows break down as follows:
- 20 are sinks that other sites call: the `pe_bootstrap.c` helpers,
  `PE_Decomp_BoundaryFrom`, the `guest_call` wrappers and the
  `PE_D_COMP_BOUNDARYn` macro bodies. Their reachability is the union of
  their callers'.
- 15 were found not reachable by the verifiers. Examples are the
  `func_8003A088` stack guards, the MDEC waits and the `func_800638D8` /
  `func_8005C498` wrappers.
- 6 remain unknown.

Those 6, plus one dormant row whose verifier said unknown, are seven specific
conditions that nobody could confirm or exclude:
- `func_80028574_port.c:81`: a shot count (`wf & 15`) of 11 or more, past the
  scales table;
- `actor_contact_port.c:183`: an actor +0x194 contact callback that neither
  resolves nor is 0x8001D268;
- `absent_ovl_port.c:77`: libgte `func_80079178`, if `func_800C65E4` runs;
- `func_80075358_port.c:40`: a GP0 word that `PE_GPU_WriteGP0` refuses;
- `func_8007512C_port.c:61` and `:95`: a draw node over 15 words, or a GP0
  opcode the packet tables cannot size;
- `func_80041108_port.c:32`: the card-operation frontier, used only in the
  test-only boundary mode and by the legacy frame functions.

Per-site conditions and evidence are in the workflow journal
(`subagents/workflows/wf_8fa94741-8da/journal.jsonl` in the session
directory).

`pc_port_coverage.py`'s "stub" bucket (36 leaves) is not a list of non-native
code. It counts any body that has a bootstrap marker on any path, and most of
those are hand ports with one guarded error path.

## Reachable gap: the ending credits need CD-XA

The credits loop (`func_801235DC`, ovl_0457) drives the drive itself. main's
arm (state `0xAA108448` with `D_800B0CD8.fF5` bit 1) enters it in two modes:

- **`D_8009D1A0` bit 16 clear (the Disc 2 ending).**
  - `func_8012562C` opens `\XASTREAM\CREDITS.XA;1`. Only Disc 2 has it (LBA
    204531, 144080896 bytes); Disc 1 has no XASTREAM directory.
  - `func_80125744` queues CdlSetmode 0xC8 and CdlSetfilter (file 1,
    channel 0) through `func_8007EE84`, and spins on `func_8007F418` until
    each completes.
  - It then registers two callbacks:
    - `func_80125980`, the data-ready callback: CdGetSector into the ring at
      `D_80172D00`.
    - `func_80125A40`, the VSync callback, every 32 frames: CdlGetlocP
      (0x11), whose callback `func_80125AF0` stores the position in
      `D_80172CF0`; or a CdlReadS restart through `func_801258EC` ->
      `func_8007F0C8`; or, once the position passes the end,
      `D_80172CE4 = 1`. That store is the only way this mode ends.
- **Bit 16 set (the second op table, after the nine-step pad code).** There
  is no XA setup. An edge on `D_8009D1F4` bit 26 or 27 calls
  `func_80125894(0 or 1)`, which spins until `func_8007F418(0, 0) == 2` and
  then sends CdlSetfilter through `func_80080DC4`.

No CD provider serves any of this:
- With the device disabled (production), nothing publishes a completion.
  Only `func_8007EB88` on the IRQ chain writes the ring at `D_800A3610`.
- Nothing decodes a queued XA read.
- The register-level device (test-only) rejects commands 8, 13 and 0x11
  (`CD_device_unported_command`).
- The generated ovl_0457 loops never test `PE_Port_ShouldStop`.

`credits_xa_commands_never_complete_without_device` shows it on the retail
EXE image, with the device disabled, after CdInit and CdReset:
- CdlSetmode is queued and kicked (`D_800A3608 = 1`, no stop);
- 100000 polls of `func_8007F418(seq)` all return 0;
- lane `D_8009B574` stays 2, so `func_8007F72C()` returns 2;
- `func_8007F418(0, 0)` returns 6.

So `func_80125744` would spin silently without presenting a frame, and so
would the channel switch.

This made the native credits of f91561717 a regression in failure mode. The
arm used to be a clean CPU_BOUNDARY stop, and it became a hang.

**The fix (e27fc416a).** main's arm now goes through `PE_Main_EnterCredits`
(`bootstrap/func_8001220C_port.c`). While `PE_Cd_XaServiceAvailable()`
(`platform/pe_libcd.c`) returns 0, it stops on the `credits_cd_xa_service`
boundary, whose argument is `D_8009D1A0 & 0x30000`.
`credits_entry_stops_without_xa_service` checks the four mode values 0,
0x10000, 0x12000 and 0x20000:
- the call returns 0;
- the stop is an unresolved-boundary stop;
- the boundary name and the mode bits are recorded;
- the loop's first store (`0x80172CD0`) is never made;
- the libcd queue is untouched.

The generated credits TUs, the libpress replacements, the E23 frame flip and
their tests all stay.

**What closes the gap** is a CD-XA provider. That is a feature milestone, not
a fix. It needs:
- CdlSetmode and CdlSetfilter completions;
- CdlReadS in XA mode at 2x, with the file/channel filter, into the SPU CD
  input (`PE_Spu_CdSector` already decodes XA for the movie feeder);
- CdlGetlocP positions;
- data-ready sectors;
- CdlStop and CdlPause;
- Disc 2 in the drive at the ending.

Verifying its end-of-music timing needs a game run.

The device model's Setfilter arm is dead code. `pe_cdreg.c` stores the
filter when command 13 completes, but the accept list omits 13. Its two
parameters would also fail the parameter-count check, which allows only
Setloc's 3 and Setmode's 1. The device model is test-only, so this is
documented and not fixed.

### Generated CD waits (a class the boundary census misses)

A busy-wait on CD state that production never advances has no boundary site.
The table covers the generated TUs that call the libcd queue or status
(`func_8007EE84`, `func_8007F0C8`, `func_8007F418`, `func_8007F72C`,
`func_8007F778`, `func_80080DC4`, `func_80080D5C`), found by grep over
`pc_port/game/decomp*`. No room TU calls any of them.

| TUs | what they wait on | reachable in production |
|---|---|---|
| ovl_0457 80125744, 8012562C, 80125894, 801255A4, 80125980, 80125A40, 801258EC | queue completions, lane 1 | only through `func_801235DC`, now gated |
| ovl_03C5 80121C04, 80122040, 80122354 | lane 1, `func_80080DC4` / `func_80080D5C` | no: the EXE ports call the hand-ported movie player by name (`game/boot/movie_overlay_port.c`) |
| ovl_03D2 801924F8, 80192934, 80192C48, 80192CE8 | the same | no: the title loop is a hand port (see Open) |
| EXE `func_8006A5BC` | `func_8007ED58() != 1`, then `func_8007F72C() != 1`, each polling VSync | yes, and it ends: the host CdReset completes synchronously and sets lane 1 |
| EXE `func_80081DF8` | queues CdlPause raw when `D_8009B70C == 1` | the flag is never set (see below) |
| EXE `func_80081268` | queues CdlPause raw when `D_8009B6CC == 1` | the flag is never set: only `func_80080E34` sets it, and production's `func_8006E6D4` reads synchronously instead |
| EXE `func_80080D34` (CdControlF) | queues any command | no direct caller; only reachable through the registry |

`D_8009B70C` is set only by `func_80081D74`. That is called only from the
`func_80080F64` and `func_80082400` callbacks, which are registered only by
raw-queue reads (`func_80080E34`, `func_80082314`). Production replaces both
reads with host reads.

So the credits were the only reachable case. One raw-queue command in
production latches lane 2 for good, because only the IRQ chain or a CdReset
clears it. Every later `func_8007F72C() == 1` wait would then spin too, which
is why the gate stops before the first such command.

## Dangling callbacks (another class the boundary census misses)

A code address that some code or data stores, but that neither resolves
through the generated registry nor matches an arm of a hand dispatcher, stops
the game when it is called. Under the default stop policy, `menu_callback`,
`PE_EffectCallback`, `cd_icall` and `PE_GuestCall` all end in a boundary. The
boundary census does not see these: the stop is in the generic dispatcher,
far from the store.

`python3 tools/analysis/dangling_callback_census.py` scans for EXE function
starts (`configs/USA/disc1.yaml`) in three places:
- the hex literals of production C;
- every word of the retail EXE image;
- every word of the 515 PE.IMG overlay blobs.

It reports each start with no fnv-0 registry row and no `case`/`==` arm. A
literal inside a boundary call is reported as a boundary argument (the call is
already a boundary site), and a literal after `!=` as a comparison.

Before today's fixes it found 7 stored callbacks with no native code, one
loop sentinel, 9 boundary arguments, and one word in the EXE data:

| address | what it is | how production reached it | now |
|---|---|---|---|
| 0x800494AC | 0x0D item-list update (`func_80048918`, 12 menu paths incl. field-VM `func_8004DCA4`) | Confirm/Cancel/unhandled keys -> `menu_callback` -> stop | native, E21 (d3e872a2a) |
| 0x8004A9A0 | shop window update (`func_8004A570`, from the equip-slot list's Confirm) | first key press -> stop | native, NON_MATCHING (d3e872a2a) |
| 0x800DD9E4 | rising-sparks particle callback (`func_800DDD70` pool) | first pool tick -> `PE_EffectCallback` -> stop | native, NON_MATCHING (f1a1b1c74) |
| 0x800DB6BC | field effect type 16 main (EXE data 0x800E1CF8, node 16 of `D_800E1044`) | command code 0x55 with type 16 -> `PE_EffectCallback` -> stop | native, NON_MATCHING (f1a1b1c74) |
| 0x80083944, 0x800846AC, 0x800847B0, 0x8008486C | Psy-Q libpad/libcard hooks: card record +0x18 (`func_800837C8`) and slots `D_8009B724/72C/730` (`func_800844E4`, installed at boot) | only through the libcard driver hand ports in `absent_cd_port.c` (`cd_icall`) | open: Psy-Q, so they cannot be `func_<addr>` definitions; production saves through the host memory card (`PE_Memcard_*` in `func_80041108_port.c`), and whether the libcard driver hooks ever run in production is not established |

After f1a1b1c74 the census reported 4 stored, 9 boundary arguments and
1 comparison. The ninth boundary argument was room_m0017i `func_8018F0F0`'s
call of `func_8003A088`. The model-matrix walk has been native all along under
the name `func_8003A088_mode0_walk_cut`: despite the name it follows every
`+0x28` mode of the leaf, and production's per-actor update `func_80015240`
calls it. It now has its own entry, `func_8003A088`
(`game/boot/func_8003A088_port.c`). After regen25 the room TU calls it
directly and the registry has its row (5776 thunks). The room TU's e21 at the
shipped flags gives 32 seeds and 45/45 words.

    dangling_callback_census: 4 stored unresolved, 8 boundary arguments, 1 comparisons

The 4 are the libcard hooks. The boundary arguments are the Psy-Q-band
generated TUs, `func_80079178`, `func_80080E34` and the card frontier. The
comparison is `port6_port.c`'s `0x8007E3B4` loop sentinel.

`PE_EffectCallback` has a `[DAY1_SHIM]` arm that skips an unresolved effect
callback instead of stopping. It runs only under `PE_BOUNDARY_POLICY=record`,
the diagnostic policy, so production stops. m0023i stays the one shim that
substitutes silently in production.

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

Final tree (regen20, HEAD 99fad1188 and later doc commits), 2026-10-18:

- Generator drift: `gen_decomp_ports.py --e21 --verify` reports 1046 TUs up to
  date, 0 drifted. `gen_overlay_ports.py --verify` reports rooms 2951,
  ovl_0700 45, ovl_0457 27, ovl_03D2 26, ovl_03C9 18 and ovl_03C5 8, all with
  0 drifted, and the dispatch registry up to date. So the built tree is what
  HEAD generates.
- Sweep 7, every e21 candidate with the shipped flags
  (`E21_CFLAGS="-O3 -fno-strict-aliasing -fwrapv" E21_GTE_CFLAGS=-O3`,
  `--min-ok 4 --max-seeds 256`, 13:16-14:54 UTC):

      e21_diff: MISMATCH: 8      the stack-residue set
      e21_diff: ok: 2037
      e21_diff: skipped: 5       ovl_0700 8018F55C/80191C94, ovl_03D2 8018F958/8018FBC0/80192CE8 (dormant)

  No nocompile and no error. This includes every NON_MATCHING TU of room
  batches 1-12 and the E23 credits TU.
- `scripts/run_native_oracle_harnesses.sh`: font 54, animation 117, field VM
  693, all PASS with zero skipped (NATIVE_ORACLE_HARNESSES=PASS). It builds
  its own tests at -O2.
- Registry: no overlay entry at fingerprint 0. The 2164 fnv-0 rows are
  always-resident EXE functions, bound by address.
- Not run (owed through the runner before anything is packaged; the shipped
  code changed in this session: decomp compile flags, ILP32 `long`, E22
  casts, the E23 credits frame flip, 47 NON_MATCHING room bodies):
  - the Day-1 route regression (`day1_route_regress.sh`);
  - the movies-on Day-2 check;
  - the owner device gate;
  - any visual check (the credits picture included).

Earlier gates:

- `pe-native-tests` (isolated HOME, Disc 1 configured, built in /dev/shm from
  the regeneration with room batch 12, 2026-10-18 13:08 UTC): 1661/1661, 0
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
  (regenerations of f91561717, 249e76f38, bfdf402af and room batches 5-12): PASS relative to the
  allowlist, 1421 entries, unchanged. This is not a public-binary clearance.
- Not run on the devbox, by rule:
  - the Day-1 route regression;
  - the movies-on Day-2 check;
  - any game run;
  - the owner device gate.

## Open

- The ending credits need a CD-XA provider. Until one exists they stop on
  `credits_cd_xa_service`; see "Reachable gap".
- The four Psy-Q libcard hooks in "Dangling callbacks". As Psy-Q functions
  they need host replacements, not `func_<addr>` definitions, if the libcard
  driver ever runs them in production.
- The seven boundary conditions the triage could not settle (listed under
  "Boundary-site census").

- No room body is without C any more (census 0). The NON_MATCHING bodies are
  behaviour C proven by the e21 differential, not byte matches. The room workflow (wf_9660d38d) is working through them.
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
  to 21), retail coverage 200/200 words`.

  Sweep of all 40 (2026-10-18 06:00-07:13 UTC, on the regen12 tree with
  batch 5 in the working tree):

      e21_diff.py --with-calls --check-gte --coverage --verbose --jobs 3 --min-ok 4 --max-seeds 512 --only <the 40>
      e21_diff: ok: 33
      e21_diff: skipped: 7

  - 0 MISMATCH. 31 of the 33 reached 4 seeds within seeds 0..54.
    `func_8006B4F8` ran on its directed spec (4 seeds, 267/540 words).
    `func_80192560` ran on its r() spec (6 seeds, 48/48 words).
  - Still 0 seeds with 512:
    - ovl_0700 `8018F55C` and `80191C94`;
    - ovl_03D2 `8018F958`, `8018FBC0` and `80192CE8`;
    - m0017i `8018F580`;
    - m0273i `80193114`.

    They need a directed spec, as `func_8006B4F8` (the room loader) got
    (`r(0x8006E6A8)=0;r(0x8006E7E8)=0`). ovl_03D2's three are title-loop
    code, which the port runs as hand ports (see the intra-overlay
    boundaries above).
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
