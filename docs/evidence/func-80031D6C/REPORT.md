# func_80031D6C

- VRAM: `0x80031D6C`, size `0xFC`
- Profile: `era_o2_g0`
- Source: `src/func_80031D6C.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80031D6C
disc1_preflight: PASS (deep, 1657 c / 343 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80031D6C
LEAF_STRONG=EXACT func_80031D6C span=0xFC vram=0x80031D6C compiled=0x100 failures=[none]
```

## Load-bearing spelling levers

Tile[][3] typing (16-byte prim, x/y/w/h shorts at +8); `unsigned char i` loop; every store spelled `(D_8009E358[D_8009CDDC] + i)->f` (pointer-arithmetic form keeps the base in $s1 register-addressed; `&arr[k][i]` or a `t` local folds the symbol into the displacement); `(signed char)a0` cast inside the loop.

Three TILE prims per OT slot offset from the first; func_80077AC4 = addprim.
