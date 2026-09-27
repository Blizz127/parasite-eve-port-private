# room_m0350i: masked-body ports (2026-09-23)

Room target `room_m0350i` (PE.IMG room m0350i chunk 2, VRAM base `0x8018EFE8`).
44 C spans: 0 hand-written here, 44 ported;
26 distinct masked bodies in this room. Across all 72 overlay
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
| 0x20C | `func_8018F1F4` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2AC | `func_8018F294` | room_m0022i `func_8018F274` | room_m0350i_dispatch_8018EFF4 |
| 0x530 | `func_8018F518` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x538 | `func_8018F520` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5C0 | `func_8018F5A8` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBB0 | `func_8018FB98` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xED0 | `func_8018FEB8` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF6C | `func_8018FF54` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF74 | `func_8018FF5C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF7C | `func_8018FF64` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFF8 | `func_8018FFE0` | room_m0022i `func_8018FFC0` | room_m0350i_dispatch_8018F074 |
| 0x1270 | `func_80190258` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1278 | `func_80190260` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1300 | `func_801902E8` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1934 | `func_8019091C` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19B8 | `func_801909A0` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A10 | `func_801909F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A18 | `func_80190A00` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A20 | `func_80190A08` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1ABC | `func_80190AA4` | room_m0022i `func_80190A84` | room_m0350i_dispatch_8018F0FC |
| 0x1D04 | `func_80190CEC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D0C | `func_80190CF4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D34 | `func_80190D1C` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2130 | `func_80191118` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24D8 | `func_801914C0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2534 | `func_8019151C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x253C | `func_80191524` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2544 | `func_8019152C` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25D4 | `func_801915BC` | room_m0022i `func_8019159C` | room_m0350i_dispatch_8018F164 |
| 0x281C | `func_80191804` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2824 | `func_8019180C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x284C | `func_80191834` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D38 | `func_80191D20` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30C8 | `func_801920B0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3124 | `func_8019210C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x312C | `func_80192114` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3134 | `func_8019211C` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3170 | `func_80192158` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3208 | `func_801921F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3210 | `func_801921F8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3238 | `func_80192220` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3268 | `func_80192250` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34FC | `func_801924E4` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3518 | `func_80192500` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0350i
EXACT_REBUILD_GATE[room_m0350i]=PASS plan=8876d99132107817913a3920be3daccbc9421d903dd431dc786b9b6d2dee4dbe yaml=50adc1f1e75bcfff0f5d7359ee4e0b708b4efa88e4f100ee1df562c312ad8346 spans=[44 c, 10 asm, 1 rodata, 2 bin] sha1_orig=e9597b000d287033f981cee44f3db65deb8696e1 sha1_cand=e9597b000d287033f981cee44f3db65deb8696e1 sha1=e9597b000d287033f981cee44f3db65deb8696e1
```
