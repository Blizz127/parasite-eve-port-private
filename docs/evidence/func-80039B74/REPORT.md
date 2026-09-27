# func_80039B74

- VRAM: `0x80039B74`, size `0x1B0`
- Profile: `era_o2_g0`
- Source: `src/func_80039B74.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80039B74
disc1_preflight: PASS (deep, 1675 c / 339 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80039B74
LEAF_STRONG=EXACT func_80039B74 span=0x1B0 vram=0x80039B74 compiled=0x1B0 failures=[none]
```

## Load-bearing spelling levers

Obj struct with `Ent ent[2]` (x,y,z shorts, idx, flags bytes) at +0xA0 walked by an `int i` loop over `obj->ent[i]` (a byte-pointer walk grows a +0xA4 giv and a 4th saved reg); scratchpad SVECTOR table `sv = (SVec *)0x1F800000` kept in $s2; call arg spelled `(SVec *)((unsigned int)sv | (idx << 3))` (retail ORs); per-axis set-else-add on flag bits.

Applies per-joint position overrides/offsets into the scratchpad SVECTOR table then rebuilds the joint matrix (func_80079754).
