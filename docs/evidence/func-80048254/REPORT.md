# func_80048254 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x80048254`, file offset `0x38A54`, size `0x400`; yaml `- [0x38A54, c, func_80048254]` replaces
  `- [0x38A54, asm]` exactly (next row `[0x38E54, c, func_80048654]`).
- Profile `era_o2_g8_force_d800a1888_plus3_absolute` (existing: `-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800A1888,D_800A188C,D_800A1890,D_800A1894`).
- Equipment-screen side handler: `f24` switch (6/0x1B side 0, 0xB/0x1C side 1), >=2-positive-stat test over
  `D_800A1888..94`, first-nonzero search into `D_8009CF2C`, and the window/handler constructions.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_80048254
  disc1_preflight: PASS (deep, 2044 c / 215 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_80048254
  LEAF_STRONG=EXACT func_80048254 span=0x400 vram=0x80048254 compiled=0x400 failures=[none]
```
Necessity (manual; `profile_necessity.py` has no `--config`): plain `-O2 -G8` → 181 words; `-O2 -G0` → 206 words +
size change. The force list is load-bearing for `D_800A188C/90/94`; `D_800A1888` is declared `extern int D_800A1888[]`
(unsized, not small data) so the entry for it is a no-op (same bytes with or without it).

## Levers (ladder)

decompA draft: 25 → 22 → 16 words (unsized `D_800A1888[]` keeps the first peeled load through an `la` base; index-form
empty-body `for (i = 0; i < 4 && D_800A1888[i] == 0; i++) {}`). Residual 16 words were a pure `$a0`/`$v1` swap of the
loop counter `i` and the strength-reduced pointer.
**Closing lever: reuse `i` for the later attribute value** (`i = (func_8005332C(...) + id)[0x15] & 0x1F;` and
`(unsigned int)(i - 8) >= 3`, dropping the separate `a` local). Retail keeps both values in `$a0`; one pseudo with the
longer live range changes global-alloc priority so `i` wins `$a0` and the giv takes `$v1` → 0 words. Decl-order swap
and `unsigned int i` stay at 16.
