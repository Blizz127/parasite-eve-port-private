# func_800830DC — matching C leaf

- VRAM `0x800830DC`, ROM offset `0X738DC`, span `0x1D8`
- Source: `src/func_800830DC.c`; YAML row `[0X738DC, c, func_800830DC]`
- Build profile: `era_o2_g0_fill_epilogue_delay_slot`

## Levers
libpad SIO byte exchange; 2.7.2 + patch 3; guarded poll loops; D_8009B784 read into a local q before the volatile halfword store (retail hoists the pointer load); single shared return via goto out (reorg steals move v0,s1 into the j slot); block-local int w = byte so the >>4 test emits sra with lbu in v0 / 8 in v1

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800830DC
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800830DC
  -> LEAF_STRONG=EXACT func_800830DC span=0x1D8 failures=[none]
```
ladder 52 -> 23 (q hoist) -> 4 (shared return) -> 1 (inline shift) -> 0 (block-local int w)
