# room_m0063i: masked-body ports (2026-09-23)

Room target `room_m0063i` (PE.IMG room m0063i chunk 2, VRAM base `0x8018EFE8`).
8 C spans: 0 hand-written here, 8 ported;
6 distinct masked bodies in this room. Across all 72 overlay
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
| 0x6C | `func_8018F054` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x74 | `func_8018F05C` | room_m0358i `func_80192550` | era_o2_g0 |
| 0x260 | `func_8018F248` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x268 | `func_8018F250` | room_m0358i `func_80192744` | era_o2_g0 |
| 0x2D4 | `func_8018F2BC` | room_m0358i `func_801927B0` | era_o2_g0 |
| 0xD2C | `func_8018FD14` | room_m0358i `func_80193208` | era_o2_g0 |
| 0xD74 | `func_8018FD5C` | room_m0358i `func_80193250` | era_o2_g0 |
| 0xDEC | `func_8018FDD4` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0063i
EXACT_REBUILD_GATE[room_m0063i]=PASS plan=8af715cb89824bc73e6913ded2183ad0904b36f1d4abac7c7302b6f86042755a yaml=36497af102d4d6c90d1b5cce975b34e7ff2081b2094805a3f7bbec6692ae027e spans=[8 c, 2 asm, 0 rodata, 2 bin] sha1_orig=243ffc3252abffa3caffd1c5cf4f7f0013f56c6b sha1_cand=243ffc3252abffa3caffd1c5cf4f7f0013f56c6b sha1=243ffc3252abffa3caffd1c5cf4f7f0013f56c6b
```
