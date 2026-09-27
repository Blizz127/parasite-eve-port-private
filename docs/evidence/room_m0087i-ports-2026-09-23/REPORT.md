# room_m0087i: masked-body ports (2026-09-23)

Room target `room_m0087i` (PE.IMG room m0087i chunk 2, VRAM base `0x8018EFE8`).
55 C spans: 0 hand-written here, 55 ported;
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
| 0x1EC | `func_8018F1D4` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x218 | `func_8018F200` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x244 | `func_8018F22C` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x28C | `func_8018F274` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x318 | `func_8018F300` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x39C | `func_8018F384` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3A4 | `func_8018F38C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3AC | `func_8018F394` | room_m0065i `func_8018F1BC` | era_o2_g0 |
| 0x4F0 | `func_8018F4D8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4F8 | `func_8018F4E0` | room_m0065i `func_8018F308` | era_o2_g0 |
| 0x11E8 | `func_801901D0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x11F0 | `func_801901D8` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x1290 | `func_80190278` | room_m0022i `func_8018F274` | room_m0087i_dispatch_8018EFFC |
| 0x1514 | `func_801904FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x151C | `func_80190504` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x15A4 | `func_8019058C` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x1B94 | `func_80190B7C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x1EB4 | `func_80190E9C` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x1F50 | `func_80190F38` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F58 | `func_80190F40` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F60 | `func_80190F48` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1FDC | `func_80190FC4` | room_m0022i `func_8018FFC0` | room_m0087i_dispatch_8018F07C |
| 0x2254 | `func_8019123C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x225C | `func_80191244` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x22E4 | `func_801912CC` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x2918 | `func_80191900` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x299C | `func_80191984` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x29F4 | `func_801919DC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x29FC | `func_801919E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2A04 | `func_801919EC` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2AA0 | `func_80191A88` | room_m0022i `func_80190A84` | room_m0087i_dispatch_8018F104 |
| 0x2CE8 | `func_80191CD0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2CF0 | `func_80191CD8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2D18 | `func_80191D00` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3114 | `func_801920FC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x34BC | `func_801924A4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3518 | `func_80192500` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3520 | `func_80192508` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3528 | `func_80192510` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x35B8 | `func_801925A0` | room_m0022i `func_8019159C` | room_m0087i_dispatch_8018F16C |
| 0x3800 | `func_801927E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3808 | `func_801927F0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3830 | `func_80192818` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x3D1C | `func_80192D04` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x40AC | `func_80193094` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4108 | `func_801930F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4110 | `func_801930F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4118 | `func_80193100` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x4154 | `func_8019313C` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x41EC | `func_801931D4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x41F4 | `func_801931DC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x421C | `func_80193204` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x424C | `func_80193234` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x44E0 | `func_801934C8` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x44FC | `func_801934E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0087i
EXACT_REBUILD_GATE[room_m0087i]=PASS plan=208c0b2bd314331376313c8dc25ad056c80f3ba4f6e2584afc5d555808c664fd yaml=97c1255c2a40be8d78bf572ad6af01bf7992c1d832c1be5dd5fffdb429101648 spans=[55 c, 9 asm, 1 rodata, 2 bin] sha1_orig=4c6107aa1de07a456f12d3723abab64156f4e73b sha1_cand=4c6107aa1de07a456f12d3723abab64156f4e73b sha1=4c6107aa1de07a456f12d3723abab64156f4e73b
```
