# Native port development menu

The menu is a host-only aid. It is **in every build and reachable by
default** ("player mode"), but nothing changes until you open it; all cheats
start off. `PE_DEV_MENU=0` turns the menu off (F8 keeps its flat warp
list); `PE_CHEATS=0` hard-disables both the menu and the keyboard console. The menu does not pause simulation.

| `PE_DEV_MENU` | Mode | Behaviour |
| --- | --- | --- |
| unset (packaged default) | player | Hold Back/View, then press Start/Menu to open. Start alone still reaches the game; a short Back tap is delivered to the game as Select, and Back held alone for 0.4 s passes through as held Select. F8 opens the menu from the keyboard. |
| `1` | developer | Back/View and Start/Menu are reserved immediately (original behaviour below); developer env warps (`PE_WARP_*`) are honoured. |
| `0` | off | Dev menu disabled; the F1–F8 keyboard console keeps its original flat menus. |

## Opening it

- Controller: **hold Back/View (Select), then press Start/Menu.** The same
  combo closes it; B backs out one page and closes at the root.
- Keyboard: **F8** opens the menu; Up/Down, Enter and Escape navigate.

## Controls

On a mapped SDL GameController (Xbox labels, including Steam Input):

| Control | Action |
| --- | --- |
| Back/View + Start/Menu on the same controller | Open or close the development menu |
| D-pad (vertical) or left analog stick (vertical) | Move one row per press or stick deflection |
| A | Confirm, toggle a cheat, or select a warp |
| B | Back one page; close at the root |
| Keyboard Up/Down, Enter, Escape | Navigate, confirm, back when open |

In developer mode (`PE_DEV_MENU=1`), Back/View and Start/Menu are reserved immediately
when either is pressed, so the opening combo does not also send Select/Start
into the game. Release both before pressing the combo again. Directions and
A/B are consumed while the menu is open; after closing, held controller buttons
must return to neutral before gameplay input resumes. Controllers connected
with buttons held must return to neutral before they can operate the menu.
The combo must come from one controller; two pads cannot combine their buttons.
Up to eight mapped controllers can navigate. Navigation does not auto-repeat.
The stick threshold is 12000 on SDL's signed 16-bit range.

The root contains God Mode, Infinite Ammo + Full PE, Instant Win, Area Key
Items, Fast-forward Speed, Warp by Day, Options, and Assists. Options ▸
Enhancements holds the opt-in gameplay enhancements (Movement Speed, Always
Run); Options ▸ Graphics holds the display options (Display Scaler, Internal
Resolution, Texture Filter, Supersample AA); Assists holds 1-Hit Kills, God
Mode (No Damage) and the EXP/BP Multiplier. All three pages persist in a
config file, default to retail, and are described in
[ENHANCEMENTS.md](ENHANCEMENTS.md). These call the existing keyboard
cheat actions. Warp by Day lists Day 1 through Day 6 in order; empty days show
no selectable checkpoints. EX/Chrysler entries are not supplied yet. Current
checkpoint metadata belongs to the separate warp-sweep lane; the menu consumes
`pe_plat_cheats_warp_count/name/day` and does not invent checkpoints or room state.

Existing F1–F5, F7 and F8 remain available. F8 opens the development menu unless
`PE_DEV_MENU=0`.
F6 or L3 toggle fast-forward and R3 holds it during gameplay. Controller
fast-forward hotkeys are suppressed during menu capture. Existing fast-forward
state is retained when opening the menu.

## SDL and the Linux gameplay backend

This implementation uses **SDL2 GameController**, including SDL mappings and
hotplug. It does not substitute the Linux joystick API for menu navigation.
The existing `/dev/input/js*` backend still supplies gameplay input and has its
own hotplug behavior. `PE_JOYSTICK=off` disables that gameplay backend; it does
not disable the independently opted-in SDL development menu.

