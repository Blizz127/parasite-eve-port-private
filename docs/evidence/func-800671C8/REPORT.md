# func_800671C8 — matched (lane 4, session 2026-09-22)

- VRAM `0x800671C8`, size `0xCC`, source `src/func_800671C8.c`.
- Profile: `era_o2_g0` (-O2 -G0 (default)).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_800671C8
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_800671C8
disc1_preflight: PASS (deep, 1665 c / 338 asm / 2 rodata)
LEAF_STRONG=EXACT func_800671C8 span=0xCC vram=0x800671C8 compiled=0xD0 failures=[none]
```

## Load-bearing levers

Record typed as a struct (u32 w0, w4, u16 x,y, short cx,cy, minx,maxx,miny,maxy); `short x, y` locals with if/else-if clamps (they also produce retail`s store-less 0x20 frame); depth as int `dz + (w0 >> 20)` masked 0xFFF into bits 8..19. First attempt LINK_EXACT.
