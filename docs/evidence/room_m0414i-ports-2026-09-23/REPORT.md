# room_m0414i: masked-body ports (2026-09-23)

Room target `room_m0414i` (PE.IMG room m0414i chunk 2, VRAM base `0x8018EFE8`).
20 C spans: 0 hand-written here, 20 ported;
19 distinct masked bodies in this room. Across all 72 overlay
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
| 0x74 | `func_8018F05C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xA0 | `func_8018F088` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xCC | `func_8018F0B4` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0x114 | `func_8018F0FC` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x1A0 | `func_8018F188` | room_m0141i `func_8018F1BC` | era_o2_g0 |
| 0x200 | `func_8018F1E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x44C | `func_8018F434` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x454 | `func_8018F43C` | room_m0141i `func_8018F470` | era_o2_g0 |
| 0x560 | `func_8018F548` | room_m0141i `func_8018F57C` | era_o2_g0 |
| 0x858 | `func_8018F840` | room_m0141i `func_8018F874` | era_o2_g0 |
| 0x934 | `func_8018F91C` | room_m0141i `func_8018F950` | era_o2_g0 |
| 0x988 | `func_8018F970` | room_m0141i `func_8018F9A4` | era_o2_g0 |
| 0xA74 | `func_8018FA5C` | room_m0141i `func_8018FA90` | era_o2_g0 |
| 0xA90 | `func_8018FA78` | room_m0141i `func_8018FAAC` | era_o2_g0 |
| 0xDB0 | `func_8018FD98` | room_m0141i `func_8018FDCC` | era_o2_g0 |
| 0xE8C | `func_8018FE74` | room_m0141i `func_8018FEA8` | era_o2_g0 |
| 0x1090 | `func_80190078` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1098 | `func_80190080` | room_m0141i `func_801900B4` | era_o2_g0 |
| 0x2180 | `func_80191168` | room_m0014i `func_80193148` | era_o2_g0 |
| 0x2EEC | `func_80191ED4` | room_m0141i `func_80191F38` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0414i
EXACT_REBUILD_GATE[room_m0414i]=PASS plan=62c4f135032628c133a916d76c50c8644e57df280d4f411a36df44cfe42c148a yaml=63f34fb77ddf04e11354f170cbd7a696bd3aedc61308064704fdf743943053fc spans=[20 c, 6 asm, 0 rodata, 2 bin] sha1_orig=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1_cand=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1=c1b8f25dd4ad748e16405bccf6692dcb2bef6467
```
