# func_80026600 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x80026600`, file offset `0x16E00`, size `0x224` (ends on `jr $ra` at `0x8002681C` + delay slot; next function `func_80026824` at `0x17024`)
- Profile `era_o2_g8_expand_div_force_d8009d278_absolute`: `-O2 -G8`, `MASPSX_EXPAND_DIV=1`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278`
- YAML: `- [0x16E00, c, func_80026600]` + `- [0x17024, asm]` (replaces `- [0x16E00, asm]`)

Shape: battle target-slot fill. Depending on `D_8009D278->f68->f10 & 0xC0` it appends every
`D_8009E000[]` entry (mode `0xC0`), `((f10&0xF)*3)>>1` random entries (mode `0x40`,
`func_80071A54() % D_8009D2B0[0]`), or the single caller entry `*a0` (type 1, decrementing
`D_8009D1DC`) to the `D_800BE830[D_8009CE3C]` slot table (`{value, type, (signed char)D_8009D2D8}`).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_80026600
  disc1_preflight: PASS (deep, 2046 c / 216 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_80026600
  LEAF_STRONG=EXACT func_80026600 span=0x224 vram=0x80026600 compiled=0x230 failures=[none]
```
`profile_necessity.py` has no `--config` — skipped. Manual necessity: without
`FORCE_ABSOLUTE D_8009D278` → 136 mismatches; without `MASPSX_EXPAND_DIV` → 65.

## Levers (mismatch ladder)

Started from the parked decompA draft (82: loop-1 temps one register higher, loop-2 stores
scheduled differently).

1. **Loop 1 as `i = 0; while (D_8009E000[i].f0 != 0) { v = D_8009E000[i].f0; i++; ... }`**
   (value read into a local before the `i++`): retail's body index `andi $3,i,0xFF` then
   becomes identical to the rotated exit test's, so reorg drops it as redundant and steals the
   `addiu i` into the back-branch slot. 82 → 65. (`D_8009E000[i++].f0` → 76; `int x = i; i++`
   → 82.)
2. **`short g = (signed char)D_8009D2D8;` as the first body statement** (loop 1): the hoisted
   `sll/sra` lands before the peeled index ops like retail. 65 → 62.
3. **Loop 2: `g` and `v = D_8009E000[r].f0` computed before `s = &D_800BE830[k]`**, stores in
   natural `f0, f4, f6` order → 0 (every store permutation is 0 with this order; with `v` read
   after the pointer every permutation is 64).

The single-entry tail (`s = &D_800BE830[D_8009CE3C]; s->f0 = *a0; ...; D_8009CE3C++;`) was
already exact in the draft — `D_8009CE3C` as a *scalar* gp byte gives retail's early `n+1`.
