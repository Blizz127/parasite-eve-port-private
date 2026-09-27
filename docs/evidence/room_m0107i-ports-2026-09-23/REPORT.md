# room_m0107i: masked-body ports (2026-09-23)

Room target `room_m0107i` (PE.IMG room m0107i chunk 2, VRAM base `0x8018EFE8`).
65 C spans: 0 hand-written here, 65 ported;
39 distinct masked bodies in this room. Across all 72 overlay
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
| 0x1EC | `func_8018F1D4` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x218 | `func_8018F200` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x244 | `func_8018F22C` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x28C | `func_8018F274` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x318 | `func_8018F300` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x39C | `func_8018F384` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3A4 | `func_8018F38C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3AC | `func_8018F394` | room_m0421i `func_8018F210` | era_o2_g0 |
| 0x4B8 | `func_8018F4A0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4C0 | `func_8018F4A8` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0x540 | `func_8018F528` | room_m0421i `func_8018F3A4` | era_o2_g0 |
| 0x9F0 | `func_8018F9D8` | room_m0421i `func_8018F854` | era_o2_g0 |
| 0xAE4 | `func_8018FACC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0xAEC | `func_8018FAD4` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0xAF4 | `func_8018FADC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0xAFC | `func_8018FAE4` | room_m0421i `func_8018F960` | era_o2_g0 |
| 0xB1C | `func_8018FB04` | room_m0421i `func_8018F980` | era_o2_g0 |
| 0xCE0 | `func_8018FCC8` | room_m0421i `func_8018FB44` | era_o2_g0 |
| 0xD44 | `func_8018FD2C` | room_m0421i `func_8018FBA8` | era_o2_g0 |
| 0x1190 | `func_80190178` | room_m0421i `func_8018F854` | era_o2_g0 |
| 0x1284 | `func_8019026C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x128C | `func_80190274` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x132C | `func_80190314` | room_m0022i `func_8018F274` | room_m0107i_dispatch_8018EFFC |
| 0x15B0 | `func_80190598` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x15B8 | `func_801905A0` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1640 | `func_80190628` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x1C30 | `func_80190C18` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x1F50 | `func_80190F38` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x1FEC | `func_80190FD4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1FF4 | `func_80190FDC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1FFC | `func_80190FE4` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x2078 | `func_80191060` | room_m0022i `func_8018FFC0` | room_m0107i_dispatch_8018F07C |
| 0x22F0 | `func_801912D8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x22F8 | `func_801912E0` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2380 | `func_80191368` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x29B4 | `func_8019199C` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x2A38 | `func_80191A20` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x2A90 | `func_80191A78` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2A98 | `func_80191A80` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2AA0 | `func_80191A88` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2B3C | `func_80191B24` | room_m0022i `func_80190A84` | room_m0107i_dispatch_8018F104 |
| 0x2D84 | `func_80191D6C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2D8C | `func_80191D74` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2DB4 | `func_80191D9C` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x31B0 | `func_80192198` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3558 | `func_80192540` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x35B4 | `func_8019259C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x35BC | `func_801925A4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x35C4 | `func_801925AC` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x3654 | `func_8019263C` | room_m0022i `func_8019159C` | room_m0107i_dispatch_8018F16C |
| 0x389C | `func_80192884` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x38A4 | `func_8019288C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x38CC | `func_801928B4` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x3DB8 | `func_80192DA0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4148 | `func_80193130` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x41A4 | `func_8019318C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x41AC | `func_80193194` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x41B4 | `func_8019319C` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x41F0 | `func_801931D8` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4288 | `func_80193270` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4290 | `func_80193278` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x42B8 | `func_801932A0` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x42E8 | `func_801932D0` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x457C | `func_80193564` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4598 | `func_80193580` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0107i
EXACT_REBUILD_GATE[room_m0107i]=PASS plan=fd2150ffe2b323c6025df37e773f0c56ec819604c97b6299327a25373b5f121e yaml=08c4cd8b6726482646bb38b607d6a1a0ec5b25d17751ec4214fac103cd110318 spans=[65 c, 10 asm, 1 rodata, 2 bin] sha1_orig=dcf6cf737f829adb79fad3ecb65a08d57cf6f664 sha1_cand=dcf6cf737f829adb79fad3ecb65a08d57cf6f664 sha1=dcf6cf737f829adb79fad3ecb65a08d57cf6f664
```
