# func_80021278 — Battle escape roll

- VRAM `0x80021278`, size `0x2F4`, profile `era_o2_g8_expand_div_force_d8009d278_d8009d254_absolute`. Source: `src/func_80021278.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80021278 0x2F4`.


## Levers
1. (1) MASPSX_EXPAND_DIV=1 (the thr*1000/lvl divide keeps its break guards); (2) whole body under if (!(a->f98 & 0x4000)) {...} else r->f0 = -1 so the -1 store is cross-jumped last; (3) switch(mode){2,3,default} for the f6==6 arm; (4) static inline lim_of(thr) and the compare written lim_of(thr) > roll so roll is sign-extended after the lim ladder; (5) short roll (int roll: a0/a1 swap, 9 words); (6) if/else f1 = 3/2 in the last arm

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80021278
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80021278
disc1_preflight: PASS (deep, 1879 c / 280 asm / 2 rodata)
LEAF_STRONG=EXACT func_80021278 span=0x2F4 vram=0x80021278 compiled=0x300 failures=[none]
```
