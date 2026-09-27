# func_8006D078 — matched (lane 4, session 2026-09-22)

- VRAM `0x8006D078`, size `0x1D4`, source `src/func_8006D078.c`.
- Profile: `era_o2_g8_dispatch_80011458_force_d800b0e64_absolute` (-O2 -G8, env MASPSX_DISPATCH_FOLD=jtbl_80011458 MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800B0E64).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_8006D078
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_8006D078
disc1_preflight: PASS (deep, 1673 c / 337 asm / 2 rodata)
LEAF_STRONG=EXACT func_8006D078 span=0x1D4 vram=0x8006D078 compiled=0x1E0 failures=[none]
```

## Load-bearing levers

goto-based dispatch loop (for(;;) lets loop.c hoist constants: 117 words); `if (s[0x10] < 2) goto next;` into case 0x29`s `next: state = 0x2A` tail; entry address as `(D_8009CDCC << 3) + (int)tab` (index-first addu, no pins needed); D_800B0E64 forced absolute under -G8 so D_8009CDCC stays gp-relative (0x5C($gp)).
