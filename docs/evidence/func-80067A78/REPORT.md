# func_80067A78 — matched (lane 4, session 2026-09-22)

- VRAM `0x80067A78`, size `0xC8`, source `src/func_80067A78.c`.
- Profile: `era_o2_g0` (-O2 -G0 (default)).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80067A78
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80067A78
disc1_preflight: PASS (deep, 1664 c / 337 asm / 2 rodata)
LEAF_STRONG=EXACT func_80067A78 span=0xC8 vram=0x80067A78 compiled=0xD0 failures=[none]
```

## Load-bearing levers

`(short)(D - 0xA0)` casts stop reassociation; `b += *(int *)(b + 0x14)` then indexed `p = b + i * 0x38` inside the loop reproduces retail`s `addu a0,a0,v0` (in the beqz slot) + `move s0,a0` + strength-reduced walk; a walking-pointer for-loop costs 36-42 words.
