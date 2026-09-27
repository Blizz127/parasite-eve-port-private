# func_8003F074 — scene/light setup (three clamped light vectors + init calls)

- VRAM `0x8003F074`, file offset `0x2F874`, size `0x288` (next span `[0x2FAFC, c, func_8003F2FC]`).
- Profile `era_o2_g8_force_d8009d224_d8009d308_d8009d2e8_d800942ec_d800b8a18_d800b0ce4_d800b162c_d800b1628_absolute`
  = `-O2 -G8` + `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D224,D_8009D308,D_8009D2E8,D_800942EC,D_800B8A18,D_800B0CE4,D_800B162C,D_800B1628`.
- Lane exeB, 2026-09-27. Draft base `build/archive/fanout/decompA/drafts/func_8003F074.c`
  (previously PARKED at 119 mismatches, structurally one word per light block).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_8003F074
  -> disc1_preflight: PASS (deep, 2044 c / 215 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_8003F074
  -> LEAF_STRONG=EXACT func_8003F074 span=0x288 vram=0x8003F074 compiled=0x290 failures=[none]
```
(`compiled=0x290` is the 8-byte zero gas alignment pad past the `0x288` body.)
Profile necessity by hand: dropping `D_8009D224` → 151 mismatches, `D_8009D308` → 148,
`D_8009D2E8` → 13; dropping any of the other five fails the link (out of the `-G8`
window); plain `-O2 -G0` → 158.

## Lever: a body-local pointer, not an inline parameter

Retail keeps `&D_800BEA40[n]` in a register: it stores `vx` through `0(reg)`, stores
`vy`/`vz` to absolute `sym+off`, and derives the `func_8003F798` base argument from the
same register (`addiu $a0,$v0,-6*n`, cse related-value). That is cse's `find_best_addr`:
a plain-`REG` address is not folded to its constant, while `(plus reg 2)` / `(plus reg 4)`
are folded to `sym+8` / `sym+10`.

The old `static inline set_vec(Vec3 *p, ...)` fails because gcc-2.7 inlining substitutes
a constant argument straight into the body. Every address becomes a constant and folds
to absolute (119 mismatches). With a macro that declares a block-local `Vec3 *p` and does
`p = &D_800BEA40[n]; p->vx = x; p->vy = y; p->vz = z;`, the retail shape comes back.

| variant | mismatches |
|---|---|
| inline `set_vec(&D_800BEA40[n], ...)` | 119 |
| macro with one function-scope `p`, `x`, `y`, `z` shared by the 3 blocks | 131 (block 1 copies `p` `$v0`→`$a0`; `y`/`z` zero-init order swapped) |
| macro with block-scoped `{ Vec3 *p; int x, y, z; ... }` | **0** |

The clamp stays as the `> 0x7FFF` / `< -0x7FFF` ladder (per-component `CLAMP` macro). It
reproduces the leftover `slti`/`-0x7FFF` branch shapes on the constant inputs.
