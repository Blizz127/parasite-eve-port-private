# Native port enhancements (opt-in)

Enhancements change how the game plays, unlike the development cheats. They
are opt-in: the default is the retail behaviour, and at the default the port
runs the retail code path unchanged (see *Identity* below).

## Movement speed and Always run

Dev menu (`PE_DEV_MENU=1`): **Options ▸ Enhancements**.

| Row | Values | Default | Suggested by |
| --- | --- | --- | --- |
| Movement Speed | 100 % (retail), 125 %, 150 %, 175 %, 200 %; A cycles, 200 % wraps to 100 % | 100 % | u/stellarola |
| Always Run | Off / On; A toggles | Off | u/stellarola |

**What it scales:** only Aya's player-controlled walk and run velocity in
field scenes. The hooks are in `pc_port/game/boot/func_80035558_port.c`:
the field run core `func_8007136C` (retail speed `0x168000`) and the d-pad walk
`func_800710A4` (`0x50000`). Both scale the speed after retail's two
`func_8003708C` scalings, before the camera rotation, so facing, animation
choice and the rotated velocity vector are unchanged apart from length.

**What it does not scale:** battle (the hooks return retail's speed whenever
`D_8009D1A0 & 2` is set, so ATB, battle movement and enemy timing are
unchanged), other actors (`actor != *0x8009D254`), scripted moves (the VM walk
`func_80013988` is not hooked), cutscenes, animation rates, timers and RNG.

**Always Run** makes the field d-pad walk handler take the Circle-held run path
for Aya (field only). It combines with Movement Speed.

**Collision:** unchanged. The per-frame floor/wall resolution
(`func_8001AE40`) walks the floor mesh from the previous position to the new
one and reverts a rejected move, so a longer step is resolved the same way.
The Day-1 sweep below checks walls and doors empirically at 200 %.

**Persistence:** `$XDG_CONFIG_HOME/parasite-eve-port/enhancements.conf`
(`~/.config/parasite-eve-port/enhancements.conf` when unset), one `<id>=<value>`
line per registered option (`move_speed=<pct>`, `always_run=<0|1>`, and the
assists below), written whenever a menu row changes; deleting the file resets
everything to retail.
Overrides: `PE_ENH_CONFIG=<path>` (a different file), `PE_MOVE_SPEED=<pct>`,
`PE_ALWAYS_RUN=<0|1>`; the env overrides are not written back. A non-default
setting logs one `[ENH] movement speed N%, always run on|off` line at start.

Without `PE_DEV_MENU=1` the menu row is not reachable, but a saved config or the
env overrides still apply. `PE_CHEATS=0` disables the dev menu only.

## Assists (dev menu ▸ Assists)

Assists are for players who want to see the game rather than grind it. They
are opt-in like the enhancements above, persist in the same
`enhancements.conf`, and at their defaults every hook returns its input and
writes nothing, so the retail code path runs unchanged.

| Row | Values | Default | Config line / env override | Suggested by |
| --- | --- | --- | --- | --- |
| 1-Hit Kills | Off / On; A toggles | Off | `one_hit_kills=<0\|1>` / `PE_ONE_HIT_KILLS` | the owner |
| God Mode (No Damage) | Off / On; A toggles | Off | `god_mode=<0\|1>` / `PE_GOD_MODE` | u/SandmansSlave |
| EXP/BP Multiplier | 1x (retail), 2x, 3x; A cycles, 3x wraps to 1x | 1x | `exp_mult=<1\|2\|3>` / `PE_EXP_MULT` | u/SandmansSlave |

A non-default assist logs one `[ENH] assists: 1-hit kills on|off, god mode
on|off, EXP/BP xN` line at start. The env overrides are not written back.

**1-Hit Kills** (`pe_enh_enemy_damage`, hooked in the battle damage routine
`func_80028574`, `pc_port/game/boot/func_80028574_port.c`): every hit Aya
lands, with a weapon or with Parasite Energy, is lethal. The hook runs after
retail has computed the final damage (power, defence, element, criticals,
status, the 1-or-2 minimum) and only raises it to the enemy's remaining HP
(`body+0x10`), so the HP store lands on exactly 0 and retail's own death,
EXP, BP and item-drop logic (`func_80027D14` → `func_80028E94`) runs as for
any kill. Nothing else in the routine changes: hit flashes, status infliction
and knock-back are retail.

*Caps and caveats.* **Story-scripted battles are exempt.** When the game's
battle flag word `D_8009D1A0` has bit `0x800` set (the Day-1 Eve stage fight
has it; ordinary fights such as the rats do not), the hook returns retail's
damage unchanged. Found the hard way: with the assist active there, Eve dies
at the first shot and the fight script, which ends the battle on its own cue
rather than on her HP, waits forever (the Day-1 autopilot never left the
fight). `test_ASSIST_one_hit_kills` checks the exemption. Beyond that flag the
assist does not know which enemies are bosses. A boss
whose room script advances a phase on an HP threshold (rather than on death)
sees its HP go from full to 0 in one hit and never crosses the threshold; the
death path still runs, but whether the script then continues or waits for a
phase it never saw is per boss and has **not** been verified here. Eve's forms
are separate battle records, so each form dies in one hit and the story script
spawns the next as usual. No per-enemy cap is applied; if a boss softlocks with
the assist on, turn it off for that fight (it can be toggled mid-battle from
the dev menu). Verifying the Day-1 bosses (Eve on stage, the M34 alligator)
end to end with the assist on is the open item for this row.

**God Mode (No Damage)** (`pe_enh_player_shielded`): Aya's HP never goes
down. Three retail routines skip only their HP store while it is on:
`func_8001F4D4` (an enemy attack landing; the hit reaction, flash, damage
number and the `D_8009D1D0` attacker record still happen), the contact-damage
tick in `func_8001D340`, and `func_80020288` (the HP-halving, HP-to-three-
quarters and HP-to-1 / instant-death statuses; other statuses such as poison,
blind, confusion still apply and still time out). PE (energy) is not shielded:
the PE-shield status still spends PE for damage as retail does. Scripted story
deaths and cutscenes are untouched: they do not go through these damage
stores. This assist differs from the older **F2 God Mode** cheat on the dev-menu
root, which is session-only and simply refills HP to max every frame; the two
can be on together.

