# func_80032B0C — EXACT (lane exeB3, 2026-09-27)

- VRAM `0x80032B0C`, file offset `0x2330C`, size `0x850` (damage-number sprite emitter). Supersedes
  `PARK.md` in this directory.
- Profile `era_o2_g8_force_d8009cddc_absolute` (existing): `-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC`.
- yaml: `- [0x2330C, asm]` becomes `- [0x2330C, c, func_80032B0C]`. The span ends at the next
  row `- [0x23B5C, c, func_8003335C]`, so there is no resume row.

## Verification (private lane config)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB3/disc1.yaml --profiles build/lanes/exeB3/disc1_build_profiles.json --deep --only func_80032B0C
  disc1_preflight: PASS (deep, 2081 c / 193 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB3/disc1.yaml --profiles build/lanes/exeB3/disc1_build_profiles.json func_80032B0C
  LEAF_STRONG=EXACT func_80032B0C span=0x850 vram=0x80032B0C compiled=0x850 failures=[none]
MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CDDC tools/analysis/era_link_check.py src/func_80032B0C.c 0x80032B0C 0x850 -O2 -G8
  LINK_EXACT
```
Necessity, checked by hand (profile_necessity.py has no `--config`): without force-absolute, 510
mismatches; default `-O2 -G0`, 509.

## Levers (mismatch ladder)
1. Prior draft: 514.
2. **i-first index spelling** `#define E2 (*(Ent (*)[10][5])&D_800B01C0[D_8009CDDC])[D_8009D230]`
   (the pointer-cast forms all give retail's `D_8009CDDC*1400` first). **`char buf[8]`**: the i-first
   spelling leaves 5 unallocated pseudos (combine's `(use (reg))` insns) with 8-byte reload stack
   slots, and retail has those same 40 bytes. `buf[48]` had been sized to fake them: 518 → 510.
3. Mode-1 arm: compute `x`/`y` **before** the first `r0` store (retail loads `d->x/c/y` before it): 461.
4. **`int y`** (a `short` y narrows `c - 30` to `ori 0xffe2`): 16.
5. **Block-local `{ int t = d->c - 0x1E; y = d->y + t; }`** at both sites. The inline expression
   gets reassociated to `(y - 30) + c`; a function-scope `t` gets a global pseudo, so it no longer
   reuses `c`'s register: 16 → 7.
6. **Mode-1 `r0/g0/b0/u0/v0` stores written `(&E2[0].sp)->f = ...`** (all five). cse then
   materialises `D_800B01C0+8` first and derives the call argument as `-8`, as retail does
   (`lui/addiu 456`, `addiu -8`). The `E2[0].sp.f` form makes it load the bare symbol first. Any
   strict subset gives 497+: 7 → 0.
