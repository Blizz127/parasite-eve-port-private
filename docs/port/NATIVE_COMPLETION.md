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
    before this branch (see "The ending credits: CD-XA").
  - Since 8d5aaa6ce and 4637916e3 and the production-wiring commit after
    them, main's arm attaches the register-level CD device, which now serves
    Setfilter, XA routing, GetlocP and Stop, and runs the credits under the
    stop escape. Since the host-read ring model, the pad-code mode runs too.

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

Sweep 8 (every candidate at the shipped flags, `--coverage --min-ok 4
--max-seeds 256`, tree at 18:37:58Z, 18:37-20:14 UTC): ok 2041, MISMATCH 8,
skipped 5. The 8 are the stack-residue set again.

**The 8 stack-residue mismatches are a harness artefact (2026-10-18).** The
mismatches came from where each side's frame sat:
- The oracle enters retail with `sp = SP0` (0x801FFF00), so retail's locals
  sit at `SP0 - N + X`, with N taken from the prologue's `addiu $sp,$sp,-N`.
- The native harness allocated guest frames from 0x801FEC00.
- So both sides read uninitialized locals, but at two addresses holding
  different seeded bytes.

The lowered frame is retail's frame minus the 16-byte argument home area. In
`func_8018F388` retail has N = 144 and its matrices at sp+40/sp+72, and the
lowered TU has them at pe_fp+24/+56. So pe_fp corresponds to retail sp+16.

`E21_STACK_COINCIDE=1` (opt-in, `e21_diff.py`) places the native top frame at
`SP0 - N + 16` and widens the native stack-pointer window to match:

    E21_STACK_COINCIDE=1 E21_CFLAGS="-O3 -fno-strict-aliasing -fwrapv" E21_GTE_CFLAGS=-O3 \
      e21_diff.py --with-calls --check-gte --coverage --verbose --jobs 2 --min-ok 4 --max-seeds 256 \
      --only <the 8>
    e21_diff: ok: 8      (each 4 seeds; retail coverage 296/296 or 255/255 words)

Without the mode the same 8 still mismatch. The native code therefore reads
the same uninitialized frame offsets as retail. What remains is the
production question in "Known residuals": what the guest stack holds there,
which depends on what ran before.

The mode is not the default yet. Two things are owed before switching:
- `guest_lower.py` should record each lowered TU's retail stack-pointer
  offset. Today `e21_diff.py` infers `+16`, which holds only for frames with
  the 16-byte home area (every one of the 8 has it).
- A full sweep with the mode on. That is a heavy job, so it waits for the
  coordinator's heavy-job turn (PS4169 held it on 2026-10-19).

Prepared since then for coordinator task c447185feb53 (not applied to the
branch): `/dev/shm/pe-coincide-prep/RECIPE.md`. Findings:
- The fixed `+16` is wrong for some TUs. Static evidence from room_m0430i
  `func_8018F750`, a matched leaf with a 9-argument call: its locals start at
  sp+40, not sp+36, and its four 4-byte `CVECTOR`s sit at sp+56/64/72/80.
  cc1 2.7.2 therefore rounds the outgoing argument area to 8 and gives every
  struct local an 8-aligned slot.