**EXP/BP Multiplier** (`pe_enh_exp_award` / `pe_enh_bp_award`, hooked at the
award point in `func_80028E94`, `pc_port/game/boot/func_80027D14_port.c`):
the EXP an enemy is worth (`body+0xE`) and its BP reward (`body+0x98 −
body+0x9A × body+0x9C`, when positive) are multiplied before being added to the
battle totals `D_8009D304` / `D_8009D21C`. BP is included because it is
awarded at the same point by the same routine. The results screen and the
level-up logic that consume the totals are retail, so level-ups, stat growth
and BP spending behave normally with larger totals. Item drops are not
multiplied.

### Assist tests (`pc_port/tests/test_assists.h`, `ASSIST_*`)

| Test | What it proves |
| --- | --- |
| `ASSIST_defaults_are_retail` | defaults off / x1; every hook is the identity over a range and writes no guest state (2 MB compare); the retail battle state hashes of ATK17 case 0 (damage) and an ATK18 award case (defeat/results) hold with the hooks compiled in |
| `ASSIST_one_hit_kills` | ATK17 fixture: a 1000-HP enemy is at exactly 0 after one retail-weak hit with the retail hit-state bits set; an already-lethal hit is unchanged; the kill awards retail EXP 20 / BP 70; off again, the ATK17 hash returns |
| `ASSIST_exp_mult` | x1/x2/x3 give exactly 20/40/60 EXP and 70/140/210 BP for the ATK18 fixture enemy; item drop unchanged; clamping and cycling |
| `ASSIST_god_mode` | a retail hit takes 45 → 15 HP; with god mode the same hit leaves 45 and still registers the attacker; HP-halving and HP-to-1 statuses leave HP; the instant-death status at 1 HP does not kill; off, the statuses act as retail (45 → 22, 1 → dead) |
| `ASSIST_config_and_env` | persistence in `enhancements.conf`, movement settings untouched, env overrides apply and are not written back |
| `ASSIST_dev_menu_rows` | root row 8 opens ASSISTS; the three rows toggle/cycle; OPTIONS stays row 7 |

Native suite on the assists tree (`PE_DISC1_BIN` set): `Results: 1632 run, 1632 passed, 0 failed, 0 skipped` (after the option-registry commit; 1631 before it); the retail ATK17/ATK18 state-hash tests pass with the hooks compiled in.

Default-build identity gate of the assists binary (b4b0bf95, sha256 `37a304e0…`)
on the blizzpc runner, job `20260930-123023-pe-assists-gate-1512826`:
`REGRESS PASS until=day1_end_80 frame=67186 token=A8000048 story=80`,
`MOVIES_DAY2=PASS`, and no `enhancements.conf` was created. Showcase video:
`~/Videos/decomp-demos/parasite-eve/parasite_eve_assists.mp4` (capture
`pc_port/tools/assists_showcase.py`, encode `assists_showcase_encode.sh`):
the Day-1 autopilot's Eve fight with God Mode off/on side by side (HP
45 → 42 → 35 → 28 vs 45 throughout, everything else frame-identical), the
rat fight with 1-Hit Kills off/on, and its results with EXP/BP x1 vs x3.

## Graphics (dev menu ▸ Options ▸ Graphics)

Graphics options change how the finished frame is shown, never what the game
computes. They live in the same registry and `enhancements.conf`, and every
one defaults to the retail presentation.

| Row | Values | Default | Config line / env override | Suggested by |
| --- | --- | --- | --- | --- |
| Display Scaler | Nearest (retail), Sharp Bilinear, FSR 1 (CPU); A cycles, the last wraps to Nearest | Nearest | `display_scaler=<0\|1\|2>` / `PE_DISPLAY_SCALER` | the owner |
| Internal Resolution | Native (retail), 2x, 3x, 4x; A cycles | Native | `internal_scale=<1\|2\|3\|4>` / `PE_INTERNAL_SCALE` | the owner |
| Texture Filter | Nearest (retail), Bilinear (polygons) | Nearest | `texture_filter=<0\|1>` / `PE_TEXTURE_FILTER` | the owner |
| Supersample AA | Off, On | Off | `ssaa=<0\|1>` / `PE_SSAA` | the owner |
| Dithering | Retail, Fine (scaled), Off | Retail | `dithering=<0\|1\|2>` / `PE_DITHERING` | the owner |
| Sub-pixel Geometry | Off, On | Off | `subpixel_geometry=<0\|1>` / `PE_SUBPIXEL_GEOMETRY` | the owner |

**Where it came from.** These four rows were built and gated on the
2026-09-30 lineage, branch `claude/gfx-options` (8ea0d39e3), and ported to the
native lineage on 2026-10-20. Only the graphics hunks were carried over,
three-way merged over the native tree:

- the registry rows;
- the presenter scaler (`pc_port/platform/host_upscale.h`, `HostWindow_Blit`
  in `host_window.c` and `host_window_win32.c`);
- the shadow render surface in `pe_gpu.c`;
- the second scan-out in `host_framebuffer.c`;
- `BlitPresentationFrame` in `port_main.c`.

HD Backgrounds (`pe_hdbg*`) was **not** ported: it needs owner-local packs and
its own loader threads. Sub-pixel Geometry was added on this lineage
(below). The design and the remaining stages (frame interpolation,
perspective-correct texturing, widescreen) are in
[GRAPHICS_OPTIONS.md](GRAPHICS_OPTIONS.md).

