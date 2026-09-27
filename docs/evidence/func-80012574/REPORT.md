# func_80012574

- VRAM `0x80012574`, file offset `0x2D74`, size `0x6C` (27 words)
- Profile: `era_o2_g8`; maspsx env: none
- Source: `src/func_80012574.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80012574
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80012574
LEAF_STRONG=EXACT func_80012574 span=0x6C vram=0x80012574
```

## Load-bearing levers

`-G8` (D_8009CE04 at 0x94($gp)); load `v = a0[0]` into a named local before the gp store (store-first order costs 3 words). No dummy frame local: cc1 emits the retail 8-byte frame on its own; adding one makes it 0x10.
