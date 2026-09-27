# room_m0049i: masked-body ports (2026-09-23)

Room target `room_m0049i` (PE.IMG room m0049i chunk 2, VRAM base `0x8018EFE8`).
9 C spans: 2 hand-written here, 7 ported;
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
| 0x54 | `func_8018F03C` | room_m0167i `func_8018F1EC` | era_o2_g0 |
| 0x80 | `func_8018F068` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x144 | `func_8018F12C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x14C | `func_8018F134` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x384 | `func_8018F36C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x34AC | `func_80192494` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3698 | `func_80192680` | room_m0022i `func_801910F8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0049i
EXACT_REBUILD_GATE[room_m0049i]=PASS plan=6ba2132e7212c865725b170c868391d92e4e4a57ddf02cacfa6a6e5489b5fbb6 yaml=8014277d488fe65f75ad9c6ef25cb94630c50b1452acb208ad3dc92be2efad56 spans=[9 c, 4 asm, 0 rodata, 2 bin] sha1_orig=320861e23df3605a60e159b7db5fc081aa421607 sha1_cand=320861e23df3605a60e159b7db5fc081aa421607 sha1=320861e23df3605a60e159b7db5fc081aa421607
```
