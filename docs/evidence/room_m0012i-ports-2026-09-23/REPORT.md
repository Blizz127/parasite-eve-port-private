# room_m0012i: masked-body ports (2026-09-23)

Room target `room_m0012i` (PE.IMG room m0012i chunk 2, VRAM base `0x8018EFE8`).
1 C spans: 0 hand-written here, 1 ported;
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
| 0xC6C | `func_8018FC54` | room_m0014i `func_80193148` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0012i
EXACT_REBUILD_GATE[room_m0012i]=PASS plan=151ad32dfb9e12d255b043a4b70a1ed812b0167d672bc4e7d1c8c68697420c73 yaml=4bc47c7410c8a53480ca66ad4278c18e8905635d9484f565251648a8f98be1e3 spans=[1 c, 1 asm, 0 rodata, 2 bin] sha1_orig=7b38ab836272d86fa4d88969ecc87a59a104b284 sha1_cand=7b38ab836272d86fa4d88969ecc87a59a104b284 sha1=7b38ab836272d86fa4d88969ecc87a59a104b284
```
