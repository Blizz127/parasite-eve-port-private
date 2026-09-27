# func_80017018 — Field-VM interpreter main loop

- VRAM `0x80017018`, size `0x27C`, profile `era_o2_g8_dispatch_80010690_force_d8009d2f0_d8009d1a0_d8009d254_absolute`. Source: `src/func_80017018.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80017018 0x27C`.


## Levers
1. (1) la-form MASPSX_DISPATCH_FOLD=jtbl_80010690 (hoisted switch base in s1); (2) FORCE_ABSOLUTE D_8009D2F0/D_8009D1A0/D_8009D254 while the object list D_8009D300 (0x590(gp)) and cursor D_8009CE00 (0x90(gp)) stay gp-relative; (3) register pins p asm($2) / w asm($6) (unpinned: 9-20 words); (4) ext re-read as *D_8009CE00 after the p+1 store (p[1] lets sched hoist it above the store); (5) n reuses w's variable (w = (w >> 13) & 0xF); (6) modes >>= 3 then if (i == 4) modes = ext

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80017018
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80017018
disc1_preflight: PASS (deep, 1873 c / 284 asm / 2 rodata)
LEAF_STRONG=EXACT func_80017018 span=0x27C vram=0x80017018 compiled=0x280 failures=[none]
```
