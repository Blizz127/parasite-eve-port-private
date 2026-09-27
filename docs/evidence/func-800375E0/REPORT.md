# func_800375E0 — EXACT (lane exeB2, 2026-09-27)

- VRAM `0x800375E0`, file offset `0x27DE0`, size `0x284` (161 words; object 0x290 = gas 16-byte pad).
- Damage-popup slot allocator: 4 × 56-byte `Slot` at `D_800BCEA8`, up to 5 numbers split into
  decimal digits (count at `dig[j][5]`).
- Profile `era_o2_g8` (`-O2 -G8`). YAML rows: `- [0x27DE0, c, func_800375E0]`, `- [0x28064, c, func_80037864]` (unchanged).
- Supersedes `PARK.md` in this directory (parked at 81/83 register-home residual).

## Verification (private lane config build/lanes/exeB2/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB2/disc1.yaml --profiles build/lanes/exeB2/disc1_build_profiles.json --deep --only func_800375E0
  disc1_preflight: PASS (deep, 2071 c / 201 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB2/disc1.yaml --profiles build/lanes/exeB2/disc1_build_profiles.json func_800375E0
  LEAF_STRONG=EXACT func_800375E0 span=0x284 vram=0x800375E0 compiled=0x290 failures=[none]
tools/analysis/era_link_check.py src/func_800375E0.c 0x800375E0 0x284 -O2 -G8
  word mismatches=0
tools/analysis/era_link_check.py src/func_800375E0.c 0x800375E0 0x284 -O2 -G0
  word mismatches=153   (so era_o2_g8 is load-bearing)
```
`profile_necessity.py` has no `--config` (reads the main yaml), so it was not run against the private carve.

## Load-bearing levers (mismatch ladder)
1. Digit loop: a **`short t` quotient temp plus `v = q` inside the loop condition**:
   `while (t = (short)v / 10, p = D_800BCEA8[0].dig[0] + (j*6 + i*56), p[k] = v - (q = t) * 10, v = q, (short)t != 0) k++;`
   83 → 2. Retail tests `(short)t` via `sll $4,$4,16` on the division result and keeps the
   unsigned-short copy `q` in a separate register (`addu $5,$4,$0`, `andi $3,$5,0xffff`). Testing
   `(short)q` lets cc1 fold the test into `andi` of `q`. Putting `v = q` in the body moves every register home.
   An `int t` variant is 141/144 (the loop is restructured).
2. **Copy `x` to a local (`short x = x0;`)** with **no register pins at all**: the two entry moves
   (`addu $12,$6,$0` before `addu $9,$4,$0`) come out in retail order. 2 → 0. The earlier
   `register ... nums asm("$12")` pin is no longer needed.
