# func_80072334 — matched (lane 4, session 2026-09-22)

- VRAM `0x80072334`, size `0x6C`, source `src/func_80072334.c`.
- Profile: `era_o2_g0_cc1_281_return_via_epilogue_jump` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1 MASPSX_RETURN_VIA_EPILOGUE_JUMP=1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_80072334
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_80072334
disc1_preflight: PASS (deep, 1668 c / 337 asm / 2 rodata)
LEAF_STRONG=EXACT func_80072334 span=0x6C vram=0x80072334 compiled=0x70 failures=[none]
```

## Load-bearing levers

Band compiler 2.8.1 (2.7.2 needs -fno-strength-reduce and still misses 4 words). maspsx patch 12 rewrites the mid-function `j $31` into a jump to the final return (1 word otherwise). Span 0x6C; the trailing nop at 0x62BA0 is its own asm row.
