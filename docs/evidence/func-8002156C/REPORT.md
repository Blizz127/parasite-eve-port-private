# func_8002156C

- VRAM `0x8002156C`, file offset `0x11D6C`, size `0x178` (94 words)
- Profile: `era_o2_g8_force_d8009d20c_d8009d254_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D20C,D_8009D254 (new profile)
- Source: `src/func_8002156C.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8002156C
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8002156C
LEAF_STRONG=EXACT func_8002156C span=0x178 vram=0x8002156C
```

## Load-bearing levers

Target-list builder feeding the func_800216E4 quicksort (previously parked at 11 words). `continue` ladder over the `D_8009D20C` actor list with `-G8` for the signed-char count `D_8009CE44` (0xD4). **Register pins close the residual**: block-local `register Node *a asm("$5") = D_8009D254` and `register Info *inf asm("$6") = p->f0` for the two skip tests, and for the terminator clear `register int n asm("$2") = D_8009CE44; register int o asm("$3") = n * 12;` with a byte-offset store (a plain `D_8009E000[n].f0 = 0` swaps `$v0`/`$v1`).
