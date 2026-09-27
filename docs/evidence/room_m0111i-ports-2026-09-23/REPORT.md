# room_m0111i: masked-body ports (2026-09-23)

Room target `room_m0111i` (PE.IMG room m0111i chunk 2, VRAM base `0x8018EFE8`).
20 C spans: 0 hand-written here, 20 ported;
15 distinct masked bodies in this room. Across all 72 overlay
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
| 0x14 | `func_8018EFFC` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x40 | `func_8018F028` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x6C | `func_8018F054` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0xB4 | `func_8018F09C` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x140 | `func_8018F128` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1C4 | `func_8018F1AC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1CC | `func_8018F1B4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D4 | `func_8018F1BC` | room_m0107i `func_8018F394` | era_o2_g0 |
| 0x2E0 | `func_8018F2C8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2E8 | `func_8018F2D0` | room_m0167i `func_8018F4F8` | era_o2_g0 |
| 0x368 | `func_8018F350` | room_m0107i `func_8018F528` | era_o2_g0 |
| 0x818 | `func_8018F800` | room_m0107i `func_8018F9D8` | era_o2_g0 |
| 0x90C | `func_8018F8F4` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x914 | `func_8018F8FC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x91C | `func_8018F904` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x924 | `func_8018F90C` | room_m0107i `func_8018FAE4` | era_o2_g0 |
| 0x944 | `func_8018F92C` | room_m0107i `func_8018FB04` | era_o2_g0 |
| 0xB08 | `func_8018FAF0` | room_m0107i `func_8018FCC8` | era_o2_g0 |
| 0xB6C | `func_8018FB54` | room_m0107i `func_8018FD2C` | era_o2_g0 |
| 0xFB8 | `func_8018FFA0` | room_m0107i `func_8018F9D8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0111i
EXACT_REBUILD_GATE[room_m0111i]=PASS plan=0073b6ec5f6a1fcbd1f30171566e9e35a2ac092f95f9c423dd4af5f28c987f0c yaml=85797fd8d1a917ee8cad96ae60d2ef820da03becd957310c6c46aaa9396a43ee spans=[20 c, 2 asm, 0 rodata, 2 bin] sha1_orig=a360b6aa799167c27616a596cf75fe006d635809 sha1_cand=a360b6aa799167c27616a596cf75fe006d635809 sha1=a360b6aa799167c27616a596cf75fe006d635809
```
