# func_80062090 — matching C leaf

- VRAM `0x80062090`, ROM offset `0X52890`, span `0x220`
- Source: `src/func_80062090.c`; YAML row `[0X52890, c, func_80062090]`
- Build profile: `era_o2_g8`

## Levers
Screen-fade TILE + DR_MODE emitter; sibling of func_8005ED18 (same D_8009D100/D_8009D104 prim pool, gp-relative globals at 0x390..0x3B8($gp) so -G8, Tag bitfield addPrim). Levers: (1) each branch arm stores the blink level straight into `p->b0` (retail's shared `sb 6(s0)` is the cross-jumped tail of the two arms), then `p->r0 = p->g0 = p->b0 = p->b0;` gives the lbu reload and the 6/5/4 byte stores (computing into a local first lets cse drop the reload: 90 words); (2) the colour-word update in the non-blink arm is two statements (`&= 0xFF000000` then `|= D_8009D114 & 0xFFFFFF`: two stores); (3) `int` parameters (short params reorder the prologue param copies: 4 words); (4) x/y written x then y.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80062090
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80062090
  -> LEAF_STRONG=EXACT func_80062090 span=0x220 failures=[none]
```

