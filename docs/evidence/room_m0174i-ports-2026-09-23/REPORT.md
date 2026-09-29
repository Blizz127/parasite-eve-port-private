# room_m0174i: masked-body ports (2026-09-23)

Room target `room_m0174i` (PE.IMG room m0174i chunk 2, VRAM base `0x8018EFE8`).
93 C spans: 25 hand-written here, 68 ported;
56 distinct masked bodies in this room. Across all 72 overlay
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
| 0x318 | `func_8018F300` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x344 | `func_8018F32C` | room_m0034i `func_8018F0E4` | era_o2_g0 |
| 0x38C | `func_8018F374` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x418 | `func_8018F400` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x49C | `func_8018F484` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4A4 | `func_8018F48C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x5CC | `func_8018F5B4` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x61C | `func_8018F604` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1194 | `func_8019017C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x119C | `func_80190184` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x11C8 | `func_801901B0` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x1258 | `func_80190240` | room_m0034i `func_8018F12C` | era_o2_g0 |
| 0x12E4 | `func_801902CC` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1368 | `func_80190350` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x1690 | `func_80190678` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1718 | `func_80190700` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1720 | `func_80190708` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2EA0 | `func_80191E88` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2EA8 | `func_80191E90` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x2EB0 | `func_80191E98` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x44A0 | `func_80193488` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x44A8 | `func_80193490` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x44B0 | `func_80193498` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4978 | `func_80193960` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4980 | `func_80193968` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x4A20 | `func_80193A08` | room_m0022i `func_8018F274` | room_m0174i_dispatch_8018F07C |
| 0x4CA4 | `func_80193C8C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4CAC | `func_80193C94` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x4D34 | `func_80193D1C` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x5324 | `func_8019430C` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x5644 | `func_8019462C` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x56E0 | `func_801946C8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x56E8 | `func_801946D0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x56F0 | `func_801946D8` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x576C | `func_80194754` | room_m0022i `func_8018FFC0` | room_m0174i_dispatch_8018F0FC |
| 0x59E4 | `func_801949CC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x59EC | `func_801949D4` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x5A74 | `func_80194A5C` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x60A8 | `func_80195090` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x612C | `func_80195114` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x6184 | `func_8019516C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x618C | `func_80195174` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6194 | `func_8019517C` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x6230 | `func_80195218` | room_m0022i `func_80190A84` | room_m0174i_dispatch_8018F184 |
| 0x6478 | `func_80195460` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6480 | `func_80195468` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x64A8 | `func_80195490` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x68A4 | `func_8019588C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x6C4C | `func_80195C34` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x6CA8 | `func_80195C90` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6CB0 | `func_80195C98` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6CB8 | `func_80195CA0` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x6D48 | `func_80195D30` | room_m0022i `func_8019159C` | room_m0174i_dispatch_8018F1EC |
| 0x6F90 | `func_80195F78` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6F98 | `func_80195F80` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x6FC0 | `func_80195FA8` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x74AC | `func_80196494` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x783C | `func_80196824` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x7898 | `func_80196880` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x78A0 | `func_80196888` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x78A8 | `func_80196890` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x78E4 | `func_801968CC` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x797C | `func_80196964` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x7984 | `func_8019696C` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x79AC | `func_80196994` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x79DC | `func_801969C4` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x7C70 | `func_80196C58` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x7C8C | `func_80196C74` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0174i
EXACT_REBUILD_GATE[room_m0174i]=PASS plan=b90545056c3d5918ac01a4553a0d2d7746324f525ba1bef829795d21f2f72363 yaml=d653a36f0695a2f24898601796342e7203f511fc46b40315dc7f34dc3f94da5b spans=[93 c, 17 asm, 1 rodata, 2 bin] sha1_orig=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1_cand=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01
```

## 2026-09-29: M34 phase-1 fire-breath / ember bodies (four inline-GTE ports)

These are the masked-body twins of the four room_m0348i leaves matched in lane m348 (hand-written
there, with Psy-Q `inline_c.h`-shaped GTE macros; see `docs/evidence/func-80190E98/`,
`func-801914A0/`, `func-80191A98/`, `func-80191FE0/`). They were ported with
`ROOM_LANE_DRY=1 python3 tools/analysis/room_lane/port.py m0174i room_m0348i` (dry run first,
then copied). Every `func_`/`D_` token was remapped word by word, with no SKIP. All four carry
the `inline GTE/asm` tag, so count them separately from plain C. They add 4 C spans in this
room but no new distinct masked body (the bodies are already counted in room_m0348i).

| offset | leaf | source | profile |
| --- | --- | --- | --- |
| 0x2FC8 | `func_80191FB0` | room_m0348i `func_80190E98` (beam) | era_o2_g0 |
| 0x35D0 | `func_801925B8` | room_m0348i `func_801914A0` (beam segment update) | era_o2_g0 |
| 0x3BC8 | `func_80192BB0` | room_m0348i `func_80191A98` (floor glow) | era_o2_g0 |
| 0x4110 | `func_801930F8` | room_m0348i `func_80191FE0` (ember burst) | era_o2_g0 |

Per leaf: `era_link_check.py --target room_m0174i` gave `LINK_EXACT` (0 mismatched words), and
`disc1_preflight.py --target room_m0174i --deep --only <func>` printed
`PASS (deep, 117 c / 1 asm / 1 rodata)`.

```
$ flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh room_m0174i
EXACT_REBUILD_GATE[room_m0174i]=PASS plan=821c685be78f15091ebacca6113c7dc55138563fc3acbf360935725e53c92ce2 yaml=d3552f7f0701c0b0c3a79a94207687912136462b10b5f846a38fcaf2d354eae5 spans=[117 c, 1 asm, 1 rodata, 2 bin] sha1_orig=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1_cand=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01
```

sha256: `room_m0174i.yaml` = `d3552f7f0701c0b0c3a79a94207687912136462b10b5f846a38fcaf2d354eae5`;
`room_m0174i_build_profiles.json` = `867403f049a43dfedeb1ded388e957f9770441c4b58bd954b62bc9bd443ea749` (unchanged).
