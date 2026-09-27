# room_m0399i: masked-body ports (2026-09-23)

Room target `room_m0399i` (PE.IMG room m0399i chunk 2, VRAM base `0x8018EFE8`).
62 C spans: 1 hand-written here, 61 ported;
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
| 0x220 | `func_8018F208` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x24C | `func_8018F234` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x294 | `func_8018F27C` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x320 | `func_8018F308` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3A4 | `func_8018F38C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3AC | `func_8018F394` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B4 | `func_8018F39C` | room_m0065i `func_8018F1BC` | era_o2_g0 |
| 0x4F8 | `func_8018F4E0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x500 | `func_8018F4E8` | room_m0391i `func_8018F4F4` | era_o2_g0 |
| 0x11F0 | `func_801901D8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x11F8 | `func_801901E0` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x1298 | `func_80190280` | room_m0022i `func_8018F274` | room_m0399i_dispatch_8018EFFC |
| 0x151C | `func_80190504` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1524 | `func_8019050C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x15AC | `func_80190594` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x1B9C | `func_80190B84` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x1EBC | `func_80190EA4` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x1F58 | `func_80190F40` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F60 | `func_80190F48` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F68 | `func_80190F50` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1FE4 | `func_80190FCC` | room_m0022i `func_8018FFC0` | room_m0399i_dispatch_8018F07C |
| 0x225C | `func_80191244` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2264 | `func_8019124C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x22EC | `func_801912D4` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x2920 | `func_80191908` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x29A4 | `func_8019198C` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x29FC | `func_801919E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2A04 | `func_801919EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2A0C | `func_801919F4` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2AA8 | `func_80191A90` | room_m0022i `func_80190A84` | room_m0399i_dispatch_8018F104 |
| 0x2CF0 | `func_80191CD8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2CF8 | `func_80191CE0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2D20 | `func_80191D08` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x311C | `func_80192104` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x34C4 | `func_801924AC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3520 | `func_80192508` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3528 | `func_80192510` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3530 | `func_80192518` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x35C0 | `func_801925A8` | room_m0022i `func_8019159C` | room_m0399i_dispatch_8018F16C |
| 0x3808 | `func_801927F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3810 | `func_801927F8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3838 | `func_80192820` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x3D24 | `func_80192D0C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x40B4 | `func_8019309C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4110 | `func_801930F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4118 | `func_80193100` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4120 | `func_80193108` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x415C | `func_80193144` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x41F4 | `func_801931DC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x41FC | `func_801931E4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4224 | `func_8019320C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x4254 | `func_8019323C` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x44E8 | `func_801934D0` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4504 | `func_801934EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x450C | `func_801934F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4514 | `func_801934FC` | room_m0145i `func_801924F8` | era_o2_g0 |
| 0x4578 | `func_80193560` | room_m0145i `func_8019255C` | era_o2_g0 |
| 0x468C | `func_80193674` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x47E4 | `func_801937CC` | room_m0145i `func_801927C8` | era_o2_g0 |
| 0x4CD8 | `func_80193CC0` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4CF4 | `func_80193CDC` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0399i
EXACT_REBUILD_GATE[room_m0399i]=PASS plan=728f1762a7ceed7d6821600fe6e57fbe6735b5dcb585556d73da4e35c155dacf yaml=158e7be8ec99a1e2c34f1c71607701ab2fd6ad0b62fee886118c19e39597848c spans=[62 c, 12 asm, 1 rodata, 2 bin] sha1_orig=d05fc006543447f413ca8cd7c1c1f2de0335c6b2 sha1_cand=d05fc006543447f413ca8cd7c1c1f2de0335c6b2 sha1=d05fc006543447f413ca8cd7c1c1f2de0335c6b2
```
