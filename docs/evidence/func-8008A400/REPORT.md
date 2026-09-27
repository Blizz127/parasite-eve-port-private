# func_8008A400 — matching C leaf

- VRAM `0x8008A400`, ROM offset `0X7AC00`, span `0x350`
- Source: `src/func_8008A400.c`; YAML row `[0X7AC00, c, func_8008A400]`
- Build profile: `era_o2_g0`

## Levers
Akao nested-track stop (recursive): 12 tracks at D_800BC000 stride 0x11C gated by D_800BCD50 with bit 0x1000<<i; four selection modes (a1 & 0x0FFFFFFF track mask / a1 < 0 two child tracks by recursion / a1 & 0x40000000 highest-priority / else by id at +0x28). Levers: first parameter is a full word masked in the body (`id = a0 & 0xFFFF`; an `unsigned short` parameter makes the recursive call pass lhu), loop increments in the for header after i++, and every record field addressed off `base` (a separate `p = base + 0x38` walker rotates mask/i/p through $s0-$s2: 63 words; with base-only addressing loop.c creates the +0x38 giv itself, matching retail's $s2). Plain 2.7.2 inside the 2.8.1 band.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008A400
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008A400
  -> LEAF_STRONG=EXACT func_8008A400 span=0x350 failures=[none]
```

