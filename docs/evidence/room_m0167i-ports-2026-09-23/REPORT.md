# room_m0167i: masked-body ports (2026-09-23)

Room target `room_m0167i` (PE.IMG room m0167i chunk 2, VRAM base `0x8018EFE8`).
55 C spans: 7 hand-written here, 48 ported;
33 distinct masked bodies in this room. Across all 72 overlay
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
| 0x3B4 | `func_8018F39C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3BC | `func_8018F3A4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x508 | `func_8018F4F0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0xE80 | `func_8018FE68` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xE88 | `func_8018FE70` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0xF28 | `func_8018FF10` | room_m0022i `func_8018F274` | room_m0167i_dispatch_8018F014 |
| 0x11AC | `func_80190194` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x11B4 | `func_8019019C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x123C | `func_80190224` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x182C | `func_80190814` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x1B4C | `func_80190B34` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x1BE8 | `func_80190BD0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1BF0 | `func_80190BD8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1BF8 | `func_80190BE0` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1C74 | `func_80190C5C` | room_m0022i `func_8018FFC0` | room_m0167i_dispatch_8018F094 |
| 0x1EEC | `func_80190ED4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1EF4 | `func_80190EDC` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1F7C | `func_80190F64` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x25B0 | `func_80191598` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x2634 | `func_8019161C` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x268C | `func_80191674` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2694 | `func_8019167C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x269C | `func_80191684` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2738 | `func_80191720` | room_m0022i `func_80190A84` | room_m0167i_dispatch_8018F11C |
| 0x2980 | `func_80191968` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2988 | `func_80191970` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x29B0 | `func_80191998` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2DAC | `func_80191D94` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3154 | `func_8019213C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x31B0 | `func_80192198` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31B8 | `func_801921A0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31C0 | `func_801921A8` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x3250 | `func_80192238` | room_m0022i `func_8019159C` | room_m0167i_dispatch_8018F184 |
| 0x3498 | `func_80192480` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x34A0 | `func_80192488` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x34C8 | `func_801924B0` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x39B4 | `func_8019299C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3D44 | `func_80192D2C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3DA0 | `func_80192D88` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3DA8 | `func_80192D90` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3DB0 | `func_80192D98` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3DEC | `func_80192DD4` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3E84 | `func_80192E6C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3E8C | `func_80192E74` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3EB4 | `func_80192E9C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3EE4 | `func_80192ECC` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x4178 | `func_80193160` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4194 | `func_8019317C` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0167i
EXACT_REBUILD_GATE[room_m0167i]=PASS plan=5e4d61cbdcd15717ba4ce5d93b3d0711e42f1f675f5d44eef94e28ad5fcf7b67 yaml=f6535da63f79fb34f3d3757a06dd95f4fdf458466c5e55177c8813744265ccfa spans=[55 c, 10 asm, 1 rodata, 2 bin] sha1_orig=8414175491c6dc4ee53bde60f2801265aa64bcfc sha1_cand=8414175491c6dc4ee53bde60f2801265aa64bcfc sha1=8414175491c6dc4ee53bde60f2801265aa64bcfc
```
