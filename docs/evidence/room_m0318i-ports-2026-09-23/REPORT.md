# room_m0318i: masked-body ports (2026-09-23)

Room target `room_m0318i` (PE.IMG room m0318i chunk 2, VRAM base `0x8018EFE8`).
47 C spans: 0 hand-written here, 47 ported;
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
| 0x264 | `func_8018F24C` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x304 | `func_8018F2EC` | room_m0022i `func_8018F274` | room_m0318i_dispatch_8018EFF4 |
| 0x588 | `func_8018F570` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x590 | `func_8018F578` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x618 | `func_8018F600` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xC08 | `func_8018FBF0` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xF28 | `func_8018FF10` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xFC4 | `func_8018FFAC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFCC | `func_8018FFB4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFD4 | `func_8018FFBC` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1050 | `func_80190038` | room_m0022i `func_8018FFC0` | room_m0318i_dispatch_8018F074 |
| 0x12C8 | `func_801902B0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12D0 | `func_801902B8` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1358 | `func_80190340` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x198C | `func_80190974` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x1A10 | `func_801909F8` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A68 | `func_80190A50` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A70 | `func_80190A58` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A78 | `func_80190A60` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1B14 | `func_80190AFC` | room_m0022i `func_80190A84` | room_m0318i_dispatch_8018F0FC |
| 0x1D5C | `func_80190D44` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D64 | `func_80190D4C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D8C | `func_80190D74` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2188 | `func_80191170` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2530 | `func_80191518` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x258C | `func_80191574` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2594 | `func_8019157C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x259C | `func_80191584` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x262C | `func_80191614` | room_m0022i `func_8019159C` | room_m0318i_dispatch_8018F164 |
| 0x2874 | `func_8019185C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x287C | `func_80191864` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x28A4 | `func_8019188C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D90 | `func_80191D78` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3120 | `func_80192108` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x317C | `func_80192164` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3184 | `func_8019216C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x318C | `func_80192174` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x31C8 | `func_801921B0` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3260 | `func_80192248` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3268 | `func_80192250` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3290 | `func_80192278` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x32C0 | `func_801922A8` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3554 | `func_8019253C` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3570 | `func_80192558` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xA2AC | `func_80199294` | room_m0123i `func_801953D4` | era_o2_g0 |
| 0xA394 | `func_8019937C` | room_m0123i `func_801953D4` | era_o2_g0 |
| 0xA3A4 | `func_8019938C` | room_m0245i `func_801955E4` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0318i
EXACT_REBUILD_GATE[room_m0318i]=PASS plan=8eb19e04139499bb97ac1dbb4c8679196ed2fcaf99a14050802fa04f5ac10ca1 yaml=6de8be9e146cc92c92620d7cb5e06314dd3e9f1b14c565a65cab23a72ad57ae9 spans=[47 c, 10 asm, 1 rodata, 2 bin] sha1_orig=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b sha1_cand=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b sha1=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b
```
