# room_m0152i: masked-body ports (2026-09-23)

Room target `room_m0152i` (PE.IMG room m0152i chunk 2, VRAM base `0x8018EFE8`).
52 C spans: 0 hand-written here, 52 ported;
30 distinct masked bodies in this room. Across all 72 overlay
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
| 0x238 | `func_8018F220` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2D8 | `func_8018F2C0` | room_m0022i `func_8018F274` | room_m0152i_dispatch_8018EFF4 |
| 0x55C | `func_8018F544` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x564 | `func_8018F54C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5EC | `func_8018F5D4` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBDC | `func_8018FBC4` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEFC | `func_8018FEE4` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF98 | `func_8018FF80` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFA0 | `func_8018FF88` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFA8 | `func_8018FF90` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1024 | `func_8019000C` | room_m0022i `func_8018FFC0` | room_m0152i_dispatch_8018F074 |
| 0x129C | `func_80190284` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12A4 | `func_8019028C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x132C | `func_80190314` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1960 | `func_80190948` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19E4 | `func_801909CC` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A3C | `func_80190A24` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A44 | `func_80190A2C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A4C | `func_80190A34` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AE8 | `func_80190AD0` | room_m0022i `func_80190A84` | room_m0152i_dispatch_8018F0FC |
| 0x1D30 | `func_80190D18` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D38 | `func_80190D20` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D60 | `func_80190D48` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x215C | `func_80191144` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2504 | `func_801914EC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2560 | `func_80191548` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2568 | `func_80191550` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2570 | `func_80191558` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x2600 | `func_801915E8` | room_m0022i `func_8019159C` | room_m0152i_dispatch_8018F164 |
| 0x2848 | `func_80191830` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2850 | `func_80191838` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2878 | `func_80191860` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D64 | `func_80191D4C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30F4 | `func_801920DC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3150 | `func_80192138` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3158 | `func_80192140` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3160 | `func_80192148` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x319C | `func_80192184` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3234 | `func_8019221C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x323C | `func_80192224` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3264 | `func_8019224C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3294 | `func_8019227C` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3528 | `func_80192510` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3544 | `func_8019252C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x354C | `func_80192534` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3554 | `func_8019253C` | room_m0145i `func_801924F8` | era_o2_g0 |
| 0x35B8 | `func_801925A0` | room_m0145i `func_8019255C` | era_o2_g0 |
| 0x36CC | `func_801926B4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3824 | `func_8019280C` | room_m0145i `func_801927C8` | era_o2_g0 |
| 0x3D18 | `func_80192D00` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3D34 | `func_80192D1C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x49C4 | `func_801939AC` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0152i
EXACT_REBUILD_GATE[room_m0152i]=PASS plan=413420993c5805d50f95eb6cd698d29c27ccfedbe972af486890b8ffe7a3c92c yaml=0fc7e120a33578e787e183ae5651b7cd008c577ec8e1de7608553bff80acbfd5 spans=[52 c, 13 asm, 1 rodata, 2 bin] sha1_orig=3f71f680b594ffbcb7a2f1364a8ef33152bda33d sha1_cand=3f71f680b594ffbcb7a2f1364a8ef33152bda33d sha1=3f71f680b594ffbcb7a2f1364a8ef33152bda33d
```
