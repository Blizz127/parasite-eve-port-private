# #16 — M34 phase-2 room effects in the native port

Scope: room_m0348i (M34 phase 2, token `A8064448`) effect-VM callbacks that
the port used to skip with `[DAY1_SHIM] PE_WeaponCallback: effect callback …
not ported; record retired`.

## What changed

| Callback | What | Source | Port |
| --- | --- | --- | --- |
| `80190E98` | fire-breath beam draw (16 segments) | matched C, 054d91ee | line-for-line, `m34_boss_effect_port.c` |
| `801914A0` | beam per-segment update / spawn | matched C, 2e6f7a66 | line-for-line |
| `80191A98` | beam floor glow (two ring slots) | matched C, 054d91ee | line-for-line |
| `80191FE0` | ember burst (4 embers) | matched C, 054d91ee | line-for-line |
| `80191DB0` | glow fade | matched C | line-for-line |
| `801922E0` | ember motion | matched C | line-for-line |
| `func_800C4FC4` (EXE) | ring/ribbon renderer used by the floor glow | **unmatched asm** (B57C4.s, 0x574, handwritten GTE) | hand port, `func_800C9C20_port.c` |

All six room callbacks are dispatched from `PE_M0348iEffectChild` (resident
check `PE_M0348iEffectOverlay`). `tools/analysis/gen_overlay_ports.py --rooms
--check --verbose` still rejects every one of them (`asm` for the four with
inline GTE macros; pointer-parameter reasons for 80191DB0/801922E0), so they
follow the existing path for rejected m0348i callbacks. No generator change.

The inline `gte_CompMatrix` macro (Psy-Q `inline_c.h` shape) is reproduced
op-for-op on the port GTE (`PE_M0348iCompMatrix`): SetRotMatrix(r1); per
column ldclmv / rtir (`0x4A49E012`: MVMVA sf=1, RT, v=IR, no TR) / stclmv;
SetTransMatrix(r1); ldlv0(r2+20); rt (`0x4A480012`: MVMVA sf=1, RT, V0, TR);
stlvnl stores IR1–3 as words (the macro's `swc2 $9–$11`, as retail).

`func_800C4FC4`'s RotTransPers4 (`func_80079304`) `p`/`flag` outputs are not
written: the retail caller keeps them at sp+0x40/0x44 and never reads them
after the `jal` (only `v0` = OTZ is used). The port has no guest stack frame
for that function.

Not ported: the masked-body twins in room_m0174i (`80191FB0`, `801925B8`,
`80192BB0`, `801930F8`); that room is not on the Day-1 route.

## Evidence

### 1. Native tests (`pc_port/tests/test_m0348i_gte.h`)

- `M348G_mvmva_words` — the two raw command words decode on pe_gte to the same
  result as the short forms, and both match an independent integer reference.
- `M348G_comp_matrix` — the macro matches a pure-C reference, including
  `r2 == r3` aliasing and IR saturation; RT/TR are left = r1.
- `M348G_c4fc4_ring` — `func_800C4FC4` on analytic inputs (identity camera,
  H=256, z=1024, exact 1/4 projection), billboard and non-billboard. It checks
  segment vertex order (i, i+1 mod n, n+i, n+(i+1 mod n)), screen xy,
  OTZ = SZ3 >> 2, POLY_G4 length/code (8 / 0x38), ring-0 colour from r+8 and
  ring-1 colour from r+4, the draw-mode word, OT link order and the 0x2C
  packet-cursor step.

Suite: see the gate record below.

### 2. Route

`pc_port/tools/day1_route_regress.sh --until day1_end_80`: REGRESS PASS,
`day1_end_80` at frame 67186 (runs 3 and 4 identical, on 2e6f7a66; the
baseline was 66994). The +192 frames all come from M34 phase 2 (`m34_exit`
64950 → 65142): the fire-breath records now live their full effect lifetime
instead of being retired on the first call. No `PE_WeaponCallback` DAY1_SHIM
remains in phase 2. The only remaining shim on the route is #17 (M0023I
flash); see "The former M0359I 8018F238 shim" below.

### 3. Effect lifetime

Port (`PE_M348_FX_TRACE=1`, one line per m0348i callback with VBlank
`PE_GPU_VSyncQuery`, record tick rec+2 and state rec+1):

- There are 3 beam inits (80190D88) and 3 beam records. Each record is updated
  by 801914A0 from tick 0 to tick 121, then retires (state 2): 122 calls.
- Calls are 2 VBlanks apart, except for one 36-VBlank gap shared by all live
  records (a battle pause).

The tick source is matched code: `src/func_800C251C.c` runs each active
record's callback and then does `rec+2 += 1`, one tick per call. The port's
`func_800C251C` (`func_800C2414_port.c`) does the same, and the matched room
leaf `func_8018F128` calls it once per effect update. The retire condition
(tick ≥ 0x79) is in matched `801914A0`.

Retail: see "Retail tick rate" below. Direct RAM warps into either M34 room
give an empty encounter in retail (enemy list count 0), so the measurement
uses a reachable battle.

