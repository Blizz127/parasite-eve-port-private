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
its own loader threads. The design and the remaining stages (frame
interpolation, sub-pixel geometry, widescreen) are in
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
- Cost measured on the source lineage's devbox (Xeon E5-1650 v2), 1280x800,
  8 workers: Sharp Bilinear about 4 ms and FSR 1 about 44 ms per frame. FSR 1
  is therefore a frame-dropping presenter on a CPU, not a 60 Hz one. Not yet
  re-measured on the current host.

**Internal Resolution** renders into a separate 2x/3x/4x shadow surface. The
original 1024x512 VRAM stays the authority for game readbacks, texture
fetches and captures.

- Polygons (actors, battle arenas) gain finer edge coverage.
- Background tiles and UI keep nearest sampling, so they look the same as
  with window scaling.
- Integer projection and affine texture mapping remain; there is no
  sub-pixel geometry yet.
- Movies expand the original decoded pixels.
- The developer overlay uses the native frame.
- An allocation failure falls back to native.
- `PE_ROUTE_PRESENTATION_SHOTS=1` with `PE_ROUTE_SHOT_RANGE` also writes the
  presentation frame (`f<N>-presentation.ppm`); ordinary captures stay native.
- Cost: the CPU rasterizer does S² times the pixel work. On the source
  lineage a bounded 18881-frame run took 303 s at 4x against 31 s native.

**Texture Filter** filters polygon texels on the shadow surface only, after
the CLUT lookup. It keeps the nearest texel's transparency and STP class and
excludes incompatible neighbours. Background sprites and UI stay nearest.

**Supersample AA** box-filters the shadow into a separate native-sized
presentation frame.

Either of these last two selects a 2x surface by itself when Internal
Resolution is Native; an explicit 2x/3x/4x choice is kept.

**Tests on the native lineage (2026-10-20):**

| Check | Result |
| --- | --- |
| Native suite | `Results: 1689 run, 1689 passed, 0 failed, 0 skipped`, which includes `test_ASSIST_option_registry` (9 options, 4 graphics, labels, env overrides, the scale/filter/SSAA coupling) and `test_ASSIST_dev_menu_graphics_page` (Options ▸ Graphics rows, BACK to Options row 2, Enhancements still row 1) |
| CTest | 10/10: `upscale-filters`, `upscale-filters-single-thread`, `upscale-no-threads`, `window-scaler-failures`, `scaler-windows-policy`, `gpu-shadow-parity`, `framebuffer-shadow-parity`, `gpu-filter-parity`, `framebuffer-ssaa-parity`, `window-scale-math` |
| Identity of 1x VRAM and the 320x240 frame with the shadow on | `gpu-shadow-parity`, `framebuffer-shadow-parity`, `gpu-filter-parity` and `framebuffer-ssaa-parity` (shadow on and off over the same primitives) |
| NOT RUN | a default-build Day-1 route identity run, a route run with each row forced on (`PE_ENH_AT`), live window and device checks, and a cost re-measurement on the current host |

The build was a local `zig cc` (clang) build: the current devbox has no
system GCC. See `docs/ai_context/ACTIVE_HANDOFF.md`.

## The option registry (how options are added)

Every player-facing option is registered once, in `k_options` in
`pc_port/platform/plat_enhance.c` (`pe_plat/enhance.h`): an id that is also
its config key, a category (Graphics, Gameplay/QoL, Assists), a menu label,
its values in cycle order with the retail value first, an env override and a
"suggested by" credit. The registry drives the config file, the env overrides,
the two dev-menu pages (which list a category each) and the identity rule:
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

## Controls: remapping

Opt-in, config-file driven; there is no menu row and no registry entry. With no
file the pad word and the keyboard table are exactly the retail-port behaviour
(below). Nothing here ever creates the file.

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
