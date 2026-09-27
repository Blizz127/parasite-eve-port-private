# func_8005DAFC — offset-table entry getter

- VRAM `0x8005DAFC`, size `0x48` (18 words), ROM `0x4E2FC`, profile `era_o2_g0` (default).
- Source: `src/func_8005DAFC.c`. Twin of `func_8005DB44` (same `D_800A80xx` pool block).

## Shape (retail, `tools/analysis/pe_dis.sh 0x8005DAFC 0x48`)
`base = *D_800A802C + &D_800A8028`; if `a0 < *(u16 *)base` return
`base + ((short *)base)[a0 + 1]`, else 0.

## Levers
1. Symbol pointer local `int *p = &D_800A802C; ... p -= 1;` gives the in-place
   `addiu v0,v0,-4` after the load (as in `func_8005DB44`).
2. Nonzero case first (`if (a0 < n) return ...; return 0;`): the `>=`/early-0
   polarity costs 8 words.
3. Operand order `(a0 << 1) + (int)base` gives retail's `addu v0,v0,v1`;
   `base + (a0 << 1)` costs 1 word.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/analysis/era_link_check.py src/func_8005DAFC.c 0x8005DAFC 0x48 -O2 -G0
  -> LINK_EXACT (word mismatches=0)
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005DAFC
  -> disc1_preflight: PASS (deep, 1835 c / 303 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config ... func_8005DAFC
  -> LEAF_STRONG=EXACT func_8005DAFC span=0x48 vram=0x8005DAFC compiled=0x50 failures=[none]
```
