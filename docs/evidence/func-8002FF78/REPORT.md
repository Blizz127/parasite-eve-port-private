# func_8002FF78

- VRAM: `0x8002FF78`, size `0x194`
- Profile: `era_o2_g0`
- Source: `src/func_8002FF78.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_8002FF78
disc1_preflight: PASS (deep, 1671 c / 339 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_8002FF78
LEAF_STRONG=EXACT func_8002FF78 span=0x194 vram=0x8002FF78 compiled=0x1A0 failures=[none]
```

## Load-bearing spelling levers

`unsigned char` id switch over 18 sparse cases (cc1's binary compare tree matches retail); `unsigned char **D_8009D254` double pointer; per-case field stores of int/short/byte width.

Field-VM variable setter: writes v into the current state object's field selected by id (255 = D_800942EC). First-try LINK_EXACT.
