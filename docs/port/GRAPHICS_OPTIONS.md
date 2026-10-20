# Graphics options feasibility (Enhancements, OFF by default)

> **State on the native lineage (2026-10-20).** This is the design document of
> branch `claude/gfx-options` (8ea0d39e3, 2026-10-01), carried over as written.
> On 2026-10-20 stages 1–4 of section 6 were ported onto the native lineage
> (`claude/modern-20261020`): the GRAPHICS registry page, the display scaler,
> internal resolution 2x–4x, the polygon texture filter and supersample AA.
> Stage 7, HD backgrounds, was **not** ported. The remaining stages (60 fps
> interpolation, sub-pixel geometry, widescreen) are still designs.
> - **Line references** below point into the source lineage's files, not this
>   tree.
> - **Evidence reports** named here (`docs/evidence/graphics-*`) are on that
>   branch.
> - **Current results** on this tree are in
>   [ENHANCEMENTS.md](ENHANCEMENTS.md#graphics-dev-menu--options--graphics).


Status: display scaling and native/2x/3x/4x internal rendering are implemented
and fully gated. Polygon filtering, supersample AA and owner-local HD
background rendering are implemented; combined and automatic-scale gates pass.
Interpolation, geometry correction, model smoothing and widescreen remain
designs with the prerequisites below. See section 6 for stage status and
the evidence reports for measured costs and verification limits.
Task `c57dd513b6ed`; lane `claude/gfx-options` (based on the lead's gated
source `40b77cba` via `f2f1a26b`). This lane owns
`pc_port/platform/pe_gpu.c`, `plat_renderer.c`, `plat_enhance.c` and the
presenter (`host_framebuffer.c`, `host_window*.{c,h}`, the blit in
`port_main.c`); the Codex lead integrates and packages.

Every option below registers in the existing enhancement registry
(`pc_port/platform/plat_enhance.c`, category `PE_ENH_CAT_GRAPHICS`, which
contains the implemented graphics rows), persists in the same `enhancements.conf`, has an env
override, and at its retail default takes the identical code path. "Identity
when OFF" is testable because every parity check in the tree reads either the
1x PSX VRAM, the 320x240 present buffer, guest RAM, or the present/VBlank
counters (section 2). No option may change any of those when OFF, and the
resolution-scale family must not change them even when ON.

## 1. What the renderer is today

Facts, with references, from the read-only survey of `f2f1a26b`.

- **Pure software, no GPU API.** The game writes GP0/GP1 words straight into
  the `pe_gpu` rasterizer (OT walker `pc_port/game/boot/func_8007512C_port.c`
  ~110-130 through `PE_GPU_WriteGP0`; LoadImage `func_80076664_port.c:160`;
  StoreImage `func_800768A0_port.c:49`; DISPENV `pe_dispenv.c:352-420`).
  `pe_plat_renderer_*` (`plat_renderer.c`) is referenced by nobody; its
  `internal_scale=1` / `widescreen=0` report (`plat_renderer.c:52-53`) is
  read by nothing.
- **Rasterizer** (`pe_gpu.c`): 1024x512 BGR555 VRAM (`:88`); polygons 20h-3Fh
  by integer edge functions with a top-left rule, barycentric UV/RGB
  (`:652-716`), quads as two triangles (`:751-752`); lines 40h-5Fh by a 32.32
  DDA (`:762-827`); rects 60h-7Fh (`:327`, `:567`); fill 02h (`:470`);
  A0 upload / C0 readback / 80h VRAM copy (`:1265`); E1-E6 state. ABR 0-3
  blending per 5-bit channel (`:503-525`), texel bit 15 gates blending
  (`:440`, `:709`), texel 0 is transparent, 4x4 dither on gouraud/modulated
  polygons and lines (`:655-661`, `:814`), mask bits (`:433-444`). Texture
  sampling is nearest only (`PolygonTexel` `:636-650`, rect texel `:395-424`).
  Vertices are 11-bit integers (`:283`, `:728`); the GTE packs SX/SY to
  int16 at `pe_gte_cop2.c:273-282`, so **no sub-pixel position survives
  anywhere**.
- **Scan-out** (`host_framebuffer.c`): `HostFB_PresentDispEnv` (`:167-193`)
  reads the DISPENV from guest RAM; `ScanDisplay` (`:195-233`) copies the VRAM
  window into a fixed `fb[320*240*3]` RGB888 buffer (`:31`), width clamped to
  320 (`:218`). `PE_PORT_FB_WIDTH/HEIGHT` are compile-time 320x240
  (`host_framebuffer.h:9-10`). The game only uses 320x224 / 320x240 double
  buffers and 320-px 24-bit movies; no 512/640 modes.
- **Presenter:** `port_main.c:676-690` calls
  `HostWindow_Blit(pe_plat_cheats_overlay(HostFB_GetPixels(),320,240))`.
  X11 (dlopen) optionally uses XRender with a hard-coded `nearest` filter
  (`host_window.c:298-371`, `:330`) and rejects any fb that is not 320x240
  (`:502`); the CPU path `HostWindow_ScaleBGRA` (`host_window_scale.h:276-341`)
  is pixel-identical (KNOWN_DIVERGENCES #36, `window-scale-math` test). Win32
  uses GDI `StretchDIBits` of the CPU-scaled buffer. Scaling is the largest
  aspect-preserving fit, letterboxed, nearest, any window size. `--scale N`
  (default 2) sizes the window; `--fullscreen` is EWMH/borderless. Blits are
  skipped when a `memcmp` of the 320x240 frame shows no change (`:496`).
- **Pacing:** fixed 60000/1001 Hz deadline pacer (`host_frame_pacer.h`,
  `HostWindow_Pace`, `host_window.c:756`); fast-forward multipliers only. There
  is no vsync option, no frame limiter, no AA/AF/widescreen/texture-replacement
  or upscaler code anywhere.
- **Registry** (`pc_port/include/pe_plat/enhance.h:40-62`,
  `plat_enhance.c:16-28`): `{id, category, label, format, values[], nvalues,
  env, credit}`; formats TOGGLE / PERCENT / MULT; `snap()` picks the largest
  listed value <= v; env overrides are never written back; `save_config` runs
  only on a menu change, so the gate's "no enhancements.conf at defaults" check
  holds. Menu pages live in `plat_cheats.c` (page 7 OPTIONS, 8 GAMEPLAY,
  9 ASSIST); **there is no GRAPHICS page yet.** `PE_ENH_AT=<frame>:<id>=<v>`
  (`port_main.c:559-583`) flips any registry id at a frame for harness
  captures; ids match `[a-z_]` only.

## 2. Parity surfaces that must not move

| Surface | Who reads it | Consequence |
| --- | --- | --- |
| 1x VRAM | game StoreImage/C0 readback, 80h VRAM copies, texture fetches from earlier draws, `PE_GPU_ReadVRAM` in `test_native.c:621,11442`, `test_gpu_prims.h`, movie/MDEC tests, `visible_vram_oracle.py`, `tools/hdbg/verify_gpu.c` | native VRAM stays the authority; any higher-resolution rendering is a **separate shadow surface** |
| 320x240 present buffer | `--screenshot` (route regression), `PE_SHOT_VSYNC`, `PE_ROUTE_SHOT_RANGE`, `movies_day2_check.sh`, oracle pixel diffs (`docs/evidence/r6-day1-integration/REPORT.md:92-102`), `test_clear_image.h:116`, `test_cheats.h:55` (overlay must return the same pointer when everything is off) | scan-out into `fb` is unchanged when OFF; when a scale option is ON the 1x `fb` is still produced for shots and the HD frame is a second buffer |
| Guest RAM hashes | FNV-1a fixtures in `retail_*_cases.h`, `test_hit_camera.h` | no game-state change from any graphics option |
| Present / VBlank counters | `g_frame` (route gate end frame 66356 on `40b77cba`), `test_vsync.h`, `test_vblank_clock.h` | a 60 fps mode must not add presents that the route counter sees, or it must count only logic presents |
| Registry shape | `test_assists.h:302-304` asserts `n == 5` options and `GRAPHICS == 0`; menu navigation in `test_enhance.h:175-203` / `test_assists.h:233-276` counts keys to reach `ENHANCEMENTS >` | these tests are updated in the same commit as each option, with the new GRAPHICS page reachable by an explicit row |

There is no "900-hash" facility in this repository; the equivalents are the
route regression end frame/token, the byte-identical PPM SHA-256 of the route
screenshot, the oracle pixel diffs and the native suite with zero skips.

## 3. Baseline frame time (measured)

Gated binary `40b77cba` (`parasite-eve-port` SHA-256 `36063b88…`), blizzpc
runner (Ryzen 7 5800XT, headless, nice 19, capped scope), job
`20260930-184614-pe-gfx-baseline-1390238`, exit 0, two runs of
`day1_route_regress.sh --until day1_end_80`:

| Run | Frames | Wall | Per frame | User CPU | Max RSS |
| --- | --- | --- | --- | --- | --- |
| 1 | 66,356 | 114.24 s | 1.72 ms | 113.47 s | 16.5 MB |
| 2 | 66,356 | 115.59 s | 1.74 ms | 114.95 s | 16.4 MB |

That is the whole port per frame (game logic, GTE, software rasterizer, scan
out, no window) on one core: about 580 frames per second, roughly 10x headroom
over 60 Hz. The route mixes field, battle and menus, so it is an average; a
per-stage split (rasterizer vs logic) will be measured in Stage 2 with a
`PE_FRAME_TRACE`-style counter around `PE_GPU_WriteGP0` before the shadow
surface lands. Devbox root disk was full, so all measurements are on the
runners (owner authorization 2026-09-30 23:42Z covers blizzpc and Alienware).

## 4. Per-feature feasibility

### 4.1 Internal resolution scale (2x / 3x / 4x) — needs a shadow surface

- **Now:** nothing is parameterised by scale; the frame is 320x240 by
  construction.
- **Design:** a second colour buffer `hires[1024*S x 512*S]` (or only the
  displayed 320x240 window, 320S x 240S, which is simpler and enough) written
  in parallel by the same primitive functions: polygons at `pe_gpu.c:652`,
  lines `:762`, rects `:327`/`:567`, fills `:470`, VRAM copies `:1265` and
  uploads `:217` (copies/uploads invalidate the shadow region back to 1x
  content). 1x VRAM keeps being written exactly as today, so readbacks, tests
  and screenshots are untouched. Scan-out gains a second path that copies the
  shadow window into an `fbS[320S*240S*3]`; the presenter blits whichever is
  active. XRender's 320x240-only guard (`host_window.c:502`) is generalised.
- **Quality limit:** without sub-pixel vertices the shadow only sharpens
  texture sampling and edges stay at 1x positions. Sub-pixel capture
  (PGXP-style) at `pe_gte_cop2.c:273` carried through a side table keyed by the
  packed SXY word is a follow-on (4.4).
- **Textured rects (the field backgrounds and UI) at S>1** sample the same
  16x16 texels nearest, so they look identical to window scaling; the win is
  in 3D polygons (actors, battle arenas, Aya) and in enabling 4.5/4.6/4.7.
- **Cost:** CPU. At 4x the rasterizer does 16x the pixel work. The bounded
  18881-frame run measured 303.04 s at 4x versus 30.76 s native. This average
  does not prove paced windowed frame rate or worst-case scene timing.
- **Test:** hash of 1x VRAM and 320x240 `fb` identical with the option ON and
  OFF over a fixed primitive set (`test_gpu_prims.h` style); shadow window at
  S=1 equals `fb`; route regression end frame unchanged with the option forced
  ON via `PE_ENH_AT`.

### 4.2 Window / fullscreen resolution independent of internal — supported now

Already true: the window is any size, the frame is letterboxed and scaled
(`host_window_scale.h:276-341`). Only the menu row and the persisted value
are new (window scale currently comes from `--scale`). Low priority; folded
into the presenter work of 4.1.

### 4.3 Frame rate: 60 fps / uncapped — interpolation only, and it is the most expensive item

- **Native logic rate is 30 Hz everywhere that matters.** Field and battle share
  one tick (`func_8003F3C4_port.c:361-470`; battle runs inside it when
  `D_8009D1A0&2`, `func_80035558_port.c:590-603`), and the frame tail
  `func_80070E54` (`func_80070E54_port.c:13-70`) does DrawSync then VSync(2)
  (VSync(4) during fades). The field menu is in the same tick. Only the VBlank
  callback `func_8003E91C` runs at 60 Hz (RNG advance, timers, play time), via
  `HostFB_VBlankGameCallbacks` (`host_framebuffer.c:280`).
- **So there is no scene where "true 60 fps logic" exists;** the menu row is
  `FRAME RATE: NATIVE (30) / 60 INTERPOLATED`, never "60 fps". Decoupling
  game logic to 60 Hz would change timing-dependent gameplay (ATB, RNG per
  VBlank) and is out of scope.
- **Where an extra frame fits:** between the two VBlanks of the logic wait
  (`HostFB_VSync` tick loop, `host_framebuffer.c:382-389`, or the per-VBlank
  pacer `pace_vblank`, `host_window.c:725`). It must not advance the emulated
  VBlank count (RNG/timers are per VBlank) nor the present counter `g_frame`
  (route anchors are per present, `port_main.c:310-373`); it is a host-only
  present.
- **What it costs:** all draw state is guest RAM (OT `D_800B0E38[CDDC]`,
  packets at instance+0x54, `func_8003C5D8_port.c:21-67`) and rendering is the
  software rasterizer into emulated VRAM. An interpolated frame must
  re-project and re-rasterize with interpolated inputs into a **host-side
  target** (the 4.1 shadow surface), never into guest VRAM. Interpolable
  state exists: actor pose prev/current pairs are already snapshotted each
  tick by retail (`pe_actor_snapshot_pose`, `func_80035558_port.c:77-85`:
  +0x28/2C/30 pos, +0x38/3A/3C rot vs +0x40..+0x54), camera scroll
  `D_800BCF94/96` and bg scroll `D_800BCF8C/8E` (`func_80066800_port.c:59-97`),
  joint matrices at instance(actor+0x1B4)+0x84. Effects/particles have their
  own pools and are the long tail.
- **Realistic first deliverable:** interpolate the camera scroll (background
  plate offset and OFX/OFY) and actor root pose only, redraw actors through
  the native model path onto the shadow surface, keep UI/effects at 30 Hz.
  Label it as such. Requires 4.1 first and a re-entrant projection/packet
  path that writes to a host OT instead of guest RAM. Order: after 4.1 and
  4.6. Fallback rule: any scene where the host OT rebuild fails (unknown
  primitive source) presents the native frame twice.
- **Test:** VBlank count, `g_frame`, guest RAM hash and 1x VRAM identical with
  the row ON over the Day-1 route; host-present count doubles.

Recovery review (2026-10-01) confirms additional prerequisites: the frame tail
presents before drawing its next OT, so a GP0 batch cannot be paired by the
host route frame. The shadow shares the live GPU parser and texture VRAM;
replaying through GP0 would mutate native state. Actual camera/root-pose
interpolation needs read-only actor/resource-generation and camera snapshots
at the model projection boundary, pure host projection/OT construction,
a separate replay/texture context and a host-only presentation scheduler.
Relevant sites are func_80070E54_port.c:75, func_80035558_port.c:628,
func_8003A088_port.c:218 and func_8003C5D8_port.c:21. Until those exist,
native timing remains in effect and no 60 fps option is exposed.

### 4.4 Higher-precision geometry (no wobble, no affine warp) — feasible on the shadow surface, after 4.1

- **Projection today** is hardware-exact integer math in
  `pe_gte_project_vertex` (`platform/pe_gte.c:409-455`: 44-bit MAC, IR
  saturation, SZ clamp, UNR reciprocal `:343-358`, `sx = (ratio*IR+OFX)>>16`
  clamped ±1024), used at 23 RTPS + 23 RTPT + 68 MVMVA native sites. The
  actor path is `func_8003AC90` (`func_8003A088_port.c:218-257`) → screen XY
  in `0x800B1644[idx]`, SZ in `0x800A636C[idx]` → packet build
  `func_8003B144` (`func_8003C5D8_port.c:21-67`) copies packed XY into the
  GT3/GT4/G3/G4 packets. Wobble = int16 vertices/4.12 matrices + UNR + the
  `>>16` truncation; affine warp = the rasterizer's barycentric UV
  (`pe_gpu.c:688-690`) with no depth.
- **Design (PGXP-style sidecar):** in `pe_gte_project_vertex` also compute
  the float projection from the same inputs and store `{fx, fy, 1/sz}` in a
  host table keyed by the packed SXY word plus vertex index; the packet
  builder's copy keeps the key valid, so `pe_gpu` can look the sidecar up by
  the integer XY it receives and, when the lookup hits and the option is on,
  rasterize the shadow surface with float edges and perspective-correct UV.
  Miss → integer path (identical to retail). 1x VRAM is never affected.
- **Blocked until 4.1 exists** (needs a target other than guest VRAM).
  Confidence: medium; the sidecar key can alias when two vertices project to
  the same integer XY in one frame (resolve by primitive address, as PGXP
  does with its memory-address tracking).

### 4.5 Anti-aliasing — SSAA via 4.1 only

No GPU API, so no MSAA. SSAA = render the shadow at S and box-filter down in
the scan-out copy to the window size (or to S/2). Comes for free once 4.1
exists; a separate "AA" row selects the downsample filter.

### 4.6 Texture filtering / anisotropic — bilinear on the shadow surface; AF is not meaningful

Bilinear/"sharp bilinear" goes into `PolygonTexel` (`pe_gpu.c:636`) and the
rect texel fetch (`:395-424`) on the shadow path only: filter **after** the
CLUT lookup, treat texel 0 as transparent (no bleed), keep STP semantics,
respect the E2 texture window. Anisotropic filtering has no meaning for a
software point/bilinear sampler and is not offered; the row is "TEXTURE
FILTER: NEAREST / BILINEAR". Never at 1x: it would change VRAM.

### 4.7 FSR 1 (spatial) — supported now, presenter only

FSR 1.0 EASU+RCAS is a pure image-space pass on the 320x240 (or the 4.1
shadow) frame at the window scaler hook (`HostWindow_ScaleBGRA`,
`host_window_scale.h:308`; XRender path `host_window.c:338` needs a larger
source pixmap). CPU implementation of the reference kernels (MIT licence,
no bundled retail data). Invisible to every parity surface because it is
after `HostFB_GetPixels`. FSR 2/3 and DLSS are **not offered**: they need
depth, motion vectors and jitter from a hardware pipeline that does not
exist here; the doc says so rather than shipping a mislabelled row.

### 4.8 Widescreen — blocked in the field, plausible for actors only; last

- Projection centre is hard-coded `SetGeomOffset(160,112)`
  (`func_8006C4C4_port.c:140-143`; effects/UI use their own 160,120/160,112
  offsets), the follow camera clamps to per-view bounds
  (`func_80066800_port.c:84-96`), and the display/draw environments and the
  host framebuffer are 320 wide (`host_framebuffer.h:9`, clamp `:218`,
  `host_window_win32.c:34`).
- Models are culled only by facing and OT depth
  (`func_8003C5D8_port.c:44-58`), not against screen edges, so actors past
  x=320 would still project and draw if the clip and framebuffer were wider.
  Backgrounds are 16x16 SPRT tiles per layer; extra plate pixels exist only
  where a view is larger than the screen in that axis and the scroll clamp
  allows it. Everywhere else 16:9 means pillarbox (today's behaviour) or new
  art.
- Verdict: not offered in Stage 2. A later "wide actors" experiment would
  widen E3/E4 and the shadow surface only, keep the plate at 320 with bars,
  and needs a per-room audit. Recorded here as blocked with the reason.

### 4.9 Better assets: HD textures / backgrounds — needs 4.1 plus a content-keyed cache

Upload hooks `WriteImagePixel`/`FinishImage` (`pe_gpu.c:217,201`) and the DMA2
snapshot (`:1399`); sample hooks `PolygonTexel` and `DrawTexturedRectangle`,
keyed by tpage/CLUT/UV region and, for backgrounds, by the layer context hash
already defined in HD_BACKGROUNDS.md. Replacement content is drawn on the
shadow surface only; 1x VRAM never sees it. Everything the owner generates
stays under `~/.local/share/pe-port/hdbg/`; nothing retail-derived enters the
repository. **Models and lighting:** lighting is per-vertex Gouraud in game code
(`func_8003B97C_lighting_cut`, NCCT per triangle vertex, normals from the
shared int16 table at `0x80091A58` indexed by the halfword at vertex+6;
colours to `0x800B1638`/`0x800A6360`). Meshes are plain data in guest RAM
(bone records at instance+4, vertices at +8 with 8-byte stride, faces at
+0x10). Vertex-normal smoothing is therefore possible as a host sidecar
normal set per model (never rewriting guest RAM); per-pixel lighting needs
the shadow rasterizer to take per-vertex normals/light vectors, which is a
new interpolant on the 4.4 path. Both are last, after 4.4, and only if a
visible win is shown on one model.

### 4.10 Lighting follow-up — queued design, not an implemented option

Owner-approved task `c86647769140` depends on `c57dd513b6ed` and remains
queued. This addendum records feasibility only. Current graphics gate
investigation, fixes and review take priority; the existing graphics/presenter
owner and lead integrator retain ownership. No new lane, worker, runner job,
asset pass or publication is authorized by this addendum. Lighting work starts
after the prerequisite completes, through this same staged plan.

**Faults versus styling.** No new retail lighting fault has been established.
The live HD prototype shows dark-area BGR555 banding and softened texture
(the [combined evidence](../evidence/graphics-filter-hdbg/REPORT.md)); neither
establishes an incorrect native light calculation. Actor colours come from
native per-vertex NCCT in `func_8003B97C_port.c:119-143`, including a conditional
second colour calculation. `func_8006698C_port.c` fills the light colour matrix
from actor/material bytes and scene colour scales. Its existing oracle
contracts must remain intact. Any alleged fault needs a matched native/oracle
comparison before being called a correction. Bloom and contrast are optional
styling; a sharper AI plate beside unchanged actor textures is not proof of
broken actor lighting.

| Candidate | Supported path | Prerequisite or limit |
| --- | --- | --- |
| Restrained lamp/fire/spell glow | Screen-space bloom from positively identified, visible emissive sources on a separate host image | Native packets have no general emissive/material tag. Verify source/context or effect provenance first; brightness can select intensity within that mask, never depth or scene identity. |
| Conservative contrast/colour | Bounded scene-only curve or colour transform, shared by eligible actors and background pixels | Retail UI, text, menus, movies and visibility-critical pixels need reliable exclusion. Preserve the dark toe and readable midtones; no blanket exposure lift or black crushing. |
| Actor/background consistency | Common scene grading first; a later actor-only sidecar may use verified normals, material colours and existing light matrices | Actual model hooks exist, but stable resource/primitive identities and a pure host lighting path are required. Do not alter NCCT results, GTE state or packet colours used by native rendering. |
| Dynamic whole-scene relighting, new plate shadows | Unsupported for current field plates | Backgrounds contain baked lighting; there is no demonstrated layer depth, geometry, normals or shadow-receiver data. Screen brightness is not a depth proxy. |

**Rendering boundary and protection.** `HostFB_GetPresentationPixels`
(`host_framebuffer.c:271`) already exposes a separate scaled/SSAA image;
`BlitPresentationFrame` (`port_main.c:501`) selects it for the presenter.
The developer overlay bypass does not protect retail HUD/text already drawn
inside that image. A whole-frame postprocess is therefore insufficient.
Before enabling either proposed effect, carry host-only classification of
final visible scene/emitter/protected pixels through shadow composition,
including overwrite and semitransparent blend decisions. The validated HD
room/layer/ordinal binding can identify background occurrences, but does not
itself classify lamps or UI. Unknown or mixed provenance must exclude the
effect or bypass the frame; guessed HUD rectangles are not a general solution.

Process a copy, never the borrowed source image or native VRAM. Bloom must
start from visible emitter pixels, use a small bounded radius/intensity, and
leave protected destination pixels untouched so halos do not wash out text,
actors, enemies, pickups or occluding foreground. This is screen-space glow,
not a new physically lit surface or depth-tested shadow. Scene grading must
preserve occlusion and the horror atmosphere without revealing hidden enemies
or pickups. Movies, fades, menus, unsupported scenes and allocation failures
fall back to the original presentation. No guest-state, save, RNG, timing,
VRAM/readback or native screenshot change is permitted.

**Sequence after the prerequisite:** verify one field lamp source and one
battle effect against the existing owner-local matched captures (m0002i frame
1000 is a candidate for entrance lighting; frame10600 for battle fire/effects,
subject to provenance verification). Establish protection masks and native
bypass first. Then prototype independently adjustable, OFF-default bloom and
contrast/colour options with restrained bounds; proposed controls are not yet
registered. Only attempt actor-specific consistency if the geometry/material
hooks supply validated sidecar inputs. Missing provenance blocks that subset
instead of producing a global brightness substitute. Coordinate any HD packet
hook additions with the same presenter owner and lead.

**Acceptance before implementation handoff:** matched-camera before/after
field-lamp and battle-effect images kept owner-local; inspect bright/dark
regions, seams, foreground/actor boundaries and enemy/pickup readability.
Verify protected HUD/text/movie pixels byte for byte, all-OFF presentation
identity, unchanged native VRAM/readbacks and guest/save state, and fallback
for unknown provenance/scene cuts. Run the existing native, focused, guard,
Day-1 and movie gates only for new runtime changes, without duplicating
dispatched jobs. Record each option's CPU cost and total frame-time delta at
the tested internal/output resolution, including busy-time p50/p95 where
available and actual runner device/capacity limits. The current runtime is a
CPU software renderer; GPU inference used to prepare HD assets is a different
cost. No GPU postprocess timing, full-rate promise or whole-game lighting
coverage may be inferred from headless averages. Report supported and blocked
effects separately. No new lighting screenshots or timing results exist yet.

## 5. Frame-loop and geometry survey (summary)

- Host entry `port_main.c:1382` → retail `func_8001220C`
  (`bootstrap/func_8001220C_port.c:69`), inner loop dispatching on
  `D_8009D280`; one field tick per pass; tick order pad → ClearOTag →
  actor/battle/camera/model walk → bg controllers → OFX/OFY → fade → frame
  tail (DrawSync, VSync(2)/VSync(4), PutDispEnv/DrawOTagEnv, buffer flip) →
  `D_8009CDA4++`.
- `HostFB_VSync` (`host_framebuffer.c:356-410`) is deterministic: 564,480
  emulated cycles per tick; each VBlank runs `PE_GPU_VBlankStep`
  (`pe_gpu.c:1605`: vsync count, 4 AKAO ticks, 735 SPU samples) and the
  retail VBlank callback. Wall-clock pacing exists only in the windowed
  build (`pace_vblank`, `host_window.c:725`, 60000/1001 Hz accumulator in
  `host_frame_pacer.h:14-29`); headless is unpaced, which is why the
  baseline in section 3 is a pure CPU-cost number.
- Harness clocks: presents (`g_frame`) or emulated VBlanks
  (`PE_PAD_SCRIPT_CLOCK=vblank`); both wall-clock free.
- Timing instrumentation: `PE_FRAME_TRACE=1` (windowed only) gives per-second
  busy-time p50/p95/max per VBlank; headless has only wall time over the
  `[FB] presents=… vblanks=…` exit line. Current stage measurements use bounded
  headless process wall/user time and RSS, not a per-primitive GPU timer.
  `docs/evidence/pe-frame-pacing/` numbers predate per-VBlank pacing
  and are stale.
- Discrepancy to settle before gating: the gate checklist memory says the
  Day-1 end frame is 67186, the lead's `40b77cba` gate and today's baseline
  both give 66356. The lead's current number (66356) is the one this lane
  compares against.

## 6. Implementation order (Stage 2)

1. **Registry + GRAPHICS menu page** — done (one commit with step 2, so the
   first graphics row has an effect).
2. **Display scaler row** (`NEAREST / SHARP BILINEAR / FSR 1`) — done, presenter
   only, on a presenter thread with one present of latency. Finding: a CPU
   FSR 1 costs ~44 ms per 1280x800 frame with 8 threads on the devbox Xeon,
   so it is a frame-dropping presenter, not a 60 Hz one; a GPU API would be
   needed for a full-rate FSR, which this port does not have. Before/after
   captures come from `pe-upscale-tests --demo` on a route screenshot.
3. **Internal resolution 2x/3x/4x** — implemented as a separate full-VRAM
   shadow and second scan-out. Full runner parity, matched captures and timings
   pass for b5d2c73e; see [internal resolution evidence](../evidence/graphics-internal-resolution/REPORT.md).
4. **Texture filter** (conservative polygon-only bilinear on the shadow) and
   **SSAA** rows — implemented and gated; see
   [combined evidence](../evidence/graphics-filter-hdbg/REPORT.md).
   Each selects a 2x shadow when the requested internal resolution is native,
   so enabling either row alone has an effect. SSAA returns a 320x240
   presentation image. Explicit internal scales are retained; HD backgrounds
   takes precedence over the automatic 2x selection and uses 4x instead.
5. **60 fps** per the frame-loop survey (interpolated camera/actors where the
   state is reachable; native-rate fallback otherwise), with the present
   counter kept on logic frames.
6. **Sub-pixel geometry / perspective correction** (PGXP-style side table).
   Sub-pixel vertex positions are done on the native lineage (2026-10-20,
   branch `claude/modern-20261020`, row SUB-PIXEL GEOMETRY): the sidecar is
   keyed by the packed SXY word as designed in 4.4, holds the MAC0 fraction
   only, and refines a polygon only when every one of its vertex words hits.
   Perspective-correct texturing (the `1/sz` half of 4.4) is not done; see
   [ENHANCEMENTS.md](ENHANCEMENTS.md).
7. **HD background replacement** on the shadow surface (reuses `tools/hdbg`
   output) is implemented in 9ac4f811. A validated packet context binds current
   native source/palette words to the room/layer/ordinal pack. OFF is default;
   PE_HD_BACKGROUNDS=1 enables owner-local 4x packs and selects a 4x surface
   when the requested internal scale is native. Explicit 2x/3x/4x scales stay
   selected. Missing/mismatched art draws original sprites at the effective
   scale. Live prototype and source gate pass; see the combined evidence report.
   Model smoothing remains a later stage.
8. **Widescreen** only for scene classes proven plate-free.

Each step: option OFF-default, env override, one native test per option, the
seven-item gate checklist on the runner (native zero skips, focused CTest,
psyq/retail guards, Day-1 route to `day1_end_80`, movies into Day 2), a
before/after capture at a matched frame, and a merge-ready commit handed to
the lead. No release, tag, push or publication in this task.
