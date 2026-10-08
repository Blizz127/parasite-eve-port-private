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
In progress: E21b inline GTE asm lowering (`guest_lower.py` asm templates ->
`PE_GTE_ReadData/WriteData/ReadCtrl/WriteCtrl/Command` in `pe_gte.c`, native
tests `test_gte_regs.h`). Room asm refusals 305 -> 20, room TUs 2257 -> 2583.
If uncommitted after a restart: rebuild with `scripts/build_pc_port.sh`, run the
gates, then re-probe the GTE stop rooms (m0207i m0232i m0295i m0432i
m0434i-m0437i) with `build/lanes/warp/probe.sh <room> 90`.
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
