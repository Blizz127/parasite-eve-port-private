# func_800DEFFC — matching C leaf

- VRAM `0x800DEFFC`, ROM offset `0XCF7FC`, span `0x6B0`
- Source: `src/func_800DEFFC.c`; YAML row `[0XCF7FC, c, func_800DEFFC]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback with a nested state machine (outer a0, inner a1[8]); three func_800CEE20 calls in case 2 are cross-jumped by cc1 into one `jal` with the per-path `addiu v0,t0,0x68/0x60` in the `j` slot. Levers: (1) 8-byte unaligned template copy `typedef struct { unsigned char b[8]; } B8;` (an int struct gives lw/sw and a 72-byte frame); (2) case 2/0 reads `a1[9]` directly in each condition (a local T with a reassignment loses retail's `move v1,a0` copy: 212 words); (3) `unsigned short idx` for the D_800F336C weapon index (int idx swaps $v0/$v1 in the two w-lookups: 12 words); (4) `p[0] += (rand() & 0x7F) - 0x40` compound jitter (DD380 lever).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800DEFFC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800DEFFC
  -> LEAF_STRONG=EXACT func_800DEFFC span=0x6B0 failures=[none]
```

