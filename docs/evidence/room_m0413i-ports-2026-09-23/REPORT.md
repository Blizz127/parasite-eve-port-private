# room_m0413i: masked-body ports (2026-09-23)

Room target `room_m0413i` (PE.IMG room m0413i chunk 2, VRAM base `0x8018EFE8`).
45 C spans: 0 hand-written here, 45 ported;
27 distinct masked bodies in this room. Across all 72 overlay
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
| 0x204 | `func_8018F1EC` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2A4 | `func_8018F28C` | room_m0022i `func_8018F274` | room_m0413i_dispatch_8018EFF4 |
| 0x528 | `func_8018F510` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x530 | `func_8018F518` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5B8 | `func_8018F5A0` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBA8 | `func_8018FB90` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEC8 | `func_8018FEB0` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF64 | `func_8018FF4C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF6C | `func_8018FF54` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF74 | `func_8018FF5C` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFF0 | `func_8018FFD8` | room_m0022i `func_8018FFC0` | room_m0413i_dispatch_8018F074 |
| 0x1268 | `func_80190250` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1270 | `func_80190258` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12F8 | `func_801902E0` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x192C | `func_80190914` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19B0 | `func_80190998` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A08 | `func_801909F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A10 | `func_801909F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A18 | `func_80190A00` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AB4 | `func_80190A9C` | room_m0022i `func_80190A84` | room_m0413i_dispatch_8018F0FC |
| 0x1CFC | `func_80190CE4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D04 | `func_80190CEC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D2C | `func_80190D14` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2128 | `func_80191110` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24D0 | `func_801914B8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x252C | `func_80191514` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2534 | `func_8019151C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x253C | `func_80191524` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25CC | `func_801915B4` | room_m0022i `func_8019159C` | room_m0413i_dispatch_8018F164 |
| 0x2814 | `func_801917FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x281C | `func_80191804` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2844 | `func_8019182C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D30 | `func_80191D18` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30C0 | `func_801920A8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x311C | `func_80192104` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3124 | `func_8019210C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x312C | `func_80192114` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3168 | `func_80192150` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x3200 | `func_801921E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3208 | `func_801921F0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3230 | `func_80192218` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3260 | `func_80192248` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34F4 | `func_801924DC` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3510 | `func_801924F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x40EC | `func_801930D4` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0413i
EXACT_REBUILD_GATE[room_m0413i]=PASS plan=aeed7daec4c396b249ad0456a0a03ac848bc333439dca5e2c0d45e5e72d04bd0 yaml=38737b9c3d6d3b9bfe35236e7d09e1ccbf6c1615deeb02e35295ba9087c6f800 spans=[45 c, 10 asm, 1 rodata, 2 bin] sha1_orig=6083b41a6287b0e1015436aad411298206a40a8e sha1_cand=6083b41a6287b0e1015436aad411298206a40a8e sha1=6083b41a6287b0e1015436aad411298206a40a8e
```