- The proposal: guest_lower notes `retail sp+K` with
  K = round8(max(16, 4 × the widest call's arguments)). There is no note for
  a leaf, for `__builtin_memcpy`, or when a non-scalar sits off 8 in the
  guest frame; coincide mode then falls back to the default placement.

Focused pilot, coordinator task 77dc538195bd, 2026-10-19 04:36:51-04:38:44
UTC. It ran in a scratch mirror at the proposal's post-pins, on 17 TUs: the 8
residues, one noted EXE TU per K band (16-56) and three no-note fallbacks
(m0430i included).
- Default mode is identical to sweep 8: ok 9, and MISMATCH 8 on the
  residues.
- Coincide mode: ok 17. The residues pass at K=16 on 4 seeds with full
  retail coverage, and the other nine keep their sweep-8 seeds and coverage.
- The run used 3 min 30 s of CPU and a 169 M memory peak.
- This covers those 17 TUs only, not a full sweep. The proposal stays
  unapplied until the coordinator admits that step.

## Hand ports retired

Each of these hand ports was replaced by the generated TU of its matched leaf,
proven by the e21 differential at the shipped flags with directed inputs
(`configs/USA/overlays/nonmatching.yaml` `inputs:`). The generated TU now takes
the hand port's symbol, and the registry row binds it by that symbol. The
hand ports matched the leaves on reading, but no differential had covered
them.

| function | what | e21 (directed) | notes |
|---|---|---|---|
| `func_80057834` | Items -> Use (record kind switch, `D_8009D028` history arm) | 15 seeds, 103/104 words | retail calls `func_800512AC(0, &local)` where the hand port called `PE_MenuCommitResult(v + 3)`; same effect |
| `func_800466C0` | Equipment list window (confirm, cancel) | 11 seeds, 206/255 words | canonical prototype's `event` is `int32_t` now (the leaf's `int key`); the registry thunk's cast followed |

`func_8005A318` (Equipment tool transfer) was already generated. Sweep 8
covered it at 352/825 words. With three directed specs it covers 591/825 in
18 seeds, all matching retail. The specs cover:
- the attribute-slot move;
- the `slot < 0` stat merge and HP spill;
- the `cnt < 999` tail.

The first run with those specs mismatched on `func_80054E4C`'s argument,
which was a harness artefact. The mocked `func_80059A40(&tmp)` did not write
its out-parameter, so each side passed its own stack residue. The fix is the
`out=80059A40:0:4` term.

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
  credits now stop before entering (e27fc416a); see "The ending credits: CD-XA".
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

Those 6, plus one dormant row whose verifier said unknown, were seven
specific conditions that nobody could confirm or exclude. Follow-up work
settled them as follows.

- **Fixed: `func_80041108_port.c:32`, the card-operation frontier.**
  - Its one live caller is `func_8004D5CC`'s default arm, for the
    card-notice slot `D_8009CFFC`. The only value written there outside the
    four handled cases is 0x80042228. `func_80042264` installs it after a
    card CRC match, and it is a generated TU.
  - The arm now makes retail's argument-less call through the registry and
    stops only on an unresolved target.
  - `PV_4D5CC_card_slot_runs_registry_callback` shows that 0x80042228 runs:
    `D_8009D010 = 2` from its `func_800512AC(12)`, the slot is cleared, and
    there is no boundary.
  - The other callers are test-only: `PE_Memcard_BoundaryMode`, and the
    legacy frame functions called only from `test_card_operation_frame.h`.
- **Fixed: `func_8007512C_port.c:61` and `:95`, the draw-chain walker cuts.**
  - psx-spx gives a linked-list node's size as the tag's top byte. Its
    16-word figure is a guess about the FIFO, which would stall, not fail.
    The walker now takes any 0..255-word node.
  - The GPU model now implements the opcodes psx-spx documents that it
    used to refuse:
    - GP0(04h..1Eh, E0h, E7h..EFh) as NOP mirrors;
    - GP0(80h..9Fh) as the 4-word VRAM copy, through the copy
      `PE_GPU_MoveImage` uses but without its DMA-direction precondition;
    - A0h..BFh as A0h, since in groups 4-6 "the remaining 29 bits are
      ignored".
  - GP0(03h), GP0(1Fh) (IRQ1) and F0h..FFh stay cuts: their behaviour is
    undocumented or non-trivial.
  - Tests: `GFX1_gp0_mirrors_and_copy` and
    `DRW1_chain_long_node_and_copy` (a 20-word node of four 81h copies plus
    NOP mirrors).
- **Dormant: `func_80075358_port.c:40`, a GP0 word the GPU model refuses.**
  The opcodes still refused are 03h, 1Fh and F0h..FFh, plus the READ_DATA
  state and A0 sizes outside VRAM. Neither DrawPrim caller sends any of them:
  - **The pause overlay** (`func_8006A0E8`) submits `D_800B0CD8+0x114`, then
    `+0x104`. Only the boot initializer `func_8006A674` writes them
    (`src/func_8006A674.c:134-166`), once:
    - `+0x104` has count 3: GP0(62h), a semi-transparent monochrome 320x224
      rectangle at (0,0) in colour (0x20,0x20,0x20);
    - `+0x114` has count 1: GP0(E1h) `0xE1000440`.

    No other matched leaf writes `D_800B0CD8+0x104..0x11B`. A grep over
    `src/` for the base with those offsets finds only arena strides and
    `+0x11C`. Both opcodes are implemented.
  - **The title loop** (hand port `game/boot/func_80190660_port.c`) sends
    GP0(E1h) `0xE1000018` and a 4-word GP0(64h) textured rectangle
    (`pair[7] = 0x64`, lines 59 and 69). The generated ovl_03D2 TU is not on
    the production path.

  So the refusal stays a guard but is not reachable in production.
- **Dormant: `absent_ovl_port.c:77`.** libgte `func_80079178` is called only
  from `func_800C65E4`, and nothing references `func_800C65E4`. There is no
  C caller, no `jal 0x800C65E4` (`0x0C031979`) in the EXE or any of the
  515 overlay blobs, no stored word, and no `lui 0x800C` + `0x65E4`
  immediate pair.
- **Error-defensive: `actor_contact_port.c:183`.** The verifier enumerated
  the writers of actor +0x194:
  - Aya's record takes `D_800915E0`, which is 0 in the EXE and has no
    other writer;
  - the constant 0x8001D268 is handled;
  - `func_80035038` stores `D_800915DC[desc[0]][1]` from a 13-row table
    whose column 1 is all zero.

  So a stop needs a spawn descriptor with `desc[0]` of 13 or more, which
  would make retail call table-adjacent data too. With the dangling census
  and every room body native, every stored contact callback resolves. The
  caveat: a body whose fingerprint matches in several rooms resolves to
  NULL.
- **Stack residue: `func_80028574_port.c:81`, a shot count of 11 or more.**
  - The matched leaf indexes a local `unsigned char tbl[11]`. Retail keeps it
    at sp+16..26, so indices 11..15 read sp+27..31, which `func_80028574`
    never writes (the stores are `swl/swr` at 16-23 and `sb` at 24/25/26).
  - Valid shot counts are 1, 2, 3, 5, 7 and 10. Whether any weapon record
    reaches 11-15 is not established: a bounded search found no direct
    retail store of the `D_8009D278` block's +0x68 weapon pointer.
  - This is the RotMatrix residue class (KNOWN_DIVERGENCES #17). The port
    stops rather than invent the bytes.

  - **Settled: dormant (2026-10-19).** The count is the low nibble (`lo4`)
    of the equip block's abilities word (`weapon+16`).
    - Its only writer is `func_80051980` (`grep -rn lo4 src/` finds nothing
      else). The word starts at `0x11`, so shots are 1 by default. An
      attribute slot whose ability id `v` is 1-5 sets
      `lo4 = D_800923D0[v]` (`src/func_80051980.c:86-89`).
    - `D_800923D0` is a byte table in the retail EXE, `01 02 03 05 07 0a`.
      So ids 1-5 give 2, 3, 5, 7 and 10, and no other value can be written.
    - The port's `func_80051980_port.c:114-115` reads the same table.
    - The pause menu recomputes the block on its copy at `D_800A76D8`
      (`func_8005C498`, `func_800512AC` modes 2 and 3 -> `func_80051980`) and
      copies it back with the 15 mask (`func_800218D8`).
    - Tool transfers (`func_8005A318`) move attribute bytes and add only the
      `+0xE`/`+0x10`/`+0x12` stats, never the abilities word. The cheats touch
      `+0x0C` only.
    - An offset search finds no other store into the word. Through the weapon
      pointer (`+0x68`) there is one store, `func_800512AC.c:101`, and it
      writes the item id (`lo`, bits 0-9 of `+0x0C`):

          grep -rnE "f68->[a-zA-Z0-9_]+ *(=[^=]|\|=|&=|\+=)|f68\)->" src/

      Every `+0x10` store in the `src/` files that use the weapon pointer
      targets the player record's `f10`, an actor record, field-VM state
      (`D_8009D300`) or effect data:

          for f in $(grep -rlE "f68|\+ 0x68\)" src/ | xargs grep -lE "D_8009D278|D_8009D254"); do
            grep -nE "(\+ 0x10\)|\[4\]|->f10\b|->ab|lo4|abil).*=[^=]" $f | grep -vE "==|!=|<=|>="; done

      In `pc_port/`, the stores through `record+104` write `weapon+10` (the
      item id), not `+16`. A save load restores a block the game wrote.

    So game logic produces only 1, 2, 3, 5, 7 or 10, the six counts the
    damage table defines. 11-15 needs an externally edited or corrupt save.
    The stop stays as the guard for that case.

Per-site conditions and evidence are in the workflow journal
(`subagents/workflows/wf_8fa94741-8da/journal.jsonl` in the session
directory).

`pc_port_coverage.py`'s "stub" bucket (36 leaves) is not a list of non-native
code. It counts any body that has a bootstrap marker on any path, and most of
those are hand ports with one guarded error path.

## The ending credits: CD-XA

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
- The register-level device (test-only) rejected commands 8, 13 and 0x11
  (`CD_device_unported_command`) until the spike below.
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

### CD-XA spike: the register-level device serves the credits (unit level)

The spike takes the register-level device (`platform/pe_cdreg.c`) as the
provider, so the game's own libds/libcd code and the generated ovl_0457 TUs
run unchanged. Production is not changed by it: `PE_Cd_XaServiceAvailable()`
still returns 0 and the device is never attached in production.

Device model changes, each from psx-spx (CDROM chapter):
- **Setfilter (0Dh)** is accepted with its 2 parameters, INT3(stat).
- **XA routing.** "If XA-ADPCM (and/or XA-Filter) is enabled via Setmode,
  then INT1 is generated only for non-ADPCM sectors." With mode bit 6 or 3
  set, a Mode 2 sector whose submode has audio and real-time (0x44) goes to
  the SPU CD input and raises no INT1 (`xa_sectors` counts them). Mode 0
  still delivers such a sector as data.
- **GetlocP (11h)** answers INT3 with 8 BCD bytes and no stat: track 1,
  index 1, the in-track position (the LBA; the one data track starts at
  00:02:00) and the disc position (LBA + 150). On a stopped motor it is a
  named boundary (`CD_device_getlocp_spun_down`), not a made-up position.
- **Stop (08h)**: INT3(stat) with bit 5 clear, then INT2 with bit 1 (motor
  on) clear. Reads, seeks and Init spin the motor back up. The braking time
  is not modelled; the second response uses Pause's delay.

Port changes:
- `func_8007BF44` (CdGetSector, libcd) is a hand port in
  `game/boot/cd_stream_port.c`. The generated verbatim TU dereferenced the
  `D_8009B27C..` register pointers as guest RAM (`PE_Translate` fatal at
  0x1F801800) and busy-waited on two hardware bits. The hand port uses the
  same register models as `func_8007CEAC`. A request or DMA channel still not
  ready after one DMA3 service is an explicit stop, never a spin.
  - **Psy-Q guard: an allowlist row moved.** `pc_port/psyq_guard_allowlist.txt`
    row `func 0x8007BF44 pc_port/game/decomp/func_8007BF44_port.c` became
    `func 0x8007BF44 pc_port/game/boot/cd_stream_port.c`. The two binary rows
    are unchanged, and so is the offender count (1421). This is still a
    translation of a Psy-Q leaf, so it is disclosed here rather than renamed:
    a `PE_*` name would hide Psy-Q content from the guard. It is an allowlist
    edit, and allowlist edits go to the coordinator for approval.
  - **Approved by the owner on 2026-10-19.** The owner answered directly in
    the terminal at 04:31:05Z, and the coordinator verified the answer as
    owner authority (message c4a74096b2831f39).
- `PE_Cd_DispatchDataCallback`'s default arm runs a target the guest-code
  registry resolves (retail's `jalr (status, response)`), and only an
  unresolved target stops. This arm is reachable in production. It turns a
  stop into the retail callback, natively. The credits' `func_80125980` and
  `func_80125AF0` are such targets.
- **Poll time.** `func_8007F72C` (DsSystemStatus) and `func_8007F418(0, ..)`
  each cost 64 device cycles while the device is attached
  (`HostFB_DevicePoll`). The generated waits
  `while (DsSystemStatus() != 1 || DsQueueLen() != 0) {}` have empty bodies,
  and without that cost they never advance the drive. 64 cycles is the retail
  cost of one iteration to within about a factor of two (about 31
  instructions with about 10 uncached RAM accesses). With the 1024-cycle
  VSync(-1) quantum, the drive in `DAY2_movie_player` read past the end of
  its fixture disc (`CD_device_sector_read` instead of `movie_retry_wait`).

Tests:
- `CDXA_device_commands` covers the device rules above.
- `credits_xa_device_path` runs the credits CD steps on the retail EXE and
  ovl_0457, using a fixture disc with a 48-sector CREDITS.XA in the real
  1-in-8 interleave:
  - `func_8012562C` CdSearchFile and the start/end LBA;
  - `func_80125744`'s Setmode 0xC8, Setfilter and ReadS through the real
    queue;
  - the VSync GetlocP loop until `D_80172CE4 == 1`;
  - the channel switch `func_80125894(1)`;
  - the `func_801255A4` teardown, leaving the drive idle and the queue empty.

**Production entry state (resolved).** Production boots libds in host mode:
`func_8003E610` runs ResetCallback and the host DsInit (`D_8009B554 = 1`), and
`func_8006A5BC`'s CdReset leaves lane 1. The overlay's own DsInit is then a
no-op, and the libcd state block the device branch fills (`func_8007FA2C`,
`D_8009B558..B590`) stays mostly zero. `PE_Cd_AttachDevice`
(`game/boot/cd_stream_port.c`) supplies exactly the two hooks host mode
skipped:
- `InterruptCallback(2, 0x8007C13C)`;
- `VSyncCallbacks(0, 0x8007FE24)`.

It attaches the drive with interrupt enable 7 and reissues nothing: no
Getstat, Init or Demute. It refuses unless libds is host-booted and idle.
`PE_Cd_DetachDevice` restores both slots, and with them I_MASK and the
registered-source mask, then resets the device model. The zero state block did
not matter: the first device responses overwrite what the credits read.

**VSync(-1) with the device attached (fixed).** The production-state test
first failed because GetlocP was never sent. `func_80125A40` issues it only
when `VSync(-1) & 0x1F == 0`, and host `VSync(-1)` returned `fb_vsync_count`,
which every query increments. Inside a once-per-frame VSync callback that
value moves by 2 per frame (the callback's own query plus the wait), so it
locked onto odd values and never fired. `host_framebuffer.h` documents
`VSync(-1)` as the VBlank counter `D_800956AC`. With the device attached it now
returns that counter: VBlanks come from device time, and each query still adds
its 1024 device cycles, so a busy loop on `VSync(-1)` still advances. Without
the device the host keeps its call count, because no VBlank happens inside a
busy loop there. All the device-mode filters still pass (DAY2_cd, DAY2_movie,
CDS, CDQ, B53, B54K, CDXA).

**Tests:**
- `credits_attach_from_host_boot`:
  - host boot;
  - attach installs both hooks, and a second attach is refused;
  - the CD steps run: CdSearchFile, then Setmode, Setfilter and ReadS, then
    the GetlocP loop until `D_80172CE4 == 1`, then teardown;
  - detach restores the IRQ 2 slot, VSync slot 0, I_MASK and the
    registered-source mask exactly;
  - a host-mode `func_8006E6D4` read of the fixture's PVD then completes.
- `credits_main_loop_runs_to_exit` runs `func_801235DC` itself in mode 0 from
  that state:
  - It runs in a forked child under `alarm(240)`, on an 80000-sector fixture.
    Only the first 512 sectors are written; the rest are zero pages that are
    never touched.
  - The loop made 13608 iterations, which matches the 13623 estimated from the
    744-line table at 1 px per iteration after a 224 px lead-in. It presented
    13608 frames, played the XA channel and left through
    `D_80172CE0 = D_80172CE4 = 1`.
  - The teardown left the drive idle and the queue empty, and bit 16 of
    `D_8009D1A0` was clear.
  - It takes 5.6 s of wall time.

  On the way there, the alarm caught two stops: an unseeded pad record, which
  is a fixture gap (`Test_SeedDigitalController`), and a 512-sector disc that
  the drive read past. After each stop the loop spun until the alarm. That is
  the next blocker.

**Production wiring (this commit).** `PE_Cd_XaServiceAvailable()` now returns
1, and `PE_Main_EnterCredits` (`bootstrap/func_8001220C_port.c`) runs the
credits in production. Its order:
1. Bit 16 is the pad-code mode: the nine-step code in `func_8003EB04` sets
   0x12000. It runs too (see "The pad-code mode" below). It opens no file, so
   the next step skips it.
2. With bit 16 clear, `PE_Cd_FilePresent` (`platform/pe_libcd.c`) looks up the
   overlay's own name (`*0x80125B48` -> `\XASTREAM\CREDITS.XA;1`) on the
   active disc. Without the file, the entry stops on `credits_xa_file_absent`,
   because `func_8012562C` retries a failed CdSearchFile forever (Disc 1 has no
   XASTREAM).
3. `PE_Cd_AttachDevice`, or a stop on `credits_cd_attach`.
4. The **stop escape** (`game_port.h`) is armed around
   `PE_GuestCall(0x801235DC)`. The generated loops never test
   `PE_Port_ShouldStop`, so while armed `PE_Port_EscapePoint()` polls it and
   longjmps back to the entry once a stop is requested. It is the only user;
   disarmed, the point is one load and a branch. The escape points sit at the
   entry of:
   - `HostFB_VSync`, `PE_GuestCall` (`guest_call`) and `HostFB_DevicePoll`;
   - `func_8007F418` (DsSync);
   - `func_80080DC4` (DsControlB). Under a stop it returns 0 at once, so the
     teardown's `while (func_80080DC4(9, 0, 0) == 0)` reached no other point.
     The injection test proved this: that case spun until its 60 s alarm
     before the point was added.

   The escape is never taken from `PE_Port_RequestStop` itself. It can be
   taken inside IRQ servicing, though: the CD data-ready dispatch and a
   `VSync(-1)` in the VBlank dispatch both reach a point. The longjmp then
   abandons that dispatch's guard and mask state. That is acceptable only
   because a stop ends the run: `port_main`'s shutdown after main returns is
   host-only and runs no guest code. The attach record is static, so it stays
   valid across the longjmp.
5. Detach on both paths, through `PE_CdReg_DetachDevice`. That is
   `PE_CdReg_Reset` but keeps the CD audio mix latch (ATV pending and
   applied), which the host movie feed reads with the device off
   (`pe_str_feed.c`). The return is `!PE_Port_ShouldStop()`.

**Tests** (forked children under `alarm()`, from the production entry state:
host libds boot, pad records, the boot input config `func_8003E974`, no button
held, SPU active, an 80000-sector fixture; a test VBlank hook in VSync slot 7):
- `credits_main_loop_runs_to_exit`, mode 0 through `PE_Main_EnterCredits`:
  - 13608 iterations and 13608 presents; 6 XA sectors decoded (the
    fixture's channel 0);
  - it left through `D_80172CE0 = D_80172CE4 = 1`;
  - after detach: IRQ 2, VSync 0 and both masks restored, lane 1, queue empty,
    bit 16 clear.
- `credits_after_overlay_reload` follows production's order:
  - main's arm first runs `func_8006F044` on Disc 1: stop all sound, then
    reload ovl_039F and ovl_0457 through host reads from the mounted PE.IMG;
  - only then does it run `PE_Main_EnterCredits`;
  - the attach accepts what that leaves in libds, and the whole loop runs.
- `credits_attach_from_host_boot` also applies the game's CdMix
  (`func_800870F0(144)`) through the attached drive and checks that the latch
  survives the detach.
- `credits_mode_20000_waits_for_press`, mode 0x20000:
  - channel 1 via `func_80125894(1)`, and the fade states 1-3;
  - state 2 waited until the hook held the button the boot config maps to an
    exit bit (iteration 14000), then the 64-step fade-out ran; 14068
    iterations in all;
  - 6 channel-1 sectors decoded.
- `credits_escape_from_loop`: a boundary from the loop body (VBlank 600) and a
  host quit through the quit poll. Each returns 0 mid-scroll, detached and
  restored.
- `credits_escape_from_cd_waits`: a stop in the `func_80125744` lane wait, in
  the `func_801255A4` Pause wait and in the `func_8012562C` search retry.
  Each comes back through the escape, detached and restored.
- `credits_entry_gates` (renamed from
  `credits_entry_stops_without_xa_service`) checks the two gate boundaries:
  `credits_xa_file_absent` for 0/0x20000 with no disc, and `credits_cd_attach`
  for 0x10000/0x12000, which need the drive but open no file. In both, the
  device stays detached and nothing of the credits runs.

**The pad-code mode (bit 16).** Its op table (0x80125B68) runs the scroll with
no XA setup. A press mapped to `D_8009D1F4` bit 26 or 27 runs
`func_80125894(0/1)`, which waits for `func_8007F418(0, 0) == 2` and then
sends CdlSetfilter through `func_80080DC4`.

That 2 is retail's ring state after any read, from the matched C:
- `func_8006E6D4` -> `func_80080E34` (DsRead) queues one packet under one id
  (`func_8007F0C8`'s `push`: CdlPause, CdlSetmode, CdlSetloc, CdlReadN), which
  is finished at the ReadN acknowledge.
- It sets `D_8009B6CC = 1`.
- At read end `func_80081268` sees that, and queues CdlPause through the
  libds queue (`func_8007EE84(9, 0, 0, -1)`), finished at its INT2.

The ring's last record is therefore the Pause's CdlComplete, and
`DsSync(0)` returns its status byte.

The host read completes synchronously and used to queue neither, so the ring
stayed as DsInit left it and `DsSync(0)` read 6 ("no command"). That was a
wait that never ends.

`func_8006E6D4` now publishes both completions through the IRQ chain's own
path (`PE_Cd_HostCompletion`), the same model as `PE_Tray_HostStop`:
- the answer goes into the command-lane response buffer `0x800A3460`, with
  stat 0x02;
- `func_8007EB88` records it under the next `D_8009B53C` sequence id.

It publishes only once libds is initialized (`D_8009B554 = 1`, ring index
0..7). `6AD40_complete_effect_allowlist` now accepts the sequence counter,
the response buffer and the ring, as a retail read writes them.

**Scope of the ring model.** Only `func_8006E6D4` (and the tray's
`PE_Tray_HostStop`) publish completions. The other host CD operations that
retail sends through the libds queue publish nothing:
- the PVD verify `func_80082314`;
- the host CdSearchFile `func_80081414`.

So `DsSync(0)` is faithful only after a `func_8006E6D4` read. The pad-code
credits rely on exactly that: `func_8006F044` reads right before them. This is
a normal-play change too: every room load now writes the libds sequence
counter `D_8009B53C`, the ring `0x800A3610..0x800A3693` and the response
buffer `0x800A3460`. It has function-level proof only (the suite and 6AD40);
the Day-1 route regression is owed.

`credits_pad_code_mode` runs production's order:
- `func_8006F044`'s two host reloads on Disc 1, after which `DsSync(0)` reads 2;
- then `PE_Main_EnterCredits` in mode 0x12000:
  - the second op table ran the whole scroll (13608 iterations);
  - the press at iteration 2000 ran the channel switch, and its CdlSetfilter
    reached the attached drive;
  - bit 16 made state 4 leave;
  - the device detached with the hooks restored.

**What this does not establish.** The credits are native at unit level on a
synthetic disc. Nothing has played the real Disc 2 CREDITS.XA, run the
end-of-music timing, or run what follows the credits (the EX save prompt, the
return to the title). Those need a game run on the runner. The ending is not
verified until that run.

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

Scope: the census covers EXE function starts only.
- Room-local starts are the room census's business: every room body is
  native.
- The six subsystem overlays have three leaves without a generated TU:
  ovl_0700 8019234C, ovl_03D2 80191B64 and ovl_03C5 80121270. All three are
  hand ports called by name, and 8019234C has a registry row.
- No EXE or overlay word stores 0x8019234C or 0x80121270.
- The three 0x80191B64 words are in room blobs (m0087i, m0419i, m0421i).
  There that address is each room's own native body.

Before today's fixes it found 7 stored callbacks with no native code, one
loop sentinel, 9 boundary arguments, and one word in the EXE data:

| address | what it is | how production reached it | now |
|---|---|---|---|
| 0x800494AC | 0x0D item-list update (`func_80048918`, 12 menu paths incl. field-VM `func_8004DCA4`) | Confirm/Cancel/unhandled keys -> `menu_callback` -> stop | native, E21 (d3e872a2a) |
| 0x8004A9A0 | shop window update (`func_8004A570`, from the equip-slot list's Confirm) | first key press -> stop | native, NON_MATCHING (d3e872a2a) |
| 0x800DD9E4 | rising-sparks particle callback (`func_800DDD70` pool) | first pool tick -> `PE_EffectCallback` -> stop | native, NON_MATCHING (f1a1b1c74) |
| 0x800DB6BC | field effect type 16 main (EXE data 0x800E1CF8, node 16 of `D_800E1044`) | command code 0x55 with type 16 -> `PE_EffectCallback` -> stop | native, NON_MATCHING (f1a1b1c74) |
| 0x80083944, 0x800846AC, 0x800847B0, 0x8008486C | Psy-Q libpad/libcard hooks: card record +0x18 (`func_800837C8`) and slots `D_8009B724/72C/730` (`func_800844E4`, installed at boot) | only through the libcard driver hand ports in `absent_cd_port.c` (`cd_icall`) | dormant: those ports (`func_80082E00`, `func_80083014`, `func_800840DC`, `func_80084168`) have no direct C caller, only registry rows, and are entered only from MemCard_TimerCallback `0x80082B70`. That callback is stored into `D_800A5AB4` (`func_80082ADC`, `pe_save.c:86`), and no production code reads `D_800A5AB4` to dispatch it (`grep -rn 800A5AB4 pc_port`, tests excluded). As Psy-Q functions they would need host replacements, not `func_<addr>` definitions, if a later change ever dispatched that table |

After f1a1b1c74 the census reported 4 stored, 9 boundary arguments and
1 comparison. The ninth boundary argument was room_m0017i `func_8018F0F0`'s
call of `func_8003A088`. The model-matrix walk has been native all along under
the name `func_8003A088_mode0_walk_cut`. Despite the name it transcribes
every `+0x28` mode of the leaf, and production's per-actor update
`func_80015240` calls it for every actor. Verification is uneven:
- mode 0 (the bone walk) is checked against retail by the BTL6 oracle
  (`pc_port/tools/pe_btl6_3a088_walk_oracle.py`, a 2-bone PE.IMG object);
- modes 3 and 4 have unit tests written from the matched C (GFX2);
- mode 1 is verified by reading only.

Retail has no `andi 0xFC` here either. The port returns early on
`dest == 0` where retail does not check, but neither caller can pass 0. The
walk now has its own entry, `func_8003A088`
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
substitutes in production instead of stopping (it logs each cause).

## Known residuals (documented, not hidden)

- **Retail stack residue.** `RotMatrix` (80079754) sets only a matrix's 3x3.
  The code then reads that matrix's translation from stack the call never
  wrote:
  - the 0x4A0 body in m0065i/m0087i/m0391i/m0393i/m0399i/m0401i loads it as
    TR;
  - m0034i/m0174i `gte_CompMatrix(&o->m, &m20, ...)` uses `m20.t`, so
    `o->m.t` follows the residue.

  The lowered code reads exactly retail's frame offsets: all 8 bodies match
  retail when both sides see the same stack bytes (`E21_STACK_COINCIDE=1`,
  "e21 differential sweep"). What can differ in production is the content,
  since the guest stack's history differs from retail's: hand ports keep host
  frames and write nothing there. So effect placement can differ. A fix needs
  recorded retail stack contents (a runner `--recorded` snapshot), not
  zero-fill. This is the same class as KNOWN_DIVERGENCES #17.
- **m0023i flash `[DAY1_SHIM]`** (`m0023i_effect_port.c`, KNOWN_DIVERGENCES
  #17) is the one shim that substitutes in production instead of stopping.
  Retail reads whatever the six borrowed bytes' last writer left.
  - Fixed 2026-10-19: F710 ends every mode-2 call by writing its joint
    position over all six bytes, and the original-code oracle
    (`pe_m0023i_flash_oracle.py`, which hashes 0x801FEFB8 after the call)
    confirms it. The model used to skip that write after a substituted draw,
    so the shim fired again on later state-0 draws. It now records the
    write, so the shim fires only on the first state-0 draw after the model
    loses the bytes. `test_m0023i_pump.h` runs a second pump, in strict
    mode, and asserts it needs no substitute; without the fix that pump
    stops.
  - The substitute itself (the owner joint position) is unchanged and is not
    the original value.
  - Since 2026-10-19 the shim also names its cause. The effect-stack model
    records the callback, or marker, that last cleared the borrowed bytes
    (`PE_EffectStackCause`). The first hit of each distinct cause is logged,
    every hit is counted, and a summary prints at exit:

        [DAY1_SHIM] PE_M0023I_Flash: F710 caller-stack ring position unresolved (context draw, unknown Z, last cleared by new RAM generation in context none); using owner joint position

  - That is the cause in the pump test's fresh-generation scenario, which
    the test builds on the recorded pump state (`test_m0023i_pump.h`
    asserts it). F004's prologue writes X/Y, but no CE934 update has stored
    Z at the borrowed depth before the first flash draw.
  - Which causes normal play hits, and so which stack writers to bind next,
    needs the log of a Day-1 route run.
  - The 2026-10-19 Day-1 diagnostic run (binary d396976e) logged two shim
    causes: the hit-draw callback `800CDD0C` and the update callback
    `800CA540`. Both are bound since 2026-10-20 (`func_800D413C_port.c`),
    each from retail bytes:
    - **CDD0C** keeps the bytes at the weapon-draw depth.
      - Every draw entry (C9B68, CE144, CD8C8: frame 24) calls C2414
        (frame 32), so the callback enters at S-56 and runs at S-88.
      - The frames that cover [S-112, S-106) are outgoing-argument areas:
        C2EAC, C3098 and C3238 at sp S-112, and C3B04 at sp S-120. Those
        four store only at their own sp+16..28. Their callees are frameless
        leaves without stack stores, and no stack address escapes.
      - One path could write the bytes: C3098 calls the BIOS printf stub
        `80071A74`, which spills a1-a3 over sp+4..12. It does so only for a
        mode other than 16 or 256, and CDD0C passes the constants 0x100 and
        0x10 (`800cdd30`, `800cde00`).
      - The call tree was walked with a disassembly script over the retail
        EXE.
    - **CA540** (`800ca540..800ca570`) is a frameless leaf: no sp access
      and no calls.
      - Its two stores go through a1 (rec+1) and a2 (data+4), which C251C
        points into the slot's record table and command arena.
      - The binding keeps the bytes only while neither address can fall in
        crt0's stack reservation (`>= 0x801FDFF8`, any RAM mirror).
        Otherwise it forgets, as before.
    - Tests: `FLASHVEC_cdd0c_*`, `FLASHVEC_ca540_*` (including both sides
      of the reservation boundary) and `FLASHVEC_other_update_callback_forgets`.
    - **Falsifiable prediction.** The next Day-1 run should log neither of
      these two causes. Whether the shim still fires depends on the bytes'
      state before those calls, so this does not say the shim is removable
      or that Day-1 is fixed.
    - **Result (2026-10-20, Day-1 route on this devbox, binary 56c8eb73 from
      be90460eb, zig-cc build, pads only, aids off, bwrap sandbox).** Packet
      `/home/blizz/scratch/pe-modern-route-be90460eb/`, RESULTS.md
      69bd17cd…, SHA256SUMS 6bf3a88e…
      - The route outcome is unchanged: the same first Eve fight at frame
        42707 and the same game over at 44602, FAIL at eve1_victory, as all
        four references.
      - With the shim lines excluded, the log has no textual difference from
        any reference, and the final screenshot is byte-identical.
      - CPU_BOUNDARY 0, STOP 0, stubs 0. No `enhancements.conf` was created.
      - **CDD0C is gone.** No shim hit of any cause occurs before BATTLE_EDGE
        43659.
      - **One cause remains, there:** "last cleared by weapon-update call in
        context weapon-update", at the point where the e2r reference named
        CA540.
      - CA540 is now preserved; its port-domain stores were outside the
        window in the e2w observation. The likely reading is that another,
        unbound update callback cleared the bytes earlier in the same pass,
        and CA540's later clear had masked it.
      - Unbound update callbacks are therefore recorded by address since the
        commit after be90460eb, so the next run names that writer.
    - **Integrated gcc build (941d6878c, binary f686f4fe, the same route;
      packet `/home/blizz/scratch/pe-modern-route-941d6878c/`).**
      - No textual and no behavioural difference against any of five
        references (11d7, e2r, e2w, 6474 and be90460eb), with the shim lines
        excluded.
      - CPU_BOUNDARY 0, STOP 0, stubs 0. No `enhancements.conf` was created.
      - All new options were at their defaults; the analog, remap and
        music/SFX code was merged.
      - The single remaining shim line now names its writer:
        `callback 800CA4B4`, the moving-particle update leaf.
    - **CA4B4** (`800ca4b4..800ca53c`) is another frameless leaf: no sp
      access and no calls.
      - It stores through a2 (data+1, +2, +8..+13, +18..+19) and a1 (rec+1).
      - It is bound like CA540, guarded on that span.
      - Test: `FLASHVEC_ca4b4_update_preserves_known_z`, which also checks
        the retail update itself (x += vx, vy += 3, age++, life--). The
        unbound control is now CDF4C.
      - Prediction for the next run: CA4B4 is no longer the named cause.
    - **Result (9ba4868f6, binary cc81d5db, packet
      `/home/blizz/scratch/pe-modern-route-9ba4868f6/`).**
      - Confirmed: CA4B4 is no longer named.
      - The route is again identical to all six references (now including
        be90460eb and 941d6878c).
      - The cause moved to `callback 800CA4A8`, the third entry of the same
        pistol update table `D_800E0AA4`.
    - **All weapon update tables classified from retail bytes** (scratchpad
      `leafscan.py`, a stores-by-base-register scan with move aliases, and a
      manual check of each body):
      - `D_800E0AA4` (pistol, C9B90): CA4A8, CA4B4 and CA540.
      - `D_800E0F38` (hit, CD8F0): CDF40 and CDF4C. Its third entry, CDFE0,
        has a frame and calls the RNG, so it stays unbound.
      - `D_800E0FCC` (club, CE16C): CE464 and CE470.
      - All seven listed leaves are frameless: no sp access and no calls.
      - They store only `rec+1` and `data+[lo, hi)`: CA4B4 1..20, CA540
        4..6, CDF4C 1..64, CE470 3..4, none for the three retire-only leaves.
      - They are bound in one table, `k_frameless_update`, each guarded on
        its span.
      - Test: `FLASHVEC_frameless_update_table_preserves`.
    - **Result (049784820, binary 2fd26b8c, packet
      `/home/blizz/scratch/pe-modern-route-049784820/`): `DAY1_SHIM 0`.**
      - For the first time the flash shim did not fire on the Day-1 route.
        Every borrowed-vector writer the route reaches is now bound from
        retail bytes, so F710 reads modelled bytes rather than the
        owner-joint substitute.
      - The route is identical to all seven references (11d7, e2r, e2w,
        6474, be90, 941d, 9ba4) with the shim lines excluded.
      - CPU_BOUNDARY 0, STOP 0, stubs 0. No `enhancements.conf` was created.
      - **Scope:** the route ends at the known game over at 44602, a
        pad-timing desync, inside the first Eve fight. Later flash draws, and
        weapons whose update tables the route never reaches, are not
        exercised. This is not evidence that the shim can be removed.
- **m0005i hand ports versus generated TUs.** 8018F018, 8018F330 and 8018FB84
  now have matched C (harvested), but the effect dispatcher still sends
  m0005i's own bytes to the hand ports. Those hand ports are checked against
  retail by the ATK25/ATK26/ATK29 retail-case tests (Open, below).
- `func_8003F074` -> `func_8006B4F8` (room loader) is generated and reached only
  by a guest jalr. The production room transition uses the hand cuts in
  `func_8006B4F8_port.c`.

## Gates on this branch

Current tree: HEAD 6b2956262 (gate run on its code, 2026-10-19 01:34-01:54 UTC):
- `pe-native-tests` 1677/1677 (0 skipped), ctests 6/6.
- `psyq_port_guard.py` on both binaries: PASS (1421).
- Generator drift: 1051 EXE TUs, 0 drifted; 2951 room TUs, 0 drifted.
- `boundary_site_census.py --generated`: 38 sites. `dangling_callback_census.py`:
  4 stored, 8 boundary arguments, 1 comparison. Both unchanged.
- `scripts/run_native_oracle_harnesses.sh`: PASS.
- Changes since e6785a20d: the m0023i flash shim names its cause (03ba62003)
  and the effect-stack model records F710's write-back (6b2956262). Both are
  checked at function level only.

Earlier tree: HEAD e6785a20d (2026-10-19 00:15 UTC):
- `pe-native-tests` 1677/1677 (0 skipped), ctests 6/6.
- `psyq_port_guard.py` on both binaries: PASS (1421).
- Generator drift: 1051 EXE TUs, 0 drifted; overlay TUs 0 drifted; the
  registry is up to date.
- No overlay registry entry at fingerprint 0 (2169 fnv-0 rows, all EXE).
- `scripts/run_native_oracle_harnesses.sh`: font 54, animation 117, field
  VM 693, all PASS with zero skipped.
- `boundary_site_census.py --generated`: 38 sites. `dangling_callback_census.py`:
  4 stored, 8 boundary arguments, 1 comparison. Both unchanged.
- The two normal-play changes (host reads publish libds completions;
  Items -> Use and the Equipment list run generated TUs) have function-level
  proof only, so the Day-1 route regression is the most important owed run.

Earlier tree: HEAD after the credits production wiring and its review
fixes, 2026-10-18 22:19-22:23 UTC (incremental build in /dev/shm):
- `pe-native-tests`: 1676/1676. New since regen25:
  `credits_xa_device_path`, `CDXA_device_commands`,
  `credits_attach_from_host_boot`, `credits_main_loop_runs_to_exit`,
  `credits_after_overlay_reload`,
  `credits_mode_20000_waits_for_press`, `credits_escape_from_loop`,
  `credits_escape_from_cd_waits`, `GFX1_gp0_mirrors_and_copy`,
  `DRW1_chain_long_node_and_copy` and `PV_4D5CC_card_slot_runs_registry_callback`.
- ctests park/GTE/dev-menu 6/6.
- `psyq_port_guard.py` on both binaries: PASS (1421). One row moved:
  0x8007BF44 from the generated TU to `cd_stream_port.c` (see the CD-XA
  spike). This needs the owner's approval as an allowlist edit.
- Generator drift: 1049 EXE TUs, 0 drifted; the overlay TUs and the registry
  are up to date.
- Sweep 8 (every e21 candidate at the shipped flags, `--coverage --min-ok 4
  --max-seeds 256`, tree at 18:37:58Z, 18:37-20:14 UTC):

      e21_diff: MISMATCH: 8      the stack-residue set
      e21_diff: ok: 2041
      e21_diff: skipped: 5       ovl_0700 8018F55C/80191C94, ovl_03D2 8018F958/8018FBC0/80192CE8 (dormant)

  No E21 TU changed after it started. The two generated TUs that did change
  (`func_80080AE4`, `func_80080F98`) are verbatim, and e21 sweeps only E21
  TUs.
- Still not run, owed through the runner (it needs a coordinator admission),
  plus the owner's own device gate:
  - the Day-1 route regression;
  - the movies-on Day-2 check;
  - an ending run on Disc 2: the credits through the real CREDITS.XA, and the
    EX save prompt and return to the title after them;
  - the owner device gate;
  - any visual check.


Current tree: regen25, HEAD d9db7fcfb plus doc commits, 2026-10-18 18:26 UTC.
- `pe-native-tests`: 1665/1665. Four tests are new since regen20:
  `credits_entry_stops_without_xa_service`,
  `credits_xa_commands_never_complete_without_device`,
  `PV_494AC_item_list_confirm_runs_native` and
  `PV_4A9A0_slot_detail_input_runs_native`.
- ctests park/GTE/dev-menu 6/6.
- `psyq_port_guard.py`: PASS (1421, unchanged).
- `scripts/run_native_oracle_harnesses.sh` on this tree (18:35 UTC): font 54,
  animation 117, field VM 693, all PASS with zero skipped.
- Generator drift: 1050 EXE TUs, 0 drifted. The registry has 5776 thunks.
- `boundary_site_census.py --generated`: 38 sites.
- `dangling_callback_census.py`: 4 stored (the Psy-Q libcard hooks), 8
  boundary arguments, 1 comparison.
- Sweep 7 (below) predates five changes: the `LOWER_REASONS` addition, the
  four new EXE TUs (800494AC, 8004A9A0, 800DD9E4, 800DB6BC), the
  `func_800D1DEC` extents, the m0017i TU, and the host CdRead busy-word fix.
  Only those TUs were swept again at the shipped flags. They pass:
  - 800494AC: 446 seeds, 731/775 words;
  - 8004A9A0: 557 seeds, 249/255;
  - 800DD9E4: 216 seeds, 227/227;
  - 800DB6BC: 265 seeds, 248/248;
  - m0017i 8018F0F0: 32 seeds, 45/45.

  No sweep 8 was run. The extents only narrow the compared window to the
  bytes the callee reads, so they cannot turn an ok TU into a mismatch.
- Not run, owed through the runner before anything ships:
  - the Day-1 route regression;
  - the movies-on Day-2 check;
  - the owner device gate;
  - any visual check.

  Every production behaviour change today has function-level proof only.
  Two of them sit in menus that normal play opens early: the item list
  0x800494AC and the shop 0x8004A9A0.

Earlier tree (regen20, HEAD 99fad1188 and later doc commits), 2026-10-18:

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

- The ending credits run natively in production through the attached
  register-level device, all three modes (0, 0x20000 and the pad-code
  0x12000), at unit level only (see "The ending credits: CD-XA"). Owed: a
  Disc 2 ending run on the runner.
- The four Psy-Q libcard hooks are dormant (see the "Dangling callbacks"
  table): the only path to them, the MemCard_TimerCallback table
  `D_800A5AB4`, is never dispatched in production.
- Of the seven boundary conditions (under "Boundary-site census"), two are
  fixed and five are settled:
  - four dormant: `func_80075358`'s GP0 refusals, `absent_ovl_port.c:77`
    (and with them the libcard hooks), and `func_80028574` shot counts
    11-15, which game logic cannot produce (`func_80051980` and
    `D_800923D0`);
  - one error-defensive: `actor_contact_port.c:183`.

  The shot-count stop stays only as the guard for an edited save.

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
- `func_800339A0` with style >= 4 stays a loud boundary, because retail reads
  the stack above a 16-byte local there. No disc data reaches it:
  - `tools/analysis/battle_style_census.py` reads all 414 room scripts.
  - The style comes from field-VM op 0x55 (battle entry,
    `func_800144FC` -> `func_80029810(**a0)`). Those 402 commands all take
    immediate styles: 0 (37), 1 (26), 2 (273) and 3 (66).
  - Op 0xED key 0xAF1 (`func_80016910`) appears in none of the 1411 op-0xED
    commands.
  - The other callers pass 0 or `D_8009D02C`, which only holds 0
    (`func_800527C8`) or the current style (`func_80033A20`).

  So it is dormant for every disc script. Op 0x55 is the only way into
  `func_80029810`, and actor scripts come from the room package: the room
  loader fills the per-type table at `D_800B0CD8 + 0x1C0` (`D_800B0E98`) with
  pointers into the loaded package (`func_8006B4F8`, `func_8006BECC`), and
  `func_80035038` takes each actor's script base from it. The census decodes
  every module of every package.

  The old note in `pe_field_vm_opcode_coverage.py`, that world-map (type-0)
  slot bytecode is save data, does not hold (checked 2026-10-19):
  - An actor's script base is `(*D_800B161C)[desc[0] + 2]`
    (`func_80035038`; `func_800125E0` reads the same table). `D_800B161C` is
    `D_800B0CD8 + 0x944`, the `Tbl.p` word after the ten 0xC0-byte class
    rows, not a row entry.
  - Only `func_8006B4F8` stores a nonzero value there: the room package's
    slot-0x14 script, the one the census decodes. `func_8006B35C` clears it.
    No other code stores to it, through `D_800B0CD8` (+0x944), `%lo` (0x161C)
    or the `Tbl` base `&D_800B0E98` (+0x784):

        python3 -I tools/analysis/store_imm_scan.py 0x944 0x161C 0x784
        EXE 8006B3D8 sw $0,0x944($2)        (func_8006B35C)
        EXE 8006B8C4 sw $2,0x944($22)       (func_8006B4F8)
        room_m0085i_c1.bin +3AB34 swr $7,0x161C($2)
        room_m0162i_c1.bin +3FBC sb $16,0x944($24)
        store_imm_scan: 4 hit(s) in the EXE text and 517 PE.IMG blobs

    The two blob hits are data words, not code (their neighbours do not
    decode as code).
  - `func_8006BECC` refills class-0 rows from PE.IMG area blocks at
    `D_800B0DD8 + D_800930D8[area + 8]` (`D_800B0DD8` is PE.IMG's start,
    `func_800698D4`). Those are animation-handler rows (`func_8001A680`,
    `func_8001A784`), not script bases. They are disc data, not save data.

  So every actor script base points into a room package script that the
  census decodes. Not audited: other stores to the actor's script-base field
  (`+0x9C`) after the spawn sets it.
- The retail stack-residue residuals and m0023i's `[DAY1_SHIM]` (above).
- Done: the directed retail differential of the Items/Equipment paths
  (`func_80057834`, `func_800466C0`, `func_8005A318`). The two hand ports are
  retired for their generated TUs; see "Hand ports retired".
- m0005i: hand ports or generated TUs. The hand ports that the effect
  dispatcher binds for m0005i's own bytes (8018F018, 8018F330, 8018FB84)
  have retail-case tests generated from the original Disc 1 code:
  ATK25 charge (`retail_eve_charge_cases.h`), ATK26 flare and ATK29 beam.
  The generated TUs are the e21-checked alternative. No change is needed
  for native coverage; switching is a cleanup choice.
