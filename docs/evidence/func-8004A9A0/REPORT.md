# func_8004A9A0 — EXACT (lane exeF, 2026-09-28)

- VRAM `0x8004A9A0`, file offset `0x3B1A0`, size `0x3FC` (255 words), `asm/disc1/3B1A0.s`.
- NEW profile `era_o2_g8_dispatch_80010ff4_80011014`: `-O2 -G8`, env
  `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_80010FF4,jtbl_80011014`.
  Each knob load-bearing: without THREE_WORD 4 mismatches; without DISPATCH_FOLD 4.
- YAML row: `- [0x3B1A0, c, func_8004A9A0]` replaces `- [0x3B1A0, asm]` (next row `0x3B59C` is
  `func_8004AD9C`, so no resume row).

## Verification
```
MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010FF4,jtbl_80011014 \
  python3 tools/analysis/era_link_check.py src/func_8004A9A0.c 0x8004A9A0 0x3FC -O2 -G8
  LINK_EXACT
python3 tools/build/disc1_preflight.py --config build/lanes/exeF/disc1.yaml --profiles build/lanes/exeF/disc1_build_profiles.json --deep --only func_8004A9A0
  disc1_preflight: PASS (deep, 2132 c / 153 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeF/disc1.yaml --profiles build/lanes/exeF/disc1_build_profiles.json func_8004A9A0
  LEAF_STRONG=EXACT func_8004A9A0 span=0x3FC vram=0x8004A9A0 compiled=0x400 failures=[none]
```

## Levers (from exeC5's 6-word draft build/lanes/exeC5/drafts/func_8004A9A0_asmuse.c)
Residual was `addiu v1,a0,-1; lw v0,CFD8` vs retail `lw v0,CFD8; addiu v1,a0,-1` in sell cases 0–2.
`cc1 -dS` for the case block: both insns priority 1, and sched's "greater potential hazard"
tie-break always schedules the load first in the backward list (placed last).
Per case, now:
```
v = D_8009CFD8_k;                 /* asm-label alias, exeC3 */
asm volatile("" : : "r"(v));      /* volatile asm_operands = sched barrier -> lw first */
D_800A1A00.h[k]--;
asm("" : : "r"(v));               /* distinct source line: jump2 does not merge addiu;sh */
goto tail;
```
Ladder: 6 → volatile barrier alone (no trailing asm): lw order fixed but jump2 merges the
identical `addiu; sh` tails again (158 raw / 6 aligned) → + trailing non-volatile asm use: 0.
Equivalent exact spellings: `short t = h[k] - 1; asm volatile/asm("" : : "r"(t)); h[k] = t;`.
Note (gcc 2.7.2): an operand asm without outputs is *not* volatile unless written `asm volatile`
(so it is not a sched barrier); plain `asm("")` (ASM_INPUT) is.
