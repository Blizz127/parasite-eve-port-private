# func_8005D2B4 — inventory script-command dispatcher

- VRAM `0x8005D2B4`, size `0x440`, profile `era_o2_g8_dispatch_800112dc_force_d800c0e0c_absolute`. Source: `src/func_8005D2B4.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8005D2B4 0x440`.


## Levers
1. New profile `era_o2_g8_dispatch_800112dc_force_d800c0e0c_absolute` (-O2 -G8; DISPATCH_FOLD=jtbl_800112DC, 21 cases 1100..1120; D_800C0E0C forced absolute so each read is a fresh `lui/lbu` like retail).
2. Register levers: case 1100 counts into the `arg` parameter itself (retail $s0); case 1102 counts into `arg2` pinned `register int arg2 asm("$5") = arg2_;` with `arg2 = 0;` before the range test (retail copies a2->a1 at entry); separate pointer locals per case (one shared `p` cannot take retail's different registers per case).
3. Case 1101 `(D_800C0E0C + func_80051E58() < 51) ? D_800C0E0C + func_80051E58() : 50` (calls are re-evaluated as retail); case 1117 `(b0B + 1 >= 100) ? 99 : b0B + 1`.
4. Case 1119 is `return remove_key(arg);` (a static inline): its result lives in $a0 and is copied on return as in retail.
5. Mod-slot table D_800A1E64 (32-byte records, hA at +0xA) is absolute; items resolved through the D_800C0E00 struct.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005D2B4
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8005D2B4
disc1_preflight: PASS (deep, 1868 c / 286 asm / 2 rodata)
LEAF_STRONG=EXACT func_8005D2B4 span=0x440 vram=0x8005D2B4 compiled=0x440 failures=[none]
```