CMake enables the adapter when SDL2 >= 2.0.14 development headers are available.
The executable loads `libSDL2-2.0.so.0` (or `libSDL2.so`) only when opted in,
and initializes only the controller subsystem. It adds no SDL video, audio,
window, or mandatory link dependency. SDL GameController state is updated and
mapped devices are reconciled by instance ID each poll; removals clear the
tracked device state. See the [SDL update API](https://wiki.libsdl.org/SDL2/SDL_GameControllerUpdate).

If headers were absent at build time or the runtime library cannot load, a
`[DEV_MENU]` diagnostic explains the missing support. There is no joystick
menu fallback. Keyboard cheats and the Linux gameplay backend remain usable.
Install SDL2 development files and rebuild if the build lacks the adapter;
install the SDL2 runtime library on the target machine if loading fails.
Windows is on hold and is not wired to this Linux host adapter.

## Regression evidence

`pe-dev-menu-controller-tests` uses SDL virtual devices to exercise the real
GameController adapter without physical hardware. The virtual joystick API is
used only to inject test inputs. It checks off-by-default behavior, same-pad
combo, D-pad/stick, A/B, empty days, input drain, removal, reconnect with held
buttons, and keyboard escape after removal. `--disabled` checks `PE_CHEATS=0`.
The existing Linux joystick input and hotplug regressions remain separate.

The lane gate ran from `codex/r6-menu`, based on `bcbea70f`, using
`build/pcbuild-menu`; all builds were niced with `JOBS=2`. Runtime code was
unchanged between these checks and the commit. Logs and three host-only menu
renders are in the main checkout's ignored `build/lanes/menu/` directory.

| Check | Actual result / evidence |
| --- | --- |
| Fresh Release build | PASS; `build.log`, `final-build.log`, `host-final-build.log` |
| Native suite | 1618 run, 1618 passed, 0 failed, 0 skipped; `native.log` |
| SDL virtual controllers | 101 assertions PASS, hard-disable 3 assertions PASS; CTest `dev-menu-controller`, `dev-menu-hard-disable` |
| Linux gameplay controllers | `host-pad-input`, `host-pad-hotplug` PASS |
| Psy-Q guard, executable and native tests | PASS relative to the existing allowlist; `psyq.log` |
| Day 1 route | `normal_start_shim_off` OK; `REGRESS PASS until=day1_end_80 frame=67186`; `route-summary.log` |
| Movies through Day 2 | `MOVIES_DAY2=PASS`; five movies closed with dropped=0, story reaches 0x90; `movies-summary.log` |
| Overlay fingerprints | 221/221 distinct; `fingerprints.log`; registry has 2122 EXE-only zero-fingerprint entries |
| Executable dependencies | `ldd` lists libc and loader only; no mandatory SDL dependency; `ldd.log` |
| Actual windowed executable | Xvfb boot, XRender window, cold SDL GameController initialization; `window-smoke.log` |
| Menu rendering | Root, Day selector, and empty Day 2 page inspected: `dev-root.png`, `dev-days.png`, `dev-empty-day2.png` |

The additional CTest run passed 19/20. The older
`route-boot-day2-control-flow` harness fails at frame 2 with
`unresolved-boundary`, 0/57 milestones and an uninvoked skip stub. The unchanged
pre-existing `build/pcbuild-hotpad/pe-route-boot-day2-tests` fails identically
(`day2-baseline.log`). Its link command omits the generated dispatch registry;
the cause and repair belong to the parent integration work. This failure is
not counted as a pass or hidden as a skip.

Reproduce the required lane checks (from the worktree unless indicated):

```sh
R=/home/blizz/dev/Parasite-Eve-Decompilation
B=$R/build/pcbuild-menu
JOBS=2 TMPDIR=$R/build/tmp nice -n 10 scripts/build_pc_port.sh --no-tests --build-dir "$B"
(cd "$R" && nice -n 10 "$B/pe-native-tests")
nice -n 10 ctest --test-dir "$B" --output-on-failure -E '^native-tests$'
nice -n 10 python3 tools/analysis/psyq_port_guard.py --tree pc_port --binary "$B/parasite-eve-port" --binary "$B/pe-native-tests"
DISC="$R/rom/image/Parasite Eve (USA) (Disc 1)/Parasite Eve (USA) (Disc 1).bin" nice -n 10 pc_port/tools/day1_route_regress.sh --bin "$B/parasite-eve-port" --until day1_end_80
nice -n 10 bash pc_port/tools/movies_day2_check.sh "$B/parasite-eve-port" "$R/build/lanes/menu/movies"
nice -n 10 python3 pc_port/tools/check_release_fingerprints.py "$B/parasite-eve-port"
python3 tools/analysis/retail_data_guard.py --staged
```

The gate exported the absolute `PE_DISC1_BIN` path so disc-dependent tests ran,
and used `PE_SHOT_VSYNC_MAX=4`, `PE_CD_TRACE_MAX=64`. Full route/movie gates
were serialized with `build/lanes/full-gate.lock`. This lane is on the r5
runtime base: the combined r6 CD timing patch changes the expected route end
to 66356 and needs the parent's combined gate. Warp verification/filtering is
also supplied by that parallel lane. Physical-controller operation on the
owner's Legion Go remains untested here.
