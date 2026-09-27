# room_m0424i: masked-body ports (2026-09-23)

Room target `room_m0424i` (PE.IMG room m0424i chunk 2, VRAM base `0x8018EFE8`).
21 C spans: 0 hand-written here, 21 ported;
16 distinct masked bodies in this room. Across all 72 overlay
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
| 0x24 | `func_8018F00C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x50 | `func_8018F038` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x7C | `func_8018F064` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0xC4 | `func_8018F0AC` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x150 | `func_8018F138` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1D4 | `func_8018F1BC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1DC | `func_8018F1C4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1E4 | `func_8018F1CC` | room_m0107i `func_8018F394` | era_o2_g0 |
| 0x2F0 | `func_8018F2D8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2F8 | `func_8018F2E0` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0x378 | `func_8018F360` | room_m0421i `func_8018F3A4` | era_o2_g0 |
| 0x828 | `func_8018F810` | room_m0107i `func_8018F9D8` | era_o2_g0 |
| 0x91C | `func_8018F904` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x924 | `func_8018F90C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x92C | `func_8018F914` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x934 | `func_8018F91C` | room_m0107i `func_8018FAE4` | era_o2_g0 |
| 0x954 | `func_8018F93C` | room_m0421i `func_8018F980` | era_o2_g0 |
| 0xB18 | `func_8018FB00` | room_m0107i `func_8018FCC8` | era_o2_g0 |
| 0xB7C | `func_8018FB64` | room_m0421i `func_8018FBA8` | era_o2_g0 |
| 0xFC8 | `func_8018FFB0` | room_m0107i `func_8018F9D8` | era_o2_g0 |
| 0x1D0C | `func_80190CF4` | room_m0014i `func_80193148` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0424i
EXACT_REBUILD_GATE[room_m0424i]=PASS plan=4996508888d53b190615deebc1cfeec4ce4cd47a4f5dc444970e56b18f7c1fc7 yaml=3a4367e58937f55585437533bc232618bed7e4fb7abaf8b298139c2cb1c64a9e spans=[21 c, 3 asm, 0 rodata, 2 bin] sha1_orig=5488eb751ec4c35eb8aaecc7521b73282c701041 sha1_cand=5488eb751ec4c35eb8aaecc7521b73282c701041 sha1=5488eb751ec4c35eb8aaecc7521b73282c701041
```
