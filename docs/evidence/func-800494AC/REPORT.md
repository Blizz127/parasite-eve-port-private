# func_800494AC — EXACT (lane exeC2, 2026-09-27)

- VRAM `0x800494AC`, file offset `0x39CAC`, size `0xC1C` (775 words), `asm/disc1/39CAC.s`.
- Source `src/func_800494AC.c`; yaml row `- [0x39CAC, c, func_800494AC]` (ends exactly at the
  existing `[0x3A8C8, c, func_8004A0C8]` row — no resume row).
- Profile `era_o2_g8_force_d800a1888_plus3_absolute` (`-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800A1888,D_800A188C,D_800A1890,D_800A1894`).

## Verification (private lane config build/lanes/exeC2/)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC2/disc1.yaml --profiles build/lanes/exeC2/disc1_build_profiles.json --deep --only func_800494AC
  -> disc1_preflight: PASS (deep, 2080 c / 192 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC2/disc1.yaml --profiles build/lanes/exeC2/disc1_build_profiles.json func_800494AC
  -> LEAF_STRONG=EXACT func_800494AC span=0xC1C vram=0x800494AC compiled=0xC20 failures=[none]
era_link_check.py src/func_800494AC.c 0x800494AC 0xC1C -O2 -G8  (+FORCE env)  -> LINK_EXACT
profile necessity (manual; profile_necessity.py has no --config):
  -O2 -G0 (default) -> 687 mismatches; -O2 -G8 without FORCE -> 557; assigned profile -> LINK_EXACT
```

## Levers (on top of the decompA/exeC 13-word draft)

1. **Assign the long-lived callee-saved copy AFTER the consuming call** (6 words):
   `func_8006322C(N, s0 = func_80062D2C(...), s0)` emits `move s0,v0` before `li a0,N`;
   retail has `li a0,N` first. Fix: `u = func_80062D2C(...); t = func_8006322C(N, u, u);
   s0 = u; ...` (fresh block-local `u`). Sites 6 and 0x29 close with that alone (13 -> 9);
   site 5 needs `s1` declared `register ... asm("$17")` so `u` colours into `$s1`
   (unpinned `u` took `$s0` and added a copy: +1 word) (9 -> 7).
2. **Sum residual (7 words): empty asm barriers on three of the four loads + one pin.**
   `int A_=D_800A1888, B_=..., C_=..., D_=...;` then
   `asm("" : "=r"(A_) : "0"(A_));` for A_, B_, C_, then `if (A_ + B_ + C_ + D_ != 0)`.
   The barriers give retail's schedule a,b,c,(a+b),d,+c,+d with d reusing b's register (the
   earlier 120-form pure-expression sweep could not: every left chain got a load-delay nop).
   Remaining register colouring (sum in `$v1`, c in `$v0`) fixed by pinning
   `register int A_ asm("$3")` and `register int C_ asm("$2")`; pinning all four breaks it
   (544), pinning only A_ gives 9, pure declaration reorders 12.
