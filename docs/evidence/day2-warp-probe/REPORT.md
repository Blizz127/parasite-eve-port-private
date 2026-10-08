# Day 2+ room warp probe (2026-10-07, native-only, discs on the box)

## Method

The dev-only env warp (`PE_DEV_MENU=1 PE_WARP_ROOM=<mNNNNi> PE_WARP_STORY=<hex>`,
`plat_cheats.c`) uses the same field-VM room transfer as the F8 warp menu.
`pc_port/tools/warp_probe.sh` boots New Game headless, warps at frame 1000
with story 0x90 (the Day 2 opening), presses X 20 frames on / 20 off, and
runs 4000 frames. Slots m0037i..m0438i were probed (402 slots).

The room token is `0xA8000048 | v << 7`, where v = hundreds*0x400 + tens*0x20
+ units of the slot number. It was checked against every Day 1 checkpoint
token and the Day 2 opening token (A80040C8 = m0041i, the chief's office).

## Results

- **About 356 rooms** load and idle (or walk through a door) for 4000 frames
  with no stop.
- **20 slots** have no code chunk (`peimg.py room` reports chunk 2 with
  sector count 0). These look like unused slots. Warping there aborts in
  `func_8006B4F8_dest_load_cut` (FATAL guest address 0xFC913DA8). The real
  game never transfers to them.
- **26 rooms** stop on unported **room-local** code. Before this change they
  were silently skipped by "DAY1_SHIM" paths (init / destroy / effect /
  weapon / module-init callbacks). Under the native-only stop policy those
  paths are now a `CPU_BOUNDARY/REFUSED` STOP:

| room | missing function | caller |
|---|---|---|
| m0049i | 0x8018F03C | `@8006F39C:init` |
| m0050i | 0x8018F03C | `@8006F39C:init` |
| m0089i | 0x8018FF54 | `@8006F39C:init` |
| m0090i | 0x8018F03C | `@8006F39C:init` |
| m0123i | 0x80191540 | `@8006F39C:init` |
| m0157i | 0x8018F3A4 | `@800D413C:effect` |
| m0158i | 0x8018F3A4 | `@800D413C:effect` |
| m0159i | 0x8018F3A4 | `@800D413C:effect` |
| m0199i | 0x8018FF44 | `@8006F39C:init` |
| m0207i | 0x8018FF44 | `@8006F39C:init` |
| m0232i | 0x8018FF44 | `@8006F39C:init` |
| m0256i | 0x80192584 | `@8006F39C:init` |
| m0272i | 0x8018F014 | `@8006F39C:init` |
| m0295i | 0x80190BE0 | `@8006F39C:init` |
| m0349i | 0x8018F7C4 | `@800D413C:effect` |
| m0350i | 0x8018FF64 | `@8006F39C:init` |
| m0374i | 0x8018F03C | `@8006F39C:init` |
| m0427i | 0x8018FF44 | `@8006F39C:init` |
| m0428i | 0x8018FF44 | `@8006F39C:init` |
| m0429i | 0x8019AE8C | `@800D413C:effect` |
| m0430i | 0x8018F750 | `@800D413C:effect` |
| m0432i | 0x8018FFA4 | `@8006F39C:init` |
| m0434i | 0x8018FF58 | `@8006F39C:init` |
| m0435i | 0x8018FF58 | `@8006F39C:init` |
| m0436i | 0x8018FF44 | `@8006F39C:init` |
| m0437i | 0x8018FF5C | `@8006F39C:init` |

## Why these are not carved yet

- Ten of these have matched C in `src/overlays/room_*`. The overlay
  generator (`gen_overlay_ports.py`) still refuses them, for reasons such as
  "function address value" (an init that stores a room callback pointer),
  "leaf struct with pointer members", "pointer-to-pointer value" and
  "call arity 0!=1" (an a0 pass-through).
- Across all rooms the generator skips 1802 matched room leaves, mostly for
  guest-pointer layouts (650 pointer-to-pointer, 384 struct with pointer
  members). The rest have no matched C at all.
- Day 1 rooms got through on hand ports (`PE_M28MovementInit`,
  `PE_M34BossEffectInit`, ...). Days 2+ need the same per-room work, or
  generator support for guest-pointer structs and room function addresses.
  Each carved init also exposes the callbacks it installs, so carving one
  function at a time does not converge.

The story value is a probe approximation (0x90 for every room). Rooms whose
scripts branch on later story values were not exercised in those branches,
and nothing was walked beyond X presses.

## Round 2: carving (same evening)

Changes:

- **Generator (`gen_decomp_ports.py`), three source-level rewrites** with
  unit tests:
  - `(int)D_X` on an extern array becomes its guest address literal.
  - `*func_X(..) = v` / `*func_X(..)` on a callee declared `int *` becomes
    a guest store/load: the effect-slot idiom `*func_800C22F8() = script`.
  - Function-pointer members of leaf structs are retyped to `unsigned int`
    guest code addresses, and `x = func_X;` becomes the VMA (the room Obj
    `fn` field). A leaf that calls through such a member is still refused.
- **`src/overlays/room_m0049i`**: the a0 pass-through made explicit in
  `func_8018F03C` (init) and `func_8018F12C`. EXACT_REBUILD_GATE[room_m0049i]
  still PASSes.
- **`pe_guestcode.c`**: two room tables that list the same generated TU over
  identical bytes now resolve as one match. The generator emits one TU per
  masked-body group for all of its member rooms; before this, room twins like
  m0022i/m0428i `func_8018FF44` were ambiguous and never resolved.

Effect:

- 66 more matched room leaves are generator-eligible (1801 -> 1735 skips).
- m0374i now runs clean.
- 16 rooms get past their init and stop one level deeper, at the effect
  update callback the init installs: m0049i/m0050i/m0090i, the Obj family
  (m0089, m0199, m0207, m0232, m0350, m0427, m0428, m0432, m0434-0437) and
  m0295i.
- Full sweep after the changes: 357 clean, 20 unused slots, 25 STOP. Day 1
  route PASS (frame 67186), MOVIES_DAY2 PASS, suite 1634/1634.

Stops now:

| room | missing function | caller |
|---|---|---|
| m0049i | 0x8018F13C | `@800C2414:weapon` |
| m0050i | 0x8018F13C | `@800C2414:weapon` |
| m0089i | 0x80190250 | `@800D413C:effect` |
| m0090i | 0x8018F13C | `@800C2414:weapon` |
| m0123i | 0x80191540 | `@8006F39C:init` |
| m0157i | 0x8018F3A4 | `@800D413C:effect` |
| m0158i | 0x8018F3A4 | `@800D413C:effect` |
| m0159i | 0x8018F3A4 | `@800D413C:effect` |
| m0199i | 0x80190240 | `@800D413C:effect` |
| m0207i | 0x80190240 | `@800D413C:effect` |
| m0232i | 0x80190240 | `@800D413C:effect` |
| m0256i | 0x80192584 | `@8006F39C:init` |
| m0272i | 0x8018F014 | `@8006F39C:init` |
| m0295i | 0x80190EDC | `@800D413C:effect` |
| m0349i | 0x8018F7C4 | `@800D413C:effect` |
| m0350i | 0x80190260 | `@800D413C:effect` |
| m0427i | 0x80190240 | `@800D413C:effect` |
| m0428i | 0x80190240 | `@800D413C:effect` |
| m0429i | 0x8019AE8C | `@800D413C:effect` |
| m0430i | 0x8018F750 | `@800D413C:effect` |
| m0432i | 0x801902A0 | `@800D413C:effect` |
| m0434i | 0x80190254 | `@800D413C:effect` |
| m0435i | 0x80190254 | `@800D413C:effect` |
| m0436i | 0x80190240 | `@800D413C:effect` |
| m0437i | 0x80190258 | `@800D413C:effect` |

Where this stops converging:

- The next layer is room effect update callbacks such as `func_80190240`,
  which call through `o->fn(o)` and read guest-pointer struct members
  (`o->ctx->fE`). m0049i's `func_8018F13C` is a 1100-line particle routine.
- These need the generator to model guest-pointer struct members (PE_Load
  through the 32-bit field) and calls through code-pointer members (via
  PE_GuestCall), or hand ports.
- About 1000 skipped room leaves have the same two reasons (650
  pointer-to-pointer, 384 struct with pointer members). That generator
  feature, not more one-off carving, is the next unit of work.

## Round 3: E21 guest-memory lowering (2026-10-07 night)

**Progress note (for restarts).** Branch `cursor/pe-native-only-akao-6f51`.
Done and pushed: `e2cc954f` (generator E21 lowering + tests + e21_diff),
`47d6c84d` (effect-callback slot ABI + warp_probe timeout). Gates on that
binary: Day 1 route PASS (frame 67186), MOVIES_DAY2 PASS, suite 1634/1634
with disc, psyq guard PASS (allowlist), retail_data_guard OK, generator
`--verify` 0 drift, unit tests 14 + 24 OK, e21_diff 594 ok / 0 mismatch.
Done: full re-sweep (`build/lanes/warp/sweep4_e21_abi.txt`, git-ignored) and Day 3
probes, results below.
Done and pushed: `4df41140` E21b inline GTE asm lowering (`guest_lower.py`
asm templates -> `PE_GTE_ReadData/WriteData/ReadCtrl/WriteCtrl/Command` in
`pe_gte.c`). Room asm refusals 305 -> 20, room TUs 2257 -> 2583. Gates: Day 1
PASS (67186), MOVIES_DAY2 PASS, suite 1636/1636, guards OK, verify 0 drift,
e21_diff 631 ok / 0 mismatch (32 GTE TUs).
Done: registry `closure` fingerprint resolves byte-identical room twins
(m0437i clean). Remaining 4 stops have no matched C (decomp work).
No release (waiting on Will).

Changes:

- **`tools/analysis/guest_lower.py`** (new): room functions refused for
  room-data pointer fields or calls through code pointers are re-emitted
  from matched C with every pointer as a 32-bit guest address (O32 layout,
  `packed` honoured, unsigned plain `char`, wrap-safe signed arithmetic,
  explicit `PE_Load*/PE_Store*`). `o->fn(o)` becomes
  `PE_GuestCall("@<leaf>:jalr")` native dispatch, unknown callees stay
  `CPU_BOUNDARY`. Aggregate or address-taken locals live in a native
  guest-frame arena (`PE_GuestFrame_Enter/Leave`, 0x801FE000-0x801FEC00).
  Asm (including GTE), arity mismatches, float, bitfields and static locals
  stay refused.
- Room TUs 1023 -> 2257 (1250 E21-lowered), 0 uncompilable; dispatch registry
  4606 thunks.
- **Effect callback ABI**: retail `func_8006F9F0` calls the update callback
  with a0 = slot; `func_8006F8EC` calls draw with a0 = slot,
  a1 = min(code, 85). The port passed (mode, data), so E21 callees read
  a0 = 1/2 (m0049i/m0050i FATAL 0xFB0001F4 in `func_8018F0BC`). Fixed.

Full re-sweep at story 0x90 (402 slots, binary at `47d6c84d`):

| | before (round 2) | after (round 3) |
|---|---|---|
| clean (4000+ frames, no stop) | 357 | 370 |
| STOP on unported room code | 25 | 11 |
| scripted exit (m0050i: story 0x2F2, return to title) | - | 1 |
| unused slot (no code chunk, FATAL 0xFC913DA8) | 20 | 20 |

m0049i, m0050i, m0089i, m0090i, m0123i, m0157i, m0158i, m0199i, m0272i,
m0350i, m0427i, m0428i, m0429i and m0430i now run clean. Remaining stops:

| room | missing function | caller | why |
|---|---|---|---|
| m0159i | 0x8018F3A4 | `@800D413C:effect` | no matched C |
| m0256i | 0x80193BFC | `@800D413C:effect` | no matched C |
| m0349i | 0x8018F7C4 | `@800D413C:effect` | no matched C |
| m0207i, m0232i, m0436i | 0x80190370 | `@func_80190240:jalr` | GTE inline asm |
| m0434i, m0435i | 0x80190384 | `@func_80190254:jalr` | GTE inline asm |
| m0437i | 0x80190388 | `@func_80190258:jalr` | GTE inline asm |
| m0295i | 0x8019100C | `@func_80190EDC:jalr` | GTE inline asm |
| m0432i | 0x801903D0 | `@func_801902A0:jalr` | GTE inline asm |

The six Obj-family rooms (m0207i ...) looked clean before the slot-ABI fix
only because their update callback got a0 = 1 and took an early exit; with
the retail a0 they reach the room's movement handler, which uses inline GTE
asm (`ctc2`/`lwc2`/`.word 0x4A...`/`swc2`). 305 room leaves are refused for
asm; that is the next generator feature (lower the GTE asm templates onto
the native GTE model in `pe_gte.c`).

Day 3 probes (story 0x140 = Day 3 start, M0092I entry; 0x150/0x160 later):
m0092i..m0098i @0x140, m0092i @0x150, m0191i and m0360i @0x160 all ran
5000 frames clean. Same caveat: only the story word is set and nothing is
walked beyond X presses.

## Round 4: GTE asm lowering (E21b) re-probe

Re-probe of the 11 stopping rooms on `4df41140`: m0207i, m0232i, m0295i,
m0434i, m0435i and m0436i now run clean. Still stopping:

| room | missing function | caller | why |
|---|---|---|---|
| m0159i | 0x8018F3A4 | `@800D413C:effect` | no matched C |
| m0256i | 0x80193BFC | `@800D413C:effect` | no matched C |
| m0349i | 0x8018F7C4 | `@800D413C:effect` | no matched C |
| m0432i | 0x80193414 | `@800D413C:effect` | no matched C (got past 0x801903D0) |
| m0437i | 0x80190388 | `@func_80190258:jalr` | two generated TUs (m0413i, m0245i twins) match the same bytes, so the resolver refuses to pick |

m0050i still returns to the title (story 0x2F2 -> TOK_NEWGAME) and then waits
on DMA, as before. Story-0x90 total: 376 clean, 5 STOP, 1 scripted exit,
20 unused slots.

Twin ambiguity fix: registry entries now carry a `closure` FNV-1a (the leaf
plus every same-room function reached by `jal`, transitively, from the
extracted blob). `PE_GuestCode_Resolve` treats two different generated TUs
as the same code when both their leaf fingerprint and closure match. m0437i
now runs clean (and m0413i still does). Story-0x90 total: 377 clean, 4 STOP
(m0159i, m0256i, m0349i, m0432i, all on room functions with no matched C),
1 scripted exit (m0050i), 20 unused slots.

## Round 5: hand-matched C for the 4 stopping rooms (2026-10-08)

**Progress note (for restarts).** Working through the 4 rooms one function at
a time, each written from the retail asm until `era_link_check` prints
LINK_EXACT, then carved (`carve.py`, `fixwindow.py`, `git add -N`) with
`EXACT_REBUILD_GATE` PASS.

- m0256i: `func_80193BFC` (0x4B4) and its particle callback `func_8019377C`
  (0x480, GTE rotation-matrix asm) done (`e6711341`). Then `func_801934AC`
  (0x2D0) matched and carved (`ab6de50b`). Then the particle callback
  `func_80192844` (0xC68, 5 motion kinds plus GTE draw) matched and carved;
  it needs a local dispatch-fold profile
  (`room_m0256i_dispatch_8018F1DC_8018F1F4`: `MASPSX_DISPATCH_FOLD` for its
  two jump tables). m2c needs the rodata jump table with `.word .L8019xxxx`
  labels (raw words make m2c assert).
- m0256i then ran into a host SIGFPE: the existing door/lift step
  `func_80192680` divides by its duration field (+0x34), which is 0 in the
  warp probe. The lowering emitted a plain C `/`, which is undefined on a zero
  divisor and traps on x86. Fix: runtime `/` and `%` in lowered TUs now go
  through `pe_low_divs/divu/mods/modu`, which give the R3000 results
  (INT_MIN / -1 wraps) and treat a zero divisor as what retail's divide check
  does there (`break 7`): `GUEST_TRAP break 7 (integer divide by zero) in
  <fn>` and a `guest-trap` STOP. Constant non-zero divisors stay plain.
  m0256i now stops cleanly with
  `GUEST_TRAP ... in room_6bdd1ee9__func_80192680`. Why the duration is 0
  (warp-skipped setup or wrong upstream state) is still open.
- m0432i: `func_80193414` (0x52C) matched and carved. The room now runs
  past it and stops at the EXE helper `func_800D1384` (called from the
  lowered 80193414 with a0=0x801FEBC0, a2=0x3F2): needs EXE C next.
- m0349i: `func_8018F7C4` (0x90C, emitter callback) matched and carved; it
  has retail's `break 7` divide checks, so it is assigned
  `era_o2_g0_expand_div`. The first probe then hit
  `GUEST_TRAP ... in func_8018F7C4` because the effect dispatcher passed only
  2 arguments: retail D413C calls `h(mode, rec->a1, vm->f8)`, so the third
  argument (the effect descriptor) is now passed for generated callbacks and
  boundary STOPs (`func_800D413C_port.c`). m0349i now stops at the next
  unported effect callback `0x8018F010` (the asm block at blob offset 0x28).
- m0159i: room config created (`newroom.py`, manifest entry, profiles) with
  the twin ports `func_8018F008` and `func_8018FDF8` carved
  (EXACT_REBUILD_GATE PASS). It still stops at `func_8018F3A4`, which is
  matched (LINK_EXACT, `era_o2_g0_expand_div`) but not yet carved: it also
  needs RTPT (GTE op 0x30) in the lowering and in `PE_GTE_Command`.
- Gates for this increment (tag e22b): Day 1 route PASS (story 80), Day 2
  movies PASS, 1636/1636 tests, both guards PASS, unit tests OK.
- m0159i `func_8018F3A4` (0xA54, burst effect with a GTE line) carved
  (`era_o2_g0_expand_div`). Matching notes: `if (!x) return 1; break;` keeps
  retail's branchy tails (gcc otherwise uses store-flag or cross-jumps two
  identical tails), and a barrier after the D_800F3370 store keeps the
  `p[0x10]` reload after it. The lowering now also accepts RTPT (cop2
  0x0280030, sf=1 lm=0) and reads of SZ0-SZ3 (`mfc2 $19` in `gte_stszotz`);
  `PE_GTE_Command` runs RTPT through `PE_GTE_RTPT_coordinates`. Other RTPT
  variants (lm=1) stay refused. m0159i now runs past F3A4 and stops at the
  effect callback `0x8018F008`: its group (twin of m0104i) was refused by the
  lowering ("value of void callee func_800CEE20 used"), so no TU exists yet.
