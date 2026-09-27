# func_80019170

- VRAM `0x80019170`, file offset `0x9970`, size `0xF0` (60 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_80019170.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80019170
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80019170
LEAF_STRONG=EXACT func_80019170 span=0xF0 vram=0x80019170
```

## Load-bearing levers

Same `goto fail`/`goto found` layout and block-local `Node *q` null test as func_80019F04; found tail calls `func_8003E0A4(&D_8009D2F0->f1B4, &p->f1B4, *arg0->f8)` (short arg) then stores `p` to `D_8009D2F0->f18C`.
