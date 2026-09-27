# func_80034104 — battle enemy HP/max-HP digit sprites

- VRAM `0x80034104`, size `0x6B0`, profile `era_o2_g8_force_d8009d254_d8009cddc_d8009d1e8_absolute`. Source: `src/func_80034104.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80034104 0x6B0`.
- Prior: asm

## Levers
1. digit loop written as while (q = hp/10, buf[n] = hp - q*10, (hp = q) != 0) n++; so jump.c's exit-test duplication peels the first digit and loop.c hoists the 0x66666667 magic and &buf to the prologue; (&T[v][n].sp)->f access keeps sym+8 hoisted in s2; b0=0x80 written in each arm (cross-jumped later) so cse1 cannot relate the call's la to sym+8 (else an extra s7); unsigned char t with u=D&0x1F; t=u; if(t>16) t=32-u; D_8009CE84 declared unsigned short[2] (array = in-struct for 2.7 alias) so the tail y load stays after the x0 store

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80034104
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80034104
disc1_preflight: PASS (deep, 1891 c / 278 asm / 2 rodata)
LEAF_STRONG=EXACT func_80034104 span=0x6B0 vram=0x80034104 compiled=0x6B0 failures=[none]
```
