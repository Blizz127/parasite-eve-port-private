# room_m0065i: masked-body ports (2026-09-23)

Room target `room_m0065i` (PE.IMG room m0065i chunk 2, VRAM base `0x8018EFE8`).
10 C spans: 0 hand-written here, 10 ported;
9 distinct masked bodies in this room. Across all 72 overlay
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
| 0x14 | `func_8018EFFC` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x40 | `func_8018F028` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x6C | `func_8018F054` | room_m0167i `func_8018F244` | era_o2_g0 |
| 0xB4 | `func_8018F09C` | room_m0167i `func_8018F28C` | era_o2_g0 |
| 0x140 | `func_8018F128` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1C4 | `func_8018F1AC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1CC | `func_8018F1B4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D4 | `func_8018F1BC` | room_m0391i `func_8018F3A8` | era_o2_g0 |
| 0x318 | `func_8018F300` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x320 | `func_8018F308` | room_m0391i `func_8018F4F4` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0065i
EXACT_REBUILD_GATE[room_m0065i]=PASS plan=e4260d845a06ce0b35ff34e200192c3338e6536fb88d245d7de8a6961a1ef00f yaml=965db9de95c5032e278babe713941cd4f4ab4507d66408788d8070f7e8d02962 spans=[10 c, 1 asm, 0 rodata, 2 bin] sha1_orig=fa62e6cdf74b19728ab31b55f190143f0ec93bc8 sha1_cand=fa62e6cdf74b19728ab31b55f190143f0ec93bc8 sha1=fa62e6cdf74b19728ab31b55f190143f0ec93bc8
```
