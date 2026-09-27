# room_m0014i: masked-body ports (2026-09-23)

Room target `room_m0014i` (PE.IMG room m0014i chunk 2, VRAM base `0x8018EFE8`).
45 C spans: 1 hand-written here, 44 ported;
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
| 0x1FC | `func_8018F1E4` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x29C | `func_8018F284` | room_m0022i `func_8018F274` | room_m0014i_dispatch_8018EFF4 |
| 0x520 | `func_8018F508` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x528 | `func_8018F510` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5B0 | `func_8018F598` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBA0 | `func_8018FB88` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEC0 | `func_8018FEA8` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF5C | `func_8018FF44` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF64 | `func_8018FF4C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF6C | `func_8018FF54` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFE8 | `func_8018FFD0` | room_m0022i `func_8018FFC0` | room_m0014i_dispatch_8018F074 |
| 0x1260 | `func_80190248` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1268 | `func_80190250` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12F0 | `func_801902D8` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1924 | `func_8019090C` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19A8 | `func_80190990` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A00 | `func_801909E8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A08 | `func_801909F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A10 | `func_801909F8` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AAC | `func_80190A94` | room_m0022i `func_80190A84` | room_m0014i_dispatch_8018F0FC |
| 0x1CF4 | `func_80190CDC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1CFC | `func_80190CE4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D24 | `func_80190D0C` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2120 | `func_80191108` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24C8 | `func_801914B0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2524 | `func_8019150C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x252C | `func_80191514` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2534 | `func_8019151C` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25C4 | `func_801915AC` | room_m0022i `func_8019159C` | room_m0014i_dispatch_8018F164 |
| 0x280C | `func_801917F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2814 | `func_801917FC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x283C | `func_80191824` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D28 | `func_80191D10` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30B8 | `func_801920A0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3114 | `func_801920FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x311C | `func_80192104` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3124 | `func_8019210C` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3160 | `func_80192148` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x31F8 | `func_801921E0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3200 | `func_801921E8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3228 | `func_80192210` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3258 | `func_80192240` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34EC | `func_801924D4` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3508 | `func_801924F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0014i
EXACT_REBUILD_GATE[room_m0014i]=PASS plan=766afed9b504b879c1d01d0600b9988a2019c1d8beff1d3d924925fe05a61726 yaml=312a587405e5e6a6711d9acdcc77a563ac814a4092dadeec0a2d98d0727d7bec spans=[45 c, 10 asm, 1 rodata, 2 bin] sha1_orig=af4054c825b84d32b0c9a49131a50aa02bf974b9 sha1_cand=af4054c825b84d32b0c9a49131a50aa02bf974b9 sha1=af4054c825b84d32b0c9a49131a50aa02bf974b9
```
