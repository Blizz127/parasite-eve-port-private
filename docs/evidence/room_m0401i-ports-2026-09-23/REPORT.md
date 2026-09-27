# room_m0401i: masked-body ports (2026-09-23)

Room target `room_m0401i` (PE.IMG room m0401i chunk 2, VRAM base `0x8018EFE8`).
75 C spans: 0 hand-written here, 75 ported;
42 distinct masked bodies in this room. Across all 72 overlay
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
| 0x258 | `func_8018F240` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x284 | `func_8018F26C` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x2CC | `func_8018F2B4` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x358 | `func_8018F340` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3DC | `func_8018F3C4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3E4 | `func_8018F3CC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3EC | `func_8018F3D4` | room_m0065i `func_8018F1BC` | era_o2_g0 |
| 0x530 | `func_8018F518` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x538 | `func_8018F520` | room_m0391i `func_8018F4F4` | era_o2_g0 |
| 0x1228 | `func_80190210` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1230 | `func_80190218` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x125C | `func_80190244` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x1288 | `func_80190270` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x12D0 | `func_801902B8` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x135C | `func_80190344` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x13E0 | `func_801903C8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x15D8 | `func_801905C0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x15E0 | `func_801905C8` | room_m0137i `func_8018F428` | era_o2_g0 |
| 0x1660 | `func_80190648` | room_m0137i `func_8018F4A8` | era_o2_g0 |
| 0x1BCC | `func_80190BB4` | room_m0137i `func_8018FA14` | era_o2_g0 |
| 0x1D10 | `func_80190CF8` | room_m0137i `func_8018FB58` | era_o2_g0 |
| 0x1D94 | `func_80190D7C` | room_m0137i `func_8018FBDC` | era_o2_g0 |
| 0x1FD0 | `func_80190FB8` | room_m0137i `func_8018FE18` | era_o2_g0 |
| 0x2344 | `func_8019132C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x234C | `func_80191334` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x23EC | `func_801913D4` | room_m0022i `func_8018F274` | room_m0401i_dispatch_8018F034 |
| 0x2670 | `func_80191658` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2678 | `func_80191660` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x2700 | `func_801916E8` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x2CF0 | `func_80191CD8` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x3010 | `func_80191FF8` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x30AC | `func_80192094` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x30B4 | `func_8019209C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x30BC | `func_801920A4` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x3138 | `func_80192120` | room_m0022i `func_8018FFC0` | room_m0401i_dispatch_8018F0B4 |
| 0x33B0 | `func_80192398` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x33B8 | `func_801923A0` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x3440 | `func_80192428` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x3A74 | `func_80192A5C` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x3AF8 | `func_80192AE0` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x3B50 | `func_80192B38` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B58 | `func_80192B40` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3B60 | `func_80192B48` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x3BFC | `func_80192BE4` | room_m0022i `func_80190A84` | room_m0401i_dispatch_8018F13C |
| 0x3E44 | `func_80192E2C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3E4C | `func_80192E34` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3E74 | `func_80192E5C` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x4270 | `func_80193258` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4618 | `func_80193600` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x4674 | `func_8019365C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x467C | `func_80193664` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4684 | `func_8019366C` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x4714 | `func_801936FC` | room_m0022i `func_8019159C` | room_m0401i_dispatch_8018F1A4 |
| 0x495C | `func_80193944` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4964 | `func_8019394C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x498C | `func_80193974` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x4E78 | `func_80193E60` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x5208 | `func_801941F0` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x5264 | `func_8019424C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x526C | `func_80194254` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5274 | `func_8019425C` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x52B0 | `func_80194298` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x5348 | `func_80194330` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5350 | `func_80194338` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x5378 | `func_80194360` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x53A8 | `func_80194390` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x563C | `func_80194624` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x5658 | `func_80194640` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5660 | `func_80194648` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5668 | `func_80194650` | room_m0145i `func_801924F8` | era_o2_g0 |
| 0x56CC | `func_801946B4` | room_m0145i `func_8019255C` | era_o2_g0 |
| 0x57E0 | `func_801947C8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5938 | `func_80194920` | room_m0145i `func_801927C8` | era_o2_g0 |
| 0x5E2C | `func_80194E14` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x5E48 | `func_80194E30` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0401i
EXACT_REBUILD_GATE[room_m0401i]=PASS plan=f40a701a3be6b6043978ff34d01b4de2eeef49729c3f00ee1dd08168dfa7aa4c yaml=4b5fe0f0cd28aa2f5dbee19c1bdfb55641a1367f3071b58d24b7e3b4d416c94a spans=[75 c, 18 asm, 1 rodata, 2 bin] sha1_orig=19d2caf765e89732886d5034b92d78fd12a3e441 sha1_cand=19d2caf765e89732886d5034b92d78fd12a3e441 sha1=19d2caf765e89732886d5034b92d78fd12a3e441
```
