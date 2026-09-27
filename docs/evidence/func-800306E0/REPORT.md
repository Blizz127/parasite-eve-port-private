# func_800306E0

- VRAM: `0x800306E0`, size `0x1B4`
- Profile: `era_o2_g0`
- Source: `src/func_800306E0.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800306E0
disc1_preflight: PASS (deep, 1676 c / 339 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800306E0
LEAF_STRONG=EXACT func_800306E0 span=0x1B4 vram=0x800306E0 compiled=0x1C0 failures=[none]
```

## Load-bearing spelling levers

if/else-if ladder on the signed-char level difference (>0, ==0, ==-1, < -1) with the `rand() % 100 * k < s` test and func_80025BD8 call written in each arm (cc1 tail-merges the shared multiply chain exactly as retail); `register int s asm("$16")` pin so the `* 3` builds directly in $s0 (unpinned uses a $v1 temp, 4 words).

Level-gap proc roll: chance scales with func_80052514() vs rand()%100; triggers func_80025BD8 on success.