### 4. Retail video comparison

Retail capture `build/reference/retail-video/part02.video.webm`, 790.5–797 s
(13:10.5–13:17), was extracted at 320×240 and compared with port route shots
from M34p2 (frames 63330–63420). Contact sheets are under `build/lanes/day1/`
(`w16_vs_retail.png`, `ring_cmp.png`); images are not committed.

Both show the same sequence: white flash at the head, then a translucent
lavender ring (the floor-glow slots through `func_800C4FC4`) as the beam
starts, then the orange beam with the orange floor-glow strip.

| Region | Retail mean RGB | Port mean RGB |
| --- | --- | --- |
| ring (s_29 / 063340) | [74, 70, 50] | [77, 62, 50] |
| floor glow (v_11 / 063370) | [129, 89, 35] | [121, 88, 38] |

This is not an exact-frame pixel diff: the video is compressed, has no frame
alignment with the port, and shows a different camera moment. `func_800C4FC4`
is unverified beyond the native test and this comparison.

## Retail tick rate

The oracle (PCSX-Redux + OpenBIOS, `build/oracle/oracle_route.lua`,
`ORACLE_TICKRATE=1`, run with `ORACLE_CPU="-interpreter -debugger"`) logs exec
breakpoints on `0x800C251C` and VSync `0x80073A44` while a battle is live
(D_8009D1A0 & 2). A direct M34 warp gives an empty encounter, so this uses the
reachable M5 battle: `ORACLE_WARP=9800:A80002C8:18`, `ORACLE_REL_PAD` replay,
`ORACLE_HPLOCK=1`, `ORACLE_AUTOFIRE=1`, end at VSync 12600.

- Battle start is at VSync 11117, the same as without `-debugger`, so the
  debugger does not shift emulated timing.
- `func_800C251C`: 1150 calls on three effect slots. 1138 of the 1147
  successive-call intervals are **2 VBlanks**. The rest are 1/3 pairs
  straddling a VSync edge, plus one 37-VBlank pause gap, the same shape as the
  port's single 36-VBlank gap.
- VSync in battle: 20479 × `VSync(-1)` (queries) and 742 × `VSync(2)`. The
  `VSync(2)` cadence is 2 VBlanks in 735 of 741 intervals (the rest are 1/3
  pairs).

Caller chain: `func_80035558` (per-frame object update, asm; the port's
`func_80035558_port.c:672` makes the same single call), then matched
`src/func_80069594.c` (skipped while D_8009D1A0 & 4, i.e. paused), then
`func_8006F9F0` per arena slot, then the room update `func_8018F128` (matched
C), then matched `func_800C251C` with one `rec+2 += 1` per call.

**Result:** retail and port both advance effect ticks once per battle frame,
at 2 VBlanks per tick. The 121-tick beam lifetime (matched 801914A0) is
therefore about 242 VBlanks plus pauses in both. The +192-frame route shift is
retail behaviour, and the route's expected end frame becomes 67186.

## Gate record (base 0999c817)

- Build: `scripts/build_pc_port.sh --no-tests --build-dir build/day1wt/w16-build`, rc=0.
- Suite (`pe-native-tests`, run from the repo root): `1611 run, 1611 passed,
  0 failed, 0 skipped`.
- Route: `pc_port/tools/day1_route_regress.sh --until day1_end_80`, REGRESS
  PASS. `m34_phase2` 63064, `m34_exit` 65142, `day1_end_80` **67186**.
  DAY1_SHIM lines on the route: #17 (M0023I flash), plus the 8018F238
  worktree artifact explained below (not present in main-tree builds).

## The former "M0359I 8018F238" shim

`[DAY1_SHIM] PE_EffectCallback: room effect 8018F238 (mode 9, token A80654C8)`
appeared only in worktree builds (the w16 routes). It is not an M0359I effect,
and it is not a divergence.

- `0x8018F238` is room_m0348i's matched `func_8018F238`
  (`int func_8018F238(void) { return 0; }`, retail `jr ra; move v0,zero`). It
  is effect module 9's finalizer, called by `func_800696F0` through
  `effect_module_finish(code = 9)` ("finish modules before their overlay is
  replaced"). The token already reads A80654C8 at that point, but m0348i's
  code is still in RAM. M0359I's room chunks contain no MIPS code.
- The generated TU exists (`room_038ccb2f__func_8018F238`). It resolves only
  by fingerprint, because production never declares overlay residency. In a
  git worktree, `gen_overlay_ports.py` found no `build/extracted` blobs and
  wrote fingerprint 0 for every overlay entry, so the entry could not resolve.
  The main tree has `0xE5787985`, and the main-tree baseline route never logs
  this shim.
- Behaviour: the shim's fallback for mode != 1 is `return 0` with no side
  effects, which is exactly what the retail finalizer does. So nothing
  observable differs, even in affected builds.
- Fixed in the generator: it falls back to the main checkout for blobs and
  fails loudly instead of writing fingerprint 0 (`build/lanes/day1/genfix.patch`).
