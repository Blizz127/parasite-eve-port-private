# room_m0205i: masked-body ports (2026-09-23)

Room target `room_m0205i` (PE.IMG room m0205i chunk 2, VRAM base `0x8018EFE8`).
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
| 0x1F8 | `func_8018F1E0` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x298 | `func_8018F280` | room_m0022i `func_8018F274` | room_m0205i_dispatch_8018EFF4 |
| 0x51C | `func_8018F504` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x524 | `func_8018F50C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5AC | `func_8018F594` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0xB9C | `func_8018FB84` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0xEBC | `func_8018FEA4` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0xF58 | `func_8018FF40` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF60 | `func_8018FF48` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xF68 | `func_8018FF50` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0xFE4 | `func_8018FFCC` | room_m0022i `func_8018FFC0` | room_m0205i_dispatch_8018F074 |
| 0x125C | `func_80190244` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1264 | `func_8019024C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x12EC | `func_801902D4` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x1920 | `func_80190908` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x19A4 | `func_8019098C` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x19FC | `func_801909E4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A04 | `func_801909EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1A0C | `func_801909F4` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x1AA8 | `func_80190A90` | room_m0022i `func_80190A84` | room_m0205i_dispatch_8018F0FC |
| 0x1CF0 | `func_80190CD8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1CF8 | `func_80190CE0` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x1D20 | `func_80190D08` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x211C | `func_80191104` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x24C4 | `func_801914AC` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x2520 | `func_80191508` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2528 | `func_80191510` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2530 | `func_80191518` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x25C0 | `func_801915A8` | room_m0022i `func_8019159C` | room_m0205i_dispatch_8018F164 |
| 0x2808 | `func_801917F0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2810 | `func_801917F8` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x2838 | `func_80191820` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x2D24 | `func_80191D0C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x30B4 | `func_8019209C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x3110 | `func_801920F8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3118 | `func_80192100` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3120 | `func_80192108` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x315C | `func_80192144` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x31F4 | `func_801921DC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x31FC | `func_801921E4` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x3224 | `func_8019220C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x3254 | `func_8019223C` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x34E8 | `func_801924D0` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x3504 | `func_801924EC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x3C2C | `func_80192C14` | room_m0014i `func_80193148` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0205i
EXACT_REBUILD_GATE[room_m0205i]=PASS plan=fc0a4627b65034276c0e47955d2fb64fddd2037d862c0cbc2365e6c13501404f yaml=5fa7a950bb810eba2c7831e9e6d7ce4faa6ecce5766cd7d54e7cc857ae3e398d spans=[45 c, 10 asm, 1 rodata, 2 bin] sha1_orig=0c281041b45ca7461ec98c2536143f14a63ae43e sha1_cand=0c281041b45ca7461ec98c2536143f14a63ae43e sha1=0c281041b45ca7461ec98c2536143f14a63ae43e
```
