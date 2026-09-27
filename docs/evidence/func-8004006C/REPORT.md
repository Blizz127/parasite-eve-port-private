# func_8004006C

- VRAM: `0x8004006C`, size `0x1A4`
- Profile: `era_o2_g0`
- Source: `src/func_8004006C.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_8004006C
disc1_preflight: PASS (deep, 1673 c / 337 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_8004006C
LEAF_STRONG=EXACT func_8004006C span=0x1A4 vram=0x8004006C compiled=0x1B0 failures=[none]
```

## Load-bearing spelling levers

Natural mini-printf: `int *arg = D_800A1708`, `while (*fmt)` with `*fmt++ == '%'` test, switch on the conversion byte ('d' two SJIS digits, 'D' one digit, 's' string copy guarded by NULL), literal byte copied as `fmt[-1]`; digit glyph = value % 10 + 0x824F stored big-endian through `unsigned int w`.

Save-header text formatter writing full-width SJIS digits. First-try LINK_EXACT.
