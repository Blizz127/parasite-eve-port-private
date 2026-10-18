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
Registry `zz_dispatch.c`: 5768 thunks. 185 code rooms, 9103 (room x body)
rows, 2952 unique (vma, size, FNV) bodies.

| | session start | 2026-10-18 11:55 |
|---|--:|--:|
| native-generated bodies | 8855 | 9098 (87 via a data head) |
| not-native bodies (all statically reachable) | 217 | 3 |
| direct-addr-guarded (hand port bound by address only) | 31 | 0 |
| native-direct (hand port bound by fingerprint) | 0 | 2 |
| unique not-native bodies | 136 (183,492 bytes) | 3 (17,816 bytes) |

A body resolves only by fingerprint, as `PE_GuestCode_Resolve` does. Rooms are
never declared resident in production, and closure-equal duplicates count once.

The 3 bodies with no C anywhere are effect and particle callbacks in late
rooms:

| room | bodies |
|---|--:|
| m0429i | 1 |
| m0404i | 1 |
| m0256i | 1 |

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

The 9 skipped TUs ran no seed (`func_8018FDF8`, `801906EC`, `80190A7C`,
`80190E98`, `801929F8`, `801932EC`, `80193F70`, `801942F0`, `80194440`); they
need directed specs.

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

57 TUs are unsupported:
- 20 call a loud boundary (next item);
- 10 call the host shims `HostFB_VSync` / `HostFB_SetDispMask`;
- the rest declare callees with TU-local types (S64, RECT), which the
  generated mocks do not see.

The 78 skipped TUs need directed specs. Narrow returns, parameter-width
drift (a K&R leaf passing a full `int` where the host prototype narrows to
`short`; both harnesses mask narrow parameters to their width) and
out-of-range indices are not closed by any generator change. They are
recorded here as residuals.

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
counts each binding. `--generated` gives 39 sites (it was 8): the 8 overlay
sites above plus 31 bindings in these 21 TUs. None of the 21 is reached:
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
  the regeneration with room batch 10, 2026-10-18 11:50 UTC): 1661/1661, 0
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
  (regenerations of f91561717, 249e76f38, bfdf402af and room batches 5-10): PASS relative to the
  allowlist, 1421 entries, unchanged. This is not a public-binary clearance.
- Not run on the devbox, by rule:
  - the Day-1 route regression;
  - the movies-on Day-2 check;
  - any game run;
  - the owner device gate.

## Open

- The 3 remaining no-C room bodies: m0404i 80192540 (0xE28), m0256i
  801940B0 (0x1270), m0429i 80192F9C (0x2500, no near twin). The room workflow (wf_9660d38d) is working through them.
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
