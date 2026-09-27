# func_800742B8 — matched (lane 4, session 2026-09-22)

- VRAM `0x800742B8`, size `0x78`, source `src/func_800742B8.c`.
- Profile: `era_o2_g0_cc1_281_load_delay_call_slot_store` (-O2 -G0 -mno-split-addresses, env ERA_CC1_VER=2.8.1 MASPSX_LOAD_DELAY_CALL_SLOT_STORE=1).

## Verification (private lane config)

```text
python3 tools/build/disc1_preflight.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json --deep --only func_800742B8
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane4/disc1.yaml --profiles $S/lanes/lane4/disc1_build_profiles.json func_800742B8
disc1_preflight: PASS (deep, 1668 c / 337 asm / 2 rodata)
LEAF_STRONG=EXACT func_800742B8 span=0x78 vram=0x800742B8 compiled=0x80 failures=[none]
```

## Load-bearing levers

Band compiler 2.8.1; maspsx patch 13 adds the load-delay nop between `lw $v0,0x34($s0)` and the `jal` whose slot stores $v0 (11 words otherwise). IntrState struct typing for D_800945E4 (+0 active u16, +0x32 mask, +0x34 word, +0x38 record).
