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

## New: Options ▸ Graphics, Audio (branch `claude/modern-20261020`)

The dev menu's Options page now lists three pages. Every new row starts at
the original (retail) setting, and choices are saved in
`~/.config/parasite-eve-port/enhancements.conf`.

- **Graphics:**
  - **Display Scaler**: Nearest (original), Sharp Bilinear, or FSR 1 (CPU).
  - **Internal Resolution**: Native, 2x, 3x or 4x. Characters and 3D
    scenes get finer edges; backgrounds and menus stay pixel-exact.
  - **Texture Filter**: smooths 3D textures.
  - **Supersample AA**.
  - **Dithering**: Retail, Fine or Off.
- **Audio:**
  - **Master Volume**.
  - **Movie Audio Volume**: movies and the ending credits.

These change only what you see and hear. The game itself, its timing and
saves are untouched. Higher internal resolutions and FSR 1 cost CPU time; if
the game slows down, step them back.

Also: Eve's flash effect in the first Eve fight now tracks two more retail
stack writers (weapon-hit draw 800CDD0C, update callback 800CA540), so it
falls back to its approximation less often.
