# func_8007A4D0 — matching C leaf

- VRAM `0x8007A4D0`, ROM offset `0X6ACD0`, span `0x13C`
- Source: `src/func_8007A4D0.c`; YAML row `[0X6ACD0, c, func_8007A4D0]`
- Build profile: `era_o2_g0_fill_epilogue_delay_slot`

## Levers
PsyQ CdControl (three-argument sibling of func_8007A60C = CdControlF). Same source shape: invariant locals `one = 1` / `neg1 = -1` before the retry loop, u_char command parameter, `c = a0`, register pins i=$16 / saved=$21 and the `register int *b asm("$3") = D_8009AF2C; slot = &b[c];` base. Retail keeps the `one` compare constant in $t0 inside the loop here (no beq-slot store), so only maspsx patch 3 is needed. First adaptation exact (the literal 1 instead of `one`: 2 words).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8007A4D0
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8007A4D0
  -> LEAF_STRONG=EXACT func_8007A4D0 span=0x13C failures=[none]
```

