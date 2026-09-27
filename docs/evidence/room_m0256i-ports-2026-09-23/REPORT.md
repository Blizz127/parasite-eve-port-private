# room_m0256i: masked-body ports (2026-09-23)

Room target `room_m0256i` (PE.IMG room m0256i chunk 2, VRAM base `0x8018EFE8`).
49 C spans: 0 hand-written here, 49 ported;
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
| 0x280 | `func_8018F268` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x320 | `func_8018F308` | room_m0022i `func_8018F274` | room_m0256i_dispatch_8018EFF4 |
| 0x5A4 | `func_8018F58C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5AC | `func_8018F594` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x634 | `func_8018F61C` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xC24 | `func_8018FC0C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xF44 | `func_8018FF2C` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xFE0 | `func_8018FFC8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFE8 | `func_8018FFD0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFF0 | `func_8018FFD8` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x106C | `func_80190054` | room_m0022i `func_8018FFC0` | room_m0256i_dispatch_8018F074 |
| 0x12E4 | `func_801902CC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12EC | `func_801902D4` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1374 | `func_8019035C` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x19A8 | `func_80190990` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x1A2C | `func_80190A14` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A84 | `func_80190A6C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A8C | `func_80190A74` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A94 | `func_80190A7C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1B30 | `func_80190B18` | room_m0022i `func_80190A84` | room_m0256i_dispatch_8018F0FC |
| 0x1D78 | `func_80190D60` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D80 | `func_80190D68` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1DA8 | `func_80190D90` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x21A4 | `func_8019118C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x254C | `func_80191534` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x25A8 | `func_80191590` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x25B0 | `func_80191598` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x25B8 | `func_801915A0` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x2648 | `func_80191630` | room_m0022i `func_8019159C` | room_m0256i_dispatch_8018F164 |
| 0x2890 | `func_80191878` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2898 | `func_80191880` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x28C0 | `func_801918A8` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2DAC | `func_80191D94` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x313C | `func_80192124` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3198 | `func_80192180` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31A0 | `func_80192188` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31A8 | `func_80192190` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x31E4 | `func_801921CC` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x327C | `func_80192264` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3284 | `func_8019226C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x32AC | `func_80192294` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x32DC | `func_801922C4` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3570 | `func_80192558` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x358C | `func_80192574` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3594 | `func_8019257C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x363C | `func_80192624` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3644 | `func_8019262C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3854 | `func_8019283C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6CE0 | `func_80195CC8` | room_m0245i `func_801955E4` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0256i
EXACT_REBUILD_GATE[room_m0256i]=PASS plan=3728a6e91639eb368b5d00e4c9e0b9c8d13528a6982b2b2811f06e3c910e3707 yaml=a5b2a71bac8645ffcc00bdb3445a0366cb54b08b1184fde062f2d187e1d44713 spans=[49 c, 13 asm, 1 rodata, 2 bin] sha1_orig=618fb891329d4ac903ec617a210c4b1e0af742d1 sha1_cand=618fb891329d4ac903ec617a210c4b1e0af742d1 sha1=618fb891329d4ac903ec617a210c4b1e0af742d1
```
