# func_8004B70C — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x8004B70C`, file offset `0x3BF0C`, size `0x200`; yaml `- [0x3BF0C, c, func_8004B70C]` replaces
  `- [0x3BF0C, asm]` exactly (next row `[0x3C10C, c, func_8004B90C]`).
- Profile `era_o2_g8` (`-O2 -G8`). EXP/AP gain apply on `D_800C0E00` (player block): snapshot EXP, level lookups via
  `func_8005B8A8`, AP clamp `D_8009CF74 = min(D_8009CF74 + h1E, 99999)`, seven-stat copy loop through `func_8005B91C`.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_8004B70C
  disc1_preflight: PASS (deep, 2045 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_8004B70C
  LEAF_STRONG=EXACT func_8004B70C span=0x200 vram=0x8004B70C compiled=0x200 failures=[none]
```
Necessity (manual): `-O2 -G0` → 110 words + size change; `-G8` is load-bearing.

## Levers (ladder)

decompA draft (ternary clamp) 28 words. This lane:
1. **Inline ternary clamp** `static inline int clamp_max(int v, int max) { return v > max ? max : v; }` gives retail's
   two separate `99999` materializations (`$v0` compare, `$a1` default) — but `p = &D_800C0E00` then folds to absolute
   (68). (An if-bodied inline gives 113; plain if/`r` forms 111.)
2. **Empty barrier `asm("" : "=r"(p) : "0"(p));` placed immediately before `p->h1E = 0;`** (inside the `if`, after the
   clamp): p stays in `$s2` for both `p->exp` and the `h1E` clear, and the other `D_800C0E00` fields stay absolute
   → 15. (Barrier right after `p = &D_800C0E00` → 87: cse then keeps `&h1E` in a register.)
3. **Explicit `short *hp = D_800C0E00.h28;` walker (`t = *hp++;`) with index-form `D_800A18D8[i] = t`** swaps the two
   loop givs into retail's `$s3` (h28) / `$s2` (`D_800A18D8`) → 0. (Index form both 15; both-pointer form 119;
   `int *dp` walker 14; `short t` 15.)
