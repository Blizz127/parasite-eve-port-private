# func_8007EE84 — EXACT (lane exeD3, 2026-09-27)

- VRAM `0x8007EE84`, file offset `0x6F684`, size `0x244` (145 words). libcd command-queue push (CdControl-style). An optional "set mode" push (com 2) is followed by the command push. Both pushes are an inlined `push()` helper.
- Profile `era_o2_g0_cc1_281_fill_branch_macro_split_symbol_load_dest_temp` (already exists): `ERA_CC1_VER=2.8.1`, `-O2 -G0 -mno-split-addresses`, `MASPSX_FILL_BRANCH_MACRO_SPLIT=1`, `MASPSX_SYMBOL_LOAD_DEST_TEMP=1`.
- Yaml: `- [0x6F684, c, func_8007EE84]`, `- [0x6F8C8, asm]`.

## Verification (private config build/lanes/exeD3/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD3/disc1.yaml --profiles build/lanes/exeD3/disc1_build_profiles.json --deep --only func_8007EE84
  disc1_preflight: PASS (deep, 2082 c / 191 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD3/disc1.yaml --profiles build/lanes/exeD3/disc1_build_profiles.json func_8007EE84
  LEAF_STRONG=EXACT func_8007EE84 span=0x244 vram=0x8007EE84 compiled=0x250 failures=[none]
```

## Levers (archived tooling2 draft 102/145 aligned → 145/145)
- `MASPSX_SYMBOL_LOAD_DEST_TEMP=1` fixes the `q[head].id` indexed load. Without it: 143/145.
- `MASPSX_FILL_BRANCH_MACRO_SPLIT=1` puts the `D_8009B53C` store's `sw` in the `bne` slot. Without it: 139/145.
- `asm("" : "=r"(param) : "0"(param))` in the caller before push #1. This keeps retail's redundant `param != 0` test inside the first inlined push. Without it cc1 threads the test away: 119/145.
- The `++D_8009B53C` result goes into a temp `t` and then `id = t`, so the id is computed in `$v0` and copied to `$s2` in the `jal` slot. Without the temp: 121/145.
- `asm("" : "=r"(one) : "0"(1)); if (t == one …)`. This stops 2.8.1 CSE turning the `head*24` shift count into the `$3` that holds 1 (`sllv` instead of retail's `sll 1`).
- Prologue:
  - `int off = com * 4;` then `asm volatile("")`, then the `retry` copy.
  - `retry` is a `register int retry asm("$21")` local assigned after the barrier. Without the pin: 139/145.
  - The table is read as `*(int *)((char *)D_8009B4BC + off)`, which gives retail's `andi/sll` ahead of the saves and the `retry` move in the first `beq` slot. Using `D_8009B4BC[ci]` instead is 144/145.
