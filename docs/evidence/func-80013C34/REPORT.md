# func_80013C34

- VRAM `0x80013C34`, file offset `0x4434`, size `0x250` (148 words)
- Profile: `era_o2_g8_force_d8009d2f0_d8009d254_d8009d20c_absolute`; maspsx env: MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254,D_8009D20C (new profile)
- Source: `src/func_80013C34.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80013C34
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80013C34
LEAF_STRONG=EXACT func_80013C34 span=0x250 vram=0x80013C34
```

## Load-bearing levers

Crashed-session draft verified unchanged. `register T x asm("$2")` pins + zero-code `__asm__ __volatile__("" : "=r"(x) : "0"(x))` barriers (accepted precedent) keep the `$v0` homes; three absolute symbols under `-G8`.
