# func_80051980 — Item stat block builder

- VRAM `0x80051980`, size `0x344`, profile `era_o2_g8_dispatch_800111a8`. Source: `src/func_80051980.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80051980 0x344`.


## Levers
1. (1) la-form MASPSX_DISPATCH_FOLD=jtbl_800111A8 (hoisted switch base in t0); (2) f0 clamp written 'fE + q[7] < 1000 ? q[7] + fE : 999' (commuted operand order keeps two distinct adds; other spellings 1-14 words); (3) isqrt(u < 1000 ? u * 22500 : 999 * 22500) (constant arm folded to 0x156FABC); (4) block-scoped u temporaries give the 0x28 frame; (5) fC/f10 as bitfields (a:10 b:10 g:2 / lo4 b45 b67 flag bits b13:2), grade decode as in func_800512AC; same shape as func_80051E64

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80051980
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80051980
disc1_preflight: PASS (deep, 1878 c / 281 asm / 2 rodata)
LEAF_STRONG=EXACT func_80051980 span=0x344 vram=0x80051980 compiled=0x350 failures=[none]
```
