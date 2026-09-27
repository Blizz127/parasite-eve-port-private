# func_80019F04

- VRAM `0x80019F04`, file offset `0xA704`, size `0xDC` (55 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80019F04.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80019F04
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80019F04
LEAF_STRONG=EXACT func_80019F04 span=0xDC vram=0x80019F04
```

## Load-bearing levers

Explicit `goto fail` / `goto found` layout (the retail `return 1` stub sits between the walk exit and the found tail); the `D_8009D254` arm needs a block-local `Node *q` tested **before** `p = q` (testing `p` directly, or assigning before the test, costs 4 words: cc1 loads straight into `$a2` and threads the branch to the epilogue). Walk is the func_800181CC `goto loop` form.
