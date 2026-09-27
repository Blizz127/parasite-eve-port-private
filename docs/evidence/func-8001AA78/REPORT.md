# func_8001AA78

- VRAM `0x8001AA78`, file offset `0xB278`, size `0x268` (154 words)
- Profile: `era_o2_g8`; maspsx env: none
- Source: `src/func_8001AA78.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8001AA78
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8001AA78
LEAF_STRONG=EXACT func_8001AA78 span=0x268 vram=0x8001AA78
```

## Load-bearing levers

Floor-polygon search over the `D_8009CE08` group table (all globals gp-relative under `-G8`), using the EntA/EntB/Coef typing from func_8001ACE0. Levers: (1) real `for` loops (they give retail's 0x50 frame); (2) the group height is **`int h = ((short *)g)[0]`**: a `short h` lets loop.c hoist `h << 16` out of the inner loop (130 words); (3) the table load goes through a block-local **`register unsigned short **tbl asm("$2") = D_8009CE08`** pin, so the `i*4` index lands in `$v1` as in retail (7 words otherwise; `i[tbl]`, pointer and offset-hoist spellings do not help).
