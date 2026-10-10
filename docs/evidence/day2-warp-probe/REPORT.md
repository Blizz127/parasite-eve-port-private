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
No release (waiting on Blizz).

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
  - 14:40 CT: func_8018F028 LINK_EXACT (0x728 bytes, default era_o2_g0).
    The remaining 7-word mismatch was a `$21`/`$22` swap between `ix` and
    the shared constant 3. Splitting `i0 = ix[0]` so that load is the
    first insn after the case-1 loop note pinned it first (retail order),
    and `register unsigned short *ix asm("$21")` put `ix` back on `$21`
    (the 3 takes `$22`). Overlay EXACT_REBUILD_GATE[room_m0430i] PASS
    (sha1 475d8d7b), preflight PASS; rooms registered 5417 -> 5418
    thunks. Gates (tag k17): Day 1 route PASS (story 80, frame 67186),
    Day 2 movies PASS, 1645/1645 tests, both guards PASS, both
    `--verify` 0 drift, exact rebuild PASS (sha1 452fb033). Next: probe
    m0430i at 0x90/0x140, then clear m0437i 0x80192500.
  - Probe: m0430i clean at story 0x90, 0x140, and 0x300 (PE_WARP_DISC=2);
    stop_reason=frame-limit on all three. Room's asm span is fully carved.
  - 14:50 CT: m0437i 0x80192500 down to 40 words (from 48). Case 2 now
    keeps `$16` as the f8 base via a do{}while(0) after `s = &D_800F3368.f8`
    (the asm pin alone was not enough once case 1 already pinned it).
    Still open: `beq $3,$7` (mode-1, p[8]==mode) vs our `$19`; head slot
    `$3` vs `$2`; `mfhi`/`mflo $8` vs `$9` (the `register asm("$8")` on the
    matrix pointer); case 1 `la $16` placement; case 2 absolute vs
    `-6($16)` for the f2 read and absolute vs `-4($16)` for the f4 store.
  - 15:00 CT: m0437i 0x80192500 down to 26 words. Case 2 now uses a
    loop note after `s = &D_800F3368.f8` plus a second one before the
    f10 store (hill climb over statement order and struct vs standalone
    symbols), so `$4 = 0` fills the dispatch delay slot and the f8 store
    and `-6($16)` read go through the base as retail does. Left: mode-1
    `p[8] == 1` compare (`$7` vs our `$19`), head slot register, the
    `$8` matrix pointer vs `mfhi`/`mflo $8`, case 1 `la $16` placement,
    and case 2's shared constant 2 (`$3`, first in the block).
  - `$8` analysis (gcc 2.7.2 reload1.c): the `mfhi`/`mflo` moves are
    reloads (the high-part pattern leaves the value in HI), and
    `order_regs_for_reload` hands out the lowest call-clobbered register
    no pseudo uses. A `register asm("$8")` variable marks `$8` explicitly
    used, so the reloads move to `$9`. Dropping the register variable
    gets `mfhi`/`mflo $8` back, and an empty `asm volatile` clobbering
    `$3`-`$7` in case 0 (where `mode` is already dead) puts the matrix
    pointer on `$8` as an ordinary pseudo. That matches the head and
    case 0, but then `$8` has pseudo uses and the reloads take `$9`
    again. `spill_hard_reg` only leaves a pseudo in place if it is local
    to one block, and this pointer spans head -> case 0, so retail's
    exact mechanism for sharing `$8` is still open.
  - 21:13 CT (work done 15:05-15:30 CT, then a 6 h stall): m0437i
    func_80192500 LINK_EXACT (overlay exact gate PASS, room SHA-1
    unchanged). The `$8` reuse came from retail's rotation-matrix macro: its
    asm text loads `$8` in one statement and the next macro reads it, and
    neither tells cc1. Because cc1 never sees `$8` in use, it gives `$8` to
    the mfhi/mflo reloads. The last 16 words were constant placement: the
    2 / 16 stores in cases 1 and 2 are reloads that sched2 hoists ahead of
    `la $16`, which empty tied asms with constant inputs reproduce
    (`asm("" : "=r"(k16), "=r"(s) : "0"(16), "1"(&D_800F3368.f8))`).
    Generator changes (with tests):
    - guest_lower: a GPR an asm reads without writing or declaring it is
      carried in a function-scope `pe_hrN`. It is refused unless an
      earlier asm wrote it with no call in between. Retail is checked:
      only branches separate the `$8` load from its reuse.
    - gen_decomp_ports: tied moves with constant or address inputs now
      become assignments. Before this they were deleted as fences, which
      left m0348i func_801918E8's `b` uninitialised in its port (stores
      through `b` went to a garbage address). That port now sets
      `v_b = 0x80192E8C`.
    m0437i now runs to the frame limit at story 0x90, 0x140 and 0x300
    (Disc 2). Full gates running before the code commit.
  - 21:51 CT: code commit fe6a4f0c (gates k18: Day 1 route PASS, story 80
    frame 67186; Day 2 movies PASS; 1645/1645 tests; both guards PASS;
    EXACT_REBUILD_GATE PASS; both `--verify` 0 drift). Full sweep with
    that binary (402 slots, 2 at a time, 4000 frames), story 0x90: 382
    clean, 20 unused (the same 20 slots fault at 0xFC913DA8 as before),
    0 stopping. 0x140 and 0x300 (Disc 2) are running.
  - 22:09 CT: story 0x140 sweep at 373/402 slots: 353 clean, 20 unused,
    0 stopping so far. Gates are being re-run (k19) for a release-candidate
    stage. Staged only, not published: a public release needs Blizz's yes.
  - 22:24 CT: story 0x140 full sweep: 382 clean, 20 unused, 0 stopping.
    Gates k19 (same code as fe6a4f0c) all PASS: Day 1 route (story 80,
    frame 67186), Day 2 movies, 1645/1645 tests, retail_data_guard,
    psyq_port_guard, both `--verify` 0 drift, EXACT_REBUILD_GATE. A
    release candidate is staged (not published) under
    /workspace/pe/rel/outfe6a4f0c. The story 0x300 (Disc 2) sweep is
    running.
  - 22:45 CT: story 0x300 full sweep (PE_WARP_DISC=2, same binary): 382
    clean, 20 unused, 0 stopping. So all three story points are at
    382 / 20 / 0, and no rooms stop.
  - Release candidate (staged only; publishing needs Blizz's yes):
    /workspace/pe/rel/outfe6a4f0c/parasite-eve-native-only-fe6a4f0c-linux-x64.tar.gz,
    sha256 1f3e4bfbfc6483aaacb70aadcb4092896a3be441d2c8ca67d8e507d5247ed27b.
    Built by pc_port/package_preview.sh (rel/shim on PATH) from the
    build/pcbuild-disc Release binary of fe6a4f0c. The package holds 4
    files and no disc data (`contains_game_data: false`). Guards on the
    package:
    - retail_data_guard: OK, no retail byte runs.
    - psyq_port_guard: 0 new. The 445 allowlist entries show as stale
      only because the binary is stripped; the unstripped binary passes.
    notes.md (with the `--disc2-image` swap) and changelog_entry.md are
    drafts in the same folder.
  - 23:01 CT: harder testing (private branch, no release; RC fe6a4f0c
    stays staged, not published).
    - Story map past Day 3 (from the 247 immediate writers of persist[74]):
      the m0352i hop sets 0x1B0 (Day 4, then m0098i), 0x220 (Day 5) and
      0x2D6 (Day 6, then m0425i/m0006i). m0050i/m0373i write 0x2F2/0x2F4
      at the disc swap. Specials: 0x3E8 (m0318i) and 0x7D0 (m0431i).
    - PE.IMG is byte-identical on Disc 1 and Disc 2 (same LBA and sha1),
      so room code is the same on both. Disc 2 only changes movies and
      the swap. Every earlier PE_WARP_DISC=2 log shows Disc 2 mounted.
    - Full sweeps (frozen fe6a4f0c binary, 2 at a time, 4000 frames) at
      0x1B0, 0x220, 0x2D6 (Disc 1) and 0x2F4 (Disc 2) are running. At
      0x1B0, 168/402 slots are done: no stops apart from the known unused
      slots that fault at 0xFC913DA8.
    - New Day 2 multi-room route gate, `pc_port/tools/day2_route_regress.py`.
      Each segment is one dev warp; after that the game takes its own
      exits:
      - day2_start: m0351i (story 0x80) to m0042i, then m0041i.
      - day2_end: m0090i (0x128) to m0136i, m0126i, m0125i, m0091i,
        m0351i (end-of-day save screen, cancelled with Circle), then
        m0092i at Day 3 (story 0x140).
      Story must never drop. Checkpoints must come in order. Any
      CPU_BOUNDARY, FATAL or [STOP] fails the gate. Three runs all passed
      at the same frames (3592 and 18202), so it is deterministic. Added
      to the gates script. The PE_WARP_ROUTE dev-harness chain in
      plat_cheats.c ships with it. Full gates (k21) are running before the
      code commit.
  - 23:26 CT: code commit 1c3aa777: the Day 2 route gate
    (day2_route_regress.py) and the PE_WARP_ROUTE harness. Gates k21 all
    PASS:
    - Day 1 route (story 80, frame 67186) and Day 2 movies;
    - DAY2_ROUTE (frames 3592 and 18202);
    - 1645/1645 tests;
    - both guards, both `--verify` (0 drift) and EXACT_REBUILD_GATE.
  - 23:49 CT: later-story sweeps (fe6a4f0c binary, 4000 frames):
    - 0x1B0 (Day 4): 382 clean, 20 unused, 0 stopping.
    - 0x220 (Day 5): 382 clean, 20 unused, 0 stopping.
    - 0x2D6 (Day 6): running.
    Unused slots are now counted by name: the same 20 slots, all faulting
    in the room loader (func_8006B4F8) on the warp frame. At 0x2D6 the
    garbage address is 0x8F2ECF29 instead of 0xFC913DA8, but the code
    path is the same.
  - 00:18 CT: story 0x2D6 (Day 6) sweep: 381 clean, 20 unused, **1
    stopping**: m0350i, a CPU_BOUNDARY at pc 0x80192E4C. That is an effect
    callback (`func_800CE560` pool task) still in asm (room span 0x3E64,
    0x7C0 bytes, inline GTE). Mode 2 does three things:
    - draws a sprite trail down to the floor (func_800CEE20) and two
      func_800D0E88 effects;
    - draws a func_800D0728 model;
    - runs a 4-edge nclip test on the player's position, and if the
      player is inside, flags the actor.
    A C match is in progress; the GTE loops and structure already line
    up. The 0x2F4 (Disc 2) sweep is running.
  - 00:41 CT (2026-10-09): story 0x2F4 (Disc 2, PE_WARP_DISC=2) sweep:
    382 clean, 20 unused, 0 stopping. Totals per story point (402 slots
    each, 4000 frames): 0x1B0 382/20/0, 0x220 382/20/0, 0x2D6 381/20/1
    (m0350i), 0x2F4 382/20/0.
    - m0350i func_80192E4C is still open. The best C draft is 7 words off
      retail (size exact). What is left is two scheduling placements in
      the second sprite call. The draft is saved, not installed, at
      `wip/m0350i_func_80192E4C.c`.
    - It needs a new opt-in maspsx narrowing, K3c
      (`MASPSX_NARROW_SHIFTED_ANY_DEST`). Without it, retail's
      `lh $2,2($2)` / `sra $18,$2,5` cannot come out of cc1's
      `lw` / `sra $18,$2,21`.
      - K3c has a unit test and went in after the k22 gate run. The other 12 maspsx unit
        failures/errors were there before this change.
      - Nothing turns the flag on, so builds don't change:
        EXACT_REBUILD_GATE[room_m0350i] passes at the same sha1 (e9597b00).
    - Until the match lands, m0350i at Day 6 (0x2D6/0x2DA) stops cleanly
      with CPU_BOUNDARY. That is the expected stop, not a crash.
    - 12000-frame soaks of the busy rooms (19 on Disc 1, 4 on Disc 2;
      bosses and cutscene rooms at their own story points) are running.
  - 01:10 CT: 12000-frame soaks (frozen fe6a4f0c binary, 2 at a time):
    - Disc 1, 19 busy rooms (bosses and cutscene rooms at their own story
      points, 0x128 to 0x2E8): 18 clean to frame 12000. m0350i at 0x2DA
      hits the same known stop (CPU_BOUNDARY pc 0x80192E4C).
    - Disc 2 (PE_WARP_DISC=2): m0050i 0x2F0, m0373i 0x2EE, m0318i 0x2F4
      and m0431i 0x2F4: 4/4 clean to frame 12000.
    - No new stops.
  - 01:10 CT: maspsx K3c (commit b7bc6434) (`MASPSX_NARROW_SHIFTED_ANY_DEST`, opt-in) and
    its test are in. No build profile turns it on yet. Gates k22 all
    PASS:
    - Day 1 route (frame 67186) and MOVIES_DAY2;
    - DAY2_ROUTE (frame 18202);
    - 1645/1645 tests;
    - rdg OK (20439 files) and psyq PASS;
    - gdp 1013 TUs with 0 drift, gop up to date;
    - EXACT_REBUILD_GATE PASS (sha1 452fb033).

  - 01:27 CT: room_m0350i `func_80192E4C` (0x80192E4C, 0x7C0) **LINK_EXACT**.
    Profile `room_m0350i_narrow_any_dest` (-O2 -G0 + K3/K3b/K3c:
    `MASPSX_NARROW_SHIFTED_WORD_LOAD`/`REG_LOAD`/`ANY_DEST`). Matching
    lever for the last 3-word sched window: store sprite param f8 first
    so CSE relates f2/f4/f6 to the D_800F3368+8 base ($s0); an always-
    false `if (f6 == 0 && f4 == 3) return 0;` ends cse1's block so cse2
    rewrites the f2 read as `lh -6($s0)` without refolding, and sched
    sees no alias with the outgoing-arg `andi`/`sw 20($sp)` (retail
    order). EXACT_REBUILD_GATE[room_m0350i]=PASS (sha1 e9597b00, spans
    113 c / 2 asm / 1 rodata / 2 bin). PC port regenerated
    (`func_80192E4C__ab48ed02_port.c`) and rebuilt.
  - 01:27 CT: m0350i warp probes (4000 frames) all clean to frame-limit
    at story 0x2D6, 0x2DA, 0x90, 0x140, and 0x300. The Day 6
    CPU_BOUNDARY at pc 0x80192E4C is gone. 12000-frame soak at 0x2DA
    and a full 0x2D6 re-sweep (402 slots, 2 at a time) are running.
  - 01:28 CT: m0350i 12000-frame soak at story 0x2DA: clean to frame-limit
    (same binary as the match). Spot probes of former stop rooms
    (m0049i, m0089i, m0123i, m0256i, m0272i, m0349i, m0429i, m0437i) at
    0x90 all clean; m0049i and m0437i also clean at 0x2D6. Full 0x2D6
    re-sweep (402 slots, 2 at a time) in progress.
  - 01:55 CT: full story 0x2D6 re-sweep (new binary with func_80192E4C,
    402 slots, 2 at a time, 4000 frames): **382 clean, 20 unused, 0
    stopping**. m0350i is frame-limit (was the sole Day 6 stop). Later-
    story totals are now all 382/20/0: 0x1B0, 0x220, 0x2D6, 0x2F4.
    Story 0x90 / 0x140 / 0x300 remain 382/20/0 from earlier sweeps.
  - 02:00 CT: full gates k23 on the func_80192E4C install (b2dd55a9..e473c197), all PASS:
    - Day 1 route (frame 67186) and MOVIES_DAY2;
    - DAY2_ROUTE (frame 18202);
    - 1645/1645 tests;
    - rdg OK (20441 files) and psyq PASS;
    - gdp 1013 TUs with 0 drift, gop 0 drift (dispatch registry up to date);
    - EXACT_REBUILD_GATE PASS (sha1 452fb033); room_m0350i overlay
      exact rebuild PASS (sha1 e9597b00, 113 c / 2 asm).
    - Late-game sweeps (Disc 2, PE_WARP_DISC=2, 4000 frames, 2 at a time)
      are running at story 0x3E8 (m0318i writer), then 0x7D0 (m0431i). No
      0x300+ story values are written anywhere (max assigned is 0x2F4), so
      these two are the late-game points. 0x3E8 so far: 272 slots, 262
      clean, 10 unused, 0 stopping.
  - 02:14 CT: story 0x3E8 (Disc 2, 4000 frames, 402 slots): **382 clean,
    20 unused, 0 stopping**. 0x7D0 sweep in progress (54/402 clean so far).
  - 02:28 CT: story 0x7D0 sweep (Disc 2) progress: 251/402 slots, 242 clean,
    9 unused, 0 stopping. Queued after it: 12000-frame Disc 2 soaks of
    late-game rooms (m0318i/m0431i/m0351i/m0352i/m0050i/m0373i at
    0x2F4/0x3E8/0x7D0, including m0431i, the last story writer at 0x7D0).
  - 02:41 CT: story 0x7D0 (Disc 2, 4000 frames, 402 slots): **382 clean, 20
    unused, 0 stopping**. Late-game 12000-frame Disc 2 soaks (pcbuild-disc
    binary with func_80192E4C, 2 at a time): m0318i 0x3E8, m0431i 0x3E8,
    m0431i 0x7D0, m0351i 0x2F4/0x3E8, m0352i 0x2F4/0x3E8, m0050i
    0x3E8/0x7D0, m0373i 0x3E8: **10/10 clean to frame 12000**. No new stops.
    All story points are now 382/20/0: 0x90, 0x140, 0x1B0, 0x220, 0x2D6,
    0x2F4, 0x300, 0x3E8, 0x7D0.
  - 02:53 CT: release candidate native-only-72b2325d **staged, not published**
    (needs Blizz's yes) in the box's release staging folder:
    - tarball sha256 02a0f45c2830dde6d4fc16fe7c01958983376823c5dd0d2ba5760f2736c775f9;
    - binary sha256 aad7736d7cc01bc3a42c3d0322cc8f9ff45e6a781b14b19e5bc7a6b6788de927;
    - retail-data guard OK on all 4 packaged files, psyq guard 0 new,
      build-info contains_game_data false;
    - the staged fe6a4f0c is marked superseded-by-72b2325d.
  - 02:53 CT: Day 3 route gate `pc_port/tools/day3_route_regress.py`, using the
    Day 2 gate's harness (one dev warp, then only the room scripts drive):
    - day3_start: m0093i at 0x140, which writes 0x148.
    - day3_mid: m0108i at 0x188, which writes 0x190.
    - day3_end: the m0352i Day-4 hop at 0x1B0 goes to m0098i, which writes 0x1B8.

    All three pass (DAY3_ROUTE=PASS); DAY2_ROUTE still PASS. Most of Day 3
    needs walking or fighting, and the mashing pad can't do that:
    - m0092i and m0360i at 0x140 just wait;
    - m0123i at 0x190 is a fight that the pad loses, which ends in a game
      over.

    So the gate covers only the script-driven stretches. A full Day 3
    route needs recorded pad input, like the Day 1 gate.
  - 03:15 CT: Day 3 walked-route attempt (WIP; harness code not committed yet,
    gates k24 running):
    - Approach: a harness-only Day 3 pilot (`--route-pad` with
      PE_PAD_SCRIPT and PE_ROUTE_WAYPOINTS). After one warp to the Day 3
      start (m0092i, 0x140) it walks per-room waypoints, keyed by room,
      story and visit number, using the Day 1 learned pad->world map. In
      battles it uses the Day 1 battle aid (RouteBattleShim). Otherwise it
      uses the Day 2 gate's Cross/Circle text pad.
    - Walked natively, no stops: m0092i -> m0093i (story 0x148, Daniel
      at the car), m0093i -> m0094i -> m0095i (gun shop) -> m0094i,
      and m0093i -> m0096i.
    - Day 3 gating found in the room scripts: the car exit to the map at
      0x148 (m0093i module 6) only exists when persist[0x19] has 0x08
      (set when the gun shop, m0095i, is entered at 0x148) and 0x10 (set
      by the pharmacy scene, m0097i). The m0096i street exit to the map
      needs story > 0x148. So the route has to visit the gun shop and
      the pharmacy, then go back to the car.
    - Where it stopped: in m0094i Daniel and the NPC in blue follow and
      box Aya in at the gun shop front, and the waypoint pilot can't route
      round moving NPCs yet. Not reached: the pharmacy, the map, the
      precinct (0x160), the park rooms (0x168-0x178), the hospital
      (0x180-0x190), the m0123i fight (0x1B0) and the Day 4 hop. No new
      CPU_BOUNDARY/REFUSED stop so far.
  - 03:44 CT: Day 3 walked route reaches the world map. New stop found, fix
    staged (runtime code waits on gates k25):
    - The pilot now walks m0092i -> m0093i (0x148) -> gun shop m0095i ->
      m0094i -> m0093i -> m0096i -> pharmacy m0097i -> m0096i -> m0093i ->
      Daniel's car -> world map M0000I (frame ~12973). The z=-850 path gets
      round Daniel and the NPC.
    - New stop: `unresolved-boundary` in PE_TransitionUpdate on the first
      M0000I frame. The menu list (801942FC sp+136) is stack residue, and
      M0000I runs on the scratchpad stack (8001220C sets sp=1F8003F8). See
      docs/evidence/transition-menu-tail/REPORT.md and
      pc_port/tools/pe_menu_tail_audit.py (MENU_TAIL_AUDIT=PASS).
    - Fix (exact or stop): proven halves come from the constructor locals,
      the draw pass (80076C34/80076354 saves) and the actuator call.
      Unproven halves become unknown digit groups, which stop only if the
      text draws them. With it, the route runs about 11,000 map frames with
      no stop; native tests 1646/1646.
  - 04:08 CT: k25 full gates PASS on the menu-tail fix (exact rebuild,
    retail_data_guard, psyq_port_guard, gen_decomp/gen_overlay --verify,
    Day 1 route, Day 2 movies, tests 1646/1646, Day 2 route, Day 3 route).
    Committed as c3f63acc. The world-map pad fill (HostPadFill in the M0000I
    loop) is not in that build. It stays uncommitted until the next gate run.
    The Day 3 pilot (route_day3_pilot.h) is harness-only and does nothing
    unless PE_ROUTE_WAYPOINTS is set.
    - Walked route, continued: at story 0x178, map index 7 takes Aya into the
      precinct m0037i. Its entry scene writes 0x180. Map index 7 then reaches
      hospital m0103i. The door to m0100i writes 0x188, and m0100i door 4
      leads to m0107i.
    - New stop: a random battle in m0107i calls effect init 0x8018F1D4
      through 8006F39C, giving CPU_BOUNDARY/REFUSED. The matched leaf
      src/overlays/room_m0107i/func_8018F1D4.c exists, but gen_overlay_ports
      skips it ("call arity func_800C22F8 0!=1"): the leaf calls
      func_800C22F8 bare, and retail passes the caller's live $a0 (the
      effect slot) through to it.
  - 04:27 CT: m0107i stop fixed (gates k26 running). Day 3 walked route now
    reaches m0108i and story 0x190.
    - Fix: guest_lower E21d. A bare `f()` to a prototype-less, one-parameter
      callee forwards the leaf's live $a0. This only applies when the leaf
      takes no parameters, the callee's matched retail definition takes one,
      and the call is the first statement (`f();` or `*f() = <constant or
      global address>`).
    - Retail bytes of room_m0107i 8018F1D4 confirm it: addiu sp / sw ra /
      jal 800C22F8, with $a0 untouched.
    - 5 new guest_lower tests cover the positive case and four refusals. The
      rule emits 33 room TUs (all the `*func_800C22F8() = D_x` effect inits,
      including m0087i and m0107i).
    - Route pilot (harness only): Cross opens targeting when the target list
      is empty, and mode 9 (field after a battle) counts as walkable.
    - The m0107i random battle is won by shooting (two enemies, about 10000
      frames) with two opt-in fixtures. PE_ROUTE_MAXHP=300: the warp keeps
      Day 1 stats (45 HP) and Day 3 enemies hit for 35-117. Infinite ammo
      (cheat F3): the Day 1 handgun runs dry.
    - Next: m0108i -> m0115i -> m0044i -> m0123i fight (0x1B0).
  - 04:45 CT: k26 gates all PASS with E21d and the M0000I menu pad fill:
    Day 1 route, Day 2 movies, tests 1646/1646, retail_data_guard,
    psyq_port_guard, exact rebuild, gen_decomp_ports --verify,
    gen_overlay_ports --verify, Day 2 route, Day 3 route. This push commits
    E21d (guest_lower plus tests), the func_8019234C pad fill
    (HOST_ADAPTED, only active with a pad source) and the pilot.
    - Walked route, continued: m0108i (0x190) -> m0107i -> m0111i. Examining
      the body sets hospital flag persist[0x2C] |= 0x800. The pilot gained
      per-waypoint push timeouts (`~N`).
    - m0100i random battle: the AT bar is full but the target list stays
      empty because the enemies are out of range. The pilot now walks toward
      the nearest live actor in the 0x8009D20C chain while pulsing Cross. The
      battle is won (about 5300 frames).
    - Open: in m0099i, the 0x1000 object at (-494,-321) is examined facing -z
      from the +z side. The diagonal and z=-100/-267 lanes west are walled;
      testing other lanes.
  - 05:05 CT: Day 3 pilot (harness only, no runtime change).
    - m0099i solved. The 0x1000 object (-494,-321) is a type-2 actor with a
      300-unit collision radius, so walking into it from the side never
      faced it. Touching it from due north while pushing -z, with Cross
      pulses, sets p2C 0x1000.
    - Waypoint suffixes now parse in either order (`!N~M`), and a timeout
      also runs the waypoint's action.
    - Battle approach fix: Aya (chain type 0) was counted as the nearest live
      actor, so she never walked toward the enemies. She is now excluded.
    - New debug switches (scratch output only): PE_D3_FLOOR_DUMP dumps the
      live floor mesh (0x8009D1FC) for offline path planning, and
      PE_D3_ACTORS logs the actor chain.
    - Open: one m0100i formation still stalls at about (-326,-2807) with a
      full AT bar and no target; diagnosing.
  - 05:54 CT: k27 gates all PASS: Day 1 route, Day 2 movies, tests 1646/1646,
    retail_data_guard, psyq_port_guard, exact rebuild, gen_decomp_ports
    --verify, gen_overlay_ports --verify, Day 2 route, Day 3 route.
    - Route shim fix (tests/route_reward_sewer_pilot.h): a map reseed now
      restarts the learning window. Before, the old room's last position vs.
      the new spawn was learned as a pad displacement. After the m0115i
      vision returned Aya to m0100i, Up was flipped to point south and Aya
      could not walk north.
    - Pilot: the battle "no target" test ignores stale target-list entries
      (Aya, type 0, seen in m0100i; a bodiless type-4 actor, seen in m0121i).
      Waypoint entries can require persist[0x2C] bits (`TOKEN@STORY&MASK`).
    - Walked route (scratch waypoints, warp m0111i 0x190, fixtures F3 +
      PE_ROUTE_MAXHP=300), with four battles won on the way:
      1. m0111i body (0x800) -> m0107i battle -> m0100i battle -> m0099i
         object (0x1000, then 0x4000000) -> m0115i vision (0x20000) ->
         m0100i.
      2. m0100i battle -> m0116i -> m0121i scene (0x40000) -> m0115i hall,
         door 3 -> m0044i scene (0x400000) -> m0121i battle -> m0122i.
    - New stop: CPU_BOUNDARY/REFUSED at 0x80190444, an m0122i effect called
      via 800D413C, with no matched leaf. Fix staged (k28 gating):
      - room_lane/port.py accepts shared global profiles
        (era_o2_g0_expand_div).
      - func_80190444 is ported as the masked-body twin of room_m0159i
        func_8018F3A4. It is LINK_EXACT, carved at 0x145C, deep preflight
        PASS, and EXACT_REBUILD_GATE[room_m0122i]=PASS (25 c / 0 asm).
  - 06:23 CT: m0122i stop fixed. k28 gates all PASS: Day 1 route, Day 2
    movies, tests 1646/1646, retail_data_guard, psyq_port_guard, exact
    rebuild, gen_decomp_ports --verify, gen_overlay_ports --verify, Day 2
    route, Day 3 route.
    - func_80190444 (m0122i effect, via 800D413C) is a masked-body twin of
      room_m0159i func_8018F3A4. It is LINK_EXACT under the shared profile
      era_o2_g0_expand_div, carved at 0x145C, and
      EXACT_REBUILD_GATE[room_m0122i]=PASS (25 c / 0 asm).
    - room_lane/port.py now accepts shared global -O2 -G0 profiles
      (EXPAND_DIV / THREE_WORD_SYMBOL_STORE only).
    - Gate scaffolding:
      - day3_route_regress.py gains the `day3_walk` segment: warp m0192i
        0x178, then the recorded walk to the m0123i fight, 0x1B0.
      - The walk is driven by the committed waypoint list
        pc_port/tools/day3_walk_waypoints.txt (harness coordinates only).
      - The segment is opt-in until the whole walk passes.
      - run_segment takes a per-segment env.
    - Walked route now reaches m0122i -> m0124i -> m0044i (second scene) ->
      m0123i.
    - Next stop: in the m0123i fight, CPU_BOUNDARY/REFUSED at 0x8019537C.
      The leaf is matched, but gen_overlay_ports refused it ("pointer-global
      store D_801956B8"). Fix in progress.
  - 06:49 CT: m0123i stop 0x8019537C fixed (E21e). k29 gates all PASS: Day 1
    route, Day 2 movies, tests 1646/1646, retail_data_guard,
    psyq_port_guard, exact rebuild, gen_decomp_ports --verify,
    gen_overlay_ports --verify, Day 2 route, Day 3 route.
    - gen_overlay_ports routes the "pointer-global store" refusal to
      guest_lower. Storing an address into a pointer-typed global slot
      becomes an explicit PE_StoreU32 of the 32-bit guest address. Only one
      new room TU results: room_m0123i func_8019537C.
    - Two new tests (test_guest_lower PointerGlobalStore).
    - Walked route: the m0123i fight now runs past 0x8019537C and stops at
      CPU_BOUNDARY/REFUSED 0x80192F0C, the fight effect (mode 0 spawns
      func_80192BDC particles, mode 2 draws four sprites, two rings and a
      ranged beam).
    - Blocker for the m0123i fight: no matched C for this cluster, and no
      masked twin in any room.
      - room_m0123i: func_80192F0C 0x70C, its particle callback
        func_80192BDC 0x330, and the unmatched func_80193618 0xFD4 and
        func_8019251C 0x4E0.
      - EXE: the sprite draw func_800D3BC8 0x39C, which has no C at all.
      - Next: hand-match these, starting with func_80192F0C (decoded) and
        func_800D3BC8.
  - 08:24 CT: m0123i stop 0x80192F0C fixed. k31 gates all PASS: Day 1
    route (story 80), Day 2 movies, tests 1648/1648, retail_data_guard,
    psyq_port_guard, exact rebuild, gen_decomp_ports --verify,
    gen_overlay_ports --verify, Day 2 route, Day 3 route (story 1B8).
    - room_m0123i: func_80192F0C, func_80192BDC and func_8019251C are
      hand-matched (LINK_EXACT, no masked twins) and carved as c rows. Room
      exact gate PASS (65 c / 2 asm / 1 rodata), deep preflight PASS.
    - EXE func_800D3BC8 (sprite billboard) and func_800CF844 (orbit point)
      are native hand ports with tests (D3BC8_sprite, CF844_orbit).
      func_800D3BC8 keeps a 24-word prologue save-batching residual, written
      up in docs/evidence/func-800D3BC8/REPORT.md.
    - Part of this work came from a parallel session in the same tree. That
      session finished without committing, so its files were reviewed and
      gated here (k31) and committed in this push.
    - The wt2 BFM build (/workspace/scratch/wt2) is not this session's and
      was left alone.
    - Walked route: the m0123i fight now runs past 0x80192F0C and stops at
      CPU_BOUNDARY/REFUSED 0x80193618, the boss-model effect called from
      the effect system. Next: hand-match it (0xFD4 bytes, no twin).
  - 09:01 CT: m0123i stop 0x80193618 fixed, the m0123i fight is won, and
    day3_walk reaches story 0x1B0. day3_walk now runs by default. k32
    gates all PASS: Day 1 route (story 80), Day 2 movies, tests
    1649/1649, retail_data_guard, psyq_port_guard, exact rebuild,
    gen_decomp_ports --verify, gen_overlay_ports --verify, Day 2 route,
    Day 3 route. Day 3 ran day3_start, day3_mid, day3_end (1B8) and
    day3_walk (0x1B0 at frame 28458, 75 s).
    - room_m0123i func_80193618 (0xFD4, the boss-model effect) is
      hand-matched (LINK_EXACT, no masked twin) and carved, so every
      m0123i row is now C except one asm. Room exact gate PASS (66 c /
      1 asm / 1 rodata), deep preflight PASS.
    - Psy-Q MulMatrix0 (func_800785D4) gets the host replacement
      PE_MulMatrix0_Guest and the test INIT1_mulmatrix0_guest, the same
      way RotMatrixYXZ was already handled. The generated port of
      func_80193618 has no boundaries.
    - The m0123i fight is Kerberos (heads of 200/300/400 HP; the centre
      head heals the others). With warped Day-1 stats Aya hits for about
      3 HP, so the shim could not win. day3_walk now also sets the 1-Hit
      Kills assist PE_ONE_HIT_KILLS=1. Retail still computes every hit and
      runs its own death and victory path: all three heads die, then
      0x1B0.
    - Waypoint fix: the faster m0107i battle leaves Aya west of a blocked
      line to the door, so the m0107i@188 entry now goes through the open
      east corridor.
    - A continuous walk from the Day 3 start (m0093i@140) also reaches
      0x1B0 (frame 61674) with no stops. It is not in the gate yet.
    - Open: 0x1B0 to 0x1B8. After the fight the pilot has no 0x1B0
      entries, so Aya never talks to Baker. The gate covers 1B0 to 1B8
      with the day3_end warp (m0352i).
    - The wt2 BFM build (/workspace/bfm-b28-wt2, srun.sh) was left alone.
  - 10:06 CT: day3_walk now starts at the Day 3 start (m0093i@140) and plays
    0x140 to 0x1B0 in one run. k33b gates all PASS: Day 1 route (story 80),
    Day 2 movies, tests 1649/1649, retail_data_guard, psyq_port_guard,
    exact rebuild, gen_decomp_ports --verify, gen_overlay_ports --verify,
    Day 2 route, Day 3 route. Day 3 ran day3_start (148), day3_mid (190),
    day3_end (1B8) and day3_walk (0x1B0 at frame 61674, 180 s).
    - New day3_walk checkpoints: m0093i, 0x148, m0096i, m0097i, 0x160,
      m0191i, 0x170, m0239i, 0x178, then the existing precinct to
      m0123i-fight-won (0x1B0) ones.
    - route_day3_pilot.h: env-gated field-actor dump (PE_D3_FIELD_ACTORS)
      for route debugging. It is off by default and doesn't affect the
      gates.
    - Harness note: the first chain (k33) inherited exported pilot env
      (PE_ONE_HIT_KILLS=1 and waypoints) from the shell, so the Day 1 M34
      fight and the 7 retail-damage/assist tests failed. That run was
      thrown out and k33b was re-run with a clean env; all pass.
    - Open: continuous 0x1B0 to 0x1B8. After the fight, m0044i mod2 plays
      the Baker scene and returns to m0121i. The m0124i door to m0044i
      (-1000,0) and the m0122i west doors (about -960,-150) don't open at
      0x1B0, even with Cross. Warping into m0044i at 0x1B0 replays the
      scene. The m0044i mod4 hop to m0352i (reads story==1B0 at 801D9960)
      needs a script condition not yet identified (m0120i reads 1B0 in
      mod0/mod7). The gate still covers 1B0 to 1B8 with the day3_end warp.
    - The wt2 BFM build (/workspace/bfm-b28-wt2) is not this session's and
      was left alone.
  - 10:54 CT: full Day 3 by play. day3_walk runs from the Day 3 start
    (m0093i@140) to story 0x1B8 (m0098i, frame 65002) in one run, with
    no warp after the start. k34 gates all PASS (harness PE_* env cleared
    inside the chain): Day 1 route (story 80), Day 2 movies, tests
    1649/1649, retail_data_guard, psyq_port_guard, exact rebuild,
    gen_decomp_ports --verify, gen_overlay_ports --verify, Day 2 route,
    Day 3 route (day3_start 148, day3_mid 190, day3_end 1B8, day3_walk 1B8).
    - The 0x1B0 condition was not a port bug. m0044i module 1 at story
      0x1B0 starts modules 0/4/2/7 only when persist[0x2C] bit 0x400000
      is set. The m0044i Baker scene before the fight sets that bit
      (module 2 @801D90BC). Module 7's polygon trigger (x -96..1830,
      z 912..2541) then posts mailbox payload 0xC to module 4, whose
      cutscene transfers to m0352i (801DA8D8). m0352i enters m0098i,
      which writes 0x1B8.
    - The bit is never cleared, and both doors into m0044i are closed
      once it is set (m0115i door 3 switches to m0121i; the m0124i door is
      story < 0x1B0 only). The fight's return is the only way in.
    - PE_VM_TRACE of the continuous walk confirms the scene, the bit write
      and the m0044i -> m0123i path at 0x190. The earlier "blocker" runs
      were warps from m0123i with the bit clear, which correctly replay the
      Baker scene. Earlier I also misread p2C=044E1800 as bit-clear.
    - New waypoint entry A8004248@1B0 walks Aya north from the spawn point
      (249,-1459) into the trigger. day3_walk adds checkpoints m0352i,
      m0098i and story>=0x1B8, with frames raised to 90000.
    - 1-Hit Kills stays off by default (assist default 0, unit test
      ASSIST_option_registry); only the day3_walk segment env sets it.
    - The wt2 BFM build (/workspace/scratch/wt2, /workspace/bfm-b28-wt2) is
      not this session's and was left alone.
  - 12:22 CT: Day 4 by play, part 1 (0x1B8 -> 0x1DC and the key hunt). k36
    gates all PASS (harness PE_* env cleared inside the chain): Day 1 route
    (story 80), Day 2 movies, tests 1649/1649, retail_data_guard,
    psyq_port_guard, exact rebuild, gen_decomp_ports --verify,
    gen_overlay_ports --verify, Day 2 route, Day 3 route (day3_walk 1B8).
    - Played from warps 0x1B8 -> 1C0 -> 1C8 -> 1CC -> 1D0 -> 1D3 -> 1D6 ->
      1D9 -> 1DC. Stops fixed by route: the m0127i elevator call panel
      (press before the door), the m0128i panel (second press after 0x1D3),
      and the m0320i bin (passed on the north side).
    - At 0x1DC the m0320i west door is locked: Day 4's basement hunt needs
      key A (item 0xD1, m0325i, persist[0x41] bit 1) and key B (0xD2,
      m0328i, bit 2). Key A walk works; m0327i is reached with it.
    - m0327i battle stop CPU_BOUNDARY/REFUSED pc=0x8019018C
      ra=@800D413C:effect fixed. m0327i uses room_m0137i's text. Its
      particle callback func_8019018C is hand-matched (LINK_EXACT, local
      profile room_m0137i_dispatch_8018F03C_8018F054_div, jump tables split
      out as [0x54, rodata]). EXACT_REBUILD_GATE[room_m0137i] PASS (sha1
      6e146c37). The port is generated with no boundaries. The battle now
      runs and Aya walks on into m0328i. Details:
      docs/evidence/room_m0137i-ports-2026-09-23/REPORT.md.
    - Pilot: entries take &SLOT:MASK (any persist slot, e.g. &41:3 for both
      keys); the plain &MASK form still tests p2C. Dev-only
      PE_WARP_PERSIST_FILE (load a persist dump at a warp) and
      PE_D3_PERSIST_DUMP are for scratch probes only and off by default.
    - pc_port/tools/day4_walk_waypoints.txt: Day 4 waypoints so far
      (work in progress; no gate segment yet).
    - Docs: owner is Blizz (four stale "Will" references fixed).
    - 1-Hit Kills stays off by default; only the harness env sets it.
    - The wt2 BFM build (/workspace/scratch/wt2, /workspace/bfm-b28-wt2) is
      not this session's and was left alone.
  - 14:40 CT: Day 4 by play, part 2 (0x1E8 elevator, m0146i). k41 gates all
    PASS (exact m0146i/m0141i, test_guest_lower, both --verify, both guards,
    build + tests, Day 1 route, Day 2 movies, Day 2 route, day3_walk).
    - m0129i elevator: the button opens the doors and calls module 2 (msg 1);
      module 4 runs the bacterium battle and replies 0x72 (won, battle 9) or
      0x71 (escaped, battle 10 via the Escape command, func_80021DE0/
      func_800255E4; chance rises per failure). Neither branch transfers:
      the ride is walking into the car (module 2 quad x -1750..-1450 z
      300..700, payload 1 -> m0132i). The car panel (x -400..-200 z
      -300..-160, facing E00+-200) needs a turn in place: new pilot suffix
      $HEX[:N] (raw pad hold). Floor exit -> m0127i.
    - Route now m0129i -> m0132i -> m0127i -> m0126i -> m0146i. The m0146i
      battle stop CPU_BOUNDARY/REFUSED pc=0x8019179C ra=@800D413C:effect is
      fixed: func_8019179C is a body twin of room_m0141i func_80191330
      (retail words equal apart from relocations, text +0x46C, data +0x4CC);
      EXACT_REBUILD_GATE[room_m0146i] PASS.
    - Harness probe PE_D3_BTL_SEQ (scripted battle pads, env-gated).
    - Next: m0146i -> m0147i (0x1EB) -> m0127i (0x1EC) -> m0149i (0x1EE) ->
      m0150i (0x1F0) -> m0154i -> m0157i (0x200) -> ... -> 0x220.
  - 15:20 CT: Day 4 by play, part 3 (0x1E8 -> 0x1EE). One continuous run
    from m0127i at 0x1E8 reaches 0x1EE at frame 21780 (k42 gates below).
    - m0146i fix confirmed: the battle completes (mode 9) and the route
      reaches m0147i.
    - m0147i: key 0xD3 in the east pocket at 985,-1250 (Cross, op_E8),
      persist[0x41] bit 4, story 0x1EB. Back via m0146i -> m0126i -> m0127i.
    - m0127i at 0x1EB: module 1 sends payload 0xC to Aya on entry (walk
      to 67,-1248, dialogue, op_1C(4,0,0xD)), which sets 0x1EC.
    - Deadlock, not a port bug: when Aya first enters m0127i FROM m0126i
      (persist[1] == 0x7E), module 0 init plays the NPC scene while
      persist[0x43] bit 1 is clear (walk to 0,-920, op_4B turn toward
      actor 2, talk; the 0xB handler sets bit 1). At 0x1EB the 0xC task
      starts in parallel on the same actor (task chain +0xA8; op_AA only
      sets D_800B0CD8|0x2000 and does not suspend anything). Its op_B8
      (toward 67,-1248, want F7D) and op_4B (toward the NPC, want 758)
      each step 0x40 from the other's write (7C0 <-> 800), and neither
      ever lands on its target. All the opcodes involved match retail
      (op_86/op_B4/op_84/op_40 are instant in src/ too), so the route
      takes the order a player would: at 0x1E8, m0126i turns straight
      back into m0127i once, so the NPC scene plays before the key is
      taken. Entries are selected on persist[0x43] bit 1 (&43:1).
    - Warp note: PE_WARP_PERSIST_FILE does not carry the inventory. A warp
      to 0x1EC has no key 0xD3, so the op_A8 check at the m0127i key
      door fails. Run from 0x1E8 or earlier.
    - m0127i at 0x1EC: the north door (x -55..55 z -510..-400, payload 9)
      shows "Door locked." The key door is the panel in x 250..350
      z -696..-596, facing +x, Cross: payload 7 uses 0xD3, sets
      persist[0x41] bit 0x20 and [0x42] bit 0x10, and opens the north way.
      Quad x -300..400 z -350..-200 (payload 4) -> m0148i -> west alcove
      -> m0149i.
    - m0149i: Cross facing +z in x 70..180 z 732..902 (persist[0x41] bit
      0x4000000), story 0x1EE. A furniture block near -404,1461 is
      bypassed along x -900.
    - Pilot: a malformed waypoint entry (a '|' inside a comment) used to
      stop the parse and drop every later entry without a message. The
      parser now skips to the next '|'. The comments no longer contain '|'.
    - 0x1EE -> 0x1F0: back to m0127i, then the east elevator. The call
      panel (x 235..350 z -1296..-1196, payload 6) checks rot A00..E00
      on the press frame. The pilot steered diagonally there and ended on
      a Down frame (rot 0), so the approach is now a straight +x line at
      z -1240 and a no-direction Cross. Car (m0132i) at >= 0x1EE: "Select
      Floor" (msg 0x75, op_43): Basement / Lobby / 13th Floor -> var 0x13
      = 0/1/0xD. The options only show after a second Cross on the fully
      printed prompt (~200 frames). Down twice for 13th Floor, Cross ->
      m0150i (module 1 sets 0x1F0 on entry).
    - m0150i -> m0152i (east alcove). m0152i stop CPU_BOUNDARY/REFUSED
      pc=0x801925A0 (@8006F6D4 handler): gen_overlay_ports skipped
      room_m0152i/func_801925A0 (E21: call arity func_80192D00 0!=1). Same
      fix as the m0145i twin: func_80192D00(o).
      EXACT_REBUILD_GATE[room_m0152i] PASS.
    - m0152i stop CPU_BOUNDARY/REFUSED pc=0x80192940 (@func_801926BC
      jalr): func_80192940 (0x3C0, GTE: rtv0 + nclip on the scratchpad) was
      unmatched asm in all five copies. Hand-matched (written from the
      retail asm, profile era_o2_g0) as one C body for all five rooms:
      room_m0145i func_801928FC, room_m0152i func_80192940, room_m0153i
      func_80193E10, room_m0399i func_80193900, room_m0401i func_80194A54.
      EXACT_REBUILD_GATE PASS for all five. Levers: scratchpad as a struct
      (in-struct stores don't alias the D_8009D1CC load), opz assigned inside
      the outer loop after the mask's first use, i++ before prev = cur, and
      the table half via an int local (lh).
    - Next m0152i stop: CPU_BOUNDARY/REFUSED pc=0x80192D24 (@800D413C
      effect). It is a body twin of room_m0137i func_8019018C (the m0126i and
      m0141i copies too). Copied as room_m0152i func_80192D24 with local data
      D_8018F1CC/D_8018F1D4 and a local profile
      room_m0152i_dispatch_8018F1DC_8018F1F4_div. EXACT_REBUILD_GATE PASS.
      Same body set up for room_m0419i func_80193F64 (D_8018F1E4/D_8018F1EC,
      jtbl_8018F1F4/jtbl_8018F20C) and room_m0421i func_80190D5C
      (D_8018F00C/D_8018F014; header split 0x34-0x64 rodata for
      jtbl_8018F01C/jtbl_8018F034). EXACT_REBUILD_GATE PASS for both.
    - m0152i now runs clean through its battle (win, frame 3741). East
      side: quad 9 (x 920..1050 z -1400..-1185) is a Cross + facing
      A00..E00 interaction (op_5E). The pilot's diagonal stop left rot 0x800
      on the press frame, so the approach is now a straight +x line at
      Aya's z (-1399). The press now sends msg 9 and module 0 runs its
      handler (op_B8 to 1422,-1256 on actors 0x15/0x16, op_EA, op_2F 0x12,
      op_4D), then msg 9 to module 3.
    - Cabinet scene traced (2026-10-09 PM): msg 9 is an examine. Module 0
      turns the camera (op_B8 is the camera turn-to-point, not a walk; its
      quick ret=1 is normal), plays anim 0x12, and tells module 3, whose msg-9
      branch only restarts its quad-9 task (op_12 0x170). No port bug there.
    - m0152i module 3 layout (op_12 starts a sub-task at 0x801BE154 + 2*arg):
      0x20E quad 1 (south exit), 0x4AA quad 2 (east pocket -> room_transfer
      m0153i), 0x2A6 quads 3/4/5 (north desk chain msg 3 -> 4 -> 5 / 0xA),
      0x170 quad 9 (cabinet examine), 0x5D2 quad 8, 0x51C quad 7. Init:
      persist[0x41] bit 0x8000000 set -> quad 9 + desk tasks, object 1 pose 5;
      clear -> object 1 pose 0, then persist[0x42] bit 0x80 clear -> show
      object 0x13, object 4 pose 5, pocket task. On this route (persist
      0x41=17C1FFEF, 0x42=23C, 0x43=408D) only quad 1, the pocket and quad 8
      run. The desk chain is unreachable here, and bit 0x8000000 is set
      either by the desk chain or from m0153i (801C26D8).
    - SOLVED (no port bug, missing in-room puzzle): the branch polarity
      above was backwards (op_09 alu 7 sets var1 = (var0 == 0)). Fresh state
      = persist[0x41] bit 0x8000000 clear -> op_80 0x13 sets the 0x80
      "blocked" bit on floor triangle 19 (the pocket mouth; the stop at
      x ~980 is triangle 19's edge at x ~1046 minus Aya's radius) and starts
      the north desk chain. Route: quad 3 x 100..250 z 950..1050 Cross facing
      north -> msg 3; quad 4 x 160..240 Cross facing east -> msg 4 (persist
      0x41 bit 0x8000000, the scene walks Aya to 380,996); quad 5 Cross facing
      north -> msg 5 (persist 0x42 bit 0x80, op_81 0x13 clears triangle 19).
      Then the east pocket -> m0153i (frame 8557 from the m0150i start).
    - m0153i: 0x80193A70 stop = the same E21 arity skip (func_801941D0 ->
      (o)); fixed together with its twins m0399i func_80193560 and m0401i
      func_801946B4. 0x80194810 stop = body twin of m0141i func_80191330;
      set up in m0153i, m0412i (func_801947C0) and m0414i (func_801912CC).
      EXACT_REBUILD_GATE PASS for all. Then the east corridor -> m0154i.
    - m0154i (0x1F0): zones A (x 600..1050 z -1300..-900) -> B (x 1051..1350
      z -950..-660) send msg 0xB -> story 0x1F4. At 0x1F4: the panel quad
      x -250..-160 z -820..-700 (Cross facing north, msg 4) opens the way
      north; quad 2 x -200..200 z -700..-300 -> story 0x1F6, m0271i, which
      runs through to m0155i at 0x1F8 by itself.
    - m0155i (0x1F8): the south exit triangle 0xE is blocked; Cross facing
      south at quad 2 x -250..-167 z -200..-139 (module 2 msg 2, op_81 0xE)
      opens it -> m0156i (frame 17159).
    - m0156i: a forced battle in the module 3 encounter zone (battle end, msg
      0xFD -> payload 1 -> m0380i). BLOCKER: battle effect callback
      CPU_BOUNDARY/REFUSED pc=0x80190F94 ra=@800D413C:effect = room_m0156i
      blob 0x1FAC, 0xC00 bytes, still asm and with no body twin in any room
      (needs a hand match).
    - Notes: m0157i (sets 0x200) is not named by any room script or blob,
      so its entry is outside the room scripts (m0380i side or an engine
      table); still to trace. Other off-route E21 arity skips still open:
      m0269i 8018F6BC, m0273i 80192D50/80192E64, m0318i 80192560, m0401i
      8018F214, m0429i 801926B4/80192BC0.
    - Then: m0152i -> m0153i -> m0154i (0x1F4, item 0xDA) -> m0157i (0x200)
      -> 0x220.

### Day 4, m0156i battle (2026-10-09)

- Off-route E21 arity skips fixed (they would stop players): m0269i
  8018F6BC, m0273i 80192D50 + 80192E64, m0318i 80192560 (3rd arg is the
  out buffer), m0401i 8018F214, m0429i 801926B4 + 80192BC0. Exact gate PASS.
- 0x80190F94 (blob 0x1FAC) hand-matched as a variant of m0159i
  func_8018F3A4 (profile era_o2_g0_expand_div; YAML split at 0x2BAC).
  Exact gate PASS.
- Soft-lock in the roof fight: the "1-hit kills" assist killed the boss
  while the script had it invulnerable (HP + 1000000), so its own exit was
  skipped and op_C3 waited forever ("NO TARGET"). The assist now gives
  retail damage while HP > 1000000 (test_assists covers it). This is a
  port bug, not a missing item/flag.
- 0x8019050C (sparks, blob 0x1524) hand-matched. Exact gate PASS.
- BLOCKER: the next stop in the same fight is 0x8018F058 (blob 0x70,
  0x11D8 bytes, a 2-mode particle emitter with no body twin). Mode 1 is
  byte-exact; mode 2 is still 297 masked words off (register allocation
  across cases 0-5, plus 0x18 bytes of extra division code). The WIP stays
  outside the repo until it matches. Still open after it: m0156i -> m0380i,
  the m0157i entry trace, 0x220.

### Day 4, m0156i -> m0380i -> m0381i -> m0157i (2026-10-09 evening)

- 0x8018F058 did not match quickly (mode 2 register allocation; one more
  attempt with Muse made no change, Qwen's API refused access). Installed
  as a NON_MATCHING native port, differential-tested against the retail asm
  (see docs/evidence/nonmatching/REPORT.md). The new mechanism: manifest
  configs/USA/overlays/nonmatching.yaml, sources in src/nonmatching/.
- The roof fight had a second soft-lock with the assist: the script exits
  once HP <= 1000000 (999999), and a follow-up bullet in that window was
  one-hit-killed. The assist now does retail damage for any HP > 900000.
- The fight ends -> m0380i (second phase, same code as m0156i; its end
  needed 0x80192208 + 0x80191B94, both NON_MATCHING ports, tested the same
  way) -> m0381i.
- m0157i is entered from m0381i: after the Eve scene (story 0x1FC)
  module 6 counts var7 to 0x7F (~1016 frames) and sends 0x13 -> m0438i
  (Game Over). Reaching module 5's quad x -3200..-1700 z -11000..-10000
  sends 0xFB (stops the timer) and payload 1 -> m0157i. Walking is too
  slow; the pilot gained a `+` waypoint suffix (hold Circle = run).
- Then m0157i -> m0158i (0x200) -> m0159i -> m0160i (0x208) -> m0035i ->
  m0046i (0x210, 0x216, police precinct, field control): next is routing
  m0046i to 0x220.
- m0046i (0x216) -> m0043i -> m0042i -> m0041i: module 8 quad x
  -2029..-624 z -4..209 at story 0x216 -> module 2 payload 0x64: scene,
  0x218, -> m0352i, which sets 0x220 -> world map.
- FULL DAY 4 PASS: one continuous run from m0098i at 0x1B8 (persist
  0x1B8) reaches story 0x220 at frame 97562 with no stop (stop reason
  frame-limit only). Through-run fixes (warp-tested legs that broke on a
  through run): m0328i after the fuse pickup, back down x 1450 (the
  straight line hugged the machine corner); m0132i car at 0x1EE, wait 300
  frames for the car arrival to re-place Aya before the panel, and longer
  waits in the "Select Floor" menu.

### Gate: day4_walk (2026-10-09 night)

- pc_port/tools/day4_route_regress.py: segment day4_walk, warp m0098i at
  0x1B8 with the persist block the same gate run's day3_walk dumped
  (day3_route_regress now sets PE_D3_PERSIST_DUMP=<log-dir>/persist for
  day3_walk and lingers 120 frames after its last checkpoint so the 0x1B8
  block lands), then the committed day4_walk_waypoints.txt to 0x220. 78
  ordered checkpoints (every first room entry and story step). Nothing
  from the game is committed. The gate chain runs it after day3_walk.
  PASS: day3_walk 0x1B8 at frame 65002, day4_walk 0x220 at frame 97562.
- Fix: the m0046i/m0043i/m0042i/m0041i (0x216) entries of the full pass
  were only in the scratch copy; now committed (the gate caught it).
- Smoke test of the 7 off-route arity fixes (warp, story 0x220, 6000
  frames, X mash, with a recorder probe on each function): m0269i, m0273i,
  m0318i, m0401i, m0429i all load and run clean to the frame limit, but
  none of the 7 functions is called by a plain warp (they are enemy/effect
  callbacks of scripted battles: m0273i/m0318i are entered from m0360i,
  m0429i from m0349i). Not exercised in game yet; m0273i/m0318i lie on the
  Day 5 m0360i branch.

### Day 5, 0x220 -> m0173i (2026-10-09 night)

Waypoints: `pc_port/tools/day5_walk_waypoints.txt` (from the 0x218 persist).
World map location 1 -> m0161i -> m0162i (fight) -> m0163i (fight) -> m0165i
(scene, story 0x228/0x230) -> m0166i manhole (0x238) -> sewer m0167i hub ->
south arm -> m0168i -> west end -> m0167i (west arm) -> south arm -> m0168i ->
west end -> m0173i.  m0168i's west exit goes to m0173i only when p[0x37] == 1;
otherwise it bumps p[0x37] and sends Aya back into the hub, so the loop is run
twice.

Fixes on the way:
- m0163i fight stopped at CPU_BOUNDARY 80192A68, then 801924FC (enemy shot
  effect and its particle callback, room_m0162i bytes): both ported
  NON_MATCHING (docs/evidence/nonmatching/REPORT.md).
- m0167i fight stopped at func_80078BE4 (Psy-Q libgte ApplyMatrix, called from
  m0167i func_8018F8B8): host replacement `PE_ApplyMatrix_Guest`
  (pe_port_compat.h host-replacement marker; func_800794C4_port.c), with test
  INIT1_applymatrix_guest.  No func_80078BE4 is defined.

Open issue (not yet fixed): with the moving battle shim, the m0167i fight ends
with Aya at (0,404) after the item-full "Exit Menu", and she can turn but not
walk in any direction afterwards (field mode 9, no STOP).  Fighting standing
(`PE_D3_BTL_SEQ`, Cross/release) avoids it; the route uses that from m0162i on.
Needs checking whether a player can reach that spot in retail.

Blocker (0x238, m0173i): Aya arrives from m0168i on the lower deck (4289,125).
The exit to m0433i (story < 0x248: story = 0x240) / m0174i is the module 2
quad around (-1150,50), across the channel on the bridge strip the floor mesh
has at z -107..314.  Aya stops at (3334,138), the channel edge; the bridge is
not drawn there.  The ladder at (3880,2970) does not take Cross/Up: module 2
does not start the ladder-bottom quad thread when p[1] == 0xA8 (entry from
m0168i), only on message 1.  Next: find what lowers the bridge or sends
message 1 (the other m0173i modules / m0174i side), then m0433i.

### Day 5: m0167i "can't walk" root cause, m0173i ladder, route to m0176i (2026-10-10)

- The m0167i post-fight stop was a harness bug, not a game bug: the route walker
  (RouteM34ShimMove) learns the pad->world map online, and knockback in the fight
  taught it Down = (-251,748); after the fight it pinned Aya at (0,404). Fix
  (route_reward_sewer_pilot.h): the map is saved at a battle start and restored at
  its end, from story 0x220 on (Day 1-4 gate routes were tuned with the carried
  map; freezing/reseeding in all days broke day4_walk at m0132i/m0153i, k53-k55).
  Day 5 now runs with moving fights; m0167i x2 and m0168i end with Aya walking.
- m0173i is not a port bug: entering from m0168i (p1 0xA8) arms only the x=3649
  line (z 2756..3232, west of the ladder); crossing it climbs to the upper walkway.
  Message 1 is the arm for the other entries / after a battle in the room.
- Route now 0x220 -> m0173i -> m0433i (0x240) -> m0174i (0x248; the stair landing
  is an item-drop spot) -> m0175i -> m0176i -> m0178i/m0179i.
- Blocker: m0176i sewer gates. The door quad picks m0180i/m0179i/m0178i from p36
  0x1000/0x2000/0x4000 (set by module 3 on messages 3/4 from the control panel's
  menu, message 0xe); m0178i/m0179i: the water-too-high message; the panel
  (facing test rot 0x600..0xA00) gives the no-power message in the gated build.
- Harness: PE_D3_RAM_DUMP=FRAME:PATH,... (2 MB guest RAM), floor dump writes y.
- Gate k56: CHAIN PASS (day4_walk 0x220 at frame 97562).
