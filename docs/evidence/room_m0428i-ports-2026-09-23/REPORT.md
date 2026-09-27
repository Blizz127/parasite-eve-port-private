# room_m0428i: masked-body ports (2026-09-23)

Room target `room_m0428i` (PE.IMG room m0428i chunk 2, VRAM base `0x8018EFE8`).
45 C spans: 0 hand-written here, 45 ported;
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
| 0x1E4 | `func_8018F1CC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1EC | `func_8018F1D4` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x28C | `func_8018F274` | room_m0022i `func_8018F274` | room_m0428i_dispatch_8018EFF4 |
| 0x510 | `func_8018F4F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x518 | `func_8018F500` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5A0 | `func_8018F588` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xB90 | `func_8018FB78` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEB0 | `func_8018FE98` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF4C | `func_8018FF34` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF54 | `func_8018FF3C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF5C | `func_8018FF44` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFD8 | `func_8018FFC0` | room_m0022i `func_8018FFC0` | room_m0428i_dispatch_8018F074 |
| 0x1250 | `func_80190238` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1258 | `func_80190240` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12E0 | `func_801902C8` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1914 | `func_801908FC` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x1998 | `func_80190980` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x19F0 | `func_801909D8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x19F8 | `func_801909E0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A00 | `func_801909E8` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1A9C | `func_80190A84` | room_m0022i `func_80190A84` | room_m0428i_dispatch_8018F0FC |
| 0x1CE4 | `func_80190CCC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1CEC | `func_80190CD4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D14 | `func_80190CFC` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2110 | `func_801910F8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24B8 | `func_801914A0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2514 | `func_801914FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x251C | `func_80191504` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2524 | `func_8019150C` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25B4 | `func_8019159C` | room_m0022i `func_8019159C` | room_m0428i_dispatch_8018F164 |
| 0x27FC | `func_801917E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2804 | `func_801917EC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x282C | `func_80191814` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D18 | `func_80191D00` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30A8 | `func_80192090` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3104 | `func_801920EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x310C | `func_801920F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3114 | `func_801920FC` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3150 | `func_80192138` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x31E8 | `func_801921D0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31F0 | `func_801921D8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3218 | `func_80192200` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3248 | `func_80192230` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34DC | `func_801924C4` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x34F8 | `func_801924E0` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0428i
EXACT_REBUILD_GATE[room_m0428i]=PASS plan=6152cca2bc0cdac6f3e1d211f34cb34da87671e68ef8327b74bd6b8f28e2892e yaml=9411c1616f1c290ad36880347fc8a1fae6d9b6dd47e73c7a400cce2571542c04 spans=[45 c, 9 asm, 1 rodata, 2 bin] sha1_orig=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c sha1_cand=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c sha1=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c
```
