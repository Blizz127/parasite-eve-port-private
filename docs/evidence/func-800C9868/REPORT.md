# func_800C9868 — matching C leaf

- VRAM: `0x800C9868`  size: `0x100`
- File offset: `0XBA068`
- Profile: `era_o2_g0`
- Source: `src/func_800C9868.c`
- YAML: YAML carve [0xB9B98 asm][0xBA068 c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800C9868
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800C9868
LEAF_STRONG=EXACT func_800C9868 span=0x100 vram=0x800C9868 compiled=0x100 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- exact twin of func_800C8870 with base D_800E2328
