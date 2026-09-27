# room_m0113i: masked-body ports (2026-09-23)

Room target `room_m0113i` (PE.IMG room m0113i chunk 2, VRAM base `0x8018EFE8`).
2 C spans: 0 hand-written here, 2 ported;
2 distinct masked bodies in this room. Across all 72 overlay
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
| 0xC80 | `func_8018FC68` | room_m0012i `func_8018FC54` | era_o2_g0 |
| 0x1A94 | `func_80190A7C` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0113i
EXACT_REBUILD_GATE[room_m0113i]=PASS plan=7d55e5013d21532652760d37f9a7b5c95d6d76ee53b14b04c04461dfcbcaaee8 yaml=d296c805609dc15130f81c2ec1be9fd09be5316cdd7a47283bcc60d3809d9afe spans=[2 c, 2 asm, 0 rodata, 2 bin] sha1_orig=f55e2fa3c9c13b158e03cbd30c9677b69e141422 sha1_cand=f55e2fa3c9c13b158e03cbd30c9677b69e141422 sha1=f55e2fa3c9c13b158e03cbd30c9677b69e141422
```
