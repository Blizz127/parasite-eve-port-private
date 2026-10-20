# Release notes for the next private build (draft, not published)

## Fixed: Day 2 Central Park / zoo random battles closing the game

Random battles in the Day 2 Central Park rooms (m0057i–m0091i) stopped the
game on retail code with no native port yet (CPU_BOUNDARY/REFUSED, which
ends the run).  Reproduced on the launcher build native-only-c062ded9-ctrl
and on HEAD with forced encounters in every park room:

| Rooms | Missing code | Now |
| --- | --- | --- |
| m0062i, m0083i | leaping enemy flight state 0x8018F320 (m0063i shared) | native, NON_MATCHING, e21-diffed |
| m0075i, m0080i | weapon/effect draw 0x8018F814, shadow 0x8018FC60 | native, NON_MATCHING, e21-diffed |
| m0075i, m0080i | chain/tail segment update 0x800C5538, renderer 0x800C5A40 (EXE) | native host ports |
| m0089i | battle effects 0x80192CD4, 0x80193618, particles 0x80193200 | native, NON_MATCHING, e21-diffed |

## New: stop report file

If the game ever stops on code that has no native port yet, a windowed run
now writes `~/.local/state/parasite-eve-port/last-stop.txt`
(`$XDG_STATE_HOME/parasite-eve-port/last-stop.txt` when set) with the stop
reason, the missing function address, room and story state.  Send that file
when reporting a shutdown.

## Dev menu is in

The development menu is now reachable in the packaged build without any
environment variable:

- Controller: **hold Back/View (Select), then press Start/Menu**.  Start on
  its own still pauses as usual; a quick Back tap or a 0.4 s Back hold still acts as Select.
- Keyboard: **F8**.
- Up/Down move, A/Enter confirm, B/Escape back.

It holds God Mode, Infinite Ammo + Full PE, Instant Win, Area Key Items,
Fast-forward Speed, Warp by Day, Options and Assists.  All cheats start off.
`PE_DEV_MENU=0` turns it off (`PE_CHEATS=0` disables all cheats); `PE_DEV_MENU=1` is developer mode.  See
docs/port/DEV_MENU.md.

## New: Options ▸ Graphics, Audio, Controls (branch `claude/modern-20261020`)

The dev menu's Options page now lists four pages: Enhancements, Graphics,
Audio and Controls. Every new row starts at the original (retail) setting,
and choices are saved in `~/.config/parasite-eve-port/enhancements.conf`.

- **Graphics:**
  - **Display Scaler**: Nearest (original), Sharp Bilinear, or FSR 1 (CPU).
  - **Internal Resolution**: Native, 2x, 3x or 4x. Characters and 3D
    scenes get finer edges; backgrounds and menus stay pixel-exact.
  - **Texture Filter**: smooths 3D textures.
  - **Supersample AA**.
  - **Dithering**: Retail, Fine or Off (high-resolution surface only).
  - **Sub-pixel Geometry**: at 2x-4x, characters and 3D scenes are placed
    on half to quarter pixels instead of whole pixels, so their edges move
    in finer steps. Geometry very close to the camera still steps as on the
    original. Menus and text stay pixel-exact, and textures on large
    polygons still warp as on the original.
- **Audio:**
  - **Master Volume**.
  - **Movie Audio Volume**: movies and the ending credits.
  - **Music Volume** and **SFX Volume**, set separately.

The Graphics and Audio rows are designed to change only what you see and
hear: the game itself, its timing and saves are untouched. This was checked
with internal resolution 2x, the texture filter, fine dithering, SSAA,
sub-pixel geometry and the movie/music/SFX levels all switched on, in two
places. One was the automated Day-1 route, which skips the movies. The other
was a normal boot that plays the title-screen movie. Game behaviour, the
movie frames and the native screenshots were identical to the default runs.

Higher internal resolutions and FSR 1 cost CPU time. They act differently
when the CPU cannot keep up:

- **FSR 1** runs on its own threads. In a large window it is designed to
  show fewer frames per second rather than slow the game, but that was not
  measured with the game running, and on CPUs with few cores it may slow
  the game too.
- **Internal Resolution** set too high slows the game itself.

On the development server the FSR 1 filter, timed on its own, was fast
enough for a 1280x800 window but not for 1080p, and 4x was too slow. Your
device's CPU may do better or worse. If the game slows down, step Internal
Resolution back, or the Display Scaler.

- **Controls** (these change how you play):
  - **Analog Stick**: play with the left stick as on a DualShock with the
    Analog light on. Half tilt walks and full tilt runs, as the original
    game does in analog mode. As in the original, the d-pad then does not
    move Aya or the menu cursor.
- **Button remapping:** an optional `~/.config/parasite-eve-port/controls.conf`
  can swap controller buttons (for example Cross and Circle) or rebind
  keyboard keys. Esc, F1-F8 and the Back+Start menu combo always keep
  working. The format is in docs/port/ENHANCEMENTS.md.

Also, Eve's flash effect in the first Eve fight now tracks every retail stack
writer the Day-1 route reaches: the weapon-hit draw 800CDD0C and the
frameless update callbacks of the pistol, hit and club effects. In the
automated Day-1 route (pads only) the approximation no longer triggered up to
the route's end in that fight. Later play is not verified yet.

## Device checks for this build

This file is a branch draft and nothing is packaged yet. The integrator
fills in the build ID, source commit and package SHA256 when it packages
the build. Every check stays NOT RUN until the owner reports PASS or FAIL
from a device, with the device, OS, Game Mode/Gamescope, controller and
Steam Input setup.

| Check | Result |
| --- | --- |
| Controller works out of the box through SDL gamepad / Steam Input, in menus and gameplay, with no manual remap or keyboard workaround | NOT RUN |
| Window is borderless/fullscreen with no white border | NOT RUN |
| Focusing a text field opens the on-screen keyboard | NOT RUN. The only text entry is the game's own name entry at New Game: the original letter grid, driven by the controller. The port has no system text field (no SDL or X11 text-input call in `pc_port/`), so nothing asks for the on-screen keyboard; the device run should confirm the name entry works with the controller alone |
| Display Scaler: Sharp Bilinear and FSR 1 change the picture in the window, and the game keeps its speed with FSR 1 at the device's screen resolution | NOT RUN |
| Master Volume changes the level at the audio device | NOT RUN |
| Analog Stick: half tilt walks, full tilt runs, and the d-pad does not move Aya while it is on | NOT RUN |
| controls.conf: a swapped button pair takes effect, and Esc, F1-F8 and Back+Start still work | NOT RUN |
| Sub-pixel Geometry at 2x-4x: character edges move in finer steps, with no gaps or flicker on models, menus or text | NOT RUN |
