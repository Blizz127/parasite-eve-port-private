# func_8008900C — matching C leaf

- VRAM `0x8008900C`, ROM offset `0X7980C`, span `0x20C`
- Source: `src/func_8008900C.c`; YAML row `[0X7980C, c, func_8008900C]`
- Build profile: `era_o2_g0`

## Levers
Akao voice key-on/slot assignment loop over a track bitmask (Voice records stride 0x11C). cc1 2.7.2 does hoist the loop constants 1 / 0x1FF93 / 24 into $s7/$s6/$s8 here once the body is right. Levers: (1) the voice record as a real struct (Voice { f38, fF0, fF4, h118, h11A }) walked with `a0++`, so loop.c builds the +0x38 giv in $s0 and the record stores are MEM_IN_STRUCT (a separate p = a0 + 0x38 walker: 91 words; byte offsets off a0: 59); (2) the free-slot scan over D_800B002C uses a plain `short *q` stepped by 4 (an in-struct VSlot store reorders past the D_8009D2C4 load; indexing D_800B002C[j] makes loop.c recompute the giv after `j = 24`); (3) `j = 24` as the found-exit and `j++; if (j == 24)` exhaustion arm, as retail. Plain 2.7.2 inside the 2.8.1 band.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008900C
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008900C
  -> LEAF_STRONG=EXACT func_8008900C span=0x20C failures=[none]
```