- m0349i `func_8018F010` (0x7B4, the particle task the emitter starts)
  matched and carved; m0349i is now all C and runs clean to the frame limit
  (it takes its scripted exit; the token moves on). Matching notes:
  `p[9]++` first, `p[5] *= -1` (keeps retail's sign-extend before the
  negate), and D_800F3374 as an array keeps its store after the p[7] load.
- Lowering: a call used as a statement no longer refuses when the guest
  prototype says `int` but the host function is `void` (vendor TUs declare
  `int func_800CEE20();`); using the value is still refused. And
  `func_800C4FC4` (ring glow renderer, hand port) now has a shim-header
  prototype, so callers can lower. That clears 15 of the 20 asm refusals
  (8 void-callee, 7 no-prototype). Room TUs 2590 -> 2610; no compile-gate
  drops. Probes: m0104i, m0107i, m0111i, m0113i, m0116i, m0122i, m0156i,
  m0174i, m0263i, m0348i, m0391i, m0392i, m0421i, m0424i and m0005i all run
  clean to the frame limit. m0159i now runs past its F008 callback and stops
  at the EXE helper `func_800D2104` (called from F008's rise draw).
- The last 5 asm refusals now lower (room TUs 2610 -> 2615, no compile-gate
  drops, no asm refusals left): comments are stripped before the asm passes
  (m0141i quoted an `asm(...)` in its header comment); a `register`
  declarator list with several pins (`register int m0 asm("$12"), m1
  asm("$13")`) loses all of them, not just the first (m0273i; single pins are
  unchanged, so no EXE TU drifts); an empty-template asm with
  side-effect-free inputs only (an `"m"` fence) is dropped (m0418i); and an
  `asm("SYM")` label on a plain function declaration counts as extern
  (m0418i). m0141i, m0273i and m0418i run clean to the frame limit.
- EXE `func_800D2104` (0x26C, glow point sprite: RTPS, TILE_1 with the
  colour scaled by scale/128, optional draw-mode word) is now a hand port
  (`pc_port/game/boot/func_800D2104_port.c`, written from the retail asm,
  with a unit test on analytic GTE inputs) plus a shim prototype. The
  lowering and `PE_GTE_Command` also accept RTPS (cop2 0x0180001, sf=1 lm=0).
  m0159i now runs clean to the frame limit (it takes its scripted exit), and
  m0104i, m0263i, m0391i and m0392i, which call it too, stay clean.
- Matching notes: gcc 2.7 assumes a struct-member access never aliases a
  fixed-address scalar, so `short *p` indexing (not a struct) plus an array
  global (`D_800942EC[0]`) is what keeps the retail store/load order. Use the
  PsyQ `gte_SetRotMatrix` macro, not register-asm locals, so `$8` stays free
  for later `mflo`. A bare `p[0]` store folds to `*p`, which gcc 2.7 does not
  mark as in-struct, so global loads cannot move above it. Writing it as a
  member store (`((EffectHead *)p)->x = ...`) lets them move, as retail does.
- EXE `func_800D1384` (gradient trail ribbon: shifts the trail, then per
  segment RTPT + RTPS, AVSZ3 depth, a POLY_G4 with a colour fade and an
  optional draw-mode word) is now a hand port
  (`pc_port/game/boot/func_800D1384_port.c`, from the retail asm, with a
  unit test) plus a shim prototype. m0432i runs past it and now stops at its
  effect callback `0x801931B8` (asm in the 3560 block). Gates (tag e22g):
  Day 1 route PASS (story 80), Day 2 movies PASS, 1638/1638 tests, both
  guards PASS, both `--verify` 0 drift.
- m0256i (still `GUEST_TRAP` in `func_80192680`): the durations it divides by
  (+0x28/+0x2A/+0x2C, heights at +0x22/+0x24) are written only by messages
  0x100/0x101 to the 6-argument handler `func_801925AC` (uses a3, a4, a5).
  `PE_GuestCall` carries at most 4 arguments, so that handler has no dispatch
  thunk, and no STOP is seen before the trap. So the message is most likely
  never delivered in the warp probe (or a4/a5 are lost). Open; the trap
  stops the run, so nothing is skipped.
- m0050i still hangs in the probe (no final token, no stop reason). Not yet
  investigated this round.
- m0432i `func_801931B8` (0x25C, the particle callback 80193414 starts:
  damped drift with a floor bounce, then the rotating sprite draw) matched
  and carved (default `era_o2_g0`). Matching notes: `p[9] * 32` (not `<< 5`)
  gives retail's `lh`, and the clut offset written as one ternary keeps the
  early constant 4. m0432i now runs past it and stops at the next effect
  callback `0x80192AF0` (asm block 3560). Gates (tag e22h): Day 1 route PASS
  (story 80), Day 2 movies PASS, 1638/1638 tests, both guards PASS, both
  `--verify` 0 drift, unit tests OK, EXACT_REBUILD_GATE PASS.
- m0050i hang found and turned into a STOP. A gdb backtrace of the hung run
  showed `func_8001220C` spinning. m0050i's script raises the story word
  persist[74] (`D_800A7918`) to 0x2F2. The main loop's disc gate needs the
  disc-1 flag below story 0x258 and the disc-2 flag (`D_800B0DCD` bit 2)
  from 0x258 up, so it calls `func_80069B08(2)`, the retail disc-swap screen
  ("insert disc 2", drive polling, then `D_800B0DCD = 2`). That function was
  a `Bootstrap_ReturnVoid` stub returning without doing anything (a silent
  skip). Later the script's New Game init token (0xAA108448) asked for disc 2
  again; with bit 2 never set, the loop never ended. `func_80069B08` is now an
  honest boundary (`PE_Port_DiscSwapBoundary`): `CPU_BOUNDARY/REFUSED
  func_80069B08 disc swap (insert disc N; story ..., D_800B0DCD ...)` and an
  `unresolved-boundary` STOP, with a unit test. The Day 1 route never calls
  it. Probe: m0050i, m0049i, m0196i, m0226i, m0236i, m0237i, m0238i,
  m0246i, m0248i, m0273i, m0340i, m0343i, m0373i and m0428i all stop there
  (their scripts move the story to 0x260-0x2F2, disc-2 content); before, the
  skip let them run on. m0273i was listed clean above only because of
  that skip. A real port needs the disc-swap screen plus a disc-2 image in
  the host drive model. Gates (tag e22i): Day 1 route PASS (story 80), Day 2
  movies PASS, 1639/1639 tests, both guards PASS, both `--verify` 0 drift.
- m0432i `func_80192AF0` (0x6C8, effect callback: arm with the particle
  pool, three particle bursts, then the camera-RT sprite and two rings)
  matched and carved (default `era_o2_g0`). Matching notes: one variable for
  both cosines; `w = s / 2` assigned inside the func_800CEE20 call so it
  follows func_80077AA4; the palette index (pre-scaled) and the 0x20
  constant read before an `"r"(p)` barrier, and the table load behind a
  second barrier, keep retail's store order. m0432i now stops at its
  particle task `0x80192548` (asm, 0x5A8). That one calls the EXE helpers
  `func_800D2B58` and `func_800D0E88`, which have no native port yet, so
  m0432i will stop at those next. Gates (tag e22j): Day 1 route PASS (story
  80), Day 2 movies PASS, 1639/1639 tests, both guards PASS, both `--verify`
  0 drift, unit tests OK, EXACT_REBUILD_GATE PASS.
- Native disc swap (Disc 2 handoff). `func_80069B08` is now a hand port of
  the retail swap screen (`pc_port/game/boot/func_80069B08_port.c`, from
  `src/func_80069B08.c`): message graphics read into the two buffers named
  by the mount record, CdlStop, then the state machine that waits for the lid
  (status 0x10), waits for it to close, re-mounts (`func_800698D4`) until the
  wanted `PEDISC0N.IDF` answers, sets `D_800B0DCD` to 1 or 2 and waits for
  CdReady. The drive side is a host tray model
  (`pc_port/platform/pe_drive_tray.c`). A second image goes in with
  `--disc2-image` (identified like disc 1). On the swap screen the lid opens
  60 frames after the prompt and closes 60 frames later on the other disc,
  which becomes the active disc. CdlStop uses the SDK queue when the
  register-level CD device is on. With the device off it completes on the
  host (status 2 in the completion ring, the `func_80080DC4` precedent).
  With no image for the wanted disc, the screen is still the STOP above
  (`CPU_BOUNDARY/REFUSED func_80069B08 disc swap ...`). New unit test
  `drive_tray_swap` covers the lid timing, the swap and the CdlStop
  completion that `func_8007F418` reads back.
- First runs crashed after the swap (FATAL invalid guest address in
  `func_8003D050_packets`). Cause: a port divergence in the main loop, not in
  the swap. The swap screen loads its message graphics at 0x8018EFE8, inside
  the room overlay. Retail only reaches the disc gate after `func_8003F3C4`
  returns, at a room exit. The port's `func_8003F3C4` returns once per frame
  (one pass of the retail `D1C4 == D280` loop), so the gate ran every frame.
  A story bump past 0x258 inside a room then started the swap over the live
  room. Fix: `func_8003F3C4` sets `PE_Field_PassContinues` when it returns
  only because of that one-pass rule. The main loop skips the volume gate on
  those ticks and still runs it on every real retail return. The skip chain
  and the 0x100 restart are unchanged. Result: m0050i's bump to 0x2F2 at
  frame 1306 swaps at the room exit (frame 2624), and the next room loads
  from Disc 2.
- Dev warp harness: `PE_WARP_DISC=2` puts Disc 2 in the drive and mounts it
  at the warp, as if the player had already swapped (needs `--disc2-image`).
- Probe with Disc 2 (warp story = each room's own disc-2 story value):
  m0050i, m0049i, m0196i, m0226i, m0236i, m0237i, m0238i, m0246i, m0248i,
  m0273i, m0340i, m0343i, m0373i and m0428i all run clean to the frame limit
  (no FATAL, boundary or trap). m0050i without `PE_WARP_DISC` (Disc 1 in the
  drive, warp story 0x2F2) runs the swap screen at the warp transition and
  runs on. Without `--disc2-image` it still stops at the disc-swap boundary. Gates (tag k2): Day 1 route PASS
  (story 80), Day 2 movies PASS, 1640/1640 tests, both guards PASS, both
  `--verify` 0 drift, unit tests OK.
- m0432i is clean. Three steps:
  - The effect callback `func_80192548` (0x3A4, three effect kinds:
    a bouncing drift spark plus a scaled blade, an orbiting blade and a
    streak) is matched and carved (`EXACT_REBUILD_GATE` PASS). Matching
    notes: the `D_800F33xx` / `D_800942EC` globals declared as arrays keep
    retail's load order. A second `matrix_slot` barrier sits on the
    case 2/2 path. `break` out of the outer switch flips the mode-1 tail.
  - EXE `func_800D0E88` (shaded blade, POLY_G3) is now a native port of
    its matched C (`func_800D0E88_port.c`, unit test `D0E88_blade`) with a
    shim prototype. The lowered room code and `func_800DAF8C` (batch 21)
    call it directly instead of through a boundary.
  - EXE `func_800D2B58` (streak: LINE_G2 plus a POLY_GT4 glow quad
    oriented by ratan2) is split out of its asm block, matched
    (LINK_EXACT, `docs/evidence/func-800D2B58/REPORT.md`, EXE gate
    sha1 452fb033 unchanged) and ported natively (`func_800D2B58_port.c`,
    unit test `D2B58_streak`). The retail quirk where point 1's blue
    channel is scaled by the first brightness is kept. So is the read of
    a NULL second colour (kuseg 0 = RAM).
  - `warp_probe.sh m0432i 90 6000` now runs clean to the frame limit
    (token A8000148, no stop).
  - `warp_probe.sh` passes `DISC2` as `--disc2-image`.
  - Gates (tag k3): Day 1 route PASS (story 80), Day 2 movies PASS,
    1642/1642 tests, both guards PASS, both `--verify` 0 drift, unit
    tests OK.
- m0256i is clean: the root cause is fixed generally.
  - Cause: the hand port of the message dispatcher `func_8006F6D4` called
    only a few known handler addresses. For any other handler it returned
    0 without calling it, a silent skip. So messages 0x100/0x101 never
    reached the 6-argument handler `func_801925AC`, and the durations that
    `func_80192680` divides by stayed 0.
  - Call mechanism: `PE_GuestCallN` carries up to 8 arguments. a4..a7 are
    the O32 stack arguments, which the old 4-argument `PE_GuestCall`
    dropped.
  - Registry: the generator emits 8-argument `thunk8` entries, with their
    parameter count, for callees with 5..8 integer parameters. 245 new
    thunks; Psy-Q-classified EXE bodies are excluded so the registry links
    no Psy-Q code.
  - A site that passes fewer arguments than the callee takes is a loud
    boundary, never a zero fill.
  - The lowering turns 5..8-argument indirect calls into `PE_GuestCallN`.
  - `func_8006F6D4` now forwards every other handler through
    `PE_GuestCallN` with all six arguments and returns the handler's
    result.
  - E21c in the lowering: unread arguments. A leaf that calls an
    unprototyped callee with more arguments than it takes, e.g. the
    6-argument message wrappers `return func_800C2AF0(a0..a5)` that m0034i,
    m0348i and others carry, is lowered with the extras dropped. All three
    conditions must hold: the callee's matched retail definition takes
    exactly the host prototype's count, the dropped arguments are side
    effect free, and the leaf's declaration is unprototyped. The
    analyzer's "call arity" refusal now routes to the lowering, which
    still refuses every other mismatch, including too few arguments.
    About 75 more room TUs are lowered.
  - Day 1 shows why this matters: the M34 boss fight sends messages 4..7 to
    m0348i's `func_8018F098`, which used to be skipped silently. With the
    dispatcher fixed and before E21c, that was an honest STOP on the Day 1
    route. With E21c it runs natively, and Day 1 still ends at the same
    frame and token.
  - New tests: native `PORTABSENT_stack_args` (the m0256i handler through
    `PE_GuestCallN` and through `func_8006F6D4`; a 4-argument site is
    refused), lowering tests (stack-argument call; E21c drop and its four
    refusals), and generator tests (thunk limits, thunk8 entry).
  - `warp_probe.sh m0256i 90 6000` and `m0348i 90 4000` now run clean to
    the frame limit.
  - Gates (tag k6): Day 1 route PASS (story 80), Day 2 movies PASS,
    1643/1643 tests, both guards PASS, both `--verify` 0 drift, unit
    tests OK (191).
- Story-0x90 regression sweep on `356c83c3` (402 slots, 3 at a time,
  4000 frames): 365 clean, 17 stopping, 20 unused. Before this round it
  was about 381 clean. None of the new stops is a wrong result. Each is an
  honest STOP that a silent skip used to hide: with `func_8006F6D4` now
  delivering every message (and about 75 more lowered room handlers), room
  scripts get further and reach code that has no native port yet.
- E2b tied register move. `asm("" : "=r"(a) : "0"(b))` with a != b is
  not a fence: the output is tied to the input, so it computes `a = b`. The
  generator removed it as a fence. Two consequences:
  - Pinned-register leaves were refused (m0049i's weapon callback
    `func_801929B0`, plus 5 more room leaves).
  - Where the target was not a pinned register, the assignment was
    silently dropped. m0273i `func_80194470` read `r` uninitialised for
    its sprite brightness, a latent port bug.
  Fix: such moves are rewritten to `a = b;` before fence removal, and the
  read-before-write check sees the rewrite. With a == b it stays a fence.
  New unit test. No EXE TU changed. Six room TUs are new, and m0273i's TU
  now has `r = t`.
  - m0049i and m0050i get past the weapon callback and reach their scripted
    disc swap (story 0x2EC / 0x2F4).
  - Disc swap rooms with `DISC2` (Disc 2 in the tray): m0049i, m0050i,
    m0288i, m0373i and m0425i run clean. m0427i stops at effect callback
    0x80193018 (same as m0428i). m0259i hits
    `FATAL PE_StoreU16 0x80200000` after its swap (open).
  - Gates (tag k7): Day 1 route PASS (story 80), Day 2 movies PASS,
    1643/1643 tests, both guards PASS, both `--verify` 0 drift, unit
    tests OK (192).
- Remaining story-0x90 stops (Disc 1 only, before this fix):
  - Effect callbacks with no matched C: m0349i and m0429i 0x8019AE8C,
    m0350i 0x801958DC, m0428i and m0427i 0x80193018, m0430i 0x8018F750,
    m0437i 0x80192D74.
  - Missing EXE function: `func_80079754` (RotMatrix) called from room
    `func_8018F644` / `func_8018F630` (m0434i, m0435i, m0436i).
  - Missing EXE function: `func_80073A44` called from room `func_8018F014`
    (m0272i).
  - Disc swap without a Disc 2 image: m0259i, m0288i, m0373i, m0425i and
    m0427i, plus m0049i and m0050i after this fix.
- Host replacements for Psy-Q entries in lowered room code. 54 room call
  sites (m0434i, m0435i, m0436i and others) stopped at `func_80079754`
  (libgte RotMatrixYXZ). The port already has the math
  (`PE_RotMatrix79754`). The guard forbids defining a
  `func_<psyq address>`, so pe_port_compat.h now marks a host function
  `/* host-replacement: func_80079754 */ PE_RotMatrixYXZ_Guest(r, m)`.
  The overlay generator gives such replacements to the room lowering only,
  and the lowering calls the host name. The 54 boundaries are gone, the
  guard stays clean, and there is a new unit test
  (`INIT1_rotmatrix_guest`).
- The overlay generator now treats header `static inline` shims (VSync
  `func_80073A44` -> `HostFB_VSync`) as implementations, as
  gen_decomp_ports already did. That removes m0272i's VSync boundary and 4
  subsystem-overlay VSync boundaries.
  - m0434i, m0435i and m0436i run clean. m0272i reaches its scripted disc
    swap (Disc 1 only).
  - Gates (tag k8/k8b): Day 1 route PASS (story 80), Day 2 movies PASS,
    1644/1644 tests, both guards PASS, both `--verify` 0 drift, rebuild
    check no work, unit tests OK (194).
- m0259i FATAL (`PE_StoreU16 0x80200000` after its swap) fixed. At story
  0x2A6 the room's script exits the field with the destination unchanged
  (A80454C8 again), and the volume gate swaps to Disc 2. Retail's next
  `func_8003F3C4` entry runs `func_8003F074`, which loads D280 again. The
  host calls `func_8003F3C4` once per tick and change-gates that load, so
  the reload was skipped. The bone walk (`func_8003A088`) then read a
  model header from clobbered memory (count 20152) and its output cursor
  ran off the end of RAM. Fix: a real field exit (the epilogue) clears the
  host's loaded-dest gate, so the next entry loads as retail does. The
  one-pass ticks inside retail's loop keep the gate. New unit test
  `FLD1_3F3C4_exit_reloads_dest`. With Disc 2 in the tray, m0259i now
  reloads from Disc 2, reaches story 0x2A8 and room A80470C8, and runs
  clean to the frame limit.
  - Gates (tag k9): Day 1 route PASS (story 80, same frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift, unit tests OK.
- Story-0x90 sweep on `cb84e7ef` with Disc 2 in the tray (402 slots, 3 at
  a time, 4000 frames): 374 clean, 8 stopping, 20 unused. m0256i is clean.
  Stops: m0259i (fixed above), m0350i, and the effect callbacks below.
- E2b asm outputs. 26 vendor room leaves were refused as "pinned register
  x read before written". Their first use of the pin is an asm output
  operand, which writes it: `mfc2 %0,$9 : "=r"(x)` (GTE reads, 20
  leaves), or a two-output tied move
  `asm("" : "=r"(a), "=r"(b) : "0"(c), "1"(d))` (6 leaves). The guard now
  treats an `"=r"` output as a write unless the same asm also takes the
  variable as an input. Multi-output tied moves are rewritten to
  `a = c; b = d;` when no output is also an input; anything else stays a
  fence and stays refused. Room bodies: 2693 -> 2719 eligible; registry
  5337 -> 5363 thunks; no EXE TU changed. New unit tests.
  - m0350i's effect callback `func_801958DC` (vendor-matched, GTE stsv
    through pinned $12-$14) is one of them. m0350i now runs clean. The 18
    other rooms with newly lowered leaves (m0065i, m0087i, m0107i, m0111i,
    m0122i, m0141i, m0146i, m0153i, m0167i, m0186i, m0391i, m0393i,
    m0399i, m0401i, m0412i, m0414i, m0421i, m0424i) stay clean.
  - Gates (tag k10): Day 1 route PASS (story 80, frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift, unit tests OK (196).
- Remaining story-0x90 stops (all effect callbacks with no matched C):
  m0429i 0x8019AE8C (also reached from m0349i), m0428i 0x80193018 (also
  from m0427i), m0430i 0x8018F750, m0437i 0x80192D74.
- m0429i (also reached from m0349i): two more matched room leaves, both
  LINK_EXACT and written from the retail asm. `func_8019AE8C` is the effect
  callback the stop named: mode 0 opens a 20-entry particle pool, mode 1
  spawns up to 3 sparks around the player actor, mode 2 sets the sprite
  globals. `func_8019AC30` is the spark callback it registers (drift,
  63/64 damping, floor bounce, jitter, rotating sprite). Carved out of the
  asm span 0x3FB4-0xC1F0; preflight PASS (74 c / 3 asm / 1 rodata),
  EXACT_REBUILD_GATE[room_m0429i] PASS (sha1 unchanged). m0429i and m0349i
  now run past both and stop at the next effect callback, 0x80199684.
  - Gates (tag k11): Day 1 route PASS (story 80, frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift. The full `unittest discover` has two failures in areas this
    change does not touch (the m0123i coverage bucket in
    test_pc_port_coverage, and the YAML-syntax case in
    test_disc1_preflight); they are listed as known, not fixed here.
- Day 3 sweep (story 0x140, Disc 2 in the tray, 402 slots, 3 at a time,
  4000 frames, binary 065723a3): 376 clean, 6 stopping, 20 unused. The
  stops are the same effect callbacks as at story 0x90 (m0349i/m0429i,
  m0427i/m0428i, m0430i, m0437i).
- Three more matched room leaves (LINK_EXACT, written from the retail asm):
  - m0428i `func_80193018`, the effect callback m0428i and m0427i stopped
    at (ring radius from the story frame counter, a proximity trigger that
    sets the player flag 0x4000, two rings via func_800D0728). It was the
    whole 0x4030 asm span. m0428i and m0427i now run clean.
  - m0429i `func_80199684` (effect callback: three bone points, two
    gradient segments, a 20-entry particle pool) and `func_801991F8` (its
    particle callback). m0429i and m0349i now run past both and stop at
    the next callback, 0x8019A054.
  - preflight PASS for both rooms (m0429i 76 c / 4 asm / 1 rodata, m0428i
    59 c / 3 asm / 1 rodata), EXACT_REBUILD_GATE PASS for both; room
    bodies 2721 -> 2724, registry 5365 -> 5368 thunks.
  - Gates (tag k12): Day 1 route PASS (story 80, frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift.
- Disc 2 sweep (story 0x300, PE_WARP_DISC=2 so Disc 2 is in the drive at
  the warp, 402 slots, 3 at a time, 4000 frames, binary before the three
  leaves above): 377 clean, 5 stopping, 20 unused. Stops: m0349i/m0429i
  (0x80199684), m0427i/m0428i (0x80193018), m0437i (0x80192D74). m0430i
  is clean at this story point. With the leaves above, m0427i/m0428i are
  clean and m0349i/m0429i move on to 0x8019A054.
- m0429i (also reached from m0349i): the last two leaves in the room's asm
  span, both LINK_EXACT and written from the retail asm.
  `func_8019A054` is the effect callback the stop named (mode 0 opens a
  particle pool, mode 1 spawns sparks on a rotating angle, mode 2 builds a
  GTE matrix and sets the sprite globals). `func_80199C28` is the particle
  callback it registers. The asm span is now fully carved; preflight PASS
  (78 c / 3 asm / 1 rodata), EXACT_REBUILD_GATE[room_m0429i] PASS; room
  bodies 2724 -> 2726, registry 5368 -> 5370 thunks.
  - Re-probe with this binary (4000 frames each), m0427i/m0349i/m0428i/
    m0429i/m0430i/m0437i: at story 0x140 only m0430i (0x8018F750) and
    m0437i (0x80192D74) stop; at story 0x300 (PE_WARP_DISC=2) only m0437i
    stops. m0349i and m0429i are clean at 0x90, 0x140 and 0x300.
  - Gates (tag k13): Day 1 route PASS (story 80, frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift.
- Final story-0x90 sweep with the binary above (402 slots, 2 at a time,
  4000 frames): 380 clean, 2 stopping, 20 unused, nothing else. Stops:
  m0430i 0x8018F750, m0437i 0x80192D74.
- m0437i gets a room config (slot 436, PE.IMG sectors [0x18889,0x18924),
  size 0x4D800, text window 0x1E4-0x4108, grouped with m0413i). doroom
  ported 45 leaves as masked twins of matched functions in other rooms
  (all LINK_EXACT). The stopping effect callback `func_80192D74` is matched
  from the retail asm (LINK_EXACT, profile era_o2_g0_expand_div because the
  retail modulo carries the divide-by-zero / overflow break checks).
  Preflight PASS (46 c / 10 asm / 1 rodata), EXACT_REBUILD_GATE[room_m0437i]
  PASS; registry 5370 -> 5416 thunks. m0437i now runs past it and stops at
  its particle callback 0x80192500 (mode 1); m0413i stays clean.
  - 0x80192500 (0x874 bytes) is not matched yet. The draft is close in
    shape (mode 1 nearly exact) but gcc 2.7's CSE still hoists the two
    colour addresses into saved registers, which retail does not do, and
    the register allocation in mode 2 differs as a result. m0413i carries
    a byte-identical copy at the same address (also unmatched).
  - Gates (tag k14): Day 1 route PASS (story 80, frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift.
- 0x80192500 progress (no code change in this push; draft kept in box
  scratch, not in git). Word mismatches 520 -> 48, diff lines 139 -> 34.
  What the remaining gap taught us about gcc 2.7.2 (checked against its
  `cse.c` / `sched.c` / `local-alloc.c` sources):
  - The colour-address hoist comes from CSE path following: the prologue
    pseudo for the struct-copy destination `(plus fp 64)` is reused along
    the followed path prologue -> mode 2 -> inner case, and combine then
    cannot fold the single-use copy address. Retail breaks that path with a
    loop note in the mode-2 head (a `do { ... } while (0)` around the
    `gte_SetRotMatrix`, i.e. a macro). That removes the hoist and gives
    retail's 104-byte frame and its `$19`/`$7` compare registers.
  - Retail's case 1/2 stores run after the table loads because the
    D_800F3368..78 block is a struct (the scheduler treats a fixed struct
    field and a varying array element as possibly aliasing). Case 0 keeps
    plain scalars. Case 0 uses its own value/clut variables, and each
    matrix-slot load uses its own pointer variable.
  - Still open: retail's `$8` for the matrix pointer while `mfhi`/`mflo`
    also use `$8` (any `register asm("$8")` keeps the allocator off `$8`
    for the whole function), the mode-1 `p[8] == 1` compare register, and
    where the `la $16` of the case 1/2 sprite pointer is scheduled.
  - Follow-up: dropping the register variable and passing `*matrix_slot`
    straight to the macro reproduces retail's head exactly (`la $3` /
    `lw $8,0($3)`). The `$8` is a reload register (reload hands out hard
    registers no pseudo uses first), and `mfhi`/`mflo` go back to `$8`.
    The cost is case 0's first `gte_SetRotMatrix`: retail reuses `$8`
    without a reload, but reload inheritance and `find_equiv_reg` both stop
    at the case label. So retail must hold the pointer in a real pseudo
    that global-alloc put on `$8`. An ordinary pseudo lands on `$3`, and
    forcing conflicts with an empty clobber asm moves `mode` off `$7`
    instead (the pointer outranks `mode` in allocation priority).
  - Gates (tag k15, docs-only push): Day 1 route PASS (story 80, frame
    67186), Day 2 movies PASS, 1645/1645 tests, both guards PASS, both
    `--verify` 0 drift, exact rebuild PASS (sha1 452fb033).
- m0430i `func_8018F750` (0xF9C bytes, the story 0x90/0x140 stop) is
  matched from the retail asm: LINK_EXACT under a local profile
  `room_m0430i_dispatch_8018F00C_expand_div` (header jump table
  0x8018F00C folded through a new `[0x24, rodata]` subsegment, plus
  retail's divide-by-zero / overflow checks). It is the 3-mode effect
  callback: arm (anchor copy, model 0xC5543704, paired sprite), a 7-state
  per-frame update (random heading, ease out, hold, ease to the slot
  target, spark burst and 12-spark ring, floor-hit test, fade) and draw.
  Matching levers, all gcc 2.7 scheduler / allocator effects:
  `D_800F3368` is a struct only where a store must stay after a table
  load; `D_800B0E64` is read as an array element for the same reason;
  one local doubles as the draw flag so the 1024 scale constant lands in
  `$17`; the mode-0 statement order came from a 720-permutation search.
  Overlay EXACT_REBUILD_GATE[room_m0430i] PASS (sha1 475d8d7b), preflight
  PASS; registry 5416 -> 5417 thunks.
  - Probe (4000 frames): m0430i now runs past it at story 0x90 and 0x140
    and stops at its particle callback `0x8018F028` (mode 1, 0x728 bytes,
    the last unmatched function in the room's asm span). Draft in box
    scratch: structure right, register assignment not yet (see below).
  - 0x8018F028 notes so far: the struct-copy address hoist from
    0x80192500 shows up here too (the `&col1` copy address lands in
    `$fp`), and a loop note in mode 2 case 1 removes it. The `bgez` delay
    slot `addiu $20,$0,64` means the 64 is its own variable set before the
    `func_80077DC4` call (sched1 sinks it), not a reuse of `a`.
  - Gates (tag k16): Day 1 route PASS (story 80, frame 67186), Day 2
    movies PASS, 1645/1645 tests, both guards PASS, both `--verify` 0
    drift, exact rebuild PASS (sha1 452fb033).
  - 0x8018F028 progress (12:30 PM CT, docs-only): 39 -> 33 word mismatches
    left, every global-alloc register now matches retail (`p` $18, `b`
    $19, `c` $17, `a`/the 64 local $20, the `D_800E11EA` table pointer
    and the reused 4 constant $21). Findings: a pin written
    `x = K; asm("" : "=r"(x) : "0"(x))` counts as 5 refs in the flow dump,
    which lifts the pointer above `a` in allocation priority. Seeding the
    asm directly (`asm("" : "=r"(ix) : "0"(&K[3]))`) leaves 3 refs and gives
    retail's order. Retail's signed `lh` for `rot.vz` means `p[9]` is read
    a second time into an int local after a stack store, so CSE cannot
    merge it with the first read. `D_800E120A` is read into a local at the
    top of case 1 (retail keeps it in `$5`). Left: scheduling order in the
    case 1 head (search running) and two `$2`/`$3` swaps elsewhere.
  - 0x8018F028 progress (1:10 PM CT, docs-only): 33 -> 12 word mismatches,
    all in the first 30 insns of mode 2 case 1. Everything outside that
    window now matches retail. Fixes, all gcc 2.7 allocator or scheduler
    effects:
    `matrix_slot` split into one block-local pointer per case (shared, it
    was a global pseudo that lost `$2` to a local), one `value` local for
    case 1 apart from case 0's (global vs local `$2`), and a block-local
    asm-seeded copy of `a` at the top of the `p[9] < 8` branch. That copy
    keeps `$4` live across the sign temp, so the temp lands in `$5` and
    reorg can pull `addu $4,$20,$0` into the `beq` delay slot as retail
    does.
    Scheduler notes from the gcc 2.7.2 sources: `schedule_select` breaks
    equal-priority ties by potential function-unit hazard, so loads sink
    as far as their dependences allow in the backward list scheduler. A
    load stays high only if a later store may alias it, and a fixed-address
    scalar never aliases a struct field written through a pointer
    (MEM_IN_STRUCT_P rule), so retail's early `D_800E120A` read is an
    array element. Loop notes and volatile asm are full sched barriers.
    Remaining work: the scheduling order of that case 1 head (search
    running).
  - 13:45 CT: no asm pins left in the case 1 head; down to 16 words.
    Retail never pinned the pointers. `sp = &D_800F3368; sp->f0 = 32;`
    stays in `$16` because the later read is written `D_800F3368.f2`:
    expand puts that constant address through a pseudo, and CSE's
    related-value lookup rewrites it as `sp+2` (`lh $3,2($16)`). The
    absolute stores go to the standalone symbols (`D_800F336A`,
    `D_800F3376`, `D_800F3378`, `D_800F336C/E`, `D_800F3370`); written
    as struct fields they would also turn into `2($16)` etc. The same
    holds for `ix = &D_800E11E4[3]`, with the post-call read written
    `D_800E11E4[11]`, which gives `16($21)`. One loop note
    (`do {} while (0)`) after the f2 store is needed, so combine can't
    fold either pointer back into an absolute address. With no asm in
    the block, reorg fills the AA4 delay slot with `$4 = 0` as retail
    does (reorg's backward delay-slot search stops at any asm insn).
    Left: the order of the `x`/`d` loads in the top block, the `vz`
    store in the bottom block, and a `$21`/`$22` swap between `ix` and
    the shared constant 3.
  - 13:55 CT: the `d` load now lands where retail has it (15 words, 8
    diff lines). In retail, `lh $17,18($18)` (`p[9]`) is scheduled above
    the `rot.vx` stack store. Under gcc 2.7.2's alias rules that is only
    possible if the store is not MEM_IN_STRUCT (a varying in-struct
    read never aliases a fixed non-struct write). So the case 1 `rot`
    stores are written `*(short *)&rot.vx = ...`; the inner object is a
    scalar field, so the MEM isn't flagged in-struct. Left: the
    `D_800E120A` (`x`) load still sinks below `lh $6` (in sched1 it births
    a pseudo, so it gets max priority and sinks), and the bottom-block
    `addiu $22,3` and `vz` store are out of order. Order search running.
  - m0437i 0x80192500 (still stopping): best draft is 48 words. Retail
    keeps `mode` in `$7` (copied in the prologue, then reused for the
    constants 1/2 in the dispatch compares), keeps the head matrix
    pointer in `$8` (`lw $8,0($3)`, reused by case 0's first GTE op),
    and points `$16` at `D_800F3368+8` (f8) in cases 1/2, with
    `lh -6($16)` for the post-call read. Not matched yet: `$8`
    allocation (a register asm variable forces `mfhi $9`) and the
    case 2 f8 base pointer.
