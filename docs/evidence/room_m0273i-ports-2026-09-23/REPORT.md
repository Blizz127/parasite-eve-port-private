# room_m0273i: masked-body ports (2026-09-23)

Room target `room_m0273i` (PE.IMG room m0273i chunk 2, VRAM base `0x8018EFE8`).
53 C spans: 1 hand-written here, 52 ported;
28 distinct masked bodies in this room. Across all 72 overlay
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
| 0x208 | `func_8018F1F0` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2A8 | `func_8018F290` | room_m0022i `func_8018F274` | room_m0273i_dispatch_8018EFF4 |
| 0x52C | `func_8018F514` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x534 | `func_8018F51C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5BC | `func_8018F5A4` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBAC | `func_8018FB94` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xECC | `func_8018FEB4` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF68 | `func_8018FF50` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF70 | `func_8018FF58` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF78 | `func_8018FF60` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFF4 | `func_8018FFDC` | room_m0022i `func_8018FFC0` | room_m0273i_dispatch_8018F074 |
| 0x126C | `func_80190254` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1274 | `func_8019025C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12FC | `func_801902E4` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1930 | `func_80190918` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19B4 | `func_8019099C` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A0C | `func_801909F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A14 | `func_801909FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A1C | `func_80190A04` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AB8 | `func_80190AA0` | room_m0022i `func_80190A84` | room_m0273i_dispatch_8018F0FC |
| 0x1D00 | `func_80190CE8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D08 | `func_80190CF0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D30 | `func_80190D18` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x212C | `func_80191114` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24D4 | `func_801914BC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2530 | `func_80191518` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2538 | `func_80191520` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2540 | `func_80191528` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25D0 | `func_801915B8` | room_m0022i `func_8019159C` | room_m0273i_dispatch_8018F164 |
| 0x2818 | `func_80191800` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2820 | `func_80191808` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2848 | `func_80191830` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D34 | `func_80191D1C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30C4 | `func_801920AC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3120 | `func_80192108` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3128 | `func_80192110` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3130 | `func_80192118` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x316C | `func_80192154` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3204 | `func_801921EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x320C | `func_801921F4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3234 | `func_8019221C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3264 | `func_8019224C` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34F8 | `func_801924E0` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3514 | `func_801924FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x351C | `func_80192504` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3620 | `func_80192608` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3628 | `func_80192610` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3650 | `func_80192638` | room_m0269i `func_8018F160` | era_o2_g0 |
| 0x3E54 | `func_80192E3C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3E5C | `func_80192E44` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3F6C | `func_80192F54` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4124 | `func_8019310C` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0273i
EXACT_REBUILD_GATE[room_m0273i]=PASS plan=863dd71157c6412be43493510d4395ef195a0317fa5cbf2a4e8873179d87999e yaml=9e1ebb96b3adb2e6fb1b32a8384d28fa23db470edb5edde68097d14bdec09a4b spans=[53 c, 15 asm, 1 rodata, 2 bin] sha1_orig=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1_cand=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a
```
