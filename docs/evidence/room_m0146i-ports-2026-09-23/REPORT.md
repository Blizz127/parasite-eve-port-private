# room_m0146i: masked-body ports (2026-09-23)

Room target `room_m0146i` (PE.IMG room m0146i chunk 2, VRAM base `0x8018EFE8`).
33 C spans: 0 hand-written here, 33 ported;
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
| 0x9C | `func_8018F084` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xC8 | `func_8018F0B0` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xF4 | `func_8018F0DC` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0x13C | `func_8018F124` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x1C8 | `func_8018F1B0` | room_m0141i `func_8018F1BC` | era_o2_g0 |
| 0x228 | `func_8018F210` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x474 | `func_8018F45C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x47C | `func_8018F464` | room_m0141i `func_8018F470` | era_o2_g0 |
| 0x588 | `func_8018F570` | room_m0141i `func_8018F57C` | era_o2_g0 |
| 0x880 | `func_8018F868` | room_m0141i `func_8018F874` | era_o2_g0 |
| 0x95C | `func_8018F944` | room_m0141i `func_8018F950` | era_o2_g0 |
| 0x9B0 | `func_8018F998` | room_m0141i `func_8018F9A4` | era_o2_g0 |
| 0xA9C | `func_8018FA84` | room_m0141i `func_8018FA90` | era_o2_g0 |
| 0xAB8 | `func_8018FAA0` | room_m0141i `func_8018FAAC` | era_o2_g0 |
| 0xDD8 | `func_8018FDC0` | room_m0141i `func_8018FDCC` | era_o2_g0 |
| 0xEB4 | `func_8018FE9C` | room_m0141i `func_8018FEA8` | era_o2_g0 |
| 0x10B8 | `func_801900A0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x10C0 | `func_801900A8` | room_m0141i `func_801900B4` | era_o2_g0 |
| 0x1558 | `func_80190540` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1560 | `func_80190548` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x158C | `func_80190574` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x15B8 | `func_801905A0` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x1600 | `func_801905E8` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x168C | `func_80190674` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1710 | `func_801906F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1908 | `func_801908F0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1910 | `func_801908F8` | room_m0137i `func_8018F428` | era_o2_g0 |
| 0x1990 | `func_80190978` | room_m0137i `func_8018F4A8` | era_o2_g0 |
| 0x1EFC | `func_80190EE4` | room_m0137i `func_8018FA14` | era_o2_g0 |
| 0x2040 | `func_80191028` | room_m0137i `func_8018FB58` | era_o2_g0 |
| 0x20C4 | `func_801910AC` | room_m0137i `func_8018FBDC` | era_o2_g0 |
| 0x2300 | `func_801912E8` | room_m0137i `func_8018FE18` | era_o2_g0 |
| 0x33BC | `func_801923A4` | room_m0141i `func_80191F38` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0146i
EXACT_REBUILD_GATE[room_m0146i]=PASS plan=ffd5bce7a03f032b084706c74caa1fa5d735f026cf7044ae20edb36a0710a30e yaml=87d488e1c69d10e1cd2fe9fdeb7d1c04195aa2fdc1a289e88bac9b68a2b7a45b spans=[33 c, 10 asm, 0 rodata, 2 bin] sha1_orig=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1_cand=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1=72a6cf583e2916e84a5d452ae90a444d19688f68
```
