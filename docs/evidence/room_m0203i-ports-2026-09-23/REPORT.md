# room_m0203i: masked-body ports (2026-09-23)

Room target `room_m0203i` (PE.IMG room m0203i chunk 2, VRAM base `0x8018EFE8`).
45 C spans: 1 hand-written here, 44 ported;
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
| 0x240 | `func_8018F228` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2E0 | `func_8018F2C8` | room_m0022i `func_8018F274` | room_m0203i_dispatch_8018EFF4 |
| 0x564 | `func_8018F54C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x56C | `func_8018F554` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5F4 | `func_8018F5DC` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBE4 | `func_8018FBCC` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xF04 | `func_8018FEEC` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xFA0 | `func_8018FF88` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFA8 | `func_8018FF90` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFB0 | `func_8018FF98` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x102C | `func_80190014` | room_m0022i `func_8018FFC0` | room_m0203i_dispatch_8018F074 |
| 0x12A4 | `func_8019028C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12AC | `func_80190294` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1334 | `func_8019031C` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1968 | `func_80190950` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19EC | `func_801909D4` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A44 | `func_80190A2C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A4C | `func_80190A34` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A54 | `func_80190A3C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AF0 | `func_80190AD8` | room_m0022i `func_80190A84` | room_m0203i_dispatch_8018F0FC |
| 0x1D38 | `func_80190D20` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D40 | `func_80190D28` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D68 | `func_80190D50` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2164 | `func_8019114C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x250C | `func_801914F4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2568 | `func_80191550` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2570 | `func_80191558` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2578 | `func_80191560` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x2608 | `func_801915F0` | room_m0022i `func_8019159C` | room_m0203i_dispatch_8018F164 |
| 0x2850 | `func_80191838` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2858 | `func_80191840` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2880 | `func_80191868` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D6C | `func_80191D54` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30FC | `func_801920E4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3158 | `func_80192140` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3160 | `func_80192148` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3168 | `func_80192150` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x31A4 | `func_8019218C` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x323C | `func_80192224` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3244 | `func_8019222C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x326C | `func_80192254` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x329C | `func_80192284` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3530 | `func_80192518` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x354C | `func_80192534` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0203i
EXACT_REBUILD_GATE[room_m0203i]=PASS plan=1bb4929e8572cca9c89e4de23354cbc23f34d8b66fa280d5bdb176b599325b83 yaml=51dd18616a36590f9b399b071689be92c0af9ae2f7295b5213681eaf88ae8f67 spans=[45 c, 10 asm, 1 rodata, 2 bin] sha1_orig=6f9743ef89e5fb16a761b32a00d167402203c376 sha1_cand=6f9743ef89e5fb16a761b32a00d167402203c376 sha1=6f9743ef89e5fb16a761b32a00d167402203c376
```
