# func_8001CAB0

- VRAM `0x8001CAB0`, file offset `0xD2B0`, size `0xF0` (60 words)
- Profile: `era_o2_g0`; maspsx env: none
- Source: `src/func_8001CAB0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8001CAB0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8001CAB0
LEAF_STRONG=EXACT func_8001CAB0 span=0xF0 vram=0x8001CAB0
```

## Load-bearing levers

Point-in-polygon crossing test (previously parked at 10 words). Levers: `do`/`while` over `unsigned short i`; the crossing condition written as `(!c && z < zp) || (c && !(z < zp))`; `zp = zc` at the **top** of the loop body; one `c` local reused for the z-test, the x-test and the edge product; a separate `short r` compare result; `i = 0` before `inside = 0`; and finally **`register int inside asm("$11")`**, which resolves the last `$t3`/`$t4` priority tie with `zc` (pinning `zc` to `$12` works equally).
