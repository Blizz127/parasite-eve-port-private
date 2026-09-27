# room_m0034i: masked-body ports (2026-09-23)

Room target `room_m0034i` (PE.IMG room m0034i chunk 2, VRAM base `0x8018EFE8`).
16 C spans: 0 hand-written here, 16 ported;
14 distinct masked bodies in this room. Across all 72 overlay
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
| 0x24 | `func_8018F00C` | room_m0174i `func_8018F254` | era_o2_g0 |
| 0xD0 | `func_8018F0B8` | room_m0167i `func_8018F218` | era_o2_g0 |
| 0xFC | `func_8018F0E4` | room_m0186i `func_8018FEE8` | era_o2_g0 |
| 0x144 | `func_8018F12C` | room_m0186i `func_8018FF30` | era_o2_g0 |
| 0x1D0 | `func_8018F1B8` | room_m0167i `func_8018F318` | era_o2_g0 |
| 0x254 | `func_8018F23C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x25C | `func_8018F244` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x264 | `func_8018F24C` | room_m0174i `func_8018F494` | era_o2_g0 |
| 0x384 | `func_8018F36C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x38C | `func_8018F374` | room_m0174i `func_8018F5BC` | era_o2_g0 |
| 0x398 | `func_8018F380` | room_m0174i `func_8018F5C8` | era_o2_g0 |
| 0x3D4 | `func_8018F3BC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3DC | `func_8018F3C4` | room_m0174i `func_8018F60C` | era_o2_g0 |
| 0xDEC | `func_8018FDD4` | room_m0174i `func_8019001C` | era_o2_g0 |
| 0xDFC | `func_8018FDE4` | room_m0174i `func_8019002C` | era_o2_g0 |
| 0xEF8 | `func_8018FEE0` | room_m0174i `func_80190128` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0034i
EXACT_REBUILD_GATE[room_m0034i]=PASS plan=27089c786444e96b50656201b6eb878b4b9fd820085663436be73686c3691a50 yaml=2428707e41c3680face471688b0fc96890ace45246337620742776b69a6ed0f6 spans=[16 c, 1 asm, 0 rodata, 2 bin] sha1_orig=2d09f16a1668c9133294b92bc3bd6acc36052d16 sha1_cand=2d09f16a1668c9133294b92bc3bd6acc36052d16 sha1=2d09f16a1668c9133294b92bc3bd6acc36052d16
```
