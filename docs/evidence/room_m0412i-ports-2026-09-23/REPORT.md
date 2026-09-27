# room_m0412i: masked-body ports (2026-09-23)

Room target `room_m0412i` (PE.IMG room m0412i chunk 2, VRAM base `0x8018EFE8`).
65 C spans: 0 hand-written here, 65 ported;
43 distinct masked bodies in this room. Across all 72 overlay
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
| 0x24C | `func_8018F234` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x278 | `func_8018F260` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x2A4 | `func_8018F28C` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0x2EC | `func_8018F2D4` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x378 | `func_8018F360` | room_m0141i `func_8018F1BC` | era_o2_g0 |
| 0x3D8 | `func_8018F3C0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x624 | `func_8018F60C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x62C | `func_8018F614` | room_m0141i `func_8018F470` | era_o2_g0 |
| 0x738 | `func_8018F720` | room_m0141i `func_8018F57C` | era_o2_g0 |
| 0xA30 | `func_8018FA18` | room_m0141i `func_8018F874` | era_o2_g0 |
| 0xB0C | `func_8018FAF4` | room_m0141i `func_8018F950` | era_o2_g0 |
| 0xB60 | `func_8018FB48` | room_m0141i `func_8018F9A4` | era_o2_g0 |
| 0xC4C | `func_8018FC34` | room_m0141i `func_8018FA90` | era_o2_g0 |
| 0xC68 | `func_8018FC50` | room_m0141i `func_8018FAAC` | era_o2_g0 |
| 0xF88 | `func_8018FF70` | room_m0141i `func_8018FDCC` | era_o2_g0 |
| 0x1064 | `func_8019004C` | room_m0141i `func_8018FEA8` | era_o2_g0 |
| 0x1268 | `func_80190250` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1270 | `func_80190258` | room_m0141i `func_801900B4` | era_o2_g0 |
| 0x1708 | `func_801906F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1710 | `func_801906F8` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x17B0 | `func_80190798` | room_m0022i `func_8018F274` | room_m0412i_dispatch_8018F044 |
| 0x1A34 | `func_80190A1C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A3C | `func_80190A24` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1AC4 | `func_80190AAC` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x20B4 | `func_8019109C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x23D4 | `func_801913BC` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x2470 | `func_80191458` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2478 | `func_80191460` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2480 | `func_80191468` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x24FC | `func_801914E4` | room_m0022i `func_8018FFC0` | room_m0412i_dispatch_8018F0C4 |
| 0x2774 | `func_8019175C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x277C | `func_80191764` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2804 | `func_801917EC` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x2E38 | `func_80191E20` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x2EBC | `func_80191EA4` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x2F14 | `func_80191EFC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2F1C | `func_80191F04` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2F24 | `func_80191F0C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2FC0 | `func_80191FA8` | room_m0022i `func_80190A84` | room_m0412i_dispatch_8018F14C |
| 0x3208 | `func_801921F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3210 | `func_801921F8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3238 | `func_80192220` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3634 | `func_8019261C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x39DC | `func_801929C4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3A38 | `func_80192A20` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3A40 | `func_80192A28` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3A48 | `func_80192A30` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x3AD8 | `func_80192AC0` | room_m0022i `func_8019159C` | room_m0412i_dispatch_8018F1B4 |
| 0x3D20 | `func_80192D08` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3D28 | `func_80192D10` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3D50 | `func_80192D38` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x423C | `func_80193224` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x45CC | `func_801935B4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4628 | `func_80193610` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4630 | `func_80193618` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4638 | `func_80193620` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x4674 | `func_8019365C` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x470C | `func_801936F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4714 | `func_801936FC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x473C | `func_80193724` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x476C | `func_80193754` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x4A00 | `func_801939E8` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4A1C | `func_80193A04` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5674 | `func_8019465C` | room_m0014i `func_80193148` | era_o2_g0 |
| 0x63E0 | `func_801953C8` | room_m0141i `func_80191F38` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0412i
EXACT_REBUILD_GATE[room_m0412i]=PASS plan=d703cfb14ed2cc38c9de852772cdca1f249632a8bbbfd8b85f6a8cf2eecb6c5c yaml=aba4f399ef72cd36f4f00f0c84c2e6298e646ee33e5951fcb038335195862ec1 spans=[65 c, 16 asm, 1 rodata, 2 bin] sha1_orig=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1_cand=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1=673097c6bd36986925a38e6a12d2b039092ed9c8
```
