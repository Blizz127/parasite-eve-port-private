# room_m0187i: masked-body ports (2026-09-23)

Room target `room_m0187i` (PE.IMG room m0187i chunk 2, VRAM base `0x8018EFE8`).
12 C spans: 0 hand-written here, 12 ported;
12 distinct masked bodies in this room. Across all 72 overlay
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
| 0x34 | `func_8018F01C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x60 | `func_8018F048` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x8C | `func_8018F074` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0xD4 | `func_8018F0BC` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x160 | `func_8018F148` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1E4 | `func_8018F1CC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B8 | `func_8018F3A0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3C0 | `func_8018F3A8` | room_m0186i `func_8019021C` | era_o2_g0 |
| 0x3F8 | `func_8018F3E0` | room_m0186i `func_80190254` | era_o2_g0 |
| 0x680 | `func_8018F668` | room_m0186i `func_801904DC` | era_o2_g0 |
| 0xDB0 | `func_8018FD98` | room_m0186i `func_80190C0C` | era_o2_g0 |
| 0xEE0 | `func_8018FEC8` | room_m0186i `func_80190D3C` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0187i
EXACT_REBUILD_GATE[room_m0187i]=PASS plan=f38f80408567d0d48dde5b1b646638300d714449a5a64322f470bd1d4828a959 yaml=f181bec8701d62312926efa18c5f26b46832d96ea943b49a1b7b038c518ef43b spans=[12 c, 3 asm, 0 rodata, 2 bin] sha1_orig=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1_cand=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1
```