**Display Scaler** picks the filter that fits the 320x240 frame into the
window. It runs after `HostFB_GetPixels`, so emulated VRAM, the present
buffer, screenshots, the VBlank and present counters and guest RAM are
byte-identical whatever the setting; only window pixels differ.

- **Nearest** is the retail path: `HostWindow_ScaleBGRA`, or the
  pixel-identical XRender transform, synchronous on the game thread exactly
  as before.
- **Sharp Bilinear** pre-scales by the largest whole factor that fits, then
  interpolates only between those big texels. That gives crisp pixels
  without the shimmer nearest shows at non-integer window sizes.
- **FSR 1** is a CPU port of AMD FidelityFX Super Resolution 1.0 (EASU plus
  RCAS, MIT reference, licence in
  `pc_port/third_party/FidelityFX-FSR-LICENSE.txt`). It is a spatial upscaler
  of the 2D frame and adds no geometry detail.
- The filtered modes run on a presenter thread with one present of latency.
  A slow filter drops frames rather than slowing the game.
- Windows offers only Nearest and Sharp Bilinear.
- Cost on this devbox, measured 2026-10-20: an Intel Xeon Gold 6338N at
  2.2 GHz, shared with other work at load 16-25. Each figure is the best of
  5 from this branch's `pe-upscale-tests --demo` on a 320x240 route
  screenshot, with the default pool (8 threads here). Two runs (10:12Z and
  10:50Z) gave the ranges shown.

  | Window | Sharp Bilinear | FSR 1 |
  | --- | --- | --- |
  | 1280x800 | 3.1-3.2 ms | 23.7-25.7 ms |
  | 1920x1080 | 5.2-5.8 ms | 42.0-43.6 ms |
  | 2560x1600 | 12.3-12.7 ms | 97.7-102.9 ms |
  | 3840x2160 | 22.9-25.4 ms | 170.5-177.5 ms |

  On one thread FSR 1 took 135-151 ms at 1280x800 and 902-1052 ms at
  3840x2160.
  The game shows a new frame every 33 ms, because it draws at 30 Hz: 39,998
  VBlanks for 20,000 presents in the runs below. FSR 1 keeps up at
  1280x800 here. At 1080p and above the presenter shows fewer filtered
  frames, while the game itself keeps its speed. The owner's devices have
  other CPUs, so these figures hold for this host only.

**Internal Resolution** renders into a separate 2x/3x/4x shadow surface. The
original 1024x512 VRAM stays the authority for game readbacks, texture
fetches and captures.

- Polygons (actors, battle arenas) gain finer edge coverage.
- Background tiles and UI keep nearest sampling, so they look the same as
  with window scaling.
- Projection stays integer unless Sub-pixel Geometry is on, and texture
  mapping stays affine (no perspective correction).
- Movies present the native 320x240 decoded frame: a larger copy adds
  nothing, and the native frame keeps the X11 XRender fast path. The
  Display Scaler still applies.
- While the dev menu, the help text or a toast is up, the window shows the
  native frame, because that text is laid out for 320x240. The CHEATS badge
  alone is drawn on the presentation frame, so a used cheat or warp does not
  turn these options off.
- An allocation failure falls back to native.
- `PE_ROUTE_PRESENTATION_SHOTS=1` with `PE_ROUTE_SHOT_RANGE` also writes the
  presentation frame (`f<N>-presentation.ppm`); ordinary captures stay native.
- Cost: the CPU rasterizer does S² times the pixel work, on the game
  thread. Measured on this devbox on 2026-10-20 with binary 2fd26b8c
  (049784820, before Sub-pixel Geometry) over the first 20,000 presents of
  the scripted Day-1 route: movies skipped, field rooms and in-engine
  scenes, no battle. Four runs went at once on one core each, with the host
  at load about 22. CPU time was read inside the sandbox; the wall range
  spans that set and an earlier one timed from outside it.

  | Internal Resolution | CPU per present | Wall per present | Peak RSS |
  | --- | --- | --- | --- |
  | Native | 4.8 ms | 4.8 ms | 8.7 MB |
  | 2x | 15.6 ms | 15.6-16.7 ms | 13.4 MB |
  | 3x | 26.5 ms | 26.6-28.7 ms | 19.4 MB |
  | 4x | 41.3 ms | 41.3-45.9 ms | 28.4 MB |

  The budget is 33 ms per present. Above it the game runs slower than
  real time, because the window's pacer drops the backlog instead of
  catching up. On this host that means 4x, before any battle; a battle
  draws more polygons than this segment. Two sets of runs gave the same
  result: each 2x/3x/4x log matched the native one line for line (the
  `[ENH]` line aside), and every run ended on the same native screenshot
  (sha256 17461834…). So 2x-4x did not change the game over that segment.

**Texture Filter** filters polygon texels on the shadow surface only, after
the CLUT lookup. It keeps the nearest texel's transparency and STP class and
excludes incompatible neighbours. Background sprites and UI stay nearest.

**Supersample AA** box-filters the shadow into a separate native-sized
presentation frame.

**Dithering** sets how the shadow surface dithers gouraud shading and
modulated textures. Retail dithers them with a 4x4 pattern, and native VRAM
always does.

- **Retail** draws that pattern at native-pixel size, magnified.
- **Fine** draws the same pattern at the shadow's own resolution: a finer
  grain, the "scaled dithering" look of modern PS1 renderers.
- **Off** skips it. Gradients are smooth, but 15-bit banding shows in dark
  ramps.

**Sub-pixel Geometry** places 3D polygons on the shadow surface at the
position the GTE computed before it rounded down to whole pixels. Vertices
land on the nearest half, third or quarter pixel (at 2x, 3x, 4x) instead of
a whole pixel, so moving edges step in finer increments. Geometry very close
to the camera still steps by a pixel or more, as on the original: the GTE's
view-space coordinates are whole units, and the kept fraction is only what
its 16.16 screen result drops.

