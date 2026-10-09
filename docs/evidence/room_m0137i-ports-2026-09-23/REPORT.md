# room_m0137i: masked-body ports (2026-09-23)

Room target `room_m0137i` (PE.IMG room m0137i chunk 2, VRAM base `0x8018EFE8`).
18 C spans: 10 hand-written here, 8 ported;
18 distinct masked bodies in this room. Across all 72 overlay
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
| 0x90 | `func_8018F078` | room_m0049i `func_8018F03C` | era_o2_g0 |
| 0xBC | `func_8018F0A4` | room_m0034i `func_8018F0B8` | era_o2_g0 |
| 0xE8 | `func_8018F0D0` | room_m0065i `func_8018F054` | era_o2_g0 |
| 0x130 | `func_8018F118` | room_m0065i `func_8018F09C` | era_o2_g0 |
| 0x1BC | `func_8018F1A4` | room_m0034i `func_8018F1B8` | era_o2_g0 |
| 0x240 | `func_8018F228` | room_m0022i `func_8018F1CC` | era_o2_g0 |
| 0x438 | `func_8018F420` | room_m0022i `func_801910F8` | era_o2_g0 |
| 0x1E2C | `func_80190E14` | room_m0126i `func_8018FCC0` | era_o2_g0 |

Gate line from the last serial run after this YAML's last change (overlays2 pass 3, 2026-09-24):

```
$ scripts/exact_rebuild_overlay.sh room_m0137i
EXACT_REBUILD_GATE[room_m0137i]=PASS plan=5e4ac5f375304d92cc8852c8637bf68f2f6d9db7bba5e955fa2b8659b75d0bf0 yaml=37fdc1cdf622fe3e258ed0b7565a34009ca2d5a4da18eaae040855c0d574875a spans=[18 c, 5 asm, 0 rodata, 2 bin] sha1_orig=6e146c37de2790b03bcb104954571437abb82c1a sha1_cand=6e146c37de2790b03bcb104954571437abb82c1a sha1=6e146c37de2790b03bcb104954571437abb82c1a
```

### `func_8019018C` (0x11A4, 0x9C0 bytes, 2026-10-09)

This is the particle callback of the room's effect set. The battle effect dispatcher (`func_800D413C`) calls it. It was the Day 4 stop in m0327i, which shares this text group: `CPU_BOUNDARY/REFUSED pc=0x8019018C ra=@800D413C:effect`.
- Mode 1 integrates five particle kinds. Kind 0 can trigger the room event and spawns kind-4 sparks.
- Mode 2 sets the effect palette (`D_800F3368` block) and draws through `func_800CEE20`/`func_800D1DEC`.

It is hand-matched (no masked twin) and LINK_EXACT at the room VMA under the local profile `room_m0137i_dispatch_8018F03C_8018F054_div` (MASPSX_THREE_WORD_SYMBOL_STORE, MASPSX_DISPATCH_FOLD=jtbl_8018F03C,jtbl_8018F054, MASPSX_EXPAND_DIV). The two jump tables are split out of the header bin as `[0x54, rodata]` (pool for the fold).

The draft went from 248 to 89 to 66 to 31 to 9 to 0 word mismatches. The levers:
- The effect parameters at 0x800F3368 are one struct (in-struct stores keep retail's store order).
- The clut index is read first, and the clut is loaded before the kind/f6E stores.
- Both size computations are two-step.
- `pos` is filled vx, vz, vy.

The generated port has no boundaries: the inline GTE block is retail's rot+trans ctc2 sequence, lowered to native GTE writes.

```
$ scripts/exact_rebuild_overlay.sh room_m0137i
EXACT_REBUILD_GATE[room_m0137i]=PASS plan=5fc6643a415b282083e7764b5f2397a9c4eeb78a9e2a615392163abe9e771d4c yaml=e439add8ce21dcf7ec864832e81cc0a0a33ca3cbc523934cb7812f91f87f0b50 spans=[25 c, 0 asm, 1 rodata, 3 bin] sha1_orig=6e146c37de2790b03bcb104954571437abb82c1a sha1_cand=6e146c37de2790b03bcb104954571437abb82c1a sha1=6e146c37de2790b03bcb104954571437abb82c1a
```

Day 4 probe (warp m0322i@0x1DC, run a32): the m0327i battle now runs and Aya walks on into m0328i. The next stop is m0328i's battle effect (text group m0141i) `func_80191330`.
