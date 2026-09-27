# room_m0392i: masked-body ports (2026-09-23)

Room target `room_m0392i` (PE.IMG room m0392i chunk 2, VRAM base `0x8018EFE8`).
59 C spans: 0 hand-written here, 59 ported;
38 distinct masked bodies in this room. Across all 72 overlay
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
| 0x238 | `func_8018F220` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x264 | `func_8018F24C` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x290 | `func_8018F278` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x2D8 | `func_8018F2C0` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x364 | `func_8018F34C` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3E8 | `func_8018F3D0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5E0 | `func_8018F5C8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x5E8 | `func_8018F5D0` | room_m0137i `func_8018F428` | era_o2_g0 |
| 0x668 | `func_8018F650` | room_m0137i `func_8018F4A8` | era_o2_g0 |
| 0xBD4 | `func_8018FBBC` | room_m0137i `func_8018FA14` | era_o2_g0 |
| 0xD18 | `func_8018FD00` | room_m0137i `func_8018FB58` | era_o2_g0 |
| 0xD9C | `func_8018FD84` | room_m0137i `func_8018FBDC` | era_o2_g0 |
| 0xFD8 | `func_8018FFC0` | room_m0137i `func_8018FE18` | era_o2_g0 |
| 0x134C | `func_80190334` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1354 | `func_8019033C` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x13F4 | `func_801903DC` | room_m0022i `func_8018F274` | room_m0392i_dispatch_8018F02C |
| 0x1678 | `func_80190660` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1680 | `func_80190668` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1708 | `func_801906F0` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x1CF8 | `func_80190CE0` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x2018 | `func_80191000` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x20B4 | `func_8019109C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x20BC | `func_801910A4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x20C4 | `func_801910AC` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x2140 | `func_80191128` | room_m0022i `func_8018FFC0` | room_m0392i_dispatch_8018F0AC |
| 0x23B8 | `func_801913A0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x23C0 | `func_801913A8` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2448 | `func_80191430` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x2A7C | `func_80191A64` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x2B00 | `func_80191AE8` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x2B58 | `func_80191B40` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2B60 | `func_80191B48` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2B68 | `func_80191B50` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2C04 | `func_80191BEC` | room_m0022i `func_80190A84` | room_m0392i_dispatch_8018F134 |
| 0x2E4C | `func_80191E34` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2E54 | `func_80191E3C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2E7C | `func_80191E64` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3278 | `func_80192260` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3620 | `func_80192608` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x367C | `func_80192664` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3684 | `func_8019266C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x368C | `func_80192674` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x371C | `func_80192704` | room_m0022i `func_8019159C` | room_m0392i_dispatch_8018F19C |
| 0x3964 | `func_8019294C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x396C | `func_80192954` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3994 | `func_8019297C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x3E80 | `func_80192E68` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4210 | `func_801931F8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x426C | `func_80193254` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4274 | `func_8019325C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x427C | `func_80193264` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x42B8 | `func_801932A0` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4350 | `func_80193338` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4358 | `func_80193340` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4380 | `func_80193368` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x43B0 | `func_80193398` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x4644 | `func_8019362C` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4660 | `func_80193648` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5458 | `func_80194440` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0392i
EXACT_REBUILD_GATE[room_m0392i]=PASS plan=b2fda5b9853df8adab4deeca2c3a12ec82604990adfbe6083a11cbd9b6bf75d1 yaml=b44eacff1a6f86c805c9d50101eb91cf8ce25d62aa7f01e0c0516d028f1b3a59 spans=[59 c, 15 asm, 1 rodata, 2 bin] sha1_orig=b51734dcce487ce7fb0e79610bd81ace8f2d8462 sha1_cand=b51734dcce487ce7fb0e79610bd81ace8f2d8462 sha1=b51734dcce487ce7fb0e79610bd81ace8f2d8462
```