- The GTE's projection (RTPS/RTPT) computes each screen coordinate in 16.16
  fixed point (MAC0) and keeps only the integer part in SXY. With the row on,
  `pe_gte_commands.c` also records the dropped fraction, under the packed
  SXY word it wrote, in a host-only table. Every GTE register and FLAG bit
  stays the same; the hardware-log replay checks that with recording on.
- When a polygon reaches the GPU, `pe_gpu.c` looks up each vertex word. If
  every vertex was recorded, the polygon is drawn on the shadow surface at
  those fractions, rounded to the nearest shadow pixel inside the original
  pixel. If any vertex is missing, the whole polygon keeps the integer shape.
  Menus, text boxes and other 2D primitives are built from CPU coordinates,
  so they stay pixel-exact even when one corner equals a projected vertex.
- Native VRAM, captures and game state never see it. Vertices that share a
  word get the same answer, so adjoining recorded polygons meet without gaps.
  A polygon with an unrecorded vertex next to a recorded one can show a seam
  under one native pixel wide.
- The all-or-nothing rule was chosen from a count over the Day-1 route
  segment (44,600 presents up to the frame-44602 game over), taken with a
  measurement-only prototype of the same lookup (4,096 slots, saturated
  coordinates not recorded, no drawing change). Of the textured polygons,
  22,386,108 had every vertex recorded (99.87 %), 4,763 some (0.02 %) and
  25,081 none. Of the untextured ones, 836,682 had all, 863 some and
  23,824 none. The shipped table is larger (8,192 slots) and also records
  saturated coordinates; it was not re-counted.
- It does not change texture mapping: textures stay affine, so large floor
  polygons still warp as on the original hardware.
- Like the Texture Filter, it selects a 2x surface when Internal Resolution
  is Native. The effect grows with the scale: at 2x a vertex can sit on two
  positions per original pixel, at 4x on four.
- Cost: in the capture runs below (binary c0c298fc, four runs at once),
  turning the row on added 0.6 % wall time at 2x (44,210 presents: 658.5 s
  to 662.4 s) and 1.8 % at 4x (4,810 presents: 199.7 s to 203.2 s). That is
  within the noise of runs sharing the host.
- The actor models are projected by `func_8003AC90`
  (`game/boot/func_8003A088_port.c`), a native translation, not matched C.
  This row is an enhancement on top of that path, not a fix to it.

Texture Filter, Supersample AA, Dithering and Sub-pixel Geometry each select a 2x surface by
themselves when Internal Resolution is Native; an explicit 2x/3x/4x choice is
kept.

**Tests on the native lineage (2026-10-20):**

| Check | Result |
| --- | --- |
| Native suite | `Results: 1690 run, 1690 passed, 0 failed, 0 skipped` (with Dithering and Audio), which includes `test_ASSIST_option_registry` (9 options, 4 graphics, labels, env overrides, the scale/filter/SSAA coupling) and `test_ASSIST_dev_menu_graphics_page` (Options ▸ Graphics rows, BACK to Options row 2, Enhancements still row 1) |
| CTest | 12/12: `upscale-filters`, `upscale-filters-single-thread`, `upscale-no-threads`, `window-scaler-failures`, `scaler-windows-policy`, `gpu-shadow-parity`, `framebuffer-shadow-parity`, `gpu-filter-parity`, `gpu-dither-parity`, `gpu-subpixel-parity`, `framebuffer-ssaa-parity`, `window-scale-math` |
| Sub-pixel Geometry, unit level | `gpu-subpixel-parity` (scales 2-4): native VRAM is identical with the lookup on and off. A polygon whose vertex words are all known moves by its fraction on the shadow; one with an unknown vertex keeps the integer shadow; scale 1 never consults the table. `gte-native-fixed` and `gte-native-hardware`: every fixed case and all 1,100 real-console cases pass again with recording on (463,492 checks, 0 failures), plus record/lookup cases for the fraction, a saturated axis, an unknown word and the clear on switch-off. The suite checks the sixth graphics row, its default, env override and the automatic 2x |
| Sub-pixel Geometry, captures (binary c0c298fc) | Day-1 route with `PE_ROUTE_PRESENTATION_SHOTS`: field rooms at 4x (frames 2200-4800 every 650) and the Eve fight at 2x (frames 43000-44200 every 300), row off and on. Logs match line for line apart from the `[ENH]` line (4,810 and 44,210 presents), and every native frame is identical. The presentation frames differ only on the 3D characters and their shadows (3.7-5.6 % of pixels; frame 2200 not at all). Backgrounds, the battle HUD and dialogue text are pixel-identical, and a zoomed crop shows no gaps |
| Dithering | `gpu-dither-parity` covers scales 2-4. Native VRAM is identical in all three modes. Retail is the magnified native pattern, Fine follows the 4x4 pattern at shadow resolution, and Off is flat. The suite checks the row's label, its default and the automatic 2x |
| Identity of 1x VRAM and the 320x240 frame with the shadow on | `gpu-shadow-parity`, `framebuffer-shadow-parity`, `gpu-filter-parity` and `framebuffer-ssaa-parity` (shadow on and off over the same primitives) |
| Day-1 route, defaults (2026-10-20, binary 2fd26b8c from 049784820) | No textual or behavioural difference against seven earlier route logs, with flash-shim lines excluded. Route packet `/home/blizz/scratch/pe-modern-route-049784820` |
| Day-1 route, rows forced on (same binary) | Internal Resolution 2x, Texture Filter, Fine dithering, SSAA, and Movie/Music/SFX Volume 50 %, set by env in the sandbox. Packet `/home/blizz/scratch/pe-modern-route-049784820-forced`. Results below |
| Day-1 route, rows and Sub-pixel Geometry forced on (binary c0c298fc from 304457606) | The same rows plus Sub-pixel Geometry. Packet `/home/blizz/scratch/pe-modern-route-304457606-subpixel`, compared with nine references: the eight used for the forced-on run above, plus that run. The only log difference is the `[ENH]` line: +1/−0 against the eight default-option runs, +1/−1 against the forced-on one. Same checkpoints (field_start 963, eve1_room 42707), same frame-44602 game over, same final native screenshot (sha256 12c810bd…). CPU_BOUNDARY, STOP, stubs and DAY1_SHIM 0; no `enhancements.conf`. 12 min 8 s wall, sampled port peak RSS 14.0 MB |
| NOT RUN | live window and device checks. Display Scaler and Master Volume act only in a window or audio device, so headless runs do not cover them. Analog Stick and controls.conf are window-only too. The costs above are headless and for this devbox only |

