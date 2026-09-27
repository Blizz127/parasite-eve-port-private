# room_m0156i: masked-body ports (2026-09-23)

Room target `room_m0156i` (PE.IMG room m0156i chunk 2, VRAM base `0x8018EFE8`).
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
| 0x3A00 | `func_801929E8` | room_m0123i `func_801953D4` | era_o2_g0 |
| 0x3A10 | `func_801929F8` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0156i
EXACT_REBUILD_GATE[room_m0156i]=PASS plan=c803208316a056fad2dce04966e36bba3e1eaa22389ffcf2337c68c180ecd3b3 yaml=36693b6ee0cd3ab321e6ba48c8a244961f02ca7737dd5e66b401ee69ce1aade5 spans=[2 c, 2 asm, 0 rodata, 2 bin] sha1_orig=2d094b8b74b918379e1ed7c213e1337fb7fe4cac sha1_cand=2d094b8b74b918379e1ed7c213e1337fb7fe4cac sha1=2d094b8b74b918379e1ed7c213e1337fb7fe4cac
```
