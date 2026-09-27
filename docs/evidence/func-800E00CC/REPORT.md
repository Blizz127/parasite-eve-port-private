# func_800E00CC — matching C leaf

- VRAM: `0x800E00CC`  size: `0xF0`
- File offset: `0XD08CC`
- Profile: `era_o2_g0`
- Source: `src/func_800E00CC.c`
- YAML: YAML [0xD08CC, c]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800E00CC
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800E00CC
LEAF_STRONG=EXACT func_800E00CC span=0xF0 vram=0x800E00CC compiled=0xF0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- NEW: a real 20-byte struct (Rec) for the record pointer: MEM_IN_STRUCT_P stores let sched1 hoist the scalar global D_800E21A4 load above the fE/fC/f8 stores (byte-pointer casts keep it below; 2.8.1 does not help)
- for (i = 0; p->f0; i++) { if (i > N) break; p--; } + re-test reload
- unsigned char stack params 5-7
