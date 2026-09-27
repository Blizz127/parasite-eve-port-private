# room_m0393i: masked-body ports (2026-09-23)

Room target `room_m0393i` (PE.IMG room m0393i chunk 2, VRAM base `0x8018EFE8`).
69 C spans: 0 hand-written here, 69 ported;
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
| 0x224 | `func_8018F20C` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x250 | `func_8018F238` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x27C | `func_8018F264` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x2C4 | `func_8018F2AC` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x350 | `func_8018F338` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3D4 | `func_8018F3BC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3DC | `func_8018F3C4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3E4 | `func_8018F3CC` | room_m0065i `func_8018F1BC` | era_o2_g0 |
| 0x528 | `func_8018F510` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x530 | `func_8018F518` | room_m0391i `func_8018F4F4` | era_o2_g0 |
| 0x1220 | `func_80190208` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1228 | `func_80190210` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x1254 | `func_8019023C` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x1280 | `func_80190268` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x12C8 | `func_801902B0` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x1354 | `func_8019033C` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x13D8 | `func_801903C0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x15D0 | `func_801905B8` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x15D8 | `func_801905C0` | room_m0137i `func_8018F428` | era_o2_g0 |
| 0x1658 | `func_80190640` | room_m0137i `func_8018F4A8` | era_o2_g0 |
| 0x1BC4 | `func_80190BAC` | room_m0137i `func_8018FA14` | era_o2_g0 |
| 0x1D08 | `func_80190CF0` | room_m0137i `func_8018FB58` | era_o2_g0 |
| 0x1D8C | `func_80190D74` | room_m0137i `func_8018FBDC` | era_o2_g0 |
| 0x1FC8 | `func_80190FB0` | room_m0137i `func_8018FE18` | era_o2_g0 |
| 0x233C | `func_80191324` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2344 | `func_8019132C` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x23E4 | `func_801913CC` | room_m0022i `func_8018F274` | room_m0393i_dispatch_8018F034 |
| 0x2668 | `func_80191650` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2670 | `func_80191658` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x26F8 | `func_801916E0` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x2CE8 | `func_80191CD0` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x3008 | `func_80191FF0` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x30A4 | `func_8019208C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x30AC | `func_80192094` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x30B4 | `func_8019209C` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x3130 | `func_80192118` | room_m0022i `func_8018FFC0` | room_m0393i_dispatch_8018F0B4 |
| 0x33A8 | `func_80192390` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x33B0 | `func_80192398` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x3438 | `func_80192420` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x3A6C | `func_80192A54` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x3AF0 | `func_80192AD8` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x3B48 | `func_80192B30` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B50 | `func_80192B38` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B58 | `func_80192B40` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x3BF4 | `func_80192BDC` | room_m0022i `func_80190A84` | room_m0393i_dispatch_8018F13C |
| 0x3E3C | `func_80192E24` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3E44 | `func_80192E2C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3E6C | `func_80192E54` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x4268 | `func_80193250` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4610 | `func_801935F8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x466C | `func_80193654` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4674 | `func_8019365C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x467C | `func_80193664` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x470C | `func_801936F4` | room_m0022i `func_8019159C` | room_m0393i_dispatch_8018F1A4 |
| 0x4954 | `func_8019393C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x495C | `func_80193944` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x4984 | `func_8019396C` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x4E70 | `func_80193E58` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x5200 | `func_801941E8` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x525C | `func_80194244` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5264 | `func_8019424C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x526C | `func_80194254` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x52A8 | `func_80194290` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x5340 | `func_80194328` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5348 | `func_80194330` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x5370 | `func_80194358` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x53A0 | `func_80194388` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x5634 | `func_8019461C` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x5650 | `func_80194638` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0393i
EXACT_REBUILD_GATE[room_m0393i]=PASS plan=5dfa06f8677e7db26971f0124e205c250756e280093e80d24fb3bf93b9263e15 yaml=467f49ba223bad32ab7d1999a9b9ec3d9900dad2eb9b92bec2d7498c373b4854 spans=[69 c, 14 asm, 1 rodata, 2 bin] sha1_orig=cef7a5f2ad37426f7894315ec54d97526f322ee8 sha1_cand=cef7a5f2ad37426f7894315ec54d97526f322ee8 sha1=cef7a5f2ad37426f7894315ec54d97526f322ee8
```
