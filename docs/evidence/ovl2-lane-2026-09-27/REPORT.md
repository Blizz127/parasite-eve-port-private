# Overlay lane `ovl2` — 2026-09-27 (continuation of lane `ovl`)

Verification per body: `tools/analysis/room_lane/try.sh <t> <src> <vram> <size> -O2 -G0` (era_link_check → word mismatches=0),
`disc1_preflight.py --target <t> --deep --only <func>` → PASS, then `scripts/exact_rebuild_overlay.sh <t>` serially.

## Matched bodies

### func_80192678 (room_m0145i, 0x150) — 5 rooms
Copies (byte-identical): m0152i func_801926BC, m0153i func_80193B8C, m0399i func_8019367C, m0401i func_801947D0.
Profile `era_o2_g0_expand_div` (MASPSX_EXPAND_DIV=1; without it: 61 word mismatches, the inline div guards).
Body: indirect update call, copy three short fields into +0x1FC.., func_80079754, scale model by a scratchpad
VECTOR (0x1F800008) = +0x26, then a sin-table (D_800966EC) breathing scale while counter +0x32 runs down.
Lever — scratchpad addressing: an `int *b = (int *)0x1F800000` base with `b[2]/b[3]/b[4]` stores and `b + 2`
as the call argument. cc1 folds the offset stores to absolute `lui $at` stores (retail) but keeps the
`b + 2` value in a callee-saved register for the second block's vx store (`lui $s3` early, `ori $s3,8`
in the beqz delay slot). Ladder: VEC* constant (0x1F800008) 59; per-constant `*(int *)0x1F80000x` 73
(every constant forced into a register); b-base with constant arg 79; b-base + `b+2` arg **0**.

Gate lines:
```
EXACT_REBUILD_GATE[room_m0145i]=PASS plan=cf719da053dc0c149f0e7be56843f7b2ff38eb46364410b9a9e0dfe239fcf12f yaml=e5b18e8852648f13158764dd4ca0506cd7a5a905eff00bc7418f0e041089f6cf spans=[53 c, 10 asm, 1 
EXACT_REBUILD_GATE[room_m0152i]=PASS plan=69dc1bfe595dcaa4101d0b9e32d75efafbc40845f961af2e768870e6f3628d78 yaml=f21b74cd9c6d4d6c115a877e85f76b1493f988ce47a24cf0467736cf565cd215 spans=[55 c, 11 asm, 1 
EXACT_REBUILD_GATE[room_m0153i]=PASS plan=80e2baec4fde982d75c608948540e2f7ce6c6ae953179f90eed4f0294e09fdbd yaml=d5ce6b98d227d15e33363b1d01c4e48ae7c8fcb6d3e70dd57a4222f9a70b002b spans=[73 c, 15 asm, 1 
EXACT_REBUILD_GATE[room_m0399i]=PASS plan=48f106bcea0276551e1def754caa4b043cb8b4d290b14a81fa72ff67a5554c28 yaml=1d0a3823ca3014726a689f5c521ac0e1d363b101f119333ea98195c7256e708f spans=[63 c, 11 asm, 1 
EXACT_REBUILD_GATE[room_m0401i]=PASS plan=a44b4e6505e029ac6b39d9b861f880a059c6415c991d19b4d419c44601dacde9 yaml=5db05c04076f9368be0ba0ffbfbf82a490098ba25ed6e0035bccb957d6bf0599 spans=[85 c, 11 asm, 1 
```

## Parked
- m0141i `func_8018FB70` (0x25C, 5 rooms) — 14 words: all in the loop-invariant setup (i=0, &D_801921DA, &D_801921D0).
  Retail puts them after the first call (temp `$v1`); cc1 sched1 floats them above it into the prologue. The rest matches
  (SVECTOR-indexed loop `((SV *)a2)[i]`, not a pointer walk, which saves an extra giv register). Tried: int/unsigned
  counter, do-while, i=0 at top, declaration-initialized r, explicit q/o locals (147). Draft build/lanes/ovl2/drafts/park_func_8018FB70.c.

## Non-C
- ovl_039F `func_8010C4C4`: a lone alignment `nop`.
