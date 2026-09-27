# func_80075CE8 — matched (lane 4, session 2026-09-22)

- VRAM `0x80075CE8`, size `0x1F8`, source `src/func_80075CE8.c`.
- Profile: `era_o2_g0_cc1_281` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80075CE8
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80075CE8
disc1_preflight: PASS (deep, 1672 c / 337 asm / 2 rodata)
LEAF_STRONG=EXACT func_80075CE8 span=0x1F8 vram=0x80075CE8 compiled=0x200 failures=[none]
```

## Load-bearing levers

Band compiler 2.8.1 -mno-split-addresses (2.7.2: 66/94 words). Draft recovered from the crashed session; `register int t asm("$8")` packet index and `register int lenv asm("$2")` pins; local 8-byte RECT copy read back as two words; clamp via nested if on short views of the unsigned w/h.
