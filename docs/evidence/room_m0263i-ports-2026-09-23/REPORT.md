# room_m0263i: masked-body ports (2026-09-23)

Room target `room_m0263i` (PE.IMG room m0263i chunk 2, VRAM base `0x8018EFE8`).
46 C spans: 0 hand-written here, 46 ported;
28 distinct masked bodies in this room. Across all 72 overlay
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
| 0x210 | `func_8018F1F8` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2B0 | `func_8018F298` | room_m0022i `func_8018F274` | room_m0263i_dispatch_8018EFF4 |
| 0x534 | `func_8018F51C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x53C | `func_8018F524` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5C4 | `func_8018F5AC` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBB4 | `func_8018FB9C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xED4 | `func_8018FEBC` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF70 | `func_8018FF58` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF78 | `func_8018FF60` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF80 | `func_8018FF68` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFFC | `func_8018FFE4` | room_m0022i `func_8018FFC0` | room_m0263i_dispatch_8018F074 |
| 0x1274 | `func_8019025C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x127C | `func_80190264` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1304 | `func_801902EC` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1938 | `func_80190920` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19BC | `func_801909A4` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A14 | `func_801909FC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A1C | `func_80190A04` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A24 | `func_80190A0C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AC0 | `func_80190AA8` | room_m0022i `func_80190A84` | room_m0263i_dispatch_8018F0FC |
| 0x1D08 | `func_80190CF0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D10 | `func_80190CF8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D38 | `func_80190D20` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2134 | `func_8019111C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24DC | `func_801914C4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2538 | `func_80191520` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2540 | `func_80191528` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2548 | `func_80191530` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25D8 | `func_801915C0` | room_m0022i `func_8019159C` | room_m0263i_dispatch_8018F164 |
| 0x2820 | `func_80191808` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2828 | `func_80191810` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2850 | `func_80191838` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D3C | `func_80191D24` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30CC | `func_801920B4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3128 | `func_80192110` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3130 | `func_80192118` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3138 | `func_80192120` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3174 | `func_8019215C` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x320C | `func_801921F4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3214 | `func_801921FC` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x323C | `func_80192224` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x326C | `func_80192254` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3500 | `func_801924E8` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x351C | `func_80192504` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4174 | `func_8019315C` | room_m0014i `func_80193148` | era_o2_g0 |
| 0x4F88 | `func_80193F70` | room_m0104i `func_8018FDF8` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0263i
EXACT_REBUILD_GATE[room_m0263i]=PASS plan=f69cee22f357a8afa85e7388c30908cc043f751db701f2df706f35757c80a8b1 yaml=6eae8849d1603064a8c0376d68fee4359946afadbe37f80a88b65d148cd2c31c spans=[46 c, 11 asm, 1 rodata, 2 bin] sha1_orig=02f1b9cdb8ebc50504b5622d1b663246c8427dea sha1_cand=02f1b9cdb8ebc50504b5622d1b663246c8427dea sha1=02f1b9cdb8ebc50504b5622d1b663246c8427dea
```
