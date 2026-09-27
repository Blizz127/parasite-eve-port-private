# func_80025BD8 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x80025BD8`, file offset `0x163D8`, size `0x310` (ends at `0x166E8`, the next row `func_80025EE8`)
- Profile `era_o2_g8_force_d8009d278_d8009d20c_d8009d254_absolute`: `-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D20C,D_8009D254`
- YAML: `- [0x163D8, c, func_80025BD8]` replaces `- [0x163D8, asm]` (span is the whole former asm row)

Shape: battle target selection. Seeds `D_8009D1DC` (count) and `D_8009CE38[0..3]` from
`D_8009D278->f68`, picks a target actor `t` (ally search / counter search / self), and appends
it to the `D_800BE830[D_8009CE3C]` slot table either once (type 2, when `f10 & 0xC0` is `0xC0`
or `0x40`) or `D_8009D1DC` times (type 1), then calls `func_80021128()` and sets
`D_8009D278->f4C |= 0x200000`.

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_80025BD8
  disc1_preflight: PASS (deep, 2047 c / 215 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_80025BD8
  LEAF_STRONG=EXACT func_80025BD8 span=0x310 vram=0x80025BD8 compiled=0x310 failures=[none]
```
`profile_necessity.py` has no `--config` — skipped. Manual necessity: `-O2 -G8` without the
force-absolute list → 184 mismatches.

## Levers (mismatch ladder)

Started from the parked decompA draft (51 here; its note said ~31, with a `$6` pin on `t` and
`D_8009CE3C` spelled `D_8009CE38[4]`).

1. Inline `append()` helpers (various orders) → 34–42 — abandoned.
2. **`D_8009CE3C` as its own scalar gp byte** (`extern unsigned char D_8009CE38[4]; extern
   unsigned char D_8009CE3C;` instead of `D_8009CE38[8]` with `[4]`) and the
   `s = &D_800BE830[D_8009CE3C]; s->a = t; s->type = K; s->grp = (signed char)D_8009D2D8;
   D_8009CE3C++;` append → 33.
3. **Block-local `Slot *s = &D_800BE830[D_8009CE3C];` initializer in the type-2 arm**
   → 2 (retail's `n+1` before the `n*8+base` pointer that reuses `n`'s register). The `$6`
   pin on `t` is no longer needed.
4. **`short g = (signed char)D_8009D2D8;` as the first statement of the type-1 loop body**
   → 0 (the hoisted `sra` lands before the hoisted `D_8009D1DC - 1`).

The same append idiom closed `func_80026600` (see its REPORT).
