# func_80040210

- VRAM: `0x80040210`, size `0x144`
- Profile: `era_o2_g0`
- Source: `src/func_80040210.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_80040210
disc1_preflight: PASS (deep, 1666 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_80040210
LEAF_STRONG=EXACT func_80040210 span=0x144 vram=0x80040210 compiled=0x150 failures=[none]
```

## Load-bearing spelling levers

Straight transcription: clear 0x44-byte header, split seconds into h/m/s with `secs / 3600`, `secs / 60 - h * 60`, `secs - (secs / 60) * 60`; if/else on D_800A1704 with the func_8004006C call duplicated in each arm (retail tail-merges it).

Save-slot header builder (play time h/m/s + location ids). First-try LINK_EXACT.
