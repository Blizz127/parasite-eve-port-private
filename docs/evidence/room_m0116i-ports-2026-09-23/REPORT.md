# room_m0116i: masked-body ports (2026-09-23)

Room target `room_m0116i` (PE.IMG room m0116i chunk 2, VRAM base `0x8018EFE8`).
45 C spans: 0 hand-written here, 45 ported;
27 distinct masked bodies in this room. Across all 72 overlay
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
| 0x200 | `func_8018F1E8` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2A0 | `func_8018F288` | room_m0022i `func_8018F274` | room_m0116i_dispatch_8018EFF4 |
| 0x524 | `func_8018F50C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x52C | `func_8018F514` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5B4 | `func_8018F59C` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBA4 | `func_8018FB8C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEC4 | `func_8018FEAC` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF60 | `func_8018FF48` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF68 | `func_8018FF50` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF70 | `func_8018FF58` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFEC | `func_8018FFD4` | room_m0022i `func_8018FFC0` | room_m0116i_dispatch_8018F074 |
| 0x1264 | `func_8019024C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x126C | `func_80190254` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12F4 | `func_801902DC` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1928 | `func_80190910` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19AC | `func_80190994` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A04 | `func_801909EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A0C | `func_801909F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A14 | `func_801909FC` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AB0 | `func_80190A98` | room_m0022i `func_80190A84` | room_m0116i_dispatch_8018F0FC |
| 0x1CF8 | `func_80190CE0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D00 | `func_80190CE8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D28 | `func_80190D10` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2124 | `func_8019110C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24CC | `func_801914B4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2528 | `func_80191510` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2530 | `func_80191518` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2538 | `func_80191520` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25C8 | `func_801915B0` | room_m0022i `func_8019159C` | room_m0116i_dispatch_8018F164 |
| 0x2810 | `func_801917F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2818 | `func_80191800` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2840 | `func_80191828` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D2C | `func_80191D14` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30BC | `func_801920A4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3118 | `func_80192100` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3120 | `func_80192108` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3128 | `func_80192110` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3164 | `func_8019214C` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x31FC | `func_801921E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3204 | `func_801921EC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x322C | `func_80192214` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x325C | `func_80192244` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34F0 | `func_801924D8` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x350C | `func_801924F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4304 | `func_801932EC` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0116i
EXACT_REBUILD_GATE[room_m0116i]=PASS plan=659220c0135d4324741d1a30f9f0de3d8b4a0a50b804dfbe7a9da77db15b85b4 yaml=2df878b41596897456ee1491fdec821ff45d6543ceef46fca4010f464af737e0 spans=[45 c, 10 asm, 1 rodata, 2 bin] sha1_orig=cfc9e33ffd9b52879626a01bac21366e462212f1 sha1_cand=cfc9e33ffd9b52879626a01bac21366e462212f1 sha1=cfc9e33ffd9b52879626a01bac21366e462212f1
```
