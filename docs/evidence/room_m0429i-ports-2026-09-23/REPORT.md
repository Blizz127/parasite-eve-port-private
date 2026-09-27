# room_m0429i: masked-body ports (2026-09-23)

Room target `room_m0429i` (PE.IMG room m0429i chunk 2, VRAM base `0x8018EFE8`).
55 C spans: 0 hand-written here, 55 ported;
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
| 0x290 | `func_8018F278` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x330 | `func_8018F318` | room_m0022i `func_8018F274` | room_m0429i_dispatch_8018EFF4 |
| 0x5B4 | `func_8018F59C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5BC | `func_8018F5A4` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x644 | `func_8018F62C` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xC34 | `func_8018FC1C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xF54 | `func_8018FF3C` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xFF0 | `func_8018FFD8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xFF8 | `func_8018FFE0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1000 | `func_8018FFE8` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x107C | `func_80190064` | room_m0022i `func_8018FFC0` | room_m0429i_dispatch_8018F074 |
| 0x12F4 | `func_801902DC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x12FC | `func_801902E4` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1384 | `func_8019036C` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x19B8 | `func_801909A0` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x1A3C | `func_80190A24` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A94 | `func_80190A7C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A9C | `func_80190A84` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1AA4 | `func_80190A8C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1B40 | `func_80190B28` | room_m0022i `func_80190A84` | room_m0429i_dispatch_8018F0FC |
| 0x1D88 | `func_80190D70` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D90 | `func_80190D78` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1DB8 | `func_80190DA0` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x21B4 | `func_8019119C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x255C | `func_80191544` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x25B8 | `func_801915A0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x25C0 | `func_801915A8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x25C8 | `func_801915B0` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x2658 | `func_80191640` | room_m0022i `func_8019159C` | room_m0429i_dispatch_8018F164 |
| 0x28A0 | `func_80191888` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x28A8 | `func_80191890` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x28D0 | `func_801918B8` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2DBC | `func_80191DA4` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x314C | `func_80192134` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x31A8 | `func_80192190` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31B0 | `func_80192198` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31B8 | `func_801921A0` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x31F4 | `func_801921DC` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x328C | `func_80192274` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3294 | `func_8019227C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x32BC | `func_801922A4` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x32EC | `func_801922D4` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3580 | `func_80192568` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x359C | `func_80192584` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x35A4 | `func_8019258C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3670 | `func_80192658` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3678 | `func_80192660` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x36A0 | `func_80192688` | room_m0269i `func_8018F160` | era_o2_g0 |
| 0x3E18 | `func_80192E00` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xC1F0 | `func_8019B1D8` | room_m0245i `func_801955E4` | era_o2_g0 |
| 0xC21C | `func_8019B204` | room_m0245i `func_801955E4` | era_o2_g0 |
| 0xC22C | `func_8019B214` | room_m0245i `func_801955E4` | era_o2_g0 |
| 0xC23C | `func_8019B224` | room_m0245i `func_801955E4` | era_o2_g0 |
| 0xC24C | `func_8019B234` | room_m0245i `func_801955E4` | era_o2_g0 |
| 0xC25C | `func_8019B244` | room_m0245i `func_801955E4` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0429i
EXACT_REBUILD_GATE[room_m0429i]=PASS plan=5a656d1a37095556143fdc431b370eca2601682d1bd9f0d607430db25ad4f3d2 yaml=f9bb9212e9997093d51b8aed206a81de278260fe5e0af5b3e00e990f1c6a6188 spans=[55 c, 13 asm, 1 rodata, 2 bin] sha1_orig=1487c56a4d420ebe6a13bb261622428226a66034 sha1_cand=1487c56a4d420ebe6a13bb261622428226a66034 sha1=1487c56a4d420ebe6a13bb261622428226a66034
```
