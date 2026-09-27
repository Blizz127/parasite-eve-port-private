# func_80076C34 — LEAF_STRONG=EXACT (lane exeD, 2026-09-27)

- VRAM `0x80076C34`, file `0x67434`, size `0x2B0` (172 words). GPU command-queue push: waits for a
  free slot of the 64-entry `D_800BD030` ring, copies the packet, then stores `a1`/`a2`/`func`
  and bumps the volatile head `D_80095874`.
- Profile `era_o2_g0_cc1_281_fill_call_delay_slot_fill_jump_symbol_store` (new):
  `-O2 -G0 -mno-split-addresses`, `ERA_CC1_VER=2.8.1`, `MASPSX_FILL_CALL_DELAY_SLOT=1`,
  `MASPSX_FILL_JUMP_SYMBOL_STORE=1` (**new maspsx LOCAL PATCH 34**, written by lane exeD).
- YAML: `- [0x67434, c, func_80076C34]` replaces `[0x67434, asm]`; the span ends at the existing
  `[0x676E4, c, func_80076EE4]`.

## Verify

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json --deep --only func_80076C34
  -> disc1_preflight: PASS (deep, 2045 c / 217 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json func_80076C34
  -> LEAF_STRONG=EXACT func_80076C34 span=0x2B0 vram=0x80076C34 compiled=0x2B0 failures=[none]
```

## Levers

1. The archived decompB draft (38 words on 2.8.1). Its tail wrote
   `D_800BD030[D_80095874].a2 = a3; ...func = func;`. gcc 2.8's alias rule treats a fixed scalar
   (`D_80095874`) and a varying *in-struct* address as non-aliasing, so sched2 hoisted each
   volatile index reload above the previous table store. Retail keeps store-then-reload.
   Writing the two stores as raw pointer arithmetic
   (`*(int *)((char *)D_800BD030 + D_80095874 * sizeof(GpuQ) + 8) = a3;`) drops the in-struct
   flag, so the dependency holds. 57 mismatches / 19 diff lines -> 2 diff lines.
2. The last difference is ROM `lui $at / addu $at,$at,$a0 / j .L80076E38 / sw $v0,%lo(D_800BD034)($at)`
   at 0x80076E04: an indexed symbolic store in a plain `j label` delay slot. No existing knob did
   this (patch 4 covers `j $31` only), so I added maspsx patch 34, `MASPSX_FILL_JUMP_SYMBOL_STORE`.
   Its test is `tools/era/maspsx/tests/test_fill_jump_symbol_store.py` (8 tests), registered in
   `scripts/setup_era.sh` MASPSX_TRACKED and in the `.gitignore` negations.
   - Flag-off identity: `tools/analysis/maspsx_knob_sweep.py --knob MASPSX_FILL_JUMP_SYMBOL_STORE
     --baseline <pre-patch copy>` gave `4507 era leaves, {'IDENT': 4507}`,
     `flag-off non-identical: none`.
   - Knob-on it would change 7 leaves (func_80027A08, 80028E94, 80031760, 800362B8, 800363F4,
     8003746C, ovl_03C5 func_80121004), none of which sets it.
   - maspsx suite: `python3 -m unittest discover -s tests -t .` gave 454 OK before and 462 OK after.

Knob ladder on the final C: without patch 34, 20 mismatches (a 1-word shift after the store).
Without `MASPSX_FILL_CALL_DELAY_SLOT`, 20. Both on: 0.
