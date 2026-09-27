# func_800C6C18 — matching C leaf

- VRAM: `0x800C6C18`  size: `0xC8`
- File offset: `0XB7418`
- Profile: `era_o2_g0_no_cse_skip_blocks (NEW: -O2 -G0 -fno-cse-skip-blocks)`
- Source: `src/func_800C6C18.c`
- YAML: YAML [0xB7418, c, func_800C6C18]

## Verification (lane 6 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json --deep --only func_800C6C18
disc1_preflight: PASS (deep, 1671 c / 335 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane6/disc1.yaml --profiles $S/lanes/lane6/disc1_build_profiles.json func_800C6C18
LEAF_STRONG=EXACT func_800C6C18 span=0xC8 vram=0x800C6C18 compiled=0xD0 failures=[none]
```

(`LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` was required on this host.)

## Load-bearing levers

- t = call(); r = 1; if (t == 3) r = ...==2 puts li $s1,1 in the bne delay slot
- -fno-cse-skip-blocks stops cc1 CSEing the constant 3 into callee-saved $s2 across the two calls (default -O2: 11-word diff; -O1: prologue order differs)
- distinct pointer locals p/q for the two RMWs

Profile necessity: under plain `-O2 -G0` cc1 CSEs the constant 3 into callee-saved `$s2` (11-word diff); `-O1 -G0` differs in prologue order.