Results of the forced-on route:

- The only log difference against all eight references is the `[ENH]` startup line (+1/−0 lines).
- There is no behavioural difference: same route checkpoints, same frame-44602 game over, and the same final native screenshot (sha256 12c810bd…).
- CPU_BOUNDARY, STOP, stubs and DAY1_SHIM are all 0, and no `enhancements.conf` was created.
- The headless route runs the native SPU and AKAO driver, so the volume gains were executed in the mixer.
- Cost: 16 min 18 s wall time against about 3.5 min for the defaults (CPU capped at 2 cores), with a peak port RSS of 13.7 MB against 8.6 MB.

The first graphics gates (03d473d72) ran on a local `zig cc` (clang) build,
before this devbox had a system GCC. Every later gate and the route
binaries above (2fd26b8c, c0c298fc) are GCC 15.2 builds (`readelf -p
.comment`: `GCC: (Ubuntu 15.2.0-16ubuntu1) 15.2.0`).

## Audio (dev menu ▸ Options ▸ Audio)

Audio options set output levels only. The sound driver (AKAO, matched C),
the SPU synthesis (voices, gaussian interpolation, reverb) and every
emulated register stay retail.

| Row | Values | Default | Config line / env override | Suggested by |
| --- | --- | --- | --- | --- |
| Master Volume | 100 % (retail), 90 % … 0 %; A steps down by 10, 0 % wraps to 100 % | 100 % | `master_volume=<0..100>` / `PE_MASTER_VOLUME` | the owner |
| Movie Audio Volume | 100 % (retail), 90 % … 0 % | 100 % | `movie_volume=<0..100>` / `PE_MOVIE_VOLUME` | the owner |

**Master Volume** scales the samples handed to the PulseAudio device
(`HostAudio_Submit` in `host_audio.c`, through `HostAudio_GainSample`).

- The SPU mix, the `PE_AUDIO_WAV` dump and every test capture stay at the
  retail level.
- At 100 % the samples pass through untouched.
- Otherwise they are scaled in fixed stack chunks, so there is no allocation
  that could fail and play at full level.
- A level change ramps linearly across one submit (`HostAudio_GainRamp`), so
  it does not click.
- Windows has no audio device backend yet, so the row has no effect there.

**Movie Audio Volume** scales the SPU's CD-input contribution: FMV
soundtracks and CD-XA streams such as the ending credits.

- It covers both the dry signal and the CD reverb send, at the one point
  where CD samples meet the CD volume registers (`pe_spu.c`,
  `PE_Spu_SetCdGain`).
- At 100 % the mixer skips the multiply, so the mix is bit-identical.
- The gain is a host option, not SPU state, so `PE_Spu_Reset` keeps it.

The volume lists fall from the retail value, which is the largest. So
`snap()` now picks the largest listed value at or below the input, or else
the smallest listed one. That is unchanged for every list that rises from
retail.

Config handling (all options):

- Config and env values must be whole decimal numbers. Anything else, such
  as `loud` or an empty value, keeps the retail value.
- The config file is written to a temp file and renamed into place, so an
  interrupted save never leaves a truncated value.
- An env override is never written back, even when another option is saved
  from the menu.

**Tests** (native suite):

- `AUDIO_cd_gain`:
  - 100 renders identically;
  - 50 halves the CD contribution within rounding;
  - 0 is silent;
  - the gain clamps and survives a reset.
- `ASSIST_option_registry`:
  - rows, labels, stepping and snapping (55 → 50, -5 → 0, 150 → 100);
  - persistence, and an env override that is not written back;
  - the CD-gain coupling;
  - the device-sink gain helper.
- `ASSIST_dev_menu_graphics_page`: Options ▸ Audio (row 3) steps both levels.

## Music and SFX volume (dev menu ▸ Options ▸ Audio)

Two more rows on the Audio page, after Master Volume and Movie Audio Volume.
They scale SPU voices by class inside the mixer (`pc_port/platform/pe_spu.c`),
both the dry signal and the reverb send.

| Row | Values | Default | Config line / env override | Suggested by |
| --- | --- | --- | --- | --- |
| Music Volume | 100 % (retail), 90 %, ..., 0 %; A steps down 10, 0 % wraps to 100 % | 100 % | `music_volume=<0..100>` / `PE_MUSIC_VOLUME` | the owner |
| SFX Volume | 100 % (retail), 90 %, ..., 0 % | 100 % | `sfx_volume=<0..100>` / `PE_SFX_VOLUME` | the owner |

**Which voices are SFX.** The AKAO driver's 12 sound-effect channels own
SPU voices 12..23. A voice is SFX while its bit is set in
`D_800BCD50 | D_800BCD60`. `D_800BCD50` is the active SFX mask
(`func_8008A750` sets bits, `func_8008F1B0` clears them).
`D_800BCD60` holds SFX parked by `func_8008C8B8`. Music can use any voice.

At every key-on register write, pe_spu reads that mask through
`PE_AudioDriver_SfxVoiceMask` (`pc_port/platform/pe_audio_driver.c`). Each
keyed voice keeps its class until its own next key-on, so a release tail
keeps the class it started with. The read happens at the write because all
AKAO key-ons are issued in one `func_8008770C` store at the end of
`func_80089328`, the tick's first call, and that store reaches pe_spu
synchronously. At that point:

