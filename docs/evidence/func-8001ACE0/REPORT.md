# func_8001ACE0

- VRAM `0x8001ACE0`, file offset `0xB4E0`, size `0x160` (88 words)
- Profile: `era_o2_g8`; maspsx env: none
- Source: `src/func_8001ACE0.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_8001ACE0
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_8001ACE0
LEAF_STRONG=EXACT func_8001ACE0 span=0x160 vram=0x8001ACE0
```

## Load-bearing levers

All globals gp-relative (`-G8`): `D_8009D1D8` (0x468, 12-byte coefficient rows), `D_8009D1FC` (0x48C, header with the record table at +0x1C), `D_8009CE08` (0x98, short-pointer table). Two record strides as typed arrays (22-byte `EntA` → retail `*3,<<2,-idx,<<1`; 28-byte `EntB`); both arms end in the same `o->f2C = …; func_8001C614(e, hi(f28), hi(f30))` so cc1 cross-jumps the call with the `f2C` store in its delay slot.
