# room_m0418i: masked-body ports (2026-09-23)

Room target `room_m0418i` (PE.IMG room m0418i chunk 2, VRAM base `0x8018EFE8`).
81 C spans: 0 hand-written here, 81 ported;
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
| 0x258 | `func_8018F240` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x260 | `func_8018F248` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x28C | `func_8018F274` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x2B8 | `func_8018F2A0` | room_m0049i `func_8018F094` | era_o2_g0 |
| 0x2E0 | `func_8018F2C8` | room_m0049i `func_8018F0BC` | era_o2_g0 |
| 0x350 | `func_8018F338` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x3D4 | `func_8018F3BC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x514 | `func_8018F4FC` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x51C | `func_8018F504` | room_m0186i `func_8019021C` | era_o2_g0 |
| 0xEF4 | `func_8018FEDC` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0xEFC | `func_8018FEE4` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xF28 | `func_8018FF10` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xF54 | `func_8018FF3C` | room_m0049i `func_8018F094` | era_o2_g0 |
| 0xF7C | `func_8018FF64` | room_m0049i `func_8018F0BC` | era_o2_g0 |
| 0xFEC | `func_8018FFD4` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x1070 | `func_80190058` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x125C | `func_80190244` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1264 | `func_8019024C` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0x23F0 | `func_801913D8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x23F8 | `func_801913E0` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x2424 | `func_8019140C` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x2450 | `func_80191438` | room_m0049i `func_8018F094` | era_o2_g0 |
| 0x2478 | `func_80191460` | room_m0049i `func_8018F0BC` | era_o2_g0 |
| 0x24E8 | `func_801914D0` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x256C | `func_80191554` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x2758 | `func_80191740` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4AFC | `func_80193AE4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4B04 | `func_80193AEC` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0x4B30 | `func_80193B18` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0x4B5C | `func_80193B44` | room_m0049i `func_8018F094` | era_o2_g0 |
| 0x4B84 | `func_80193B6C` | room_m0049i `func_8018F0BC` | era_o2_g0 |
| 0x4BF4 | `func_80193BDC` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x4C78 | `func_80193C60` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x4F88 | `func_80193F70` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x4F90 | `func_80193F78` | room_m0111i `func_8018F2D0` | era_o2_g0 |
| 0x5DE8 | `func_80194DD0` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x6370 | `func_80195358` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x6378 | `func_80195360` | room_m0022i `func_8018F1D4` | era_o2_g0 |
| 0x6418 | `func_80195400` | room_m0022i `func_8018F274` | room_m0418i_dispatch_8018F068 |
| 0x669C | `func_80195684` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x66A4 | `func_8019568C` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x672C | `func_80195714` | room_m0022i `func_8018F588` | era_o2_g0 |
| 0x6D1C | `func_80195D04` | room_m0022i `func_8018FB78` | era_o2_g0 |
| 0x703C | `func_80196024` | room_m0022i `func_8018FE98` | era_o2_g0 |
| 0x70D8 | `func_801960C0` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x70E0 | `func_801960C8` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x70E8 | `func_801960D0` | room_m0022i `func_8018FF44` | era_o2_g0 |
| 0x7164 | `func_8019614C` | room_m0022i `func_8018FFC0` | room_m0418i_dispatch_8018F0E8 |
| 0x73DC | `func_801963C4` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x73E4 | `func_801963CC` | room_m0022i `func_8018F500` | era_o2_g0 |
| 0x746C | `func_80196454` | room_m0022i `func_801902C8` | era_o2_g0 |
| 0x7AA0 | `func_80196A88` | room_m0022i `func_801908FC` | era_o2_g0 |
| 0x7B24 | `func_80196B0C` | room_m0022i `func_80190980` | era_o2_g0 |
| 0x7B7C | `func_80196B64` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x7B84 | `func_80196B6C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x7B8C | `func_80196B74` | room_m0022i `func_801909E8` | era_o2_g0 |
| 0x7C28 | `func_80196C10` | room_m0022i `func_80190A84` | room_m0418i_dispatch_8018F170 |
| 0x7E70 | `func_80196E58` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x7E78 | `func_80196E60` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x7EA0 | `func_80196E88` | room_m0022i `func_80190CFC` | era_o2_g0 |
| 0x829C | `func_80197284` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x8644 | `func_8019762C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x86A0 | `func_80197688` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x86A8 | `func_80197690` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x86B0 | `func_80197698` | room_m0022i `func_8019150C` | era_o2_g0 |
| 0x8740 | `func_80197728` | room_m0022i `func_8019159C` | room_m0418i_dispatch_8018F1D8 |
| 0x8988 | `func_80197970` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x8990 | `func_80197978` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x89B8 | `func_801979A0` | room_m0022i `func_80191814` | era_o2_g0 |
| 0x8EA4 | `func_80197E8C` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x9234 | `func_8019821C` | room_m0022i `func_801914A0` | era_o2_g0 |
| 0x9290 | `func_80198278` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x9298 | `func_80198280` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x92A0 | `func_80198288` | room_m0022i `func_801920FC` | era_o2_g0 |
| 0x92DC | `func_801982C4` | room_m0022i `func_80192138` | era_o2_g0 |
| 0x9374 | `func_8019835C` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x937C | `func_80198364` | room_m0022i `func_80190CD4` | era_o2_g0 |
| 0x93A4 | `func_8019838C` | room_m0022i `func_80192200` | era_o2_g0 |
| 0x93D4 | `func_801983BC` | room_m0022i `func_80192230` | era_o2_g0 |
| 0x9668 | `func_80198650` | room_m0022i `func_801924C4` | era_o2_g0 |
| 0x9684 | `func_8019866C` | room_m0022i `func_8018F1CC` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0418i
EXACT_REBUILD_GATE[room_m0418i]=PASS plan=dda6001b0e042bb725c3ce9db51dbf113a2cb00dda544e4ed738923d84ef59ce yaml=9ddd3557275d31b1d3abd0f160dce19bdd7249c48dfb76f971167ea187b70fd1 spans=[81 c, 17 asm, 1 rodata, 2 bin] sha1_orig=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca sha1_cand=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca sha1=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca
```