- music cannot have keyed a voice in the mask;
- the SFX key-on mask `D_800BCD54` has already been zeroed;
- an SFX that ends later in the same tick has not yet cleared its
  `D_800BCD50` bit. Reading after the tick would class a one-tick SFX as
  music.

The comment above `PE_AudioDriver_SfxVoiceMask` gives the file:line chain.

**Identity.** A class at 100 % skips the multiply, so at 100/100 the mix is
bit-identical to a mixer without classes (`test_AUDIO_voice_class_gains`).
Classes are latched even at 100 %, so changing a level mid-note takes effect
on the voices already sounding.

**Known limits.**

- On unpause, `func_8008C9DC` replaces `D_800BCD50` with the parked mask
  (`src/func_8008C9DC.c:27`). That drops the bits of pause-menu SFX that were
  still playing. A key-on that one of those voices still had pending is
  classed music.
- A key-on from outside AKAO is classed by AKAO's mask at that moment: SFX
  if an AKAO sound effect owns the voice, otherwise music. There is no such
  key-on in `src/`. `pe_plat_audio_voice_key_on` could produce one. With no
  mask source installed, every voice is music.
- A key-on written through a pe_mmio host pointer would be classed at
  `PE_MMIO_Commit`, not at the store, and so would hit the one-tick case.
  None were found: the only AKAO key-on writer, `func_8008770C`
  (`spu_voice_port.c`), stores through `PE_SpuRegister_StoreU16`.

**Tests** (native suite):

- `test_AUDIO_voice_class_gains`: classes from the mask, 100/100
  bit-identity, music 50 halves only music voices and SFX 50 only SFX voices
  (dry and reverb send), gains kept across `PE_Spu_Reset`.
- `test_AUDIO_one_tick_sfx_class`: the real native `func_80089328` key-on
  followed by `func_8008F1B0` before the commit.
- `test_ASSIST_option_registry` and `test_ASSIST_dev_menu_graphics_page`:
  the rows, labels, env overrides and the Audio page.

Results on 2026-10-20 (local `zig cc` build of this branch):

- Native suite: `Results: 1692 run, 1692 passed, 0 failed, 0 skipped`.
- `akao-tick-oracle`, with its image built by
  `build_leaf_oracle.py --range 0x80085000 0x80092000 func_8008DB7C`:
  `300 cases, 219 judged, 0 mismatched, 81 oracle faults (skipped) -> PASS`.
  The oracle runs with the synthesizer inactive. It shows that the tick's
  guest RAM and SPU registers are unchanged. It does not exercise the
  classifier, which `test_AUDIO_one_tick_sfx_class` covers.

## Controls (dev menu ▸ Options ▸ Controls)

**Analog Stick** (`analog_stick=<0|1>`, `PE_ANALOG_STICK=<0|1>`, default
OFF). This is retail's DualShock analog mode.

**OFF (retail default).** Behaviour is unchanged:
- every pad fill stores ID word `0x4100` (status 0, ID 41h digital);
- the left stick maps to the d-pad.

**ON, with an SDL GameController connected:**
- the fills store `0x7300` (ID 73h, DualShock analog);
- they write the stick bytes at `0x800BE9A4..A7` (RX, RY, LX, LY; 0x80 = centre; up/left 0x00);
- the left stick no longer sets d-pad bits in the game's button word.

With no GameController (raw joystick or `/dev/input/js*` only), on Win32, or
with a route-pad / `PE_PAD_SCRIPT` source, the reply stays digital. Installing
a pad source drops the analog source. The SDL axis converts to a byte as
`(v + 32768) >> 8`. The retail game then runs its own analog code, unchanged.

What retail does in analog mode (each item checked against the matching C
in `src/`):

| Retail fact | Where |
| --- | --- |
| Analog branch taken when `(D_800BE9A0 & 0xF000) == 0x7000`. It reads `D_800BE9A6` (LX) and `D_800BE9A7` (LY): `< 0x5A` / `>= 0xA1` set Up 8 / Down 0x20 / Left 0x40 / Right 0x10 in `D_8009D26C`; `< 0x14` / `>= 0xE7` also set bit 0 (value 1, run) | `src/func_8003EB04.c:138-178` |
| **Before the stick, `D_8009D26C &= ~0x79`.** In analog mode the d-pad direction bits and the Circle run bit are cleared, so the d-pad does not move Aya or menu cursors; direction and run come from the stick only | `src/func_8003EB04.c:139`; table `D_800A76F0` slots 0, 3-6 (`func_8003E974`, `test_3E974_array_final_table`) |
| While window id 0 is open (`func_80062A34(1,0)`: a node keyed (1,0) in the `D_8009D154` window list, created by `func_80062D2C(0,…)` in `func_800438EC` / `func_8004E97C`), only full deflection counts and run is never set | `src/func_8003EB04.c:140-152`, `src/func_80062A34.c`, `src/func_80062D2C.c` (`s[8] = 1`, `s[9] = a0`) |
| The field walk steers by `atan2(LY-0x80, LX-0x80) - 0x400` plus the analog camera yaw `D_800BD022` | `src/func_800710A4.c:45-57` |
| The run walk, battle facing and camera yaw take the same analog paths | `src/func_8007136C.c`, `src/func_80071754.c`, `src/func_80066CE8.c:1374` |
| These are the only readers of the pad ID / stick bytes in retail: a load/store scan of `SLUS_006.62` and all 518 `PE.IMG` entries (`lui 0x800C` plus offsets 0xE9A0..0xE9A7) finds 3E944 (init), 3EB04, 66CE8, 710A4, 7136C and 71754, and nothing in any overlay | binary scan, 2026-10-20 |
| libpad: the game asks for analog mode itself (`func_8008292C(0,1,0)` = PadSetMainMode analog, unlocked) once `func_800825C0` (PadGetState) reports Stable and `func_80082680(0,2,0)` reports an extended ID. The port has no SIO0 device and no pad interrupt, so the record at `0x800A5B70` stays PadStateDiscon. The handshake does not run, for the digital or the analog reply; the analog branch depends only on the ID. | `src/func_8003EB04.c:56-74`, `pe_controller_sdk_port.c`, `pe_save.c` |

