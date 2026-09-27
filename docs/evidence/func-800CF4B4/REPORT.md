# func_800CF4B4 — matching C leaf

- VRAM: `0x800CF4B4`  size: `0xFC`
- File offset: `0XBFCB4`
- Profile: `era_o2_g0`
- Source: `src/func_800CF4B4.c`
- YAML: YAML carve [0xBF620 asm][0xBFCB4 c][0xBFDB0 asm]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800CF4B4
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800CF4B4
LEAF_STRONG=EXACT func_800CF4B4 span=0xFC vram=0x800CF4B4 compiled=0x100 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- RECT local on the stack
- buf = D_800E21A8 pointer local (keeps the base in $t0 and the +2 displacement on the store; the bare symbol gets a separate base+2 register)
- unsigned short i; a1 % 15 magic div
