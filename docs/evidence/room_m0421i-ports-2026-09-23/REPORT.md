# room_m0421i: masked-body ports (2026-09-23)

Room target `room_m0421i` (PE.IMG room m0421i chunk 2, VRAM base `0x8018EFE8`).
22 C spans: 8 hand-written here, 14 ported;
17 distinct masked bodies in this room. Across all 72 overlay
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
| 0x94 | `func_8018F07C` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xC0 | `func_8018F0A8` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x108 | `func_8018F0F0` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x194 | `func_8018F17C` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x218 | `func_8018F200` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x220 | `func_8018F208` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x334 | `func_8018F31C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x33C | `func_8018F324` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0x960 | `func_8018F948` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x968 | `func_8018F950` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x970 | `func_8018F958` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x100C | `func_8018FFF4` | room_m0107i `func_8018F9D8` | era_o2_g0 |
| 0x1D50 | `func_80190D38` | room_m0014i `func_80193148` | era_o2_g0 |
| 0x29FC | `func_801919E4` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0421i
EXACT_REBUILD_GATE[room_m0421i]=PASS plan=ac21acd794d56e4f890259ef42accf5ea8f3c56efe672950f514a1eeaf1f3e7f yaml=7a6e5dc032c9af84627c42b9e4a18761bcd5b60960ee05e08bdb85140f8aeca0 spans=[22 c, 4 asm, 0 rodata, 2 bin] sha1_orig=983c83c8d658d2228dca5c1d0c5fc234ea401108 sha1_cand=983c83c8d658d2228dca5c1d0c5fc234ea401108 sha1=983c83c8d658d2228dca5c1d0c5fc234ea401108
```
