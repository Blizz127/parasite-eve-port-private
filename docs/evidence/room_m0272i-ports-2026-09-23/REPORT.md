# room_m0272i: masked-body ports (2026-09-23)

Room target `room_m0272i` (PE.IMG room m0272i chunk 2, VRAM base `0x8018EFE8`).
4 C spans: 0 hand-written here, 4 ported;
1 distinct masked bodies in this room. Across all 72 overlay
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
| 0x1A0 | `func_8018F188` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A8 | `func_8018F190` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1B0 | `func_8018F198` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1B8 | `func_8018F1A0` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0272i
EXACT_REBUILD_GATE[room_m0272i]=PASS plan=e448ec60bfcba067db3d5c6dd3c857761e1fc2545d0a32b4eb43517f10547b91 yaml=544eef0ab487d52bb8d73504b415ce3a888ecf9c9c2b237a24a52ef7646e87b4 spans=[4 c, 1 asm, 0 rodata, 2 bin] sha1_orig=9684da6b0a4c7708f7faa6a05b90134a4e9f1039 sha1_cand=9684da6b0a4c7708f7faa6a05b90134a4e9f1039 sha1=9684da6b0a4c7708f7faa6a05b90134a4e9f1039
```
