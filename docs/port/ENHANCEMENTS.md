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
