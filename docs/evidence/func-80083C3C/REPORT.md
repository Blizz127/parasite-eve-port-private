# func_80083C3C

- VRAM `0x80083C3C`, size `0xC8` (50 words), file offset `0x7443C`
- Profile: `era_o2_g0`, flags `-O2 -G0`
- Source: `src/func_80083C3C.c`

## Verification (lane-5 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json --deep --only func_80083C3C
  -> disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config <S>/lanes/lane5/disc1.yaml --profiles <S>/lanes/lane5/disc1_build_profiles.json func_80083C3C
  -> LEAF_STRONG=EXACT
python3 tools/analysis/era_link_check.py src/func_80083C3C.c 0x80083C3C 0xC8 -O2 -G0
  -> word mismatches=0, LINK_EXACT
```

## Load-bearing levers

outer loop as guarded do/while (a `for` seeds the k*5 giv from k's register: `addu $t2,$t0,$zero` vs retail `addu $t2,$zero,$zero`); index spelled `k * 5 + 2` so strength reduction owns the stride.
