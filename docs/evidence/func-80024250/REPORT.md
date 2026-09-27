# func_80024250 — battle action dispatcher (switch + initialized local table)

- VRAM `0x80024250`, size `0x7EC` (compiled `0x7F0` with gas pad), ROM row `- [0x14A50, c, func_80024250]`.
- Source: `src/func_80024250.c` (decompA draft; merged in batch 10).
- Profile `era_o2_g8_dispatch_800107d4_rodata_fold_80010760_force_d8009d1a0_d8009d254_d8009d278_d8009d2e8_d8009d1ac_absolute`:
  `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_800107D4`,
  `MASPSX_RODATA_FOLD=D_80010760`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0,D_8009D254,D_8009D278,D_8009D2E8,D_8009D1AC`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80024250 0x7EC`.

## Levers
1. Both folds in one object: the `switch` table is retail's pool table `jtbl_800107D4`
   (DISPATCH_FOLD), and the initialized `unsigned short[20]` local is retail's pool block
   `D_80010760` (RODATA_FOLD; retail copies it to the stack with lwl/lwr). The same block is
   shared with `func_80022394`.
2. Tooling: the strong check originally refused this object (`.rodata 0x78 != pool table 0x50
   for jtbl_800107D4; refusing to strip`) because the jump table and the folded literal share one
   `.rodata`. tooling2's composed dispatch+rodata strip landed the fix; no source change was needed.
3. Globals read absolute in retail are forced absolute under `-G8` (else GPREL16 truncation at link).

## Verification (main config, 2026-09-24)
```
python3 tools/build/disc1_preflight.py --deep --only func_80024250
disc1_preflight: PASS (deep, 1959 c / 255 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py func_80024250
LEAF_STRONG=EXACT func_80024250 span=0x7EC vram=0x80024250 compiled=0x7F0 failures=[none]
```
Earlier (decompA private config): `era_link_check` LINK_EXACT, 0 word mismatches.
