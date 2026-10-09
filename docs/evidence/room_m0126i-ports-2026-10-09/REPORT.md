# room_m0126i: hand match func_8018F038 (2026-10-09)

Room target `room_m0126i` (PE.IMG room m0126i chunk 2, VRAM base `0x8018EFE8`).
Its text window is shared by m0126i, m0129i, m0139i, m0149i and m0326i
(room_census text-window rule).

Day 4 stop (m0326i battle, fuse 0xCE room):
`CPU_BOUNDARY/REFUSED pc=0x8018F038 ra=@800D413C:effect`.

`func_8018F038` (blob offset 0x50, 0x9C0 bytes) is the room's particle
callback. Its body is a twin of room_m0137i `func_8019018C`, where only the two local
data symbols (`D_8018EFF4` sv, `D_8018EFFC` col) move. It is hand-matched
(LINK_EXACT) under the local profile
`room_m0126i_dispatch_8018F004_8018F01C_div` (MASPSX_THREE_WORD_SYMBOL_STORE,
MASPSX_DISPATCH_FOLD=jtbl_8018F004,jtbl_8018F01C, MASPSX_EXPAND_DIV). The two
6-word jump tables are split out of the header as `[0x1C, rodata]`
(0x1C-0x4C), with `[0x4C, bin]` for the padding word before the text.

```
$ scripts/exact_rebuild_overlay.sh room_m0126i
EXACT_REBUILD_GATE[room_m0126i]=PASS plan=c9cb1dc5da474894563f06bdeecf89854aa46489cf78e35d3a7b166c4ec6ea91 yaml=72fd2aa486e0ad7f43f51c8bfcdeef1f61fc1ea910187aa641c1c8f48949ff8b spans=[3 c, 0 asm, 1 rodata, 3 bin] sha1_orig=4d06f0af693502da09500eaffbe9c6df5b1af988 sha1_cand=4d06f0af693502da09500eaffbe9c6df5b1af988 sha1=4d06f0af693502da09500eaffbe9c6df5b1af988
```

The generated port has no boundaries. The m0326i battle runs, and Aya picks
up fuse 0xCE (persist[0x41] |= 0x40).
