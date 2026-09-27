# func_8003205C — Battle panel gauge-pip sprites

- VRAM `0x8003205C`, size `0x36C`, profile `era_o2_g0`. Source: `src/func_8003205C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8003205C 0x36C`.


## Levers
1. (1) each ladder arm writes u0, v0 AND clut=func_80077AA4(0x130,cy); cc1 cross-jumping merges the identical v0-store/call/clut tail, leaving per-arm u0/k-reload/a0/a1 exactly like retail (single-assignment ladder + shared call: 191 words); (2) char stores invalidate D_8009CDDC but short stores do not, so w,h come BEFORE u0 in source; (3) clut stores use the array form D_8009E500[k][i].sp.clut (absolute sym+off) while the others use the (&(row[k]+i)->sp)->f pointer form (sets the s0/s2 base); (4) x -= 6 after the addPrim call; (5) i = 0 in the for-init of the else arm (top-level i=0 reorders the prologue sw s1: 7 words)

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8003205C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8003205C
disc1_preflight: PASS (deep, 1871 c / 286 asm / 2 rodata)
LEAF_STRONG=EXACT func_8003205C span=0x36C vram=0x8003205C compiled=0x370 failures=[none]
```
