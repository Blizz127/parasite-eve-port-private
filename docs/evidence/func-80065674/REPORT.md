# func_80065674 — EXACT (lane exeC3, 2026-09-27)

- VRAM `0x80065674`, file offset `0x55E74`, size `0x298` (compiled object 0x2A0 = body + gas pad).
- Profile `era_o2_g0_expand_div` (`-O2 -G0`, `MASPSX_EXPAND_DIV=1`). Without the knob: 66 word mismatches (load-bearing).
- YAML: `- [0x55E74, c, func_80065674]` (next row `[0x5610C, c, func_8006590C]` unchanged).

## Verify (private lane config build/lanes/exeC3/)
- `disc1_preflight.py --config build/lanes/exeC3/disc1.yaml --profiles build/lanes/exeC3/disc1_build_profiles.json --deep --only func_80065674` → `disc1_preflight: PASS (deep, ...)`
- `leaf_strong_check.py ... func_80065674` → `LEAF_STRONG=EXACT func_80065674 span=0x298 vram=0x80065674 compiled=0x2A0 failures=[none]`
- `MASPSX_EXPAND_DIV=1 era_link_check.py src/func_80065674.c 0x80065674 0x298 -O2 -G0` → `word mismatches=0`, `LINK_EXACT`

## Levers (on top of the exeC draft: volatile-cast off/hdr loads, in-loop `return 0`)
Mismatch ladder 73 → 64 → 58 → 55 → 1 → 0:
1. Second wrap test reads the header in a fresh local `c2 = r->hdr.w >> 8` (not the loop-crossing `cnt`)
   → hdr load stays after the tick/pos stores, `cnt` homes in `$a0` (73 → 58).
2. `a1 = r->pos.b; a1 |= (t1 + r->speed) << 8;` → retail's lbu-before-lh order (58 → 55).
3. The `hi >= cnt` arm computes into a local `v` and does ONE `r->pos.w = v;` store → jump2
   cross-jumps the `& 0xFF` else path into the shared `sw; j` tail as retail (55 → 1).
4. `q = (unsigned char *)((t1 >> 8) * 2 + (int)l);` → integer add keeps retail's `addu a0,v0,t0`
   operand order (pointer forms `l + x`, `x + l`, `&l[x]` all give `addu a0,t0,v0`) (1 → 0).
