# room_m0017i: masked-body ports (2026-09-23)

Room target `room_m0017i` (PE.IMG room m0017i chunk 2, VRAM base `0x8018EFE8`).
2 C spans: 0 hand-written here, 2 ported;
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
| 0x1BC | `func_8018F1A4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D4 | `func_8018F1BC` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0017i
EXACT_REBUILD_GATE[room_m0017i]=PASS plan=46cb5465e04499d8c70fe62d95f5d3ff245909e04488e346eaf63ea497476896 yaml=cb61fefae560e72440bc358ee6eaa93f5091962bbe1dc04ccfd473d36b57abe4 spans=[2 c, 3 asm, 0 rodata, 2 bin] sha1_orig=b40116e92e76cdcbc81a583c79ab2513c369fe64 sha1_cand=b40116e92e76cdcbc81a583c79ab2513c369fe64 sha1=b40116e92e76cdcbc81a583c79ab2513c369fe64
```
