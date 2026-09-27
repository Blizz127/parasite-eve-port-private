# func_80025EE8

- VRAM `0x80025EE8`, file offset `0x166E8`, size `0x718` (454 words)
- Profile: `era_o2_g8_force_d8009d254_d8009d1a0_d8009d1f4_d8009d278_d8009d288_d8009d2b0_absolute`
  (`-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D1A0,D_8009D1F4,D_8009D278,D_8009D288,D_8009D2B0`); cc1 2.7.2
- Source: `src/func_80025EE8.c`; carved from the former `[0x163D8, asm]` span: `[0x163D8, asm]`, `[0x166E8, c, func_80025EE8]`, `[0x16E00, asm]` (= `func_80026600`).

## Verification (decompC private config, session 2026-09-24)

```
python3 tools/build/disc1_preflight.py --config build/fanout/decompC/disc1.yaml --profiles build/fanout/decompC/disc1_build_profiles.json --deep --only func_80025EE8
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompC/disc1.yaml --profiles build/fanout/decompC/disc1_build_profiles.json func_80025EE8
compiled 1 YAML C leaves (1 era, 0 modern)
disc1_preflight: PASS (deep, 1963 c / 256 asm / 2 rodata)
LEAF_STRONG=EXACT func_80025EE8 span=0x718 vram=0x80025EE8 compiled=0x720 failures=[none]
python3 tools/analysis/era_link_check.py src/func_80025EE8.c 0x80025EE8 0x718 -O2 -G8 (+ the forced-absolute list) -> LINK_EXACT
```

## What it does

Battle item-menu step (khasinski TU `Battle_HandleItemMenu`). Returns `ret` (1 normally, 0 once the
battle-end check ran or the target list is empty). An `END_CHECK` sequence appears three times
inlined in retail: `ret = 0`; if the current slot's kind `D_800BE834[0].f0` is in 3..406 and
`D_8009D254->f0E >= 4`, set `D_8009D1A0 |= 0x100`; unless the kind is in 387..407, scan the
`D_8009CE3C` slots for a kind of 1/2 and call `func_80021128()` on the first hit; then clear
`D_8009D288`, `D_8009D278->f10`, set `D_8009D278->f4C |= 0x200000` and call `func_80051244()`.
The main path handles the `D_8009D1F4 & 0x2000` confirm branch (`D_8009CE60`, `func_80062F9C` /
`func_8005112C` by `D_8009D1F0`, decrementing `D_8009D2D8` and re-arming `D_8009D1DC`), the
`D_8009D1F0 == 0` cursor step (`func_800258CC(k)` from the target's `f6`/`f10`, the `0x200`, `0x80`
and `0x400` flag reactions), and otherwise the `func_80026824()` result dispatch.

## Load-bearing levers (mismatch ladder 369 -> 395 -> 430 -> 453 -> 454 aligned)

1. **`END_CHECK` as a macro with `ret = 0;` FIRST** (three textual copies). A value-returning inline
   sets `ret` at the end of the sequence, so cc1 folds each `return ret` to `move v0,zero` and the
   copies keep separate tails; retail has `move s0,zero` in the first branch slot and copies 1/2
   cross-jump into one shared `…; func_800275CC; func_8006DF50(1100); move v0,s0` tail.
2. **One shared `return ret;` after the `D_8009D1F0 == 0` if/else** (no `return` inside the then-block):
   jump2 first pairs the END_CHECK tail's return with the shared return and retargets that jump; the
   `1101` call site's jump then finds it in the jump chain and is the side that gets merged
   (retail's `j 0x800261E4` into the `1100` call sequence). With a `return` inside the block the merge
   goes the other way (+25 words).
3. `unsigned char ret`, `f1`, `f2`: the `andi 0xff` truncations, and — the last word — `D_8009CE60 = ret`
   stored from `$s0`: with `int ret`, cse's QImode class for the constant 1 holds only `f1` and the
   store uses `$s1`; with `unsigned char ret` the class is `{ret, f1}` and cse picks `ret`.
4. `k = D_8009D1F0; if (k == 0) …` with the same `k` later carrying the `func_800258CC` argument
   (the load lands in `$a0` via the call-argument preference).
5. `signed char r = func_80026824()` (one `sll/sra` reused by the `-1` and `0` tests);
   `if (--D_8009D2D8 != 0)` (no reload; `sll 24` zero test).
6. `D_800BE834` as an 8-byte `Slot` array (`sll 3` index, `lhu` at +0); `D_8009E000` as 12-byte `Entry`
   rows (`&D_8009E000[D_8009CE44]` -> `x*3<<2`).

## Main-tree integration (2026-09-27)

Integrated into `configs/USA/disc1.yaml` / `disc1_build_profiles.json` and re-verified against the
main config (not the lane copy): `disc1_preflight.py --deep --only func_80025EE8` PASS,
`leaf_strong_check.py func_80025EE8` → `LEAF_STRONG=EXACT`, `profile_necessity.py --only func_80025EE8` PASS.
Full gate: `EXACT_REBUILD_GATE=PASS plan=cda0547790149322… spans=[2041 c, 218 asm, 2 rodata]`,
sha1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
