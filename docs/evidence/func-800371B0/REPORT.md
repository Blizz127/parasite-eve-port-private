# func_800371B0 — Damage-popup table reset and panel packet init

- VRAM `0x800371B0`, size `0x2A4`, profile `era_o2_g8`. Source: `src/func_800371B0.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800371B0 0x2A4`.


## Levers
1. (1) Flags word as bitfields (b0:8,b8:8 -> sb; b16:4,b20,b21,b22:3,b25 -> one lw + five and + one sw); val=-1 written after the bitfield clears (else the hoisted -1 lands before the masks); (2) func_800719E4 declared noreturn (reorg steals the tile-pointer add into the MargePrim beqz slot); (3) pe=&D_8009EC70[i] with &pe->sp inline, then a fresh ps=&D_8009EC70[i].sp (la D_8009EC78) and clut via the array form; (4) pt assigned inside the func_80077C84 argument list so it is computed after the nested func_80077A64 call

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800371B0
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800371B0
disc1_preflight: PASS (deep, 1875 c / 283 asm / 2 rodata)
LEAF_STRONG=EXACT func_800371B0 span=0x2A4 vram=0x800371B0 compiled=0x2B0 failures=[none]
```
