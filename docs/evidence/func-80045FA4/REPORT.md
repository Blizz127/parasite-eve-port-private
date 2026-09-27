# func_80045FA4 — item detail panel

- VRAM `0x80045FA4`, size `0x268`, profile `era_o2_g8`. Source: `src/func_80045FA4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80045FA4 0x268`.


## Levers
1. `-G8` for D_8009CF0C (gp 0x19C); the four stat words at D_800A1888 are an unsized `int[]` so they stay absolute (scalars go gp-relative).
2. Retail keeps `z - (f1C - 108)` and `base - (h - 128)` unreassociated: the inner differences live in locals `y` / `u` (a direct `a - (b - c)` is folded to `(a - b) + c`, and unsigned casts do not stop it).
3. The list-row arm result is its own local `dy` (reusing `t`, which also holds a call result, costs a register copy: 13 words).
4. Branch-1 argument pieces computed `x = w->f18 - self->f18; z = ...*16; y = self->f1C - 108;` (3 of 6 orders exact).
5. Clamp `n >= 1000 ? 999 : n` (the `n < 1000 ? n : 999` polarity inverts the branch).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80045FA4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80045FA4
disc1_preflight: PASS (deep, 1857 c / 294 asm / 2 rodata)
LEAF_STRONG=EXACT func_80045FA4 span=0x268 vram=0x80045FA4 compiled=0x270 failures=[none]
```
