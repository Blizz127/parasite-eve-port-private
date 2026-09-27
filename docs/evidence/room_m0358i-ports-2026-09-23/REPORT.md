# room_m0358i: masked-body ports (2026-09-23)

Room target `room_m0358i` (PE.IMG room m0358i chunk 2, VRAM base `0x8018EFE8`).
53 C spans: 5 hand-written here, 48 ported;
31 distinct masked bodies in this room. Across all 72 overlay
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
| 0x244 | `func_8018F22C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x24C | `func_8018F234` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2EC | `func_8018F2D4` | room_m0022i `func_8018F274` | room_m0358i_dispatch_8018EFF4 |
| 0x570 | `func_8018F558` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x578 | `func_8018F560` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x600 | `func_8018F5E8` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBF0 | `func_8018FBD8` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xF10 | `func_8018FEF8` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xFAC | `func_8018FF94` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFB4 | `func_8018FF9C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFBC | `func_8018FFA4` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1038 | `func_80190020` | room_m0022i `func_8018FFC0` | room_m0358i_dispatch_8018F074 |
| 0x12B0 | `func_80190298` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12B8 | `func_801902A0` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1340 | `func_80190328` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1974 | `func_8019095C` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19F8 | `func_801909E0` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A50 | `func_80190A38` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A58 | `func_80190A40` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A60 | `func_80190A48` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AFC | `func_80190AE4` | room_m0022i `func_80190A84` | room_m0358i_dispatch_8018F0FC |
| 0x1D44 | `func_80190D2C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D4C | `func_80190D34` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D74 | `func_80190D5C` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2170 | `func_80191158` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2518 | `func_80191500` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2574 | `func_8019155C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x257C | `func_80191564` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2584 | `func_8019156C` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x2614 | `func_801915FC` | room_m0022i `func_8019159C` | room_m0358i_dispatch_8018F164 |
| 0x285C | `func_80191844` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2864 | `func_8019184C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x288C | `func_80191874` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D78 | `func_80191D60` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3108 | `func_801920F0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3164 | `func_8019214C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x316C | `func_80192154` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3174 | `func_8019215C` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x31B0 | `func_80192198` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3248 | `func_80192230` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3250 | `func_80192238` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3278 | `func_80192260` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x32A8 | `func_80192290` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x353C | `func_80192524` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3558 | `func_80192540` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3560 | `func_80192548` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3754 | `func_8019273C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x42E0 | `func_801932C8` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0358i
EXACT_REBUILD_GATE[room_m0358i]=PASS plan=a07b3a75a55e55d57ea9df1a98fe9e3153913376ac01186787b8c8a831ebc818 yaml=8f561e0cef4ef8dcde2da8c78d6b68f0d297bd5166210336ff55b495043bd3bf spans=[53 c, 10 asm, 1 rodata, 2 bin] sha1_orig=3fbd46633b22dc770516a833a8f2a711e544d382 sha1_cand=3fbd46633b22dc770516a833a8f2a711e544d382 sha1=3fbd46633b22dc770516a833a8f2a711e544d382
```
