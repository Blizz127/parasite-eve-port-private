# Native port enhancements (opt-in)

Enhancements change how the game plays, unlike the development cheats. They
are opt-in: the default is the retail behaviour, and at the default the port
runs the retail code path unchanged (see *Identity* below).

## Movement speed and Always run

Dev menu (`PE_DEV_MENU=1`): **Options ▸ Enhancements**.

| Row | Values | Default |
| --- | --- | --- |
| Movement Speed | 100 % (retail), 125 %, 150 %, 175 %, 200 %; A cycles, 200 % wraps to 100 % | 100 % |
| Always Run | Off / On; A toggles | Off |

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
(`~/.config/parasite-eve-port/enhancements.conf` when unset), lines
`move_speed=<pct>` and `always_run=<0|1>`, written when a menu row changes.
Overrides: `PE_ENH_CONFIG=<path>` (a different file), `PE_MOVE_SPEED=<pct>`,
`PE_ALWAYS_RUN=<0|1>`; the env overrides are not written back. A non-default
setting logs one `[ENH] movement speed N%, always run on|off` line at start.

Without `PE_DEV_MENU=1` the menu row is not reachable, but a saved config or the
env overrides still apply. `PE_CHEATS=0` disables the dev menu only.

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
| Default-build Day-1 route regression + movies to Day 2 | blizzpc runner job `20260930-111738-pe-enh-gate-1046668` (queued at the time of writing; result to be recorded here) |
| Showcase video | `~/Videos/decomp-demos/parasite-eve/parasite_eve_enh_move_speed.mp4` (44 s): 100 %, 150 %, 200 %, 200 % + Always Run over the same Day-1 lobby stretch; capture `pc_port/tools/enh_showcase.py` (`build/lanes/enh/show`), encode `pc_port/tools/enh_showcase_encode.sh` |
| Day-1 room sweep at 200 % (`pc_port/tools/move_speed_sweep.py`) | **7 of 13 warp rooms pass** (`build/lanes/enh/sweep200.txt`). Open, not yet explained: rooms 1, 2, 3, 5 (the opera rooms) report a *different room-change list* between 100 % and 200 % because the 100 % hold is twice as long in presents and reaches the timed scripted transition (battle token `A9400048`, then `A80830C8`, `A8001048`) that the shorter 200 % hold does not; room 7 (M0031I) reaches a different door per direction; room 4 (sewer entry) rests 96 units past the 100 % bounding box in one direction. None of these is a wall pass-through on inspection, but the sweep must be re-run with equal wall-clock holds (or a per-room hold cap) before this row can be called green. |

The sweep re-run is the remaining gate item for this branch.
