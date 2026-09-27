# func_8008CBA8 — matching C leaf

- VRAM `0x8008CBA8`, ROM offset `0X7D3A8`, span `0x3C8`
- Source: `src/func_8008CBA8.c`; YAML row `[0X7D3A8, c, func_8008CBA8]`
- Build profile: `era_o2_g0`

## Levers
sparse switch on unsigned cmd; record fields written through *(int *)((char *)ptr + off) (non-struct stores keep global loads ordered); per-call register copies r/r2 of the out-param record pointer (no stack reloads); case 0x10 writes via the stack slot directly (retail reloads it) plus p=&D_800BCD80 held in s2; counter read into a temp n, ret = n last (retail's move s1)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008CBA8
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008CBA8
  -> LEAF_STRONG=EXACT func_8008CBA8 span=0x3C8 failures=[none]
```
Akao_EnqueueStagedCommand; 2.8.1 band but 2.7.2 exact
