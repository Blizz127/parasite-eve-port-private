# func_800727B4 — matched (lane 4, session 2026-09-22)

- VRAM `0x800727B4`, size `0x19C`, source `src/func_800727B4.c`.
- Profile: `era_o2_g0_fill_call_delay_slot_fill_epilogue_delay_slot` (-O2 -G0, env MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_800727B4
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_800727B4
disc1_preflight: PASS (deep, 1670 c / 338 asm / 2 rodata)
LEAF_STRONG=EXACT func_800727B4 span=0x19C vram=0x800727B4 compiled=0x1A0 failures=[none]
```

## Load-bearing levers

Compiler is 2.7.2 even though this is inside the 2.8.1 band (2.8.1: 28 words). Patch 8 splits `la $a1` across the jal; patch 3 fills the epilogue slot. Locals `n = *(u32 *)0x154 / sizeof(Dev)` and `base` with `e < base + n` reproduce the $v1 bound plus preheader copy to $s1; `found = 1; goto done;` / `found = 0; done:` keeps the flag out of the loop (a pre-initialised found costs an extra saved register, 68 words).
