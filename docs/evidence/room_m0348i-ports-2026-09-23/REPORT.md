# room_m0348i: masked-body ports (2026-09-23)

Room target `room_m0348i` (PE.IMG room m0348i chunk 2, VRAM base `0x8018EFE8`).
31 C spans: 0 hand-written here, 31 ported;
23 distinct masked bodies in this room. Across all 72 overlay
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
| 0x84 | `func_8018F06C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xB0 | `func_8018F098` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xDC | `func_8018F0C4` | room_m0174i `func_801901DC` | era_o2_g0 |
| 0x140 | `func_8018F128` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x1CC | `func_8018F1B4` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x250 | `func_8018F238` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x578 | `func_8018F560` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x580 | `func_8018F568` | room_m0174i `func_80190680` | era_o2_g0 |
| 0x600 | `func_8018F5E8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x608 | `func_8018F5F0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x8B8 | `func_8018F8A0` | room_m0174i `func_801909B8` | era_o2_g0 |
| 0x8D8 | `func_8018F8C0` | room_m0174i `func_801909D8` | era_o2_g0 |
| 0x9C4 | `func_8018F9AC` | room_m0174i `func_80190AC4` | era_o2_g0 |
| 0xA44 | `func_8018FA2C` | room_m0174i `func_80190B44` | era_o2_g0 |
| 0xC4C | `func_8018FC34` | room_m0174i `func_80190D4C` | era_o2_g0 |
| 0xCA4 | `func_8018FC8C` | room_m0174i `func_80190DA4` | era_o2_g0 |
| 0x1100 | `func_801900E8` | room_m0174i `func_80191200` | era_o2_g0 |
| 0x1150 | `func_80190138` | room_m0174i `func_80191250` | era_o2_g0 |
| 0x1C3C | `func_80190C24` | room_m0174i `func_80191D3C` | era_o2_g0 |
| 0x1C5C | `func_80190C44` | room_m0174i `func_80191D5C` | era_o2_g0 |
| 0x1D48 | `func_80190D30` | room_m0174i `func_80191E48` | era_o2_g0 |
| 0x1D88 | `func_80190D70` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1D90 | `func_80190D78` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1D98 | `func_80190D80` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1DA0 | `func_80190D88` | room_m0174i `func_80191EA0` | era_o2_g0 |
| 0x2DC8 | `func_80191DB0` | room_m0174i `func_80192EC8` | era_o2_g0 |
| 0x32F8 | `func_801922E0` | room_m0174i `func_801933F8` | era_o2_g0 |
| 0x3388 | `func_80192370` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3390 | `func_80192378` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3398 | `func_80192380` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x33A0 | `func_80192388` | room_m0174i `func_801934A0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0348i
EXACT_REBUILD_GATE[room_m0348i]=PASS plan=652166aa34da2b5f95290cdb4b1ed343c9ef2e3e1696ca82ac2df8314432af74 yaml=32c941d03ba2f159c057d293a7ba553e097849f4a54752abcd9123aa0e70d1d5 spans=[31 c, 8 asm, 0 rodata, 2 bin] sha1_orig=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1_cand=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1=49478531f4b705ab248a8c2eb0a3b81c4cef9985
```
