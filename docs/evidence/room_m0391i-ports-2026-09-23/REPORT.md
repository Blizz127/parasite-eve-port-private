# room_m0391i: masked-body ports (2026-09-23)

Room target `room_m0391i` (PE.IMG room m0391i chunk 2, VRAM base `0x8018EFE8`).
55 C spans: 2 hand-written here, 53 ported;
33 distinct masked bodies in this room. Across all 72 overlay
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
| 0x22C | `func_8018F214` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x258 | `func_8018F240` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x2A0 | `func_8018F288` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x32C | `func_8018F314` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3B0 | `func_8018F398` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B8 | `func_8018F3A0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x504 | `func_8018F4EC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x11FC | `func_801901E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1204 | `func_801901EC` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x12A4 | `func_8019028C` | room_m0022i `func_8018F274` | room_m0391i_dispatch_8018EFFC |
| 0x1528 | `func_80190510` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1530 | `func_80190518` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x15B8 | `func_801905A0` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x1BA8 | `func_80190B90` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x1EC8 | `func_80190EB0` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x1F64 | `func_80190F4C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F6C | `func_80190F54` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1F74 | `func_80190F5C` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x1FF0 | `func_80190FD8` | room_m0022i `func_8018FFC0` | room_m0391i_dispatch_8018F07C |
| 0x2268 | `func_80191250` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2270 | `func_80191258` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x22F8 | `func_801912E0` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x292C | `func_80191914` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x29B0 | `func_80191998` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x2A08 | `func_801919F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2A10 | `func_801919F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2A18 | `func_80191A00` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x2AB4 | `func_80191A9C` | room_m0022i `func_80190A84` | room_m0391i_dispatch_8018F104 |
| 0x2CFC | `func_80191CE4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2D04 | `func_80191CEC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2D2C | `func_80191D14` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x3128 | `func_80192110` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x34D0 | `func_801924B8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x352C | `func_80192514` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3534 | `func_8019251C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x353C | `func_80192524` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x35CC | `func_801925B4` | room_m0022i `func_8019159C` | room_m0391i_dispatch_8018F16C |
| 0x3814 | `func_801927FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x381C | `func_80192804` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3844 | `func_8019282C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x3D30 | `func_80192D18` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x40C0 | `func_801930A8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x411C | `func_80193104` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4124 | `func_8019310C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x412C | `func_80193114` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x4168 | `func_80193150` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x4200 | `func_801931E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4208 | `func_801931F0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4230 | `func_80193218` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x4260 | `func_80193248` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x44F4 | `func_801934DC` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x4510 | `func_801934F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5308 | `func_801942F0` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0391i
EXACT_REBUILD_GATE[room_m0391i]=PASS plan=9878b316014e195dc5717776b225ebba0fe4d87b962ce8025a8b153a95970f49 yaml=65d9942885d0d3b65774d23bc076a9dfc27f8ef9c3ceb2384778276a61545b30 spans=[55 c, 11 asm, 1 rodata, 2 bin] sha1_orig=327d5766f293ada3d33a96d655a20273a9b95145 sha1_cand=327d5766f293ada3d33a96d655a20273a9b95145 sha1=327d5766f293ada3d33a96d655a20273a9b95145
```
