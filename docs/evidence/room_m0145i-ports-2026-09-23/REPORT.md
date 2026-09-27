# room_m0145i: masked-body ports (2026-09-23)

Room target `room_m0145i` (PE.IMG room m0145i chunk 2, VRAM base `0x8018EFE8`).
51 C spans: 3 hand-written here, 48 ported;
29 distinct masked bodies in this room. Across all 72 overlay
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
| 0x1F4 | `func_8018F1DC` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x294 | `func_8018F27C` | room_m0022i `func_8018F274` | room_m0145i_dispatch_8018EFF4 |
| 0x518 | `func_8018F500` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x520 | `func_8018F508` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5A8 | `func_8018F590` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xB98 | `func_8018FB80` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEB8 | `func_8018FEA0` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF54 | `func_8018FF3C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF5C | `func_8018FF44` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF64 | `func_8018FF4C` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFE0 | `func_8018FFC8` | room_m0022i `func_8018FFC0` | room_m0145i_dispatch_8018F074 |
| 0x1258 | `func_80190240` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1260 | `func_80190248` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12E8 | `func_801902D0` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x191C | `func_80190904` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19A0 | `func_80190988` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x19F8 | `func_801909E0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A00 | `func_801909E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A08 | `func_801909F0` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AA4 | `func_80190A8C` | room_m0022i `func_80190A84` | room_m0145i_dispatch_8018F0FC |
| 0x1CEC | `func_80190CD4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1CF4 | `func_80190CDC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D1C | `func_80190D04` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2118 | `func_80191100` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24C0 | `func_801914A8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x251C | `func_80191504` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2524 | `func_8019150C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x252C | `func_80191514` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25BC | `func_801915A4` | room_m0022i `func_8019159C` | room_m0145i_dispatch_8018F164 |
| 0x2804 | `func_801917EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x280C | `func_801917F4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2834 | `func_8019181C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D20 | `func_80191D08` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30B0 | `func_80192098` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x310C | `func_801920F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3114 | `func_801920FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x311C | `func_80192104` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3158 | `func_80192140` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x31F0 | `func_801921D8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31F8 | `func_801921E0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3220 | `func_80192208` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3250 | `func_80192238` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34E4 | `func_801924CC` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3500 | `func_801924E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3508 | `func_801924F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3688 | `func_80192670` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3CD4 | `func_80192CBC` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3CF0 | `func_80192CD8` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0145i
EXACT_REBUILD_GATE[room_m0145i]=PASS plan=cd0cf7846b6c75b86f4098b3dded7b381b424ce907211035375b3446e239fa58 yaml=be4c0f8b8c2503234cc4ce217bafe4b3710839aa00e00b214e9a959187c1e514 spans=[51 c, 12 asm, 1 rodata, 2 bin] sha1_orig=8bb899c982ce00e9d43ca6807f0f8aa770f36de7 sha1_cand=8bb899c982ce00e9d43ca6807f0f8aa770f36de7 sha1=8bb899c982ce00e9d43ca6807f0f8aa770f36de7
```
