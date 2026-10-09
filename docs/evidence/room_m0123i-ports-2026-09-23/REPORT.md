# room_m0123i: masked-body ports (2026-09-23)

Room target `room_m0123i` (PE.IMG room m0123i chunk 2, VRAM base `0x8018EFE8`).
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
| 0x220 | `func_8018F208` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x2C0 | `func_8018F2A8` | room_m0022i `func_8018F274` | room_m0123i_dispatch_8018EFF4 |
| 0x544 | `func_8018F52C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x54C | `func_8018F534` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5D4 | `func_8018F5BC` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xBC4 | `func_8018FBAC` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEE4 | `func_8018FECC` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF80 | `func_8018FF68` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF88 | `func_8018FF70` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF90 | `func_8018FF78` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x100C | `func_8018FFF4` | room_m0022i `func_8018FFC0` | room_m0123i_dispatch_8018F074 |
| 0x1284 | `func_8019026C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x128C | `func_80190274` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x1314 | `func_801902FC` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1948 | `func_80190930` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19CC | `func_801909B4` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x1A24 | `func_80190A0C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A2C | `func_80190A14` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A34 | `func_80190A1C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AD0 | `func_80190AB8` | room_m0022i `func_80190A84` | room_m0123i_dispatch_8018F0FC |
| 0x1D18 | `func_80190D00` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1D20 | `func_80190D08` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D48 | `func_80190D30` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x2144 | `func_8019112C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24EC | `func_801914D4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2548 | `func_80191530` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2550 | `func_80191538` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2558 | `func_80191540` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25E8 | `func_801915D0` | room_m0022i `func_8019159C` | room_m0123i_dispatch_8018F164 |
| 0x2830 | `func_80191818` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2838 | `func_80191820` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2860 | `func_80191848` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D4C | `func_80191D34` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30DC | `func_801920C4` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3138 | `func_80192120` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3140 | `func_80192128` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3148 | `func_80192130` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x3184 | `func_8019216C` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x321C | `func_80192204` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3224 | `func_8019220C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x324C | `func_80192234` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x327C | `func_80192264` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x3510 | `func_801924F8` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x352C | `func_80192514` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6368 | `func_80195350` | room_m0005i `func_80190A6C` | era_o2_g0 |
| 0x63EC | `func_801953D4` | room_m0203i `func_80193FC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0123i
EXACT_REBUILD_GATE[room_m0123i]=PASS plan=d7e44921bf1e40272842de8c87c344f664b8c333a4178b6f41d27670959fcc14 yaml=9b0e3df6d260887bd3d9ebab6a1f7b3a21ce238d6e23dc5e09962a15a3270b2a spans=[46 c, 11 asm, 1 rodata, 2 bin] sha1_orig=7dbc62ff99461836f22d884ef04718af387e04cd sha1_cand=7dbc62ff99461836f22d884ef04718af387e04cd sha1=7dbc62ff99461836f22d884ef04718af387e04cd
```

## Hand-matched Day 3 fight effect (2026-10-09)

The two functions below have no masked twin. Both were hand-written, are `LINK_EXACT` at this room's VMA (`era_link_check.py --target room_m0123i`) and are carved as `c` rows. The Day 3 recorded-input route stopped in the m0123i fight at `CPU_BOUNDARY/REFUSED pc=0x80192F0C` before they were ported.

| offset | leaf | role | profile |
| 0x3BF4 | `func_80192BDC` | particle callback (age/spin; draw: func_800CEE20 sprite + func_800D1DEC glow, then a func_800D2B58 streak). Inline GTE: SetRot/TransMatrix of the camera slot through the func_800CF5B0 register barrier | era_o2_g0 |
| 0x3F24 | `func_80192F0C` | effect driver (pool alloc, 16 particle spawns, four func_800D3BC8 billboards, two func_800D004C fans, the func_800D0728 ring in frames 16..32) | era_o2_g0 |

Levers for `func_80192F0C` (the first draft had 226 words off). Most are GCC 2.x alias/scheduling choices, read from which retail loads stay behind which stores:
- The p00 store of each parameter block is a plain u16 store through `&D_800F3368` (as `(s16 *)&D_800F3368` in room_m0318i `func_80196C48`); the other fields are struct members.
- The `D_80195690` copy target is an SVECTOR, and `D_80195684 = p[0]` comes first.
- Case 0 loads `D_800F33E0` into a block temp before `p[0] = 0`.
- The 16..32 window uses a block-scoped temp.
- Case 1 ends with `if (D_800E27EC > 32) return 1; break;`.

Their EXE callees `func_800D3BC8` (sprite; LINK residual = prologue save batching, docs/evidence/func-800D3BC8/REPORT.md) and `func_800CF844` (orbit point, inline GTE) are native hand ports in `pc_port/game/boot/`.

```
$ scripts/exact_rebuild_overlay.sh room_m0123i
EXACT_REBUILD_GATE[room_m0123i]=PASS plan=fd37e77c088f3b0154d7b815837decb9445b88ae42a325937dffba9480d8826c yaml=3657ba38c58e3a12970046ed739d84f9da765025173681ea763da9eaa79b8044 spans=[64 c, 3 asm, 1 rodata, 2 bin] sha1_orig=7dbc62ff99461836f22d884ef04718af387e04cd sha1_cand=7dbc62ff99461836f22d884ef04718af387e04cd sha1=7dbc62ff99461836f22d884ef04718af387e04cd
```

`func_8019251C` (0x3534, 0x4E0 bytes) is also hand-matched (LINK_EXACT, no masked twin) and carved. After these three carves the room gate PASSes with spans [65 c, 2 asm, 1 rodata, 2 bin] and the same sha1 7dbc62ff99461836f22d884ef04718af387e04cd (k31, 2026-10-09).
