# room_m0404i: masked-body ports (2026-09-23)

Room target `room_m0404i` (PE.IMG room m0404i chunk 2, VRAM base `0x8018EFE8`).
45 C spans: 0 hand-written here, 45 ported;
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
| 0x244 | `func_8018F22C` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2E4 | `func_8018F2CC` | room_m0022i `func_8018F274` | room_m0404i_dispatch_8018EFF4 |
| 0x568 | `func_8018F550` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x570 | `func_8018F558` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5F8 | `func_8018F5E0` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBE8 | `func_8018FBD0` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xF08 | `func_8018FEF0` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xFA4 | `func_8018FF8C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFAC | `func_8018FF94` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFB4 | `func_8018FF9C` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1030 | `func_80190018` | room_m0022i `func_8018FFC0` | room_m0404i_dispatch_8018F074 |
| 0x12A8 | `func_80190290` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12B0 | `func_80190298` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1338 | `func_80190320` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x196C | `func_80190954` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19F0 | `func_801909D8` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A48 | `func_80190A30` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A50 | `func_80190A38` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A58 | `func_80190A40` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AF4 | `func_80190ADC` | room_m0022i `func_80190A84` | room_m0404i_dispatch_8018F0FC |
| 0x1D3C | `func_80190D24` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D44 | `func_80190D2C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D6C | `func_80190D54` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2168 | `func_80191150` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2510 | `func_801914F8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x256C | `func_80191554` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2574 | `func_8019155C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x257C | `func_80191564` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x260C | `func_801915F4` | room_m0022i `func_8019159C` | room_m0404i_dispatch_8018F164 |
| 0x2854 | `func_8019183C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x285C | `func_80191844` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2884 | `func_8019186C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D70 | `func_80191D58` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3100 | `func_801920E8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x315C | `func_80192144` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3164 | `func_8019214C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x316C | `func_80192154` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x31A8 | `func_80192190` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3240 | `func_80192228` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3248 | `func_80192230` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3270 | `func_80192258` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x32A0 | `func_80192288` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3534 | `func_8019251C` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3550 | `func_80192538` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4E3C | `func_80193E24` | room_m0123i `func_801953D4` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0404i
EXACT_REBUILD_GATE[room_m0404i]=PASS plan=10e6b1ecd52b0d15d0a6aaadfda06a73b26277fe04ae6f6eb64faf88387188e7 yaml=af89086b1289e1e4d5a34c3ad54f727f189fe897b55daaf3e15736d16ac888ff spans=[45 c, 10 asm, 1 rodata, 2 bin] sha1_orig=c611bd599af3f8e0961c1dc5016e31d3d30e3971 sha1_cand=c611bd599af3f8e0961c1dc5016e31d3d30e3971 sha1=c611bd599af3f8e0961c1dc5016e31d3d30e3971
```
