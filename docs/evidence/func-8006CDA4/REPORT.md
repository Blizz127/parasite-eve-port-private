# func_8006CDA4 — matching C leaf (lane exeC4, 2026-09-27)

- VRAM `0x8006CDA4`, file offset `0x5D5A4`, size `0x2D4` (181 words); source `src/func_8006CDA4.c`
- YAML row `- [0x5D5A4, c, func_8006CDA4]` (replaces `[0x5D5A4, asm]`; next row `[0x5D878, c, func_8006D078]` unchanged)
- Profile `era_o2_g8_dispatch_80011428` (`-O2 -G8`, `MASPSX_DISPATCH_FOLD=jtbl_80011428`,
  `MASPSX_THREE_WORD_SYMBOL_STORE=1`), cc1 2.7.2
- Role: CD file-read state machine over `D_800B0CD8[0xF0]` (states 0/7/8/9/10), file table `D_8009317C`.

## Verification (lane private config build/lanes/exeC4/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC4/disc1.yaml --profiles build/lanes/exeC4/disc1_build_profiles.json --deep --only func_8006CDA4
  -> disc1_preflight: PASS (deep, 2092 c / 186 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_8006CDA4
  -> LEAF_STRONG=EXACT func_8006CDA4 span=0x2D4 vram=0x8006CDA4 compiled=0x2E0 failures=[none]
MASPSX_DISPATCH_FOLD=jtbl_80011428 MASPSX_THREE_WORD_SYMBOL_STORE=1 python3 tools/analysis/era_link_check.py src/func_8006CDA4.c 0x8006CDA4 0x2D4 -O2 -G8
  -> word mismatches=0 / LINK_EXACT
```
Profile necessity (by hand; profile_necessity.py reads the main YAML): default `-O2 -G0` → 131
mismatches; `-O2 -G8` without the knobs → 2; with only DISPATCH_FOLD → 2; full profile → 0.

## Levers on top of the exeC3 draft (13 raw → 0)
1. **Pinned `n` in `$5`** (`register int nn asm("$5")`) — 13 → 12.
2. **`nn = p[3]; ... nn = nn - off[idx];` split around the `D_8009D170` statement**: the sum's
   `off[idx]` read is written *before* the subtraction's read, and the subtraction's read comes
   *after* the D_8009D170 store in source, so cse1 cannot merge the two `lhu 0(s7)` reads (the store
   invalidates the varying-address MEM) while sched1 still hoists both above the stores. This gives
   retail's double read, `lw v1,0x100(s2)` early and `a0`/`a1` homes — 12 → 3.
3. **Function-level `base = (unsigned short *)&D_8009317C;` and `p = base + idx`**: the address is a
   pseudo at expand time, so insn is `(plus base Q)` instead of the canonical `(plus Q (const sym))`;
   reload then gives operand 1 (the rematerialised `la`) `$t1` and the spilled `Q` reload `$t0`, as
   retail — 3 → 0. (Spelling the constant any other way — casts, `idx + sym`, `&p[idx]`, a
   block-local pointer — keeps the canonical order or breaks the loop hoist.)
