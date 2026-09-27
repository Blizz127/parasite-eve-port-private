# room_m0153i: masked-body ports (2026-09-23)

Room target `room_m0153i` (PE.IMG room m0153i chunk 2, VRAM base `0x8018EFE8`).
71 C spans: 0 hand-written here, 71 ported;
45 distinct masked bodies in this room. Across all 72 overlay
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
| 0x244 | `func_8018F22C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x270 | `func_8018F258` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x29C | `func_8018F284` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0x2E4 | `func_8018F2CC` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x370 | `func_8018F358` | room_m0141i `func_8018F1BC` | era_o2_g0 |
| 0x3D0 | `func_8018F3B8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x61C | `func_8018F604` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x624 | `func_8018F60C` | room_m0141i `func_8018F470` | era_o2_g0 |
| 0x730 | `func_8018F718` | room_m0141i `func_8018F57C` | era_o2_g0 |
| 0xA28 | `func_8018FA10` | room_m0141i `func_8018F874` | era_o2_g0 |
| 0xB04 | `func_8018FAEC` | room_m0141i `func_8018F950` | era_o2_g0 |
| 0xB58 | `func_8018FB40` | room_m0141i `func_8018F9A4` | era_o2_g0 |
| 0xC44 | `func_8018FC2C` | room_m0141i `func_8018FA90` | era_o2_g0 |
| 0xC60 | `func_8018FC48` | room_m0141i `func_8018FAAC` | era_o2_g0 |
| 0xF80 | `func_8018FF68` | room_m0141i `func_8018FDCC` | era_o2_g0 |
| 0x105C | `func_80190044` | room_m0141i `func_8018FEA8` | era_o2_g0 |
| 0x1260 | `func_80190248` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1268 | `func_80190250` | room_m0141i `func_801900B4` | era_o2_g0 |
| 0x1700 | `func_801906E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1708 | `func_801906F0` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x17A8 | `func_80190790` | room_m0022i `func_8018F274` | room_m0153i_dispatch_8018F044 |
| 0x1A2C | `func_80190A14` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A34 | `func_80190A1C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1ABC | `func_80190AA4` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x20AC | `func_80191094` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x23CC | `func_801913B4` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x2468 | `func_80191450` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2470 | `func_80191458` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2478 | `func_80191460` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x24F4 | `func_801914DC` | room_m0022i `func_8018FFC0` | room_m0153i_dispatch_8018F0C4 |
| 0x276C | `func_80191754` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2774 | `func_8019175C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x27FC | `func_801917E4` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x2E30 | `func_80191E18` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x2EB4 | `func_80191E9C` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x2F0C | `func_80191EF4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2F14 | `func_80191EFC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2F1C | `func_80191F04` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2FB8 | `func_80191FA0` | room_m0022i `func_80190A84` | room_m0153i_dispatch_8018F14C |
| 0x3200 | `func_801921E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3208 | `func_801921F0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3230 | `func_80192218` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x362C | `func_80192614` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x39D4 | `func_801929BC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3A30 | `func_80192A18` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3A38 | `func_80192A20` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3A40 | `func_80192A28` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x3AD0 | `func_80192AB8` | room_m0022i `func_8019159C` | room_m0153i_dispatch_8018F1B4 |
| 0x3D18 | `func_80192D00` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3D20 | `func_80192D08` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3D48 | `func_80192D30` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x4234 | `func_8019321C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x45C4 | `func_801935AC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4620 | `func_80193608` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4628 | `func_80193610` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4630 | `func_80193618` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x466C | `func_80193654` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4704 | `func_801936EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x470C | `func_801936F4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4734 | `func_8019371C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x4764 | `func_8019374C` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x49F8 | `func_801939E0` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4A14 | `func_801939FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4A1C | `func_80193A04` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4A24 | `func_80193A0C` | room_m0145i `func_801924F8` | era_o2_g0 |
| 0x4A88 | `func_80193A70` | room_m0145i `func_8019255C` | era_o2_g0 |
| 0x4B9C | `func_80193B84` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4CF4 | `func_80193CDC` | room_m0145i `func_801927C8` | era_o2_g0 |
| 0x51E8 | `func_801941D0` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x5204 | `func_801941EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6430 | `func_80195418` | room_m0141i `func_80191F38` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0153i
EXACT_REBUILD_GATE[room_m0153i]=PASS plan=89b9b1d0b412e385868fada1a25452e89bb457f7473c021af89972849a1bf340 yaml=efd08f14167a89d0a8c0ad41c0885d59c8411e3fb100cc2e62aaf41eb9af4d72 spans=[71 c, 17 asm, 1 rodata, 2 bin] sha1_orig=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1_cand=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1=342c8ca376fc14e129164ce826e2a6d867602eb2
```
