# func_80036F7C

- VRAM: `0x80036F7C`, size `0x110`
- Profile: `era_o2_g0`
- Source: `src/func_80036F7C.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80036F7C
disc1_preflight: PASS (deep, 1660 c / 343 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80036F7C
LEAF_STRONG=EXACT func_80036F7C span=0x110 vram=0x80036F7C compiled=0x110 failures=[none]
```

## Load-bearing spelling levers

Ramp[4] {flags,cur,target} typing; nested ifs (`if (f & 1) { if (!(f & 4)) { if (f & 2) { if (f & 5) ...` — a flat `!(f&1) || (f&4)` lets cc1 fold the redundant `f & 5` tests); cur STORES spelled `(D_800A76A0 + i)->cur` while cur/target LOADS use `D_800A76A0[i].cur` (reproduces retail's separate base/base+4/base+8 induction pointers); re-read `D_800A76A0[i].cur` rather than caching it in a local (a local swaps $3/$4); `signed char` return of func_8006EC08.

Four volume/fade ramps stepping cur toward target by 1; sets bit 2 when reached.
