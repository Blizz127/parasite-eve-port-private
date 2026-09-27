# func_8007A60C — matching C leaf

- VRAM `0x8007A60C`, ROM offset `0X6AE0C`, span `0x134`
- Source: `src/func_8007A60C.c`; YAML row `[0X6AE0C, c, func_8007A60C]`
- Build profile: `era_o2_g0_fill_branch_macro_split_epilogue`

## Levers
PsyQ CdControlF shape (retry loop over func_8007B558 = CD_cw). The 'hoisted invariant' residual was spelling: loop constants as explicit locals initialised before the loop (`one = 1`, `neg1 = -1`, compared as `c != one` / `--i != neg1`) give retail's $s8/$s7. Other levers: `unsigned char` command parameter (the andi in the prologue, and a1 copied before a0), `c = a0` local for the compare/index while the final call passes a0 (recomputed andi), `register` pins i=$16 and saved=$20 for retail's callee-saved order, the slot address built from `register int *b asm("$3") = D_8009AF2C; slot = &b[c];` (base la before the index sll), 2.7.2 with maspsx patch 10 (absolute `sw D_8009AFB4` in the beq slot) + patch 3 (epilogue fill). Ladder: 72 -> 66 (locals) -> 22 (patch 10) -> 11 (pins) -> 7 (u_char param) -> 0 (base pin).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8007A60C
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8007A60C
  -> LEAF_STRONG=EXACT func_8007A60C span=0x134 failures=[none]
```

