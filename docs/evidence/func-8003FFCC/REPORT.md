# func_8003FFCC

- VRAM: `0x8003FFCC`, size `0xA0`
- Profile: `era_o2_g0`
- Source: `src/func_8003FFCC.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_8003FFCC
disc1_preflight: PASS (deep, 1655 c / 343 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_8003FFCC
LEAF_STRONG=EXACT func_8003FFCC span=0xA0 vram=0x8003FFCC compiled=0xA0 failures=[none]
```

## Load-bearing spelling levers

Two-level struct typing (CardRec 0x418 = sel/status/pad + CardEntry[15] of 0x44 at +0x1C); outer for with `c < end && found == 0`; inner found = (state==1)&&(f29!=0) then break.

Scans the two card records for status 0xF with an entry in state 1 and nonzero +0x29 byte. First-try LINK_EXACT.
