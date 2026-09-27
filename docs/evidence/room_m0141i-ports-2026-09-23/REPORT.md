# room_m0141i: masked-body ports (2026-09-23)

Room target `room_m0141i` (PE.IMG room m0141i chunk 2, VRAM base `0x8018EFE8`).
20 C spans: 12 hand-written here, 8 ported;
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
| 0xA8 | `func_8018F090` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xD4 | `func_8018F0BC` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x100 | `func_8018F0E8` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0x148 | `func_8018F130` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x234 | `func_8018F21C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x480 | `func_8018F468` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x10C4 | `func_801900AC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x21EC | `func_801911D4` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0141i
EXACT_REBUILD_GATE[room_m0141i]=PASS plan=10366a0162e8321944b7e0b97c9123fa30fd937601b957bbb2d3f05d0e2fbe45 yaml=2324d20d0f088d6e615efb71fa856b473ed591b20f45f79d56e270fcd37abeda spans=[20 c, 7 asm, 0 rodata, 2 bin] sha1_orig=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1_cand=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc
```
