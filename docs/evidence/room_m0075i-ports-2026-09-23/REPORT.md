# room_m0075i: masked-body ports (2026-09-23)

Room target `room_m0075i` (PE.IMG room m0075i chunk 2, VRAM base `0x8018EFE8`).
58 C spans: 6 hand-written here, 52 ported;
36 distinct masked bodies in this room. Across all 72 overlay
targets there are 193 distinct masked bodies behind 2484 C spans (the honest metric; a shared
body counts once per room that contains it).

Provenance: every ported leaf is a masked-body twin of <room> <func>, re-targeted by symbol address.
No room source is shared by reference: each room has its own copy under `src/overlays/room_<r>/`.

A ported leaf is the C of a matched leaf in another room target whose body is identical
once `jal`/`j` targets and `lui`/`%lo` immediates are masked. `tools/analysis/room_lane/port.py`
renames the function and remaps every `func_`/`D_` token by comparing the two bodies
word by word. A dispatch leaf also gets its jump table remapped and a local
`MASPSX_DISPATCH_FOLD` profile. Each port is then `era_link_check.py --target` `LINK_EXACT`
at this room's VMA before it is carved (`tools/analysis/room_lane/doroom.sh`), and the
deep preflight and the gate below cover all of them.

| offset | leaf | source | profile |
| --- | --- | --- | --- |
| 0x1FC | `func_8018F1E4` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x228 | `func_8018F210` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x254 | `func_8018F23C` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x360 | `func_8018F348` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3E4 | `func_8018F3CC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3EC | `func_8018F3D4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x72C | `func_8018F714` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1C1C | `func_80190C04` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1C24 | `func_80190C0C` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x1CC4 | `func_80190CAC` | room_m0022i `func_8018F274` | room_m0075i_dispatch_8018F00C |
| 0x1F48 | `func_80190F30` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F50 | `func_80190F38` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1FD8 | `func_80190FC0` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x25C8 | `func_801915B0` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x28E8 | `func_801918D0` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x2984 | `func_8019196C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x298C | `func_80191974` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2994 | `func_8019197C` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x2A10 | `func_801919F8` | room_m0022i `func_8018FFC0` | room_m0075i_dispatch_8018F08C |
| 0x2C88 | `func_80191C70` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2C90 | `func_80191C78` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2D18 | `func_80191D00` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x334C | `func_80192334` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x33D0 | `func_801923B8` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x3428 | `func_80192410` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3430 | `func_80192418` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3438 | `func_80192420` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x34D4 | `func_801924BC` | room_m0022i `func_80190A84` | room_m0075i_dispatch_8018F114 |
| 0x371C | `func_80192704` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3724 | `func_8019270C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x374C | `func_80192734` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3B48 | `func_80192B30` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3EF0 | `func_80192ED8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3F4C | `func_80192F34` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3F54 | `func_80192F3C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3F5C | `func_80192F44` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x3FEC | `func_80192FD4` | room_m0022i `func_8019159C` | room_m0075i_dispatch_8018F17C |
| 0x4234 | `func_8019321C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x423C | `func_80193224` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4264 | `func_8019324C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x4750 | `func_80193738` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4AE0 | `func_80193AC8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4B3C | `func_80193B24` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4B44 | `func_80193B2C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4B4C | `func_80193B34` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x4B88 | `func_80193B70` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4C20 | `func_80193C08` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4C28 | `func_80193C10` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4C50 | `func_80193C38` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x4C80 | `func_80193C68` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x4F14 | `func_80193EFC` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4F30 | `func_80193F18` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0075i
EXACT_REBUILD_GATE[room_m0075i]=PASS plan=6b6d532d6a06c13e5a4f3d5bcc4cb4a642e198c9ce94e5da550829b40b9aeaab yaml=bd508ad7ffee3ea822cdac52860da05d51d1afdcdaf0c8e148c39519dbb834c1 spans=[58 c, 13 asm, 1 rodata, 2 bin] sha1_orig=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 sha1_cand=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 sha1=ca34d1321e158763e9ee8b3cc9df0b3d94d28978
```
