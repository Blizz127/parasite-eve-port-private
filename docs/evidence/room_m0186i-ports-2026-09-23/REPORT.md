# room_m0186i: masked-body ports (2026-09-23)

Room target `room_m0186i` (PE.IMG room m0186i chunk 2, VRAM base `0x8018EFE8`).
68 C spans: 7 hand-written here, 61 ported;
40 distinct masked bodies in this room. Across all 72 overlay
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
| 0x224 | `func_8018F20C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x250 | `func_8018F238` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x27C | `func_8018F264` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x2C4 | `func_8018F2AC` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x350 | `func_8018F338` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3D4 | `func_8018F3BC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3DC | `func_8018F3C4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x528 | `func_8018F510` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x530 | `func_8018F518` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0xDA8 | `func_8018FD90` | room_m0167i `func_8018FD70` | era_o2_g0 |
| 0xEA0 | `func_8018FE88` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xEA8 | `func_8018FE90` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xED4 | `func_8018FEBC` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xFD4 | `func_8018FFBC` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1058 | `func_80190040` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x122C | `func_80190214` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1E34 | `func_80190E1C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1E3C | `func_80190E24` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x1EDC | `func_80190EC4` | room_m0022i `func_8018F274` | room_m0186i_dispatch_8018F034 |
| 0x2160 | `func_80191148` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2168 | `func_80191150` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x21F0 | `func_801911D8` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x27E0 | `func_801917C8` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x2B00 | `func_80191AE8` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x2B9C | `func_80191B84` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2BA4 | `func_80191B8C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2BAC | `func_80191B94` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x2C28 | `func_80191C10` | room_m0022i `func_8018FFC0` | room_m0186i_dispatch_8018F0B4 |
| 0x2EA0 | `func_80191E88` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2EA8 | `func_80191E90` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2F30 | `func_80191F18` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x3564 | `func_8019254C` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x35E8 | `func_801925D0` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x3640 | `func_80192628` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3648 | `func_80192630` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3650 | `func_80192638` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x36EC | `func_801926D4` | room_m0022i `func_80190A84` | room_m0186i_dispatch_8018F13C |
| 0x3934 | `func_8019291C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x393C | `func_80192924` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3964 | `func_8019294C` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3D60 | `func_80192D48` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4108 | `func_801930F0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4164 | `func_8019314C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x416C | `func_80193154` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4174 | `func_8019315C` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x4204 | `func_801931EC` | room_m0022i `func_8019159C` | room_m0186i_dispatch_8018F1A4 |
| 0x444C | `func_80193434` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4454 | `func_8019343C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x447C | `func_80193464` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x4968 | `func_80193950` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4CF8 | `func_80193CE0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4D54 | `func_80193D3C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4D5C | `func_80193D44` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4D64 | `func_80193D4C` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x4DA0 | `func_80193D88` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4E38 | `func_80193E20` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4E40 | `func_80193E28` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4E68 | `func_80193E50` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x4E98 | `func_80193E80` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x512C | `func_80194114` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x5148 | `func_80194130` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0186i
EXACT_REBUILD_GATE[room_m0186i]=PASS plan=1a70e1cb6a489f00b125ddf0e9618e9d1ec659ac2b2ad0887fa75840b5d7d5ec yaml=949aa942b851e772bb0dd2d35ad76639f7a5a05eec9593dfd7b990affaad4f9e spans=[68 c, 13 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e
```
