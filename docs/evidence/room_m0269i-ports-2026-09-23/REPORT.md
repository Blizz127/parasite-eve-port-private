# room_m0269i: masked-body ports (2026-09-23)

Room target `room_m0269i` (PE.IMG room m0269i chunk 2, VRAM base `0x8018EFE8`).
7 C spans: 1 hand-written here, 6 ported;
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
| 0x148 | `func_8018F130` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x150 | `func_8018F138` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x810 | `func_8018F7F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31A4 | `func_8019218C` | room_m0123i `func_801953D4` | era_o2_g0 |
| 0x31B4 | `func_8019219C` | room_m0245i `func_801955E4` | era_o2_g0 |
| 0x31C4 | `func_801921AC` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0269i
EXACT_REBUILD_GATE[room_m0269i]=PASS plan=57829ef8620d03da0f06654d3a32f34e7c7e157e01541c1666e00815fe1de715 yaml=7085844342c7341a9bda179686b8c05beeb6f3f21024554ec22e440ef80edec5 spans=[7 c, 3 asm, 0 rodata, 2 bin] sha1_orig=06c90576dba22ef80781421d5e9c0e802c0a339a sha1_cand=06c90576dba22ef80781421d5e9c0e802c0a339a sha1=06c90576dba22ef80781421d5e9c0e802c0a339a
```