Port change needed for analog mode: `func_800710A4`'s hand port had only the
d-pad half. With ID 73h it wrote no walk velocity, so Aya could not walk. The
analog block of `src/func_800710A4.c` is now ported
(`pe_walk_analog_apply`, `game/boot/func_80035558_port.c`). The other four
consumers were already line-for-line.

Tests (`pc_port/tests/test_analog_stick.h`, `ANALOG_*`):
- **`ANALOG_axis_to_byte`**: axis-to-byte conversion.
- **`ANALOG_fill_off_is_today`**: with the option off, or without an analog source or controller, the 8 pad bytes equal the old fill's.
- **`ANALOG_fill_on`**: ID 73h and the stick bytes.
- **`ANALOG_retail_branch`**: the generated `func_8003EB04` with the real libpad init. Checks:
  - half deflection gives a direction;
  - full deflection gives a direction plus run;
  - the d-pad and Circle-run are cleared in analog mode;
  - the menu-window case;
  - no handshake call.
- **`ANALOG_field_walk`**: the stick up/right walk equals the d-pad walk, and `D_800BD022` turns the heading.
- **`ANALOG_dev_menu`**: the dev menu row.
- **Host-pad ctests**: `MapControllerEx`, `HostPadSDL_AnalogSticks`, and the no-stick masks.

**Results (2026-10-20, local `zig cc` build, `-j 3`):**

| Check | Result |
| --- | --- |
| Native suite | `Results: 1696 run, 1696 passed, 0 failed, 0 skipped`. This includes `DAY1_game_input`, the retail-hashed `func_8003EB04` cases with ID 0x7300 and edge stick bytes. |
| CTest `-R host-pad` | `100% tests passed out of 3` (`host-pad-input`, `host-pad-sdl`, `host-pad-hotplug`) |

**Not done:**
- no live controller or device check;
- no route run, neither with the option on nor the default-off Day-1 identity run. The default path changed shape (the fill sites call a helper, and `func_800710A4` has a new ID check), and these unit tests cover it only;
- no Win32 path: that backend has no GameController, so only a stub was added.

In analog mode the keyboard's arrow keys feed the same d-pad bits, so like
the d-pad they do not move Aya or menu cursors.

## Controls: remapping

Opt-in, config-file driven; there is no menu row and no registry entry. With no
file the pad word and the keyboard table are exactly what the port did before
remapping existed (the default keys are listed below). Nothing here ever
creates the file.

**File:** `$XDG_CONFIG_HOME/parasite-eve-port/controls.conf`
(`~/.config/parasite-eve-port/controls.conf` when unset, next to
`enhancements.conf`; `%APPDATA%\parasite-eve-port\controls.conf` on Windows).
`PE_CONTROLS_CONFIG=<path>` reads a different file (`/dev/null` for a clean run).
It is read once, when the window opens, and logs one
`[CONTROLS] loaded N remaps from <path>` line; headless runs never read it.

**Format:** one setting per line; `#` starts a comment; blank lines, spaces
around `=` and `,`, and CRLF line ends are fine; names are case-insensitive.

| Line | Meaning |
| --- | --- |
| `pad.<psx>=<psx>` | the PS1 button `<psx>` that a connected controller produces is delivered to the game as the other one |
| `key.<psx>=<key>[,<key>...]` | the keyboard keys for `<psx>`; the line **replaces** that button's default keys (at most 16 keys) |

`<psx>` is one of `cross circle square triangle l1 r1 l2 r2 select start up
down left right`. `<key>` is an X11 keysym name or `0x<hex keysym>`: a letter or
digit, `Return Tab BackSpace space Left Up Right Down Home End Prior Next
Page_Up Page_Down Insert Delete`, `KP_0`..`KP_9 KP_Enter KP_Add KP_Subtract
KP_Multiply KP_Divide KP_Decimal` and the keypad navigation names (`KP_Left`,
`KP_End`, ...), `F9`..`F12`, `Shift_L Shift_R Control_L Control_R Alt_L Alt_R`,
and `comma period slash semicolon apostrophe bracketleft bracketright backslash
minus equal grave`. A letter binds both cases (`z` is `z` and `Z`).

Default keys (what you get for any button without a `key.` line): Cross =
Return, Space, Z, X; Circle = C; Triangle = V; Square = S; L1/R1 = Q/E; L2/R2 =
1/3; Select = Tab; Start = P; arrows = the d-pad.

Example, swapping confirm/cancel on the controller and adding a WASD d-pad on
the keyboard (the arrows stay listed; Square moves off `S`, which Down now takes):

```
# controller: the A button confirms as Circle, B cancels as Cross
pad.cross=circle
pad.circle=cross

# keyboard
key.up=w,Up
key.left=a,Left
key.down=s,Down
key.right=d,Right
key.square=f
```

**Rules**
- An unknown or invalid line is ignored and logs one warning,
  `[CONTROLS] <file>:<line>: <reason>`; the rest of the file still applies. One
  bad key name ignores that whole `key.` line.
- A repeated `pad.<x>` or `key.<x>` line replaces the earlier one, with a warning.
- A key drives one button: an explicit `key.` line takes its keys from whichever
  button had them (a default, or an earlier line), whatever the line order.
  `key.cross=c` therefore leaves Circle without its default `C`.
- `pad.` is per physical press, never chained (a rotation `cross→circle→square→cross`
  works), and two buttons may share a destination. It acts on the controller
  only; the keyboard is remapped only by `key.`.
