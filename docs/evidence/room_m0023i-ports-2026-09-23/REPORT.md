# room_m0023i: masked-body ports (2026-09-23)

Room target `room_m0023i` (PE.IMG room m0023i chunk 2, VRAM base `0x8018EFE8`).
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
| 0x165C | `func_80190644` | room_m0005i `func_80190A6C` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0023i
EXACT_REBUILD_GATE[room_m0023i]=PASS plan=6663266fb0a12cf482866b117da8df0940c8ddb82e8da496fe19bcaf50cc9b28 yaml=99014cf132cb3437e18276bac9cfac38e5631d0b004be0c5b4bcab291bebb283 spans=[1 c, 1 asm, 0 rodata, 2 bin] sha1_orig=3dfb340a8c3f044dc834ba113c5192413b9c3f81 sha1_cand=3dfb340a8c3f044dc834ba113c5192413b9c3f81 sha1=3dfb340a8c3f044dc834ba113c5192413b9c3f81
```
