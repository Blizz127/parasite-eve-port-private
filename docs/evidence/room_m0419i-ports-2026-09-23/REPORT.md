# room_m0419i: masked-body ports (2026-09-23)

Room target `room_m0419i` (PE.IMG room m0419i chunk 2, VRAM base `0x8018EFE8`).
58 C spans: 0 hand-written here, 58 ported;
36 distinct masked bodies in this room. Across all 72 overlay
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
| 0x26C | `func_8018F254` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x298 | `func_8018F280` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x2E0 | `func_8018F2C8` | room_m0075i `func_8018F284` | era_o2_g0 |
| 0x3A4 | `func_8018F38C` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x428 | `func_8018F410` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x430 | `func_8018F418` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x770 | `func_8018F758` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x778 | `func_8018F760` | room_m0075i `func_8018F71C` | era_o2_g0 |
| 0x7F8 | `func_8018F7E0` | room_m0075i `func_8018F79C` | era_o2_g0 |
| 0xBA0 | `func_8018FB88` | room_m0075i `func_8018FB44` | era_o2_g0 |
| 0xF6C | `func_8018FF54` | room_m0075i `func_8018FF10` | era_o2_g0 |
| 0x1B50 | `func_80190B38` | room_m0075i `func_80190AF4` | era_o2_g0 |
| 0x1C60 | `func_80190C48` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1C68 | `func_80190C50` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x1D08 | `func_80190CF0` | room_m0022i `func_8018F274` | room_m0419i_dispatch_8018F00C |
| 0x1F8C | `func_80190F74` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F94 | `func_80190F7C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x201C | `func_80191004` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x260C | `func_801915F4` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x292C | `func_80191914` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x29C8 | `func_801919B0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x29D0 | `func_801919B8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x29D8 | `func_801919C0` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x2A54 | `func_80191A3C` | room_m0022i `func_8018FFC0` | room_m0419i_dispatch_8018F08C |
| 0x2CCC | `func_80191CB4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2CD4 | `func_80191CBC` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2D5C | `func_80191D44` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x3390 | `func_80192378` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x3414 | `func_801923FC` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x346C | `func_80192454` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3474 | `func_8019245C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x347C | `func_80192464` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x3518 | `func_80192500` | room_m0022i `func_80190A84` | room_m0419i_dispatch_8018F114 |
| 0x3760 | `func_80192748` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3768 | `func_80192750` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3790 | `func_80192778` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3B8C | `func_80192B74` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x3F34 | `func_80192F1C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3F90 | `func_80192F78` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3F98 | `func_80192F80` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3FA0 | `func_80192F88` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x4030 | `func_80193018` | room_m0022i `func_8019159C` | room_m0419i_dispatch_8018F17C |
| 0x4278 | `func_80193260` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4280 | `func_80193268` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x42A8 | `func_80193290` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x4794 | `func_8019377C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4B24 | `func_80193B0C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4B80 | `func_80193B68` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4B88 | `func_80193B70` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4B90 | `func_80193B78` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x4BCC | `func_80193BB4` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4C64 | `func_80193C4C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4C6C | `func_80193C54` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4C94 | `func_80193C7C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x4CC4 | `func_80193CAC` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x4F58 | `func_80193F40` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4F74 | `func_80193F5C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5C04 | `func_80194BEC` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0419i
EXACT_REBUILD_GATE[room_m0419i]=PASS plan=a9c0c2c5844862670721b4ad904d477a33713944bc405d7d15dd0d53f54bf6ae yaml=73747b31a2bd060cd721d62a20830fbf80e26e23f82b456310379a92528675ed spans=[58 c, 15 asm, 1 rodata, 2 bin] sha1_orig=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a sha1_cand=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a sha1=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a
```
