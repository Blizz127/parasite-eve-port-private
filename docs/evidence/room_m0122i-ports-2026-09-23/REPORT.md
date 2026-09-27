# room_m0122i: masked-body ports (2026-09-23)

Room target `room_m0122i` (PE.IMG room m0122i chunk 2, VRAM base `0x8018EFE8`).
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
| 0x28 | `func_8018F010` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x54 | `func_8018F03C` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x80 | `func_8018F068` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0xC8 | `func_8018F0B0` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x154 | `func_8018F13C` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1D8 | `func_8018F1C0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1E0 | `func_8018F1C8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1E8 | `func_8018F1D0` | room_m0107i `func_8018F394` | era_o2_g0 |
| 0x2F4 | `func_8018F2DC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2FC | `func_8018F2E4` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0x37C | `func_8018F364` | room_m0107i `func_8018F528` | era_o2_g0 |
| 0x82C | `func_8018F814` | room_m0107i `func_8018F9D8` | era_o2_g0 |
| 0x920 | `func_8018F908` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x928 | `func_8018F910` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x930 | `func_8018F918` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x938 | `func_8018F920` | room_m0107i `func_8018FAE4` | era_o2_g0 |
| 0x958 | `func_8018F940` | room_m0107i `func_8018FB04` | era_o2_g0 |
| 0xB1C | `func_8018FB04` | room_m0107i `func_8018FCC8` | era_o2_g0 |
| 0xB80 | `func_8018FB68` | room_m0107i `func_8018FD2C` | era_o2_g0 |
| 0xFCC | `func_8018FFB4` | room_m0107i `func_8018F9D8` | era_o2_g0 |
| 0x1EB0 | `func_80190E98` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0122i
EXACT_REBUILD_GATE[room_m0122i]=PASS plan=f301c3dee22f3004bd6d913937bf9770913b1331d5c9c7b0a5ffb08ff344cd21 yaml=8846a307702c3bc537b3adb77c4a287ce73d0df23330c6d800ba206a4a17ef7b spans=[21 c, 3 asm, 0 rodata, 2 bin] sha1_orig=1aae0fbca4b21638827d3b304516802937e1488f sha1_cand=1aae0fbca4b21638827d3b304516802937e1488f sha1=1aae0fbca4b21638827d3b304516802937e1488f
```
