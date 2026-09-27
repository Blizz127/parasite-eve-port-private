# func_800347B4

- VRAM: `0x800347B4`, size `0x1A8`
- Profile: `era_o2_g8_force_d8009d278_d8009d254_d8009cddc_d8009cdd8_absolute`
- Source: `src/func_800347B4.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800347B4
disc1_preflight: PASS (deep, 1674 c / 338 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800347B4
LEAF_STRONG=EXACT func_800347B4 span=0x1A8 vram=0x800347B4 compiled=0x1B0 failures=[none]
```

## Load-bearing spelling levers

NEW profile: `-O2 -G8` + MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D254,D_8009CDDC,D_8009CDD8 (D_8009D290 stays gp-relative at 0x520($gp), the other four in-window words are absolute in retail); 0x800B0xxx pointer tables as incomplete arrays; `unsigned int n` for the `multu`-by-magic divide; 16-byte unused local array reproduces the 0x38 frame; D_8009D290 re-read for the size terms.

Grow-in effect: scale/size ramp over the first 10 frames then func_8007041C draws; spawns func_8006DE80(0x457) on frame 0.
