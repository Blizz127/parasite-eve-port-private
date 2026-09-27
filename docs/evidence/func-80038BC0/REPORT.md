# func_80038BC0

- VRAM: `0x80038BC0`, size `0x124`
- Profile: `era_o2_g0`
- Source: `src/func_80038BC0.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80038BC0
disc1_preflight: PASS (deep, 1661 c / 343 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80038BC0
LEAF_STRONG=EXACT func_80038BC0 span=0x124 vram=0x80038BC0 compiled=0x130 failures=[none]
```

## Load-bearing spelling levers

switch on the `unsigned char` mode (case 0/1/2/default gives retail's `beq 1`/`slti 2` tree); `p = base + 1` with `(p + i)[3]` base-first indexing; found-branch `sel = i; i = p[2];` (retail reloads the count byte); second loop returns `i` directly from an `unsigned char` function, else 0xFF.

Selects a slot of the D_80091A28 descriptor whose type byte matches the mode-derived kind, then maps it through the 0x1C table. First-try LINK_EXACT.