- **Esc and F1–F8 can never be bound** (quit, the cheat console, F6
  fast-forward): such a name is rejected with a warning, and the lookup refuses
  them even if they reach it. Menu navigation (arrows/Enter/Esc in the warp menu,
  Up/Down/Cross/Circle in the controller dev menu) is unaffected.
- The dev-menu combo (Back/View + Start/Menu on one controller) reads the
  controller's **unremapped** bits, so no remap can lock you out of the menu.
  With the dev menu on, a button remapped *from* Select or Start still has its
  lone press held back for up to 300 ms, because that filter runs on the
  physical buttons.

**Platform notes**
- X11 reports a key by its unshifted keysym (`XLookupKeysym` index 0), so a
  keypad key has its NumLock-off name (`KP_End` for keypad 1, `KP_Up` for 8). Run
  `xev` to see the name of any key.
- The SDL/joystick mapping (`PE_PAD_SWAP_XY`, `PE_PAD_RAW_MAP`, a gamecontrollerdb
  file) runs first; `pad.` lines apply to its result.
- Windows: `key.` works through an explicit virtual-key table (letters, digits,
  arrows, Return, Space, Tab, Home/End/Page Up/Page Down/Insert/Delete, keypad,
  F9–F12 and the US-layout punctuation keys). Modifier keys cannot be bound
  there, and `pad.` lines do nothing because that backend has no controller
  input.

**Checks:** `pc_port/tests/test_host_pad_remap.c` (ctest `host-pad-remap`): valid,
invalid and duplicate lines; identity with no file through the real load path
over all 65536 pad words (and the file and its directory are not created); the
confirm/cancel swap over all 65536 words; the default key table equal to a
verbatim copy of the old X11 key chain over 196,608 keysyms; key overrides;
Esc/F1–F8 never bindable by name, by hex or at lookup time; the Windows
virtual-key table equal to the old `vk_to_pad_bit` switch for every code; and the
real combo state machine fed through the remap. The windowed paths themselves
(a live key press, a live controller) were not run on this host.

## The option registry (how options are added)

Every player-facing option is registered once, in `k_options` in
`pc_port/platform/plat_enhance.c` (`pe_plat/enhance.h`): an id that is also
its config key, a category (Graphics, Gameplay/QoL, Assists, Audio, Controls), a menu label,
its values in cycle order with the retail value first, an env override and a
"suggested by" credit. The registry drives the config file, the env overrides,
the dev-menu pages (Options ▸ Enhancements / Graphics / Audio and the root
Assists page, each listing one category; the Options rows come from
`k_option_pages` in `plat_cheats.c`) and the identity rule:
every option's default is its retail value, and a value the player did not
set never touches the game. The registry is meant to be listed by a future
standalone Options/Enhancements menu unchanged; the dev menu is only where it
lives for now. Dev-only items (the F-key cheats, warps, fast-forward) are not
registry options and stay in the dev menu. `test_ASSIST_option_registry`
checks the shape (unique ids, retail-first values, category order, snapping).

## Identity (100 % is retail)

At 100 % and Always Run off, `pe_enh_field_speed` returns its input and
`pe_enh_field_always_run` returns 0; neither writes guest memory, and no config
file is created by queries. `test_ENH_default_is_retail` (native suite,
`pc_port/tests/test_enhance.h`) checks the pass-through over a speed range, a
2 MB guest-RAM compare across the hooks, and retail's walk `0x50000` and run
`0x168000` velocities through the real handlers. The default Day-1 route
regression is the whole-game identity check.

## Evidence

| Check | Result (2026-09-30, tree of the branch commit) |
| --- | --- |
| Native suite | `Results: 1625 run, 1625 passed, 0 failed, 0 skipped` (`build/pcbuild-base/pe-native-tests` with `PE_DISC1_BIN` set; the `ENH_*` tests in `pc_port/tests/test_enhance.h`) |
| Identity at the defaults | `test_ENH_default_is_retail`: pass-through over a speed range, a 2 MB guest-RAM compare across both hooks, retail walk `0x50000` / run `0x168000` through the real handlers |
| Battle unscaled | `test_ENH_battle_handlers_unscaled`, `test_ENH_only_aya_in_field` |
| Default-build Day-1 route regression + movies to Day 2 | blizzpc runner job `20260930-111738-pe-enh-gate-1046668` (binary sha256 `472ed2bf3726…`): `REGRESS PASS until=day1_end_80 frame=67186 token=A8000048 story=80`, `MOVIES_DAY2=PASS`, and no `enhancements.conf` was created (identity at the defaults) |
| Showcase video | `~/Videos/decomp-demos/parasite-eve/parasite_eve_enh_move_speed.mp4` (44 s): 100 %, 150 %, 200 %, 200 % + Always Run over the same Day-1 lobby stretch; capture `pc_port/tools/enh_showcase.py` (`build/lanes/enh/show`), encode `pc_port/tools/enh_showcase_encode.sh` |
| Day-1 room sweep at 200 % (`pc_port/tools/move_speed_sweep.py`) | **7 of 13 warp rooms pass** (`build/lanes/enh/sweep200.txt`). Open, not yet explained: rooms 1, 2, 3, 5 (the opera rooms) report a *different room-change list* between 100 % and 200 % because the 100 % hold is twice as long in presents and reaches the timed scripted transition (battle token `A9400048`, then `A80830C8`, `A8001048`) that the shorter 200 % hold does not; room 7 (M0031I) reaches a different door per direction; room 4 (sewer entry) rests 96 units past the 100 % bounding box in one direction. None of these is a wall pass-through on inspection, but the sweep must be re-run with equal wall-clock holds (or a per-room hold cap) before this row can be called green. |

The sweep re-run is the remaining gate item for this branch.

## Thanks

These options came from players. Thanks to u/stellarola on Reddit for the
idea of Aya's run/walk speed option, and to u/SandmansSlave on Reddit for the
idea of the no-grind assists (x2/x3 EXP, God Mode).
