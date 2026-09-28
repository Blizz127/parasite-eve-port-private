# func_80081714 — EXACT (lane exeD5, 2026-09-27)

VRAM `0x80081714`, file `0x71F14`, size `0x2C4` (`asm/disc1/71F14.s`). CD path-table loader
(PVD sector 16 → "CD001" check → path-table sector → 128-entry directory table). Supersedes
`PARK.md` (4-word residual, lane exeD4).

- Source: `src/func_80081714.c` (canonical copy: `build/lanes/exeD5/drafts/func_80081714.final.c`)
- yaml: `- [0x71F14, c, func_80081714]` (next row `[0x721D8, c, func_800819D8]` already exists)
- Profile (new): `era_o2_g0_symbol_load_dest_temp_fill_call_epilogue` = era 2.7.2 `-O2 -G0` +
  `MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`

## Verification (private config build/lanes/exeD5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD5/disc1.yaml --profiles build/lanes/exeD5/disc1_build_profiles.json --deep --only func_80081714
  -> disc1_preflight: PASS (deep, 2102 c / 178 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD5/disc1.yaml --profiles build/lanes/exeD5/disc1_build_profiles.json func_80081714
  -> LEAF_STRONG=EXACT func_80081714 span=0x2C4 vram=0x80081714 compiled=0x2D0 failures=[none]
```
(Run with `--source-override func_80081714=build/lanes/exeD5/drafts/func_80081714.final.c`
because lane exeE2 was writing its own drafts into src/func_80081714.c at the same time.)
profile_necessity.py has no `--config`. The manual knob-drop ladder (era_link_check word mismatches) is:
none 152, without SYMBOL_LOAD_DEST_TEMP 9, without FILL_CALL_DELAY_SLOT 152, without FILL_EPILOGUE_DELAY_SLOT 2, all three 0.

## Lever that closed the last 4 words
The residual was the preheader order. Retail has `lui/addiu $s4,&D_800A3CB8; addiu $s6,$s4,4; addu $s5,$v1,$zero`,
and the end-pointer copy comes AFTER loop.c's hoisted movables. With `end = buf + 2048` before the
`do`, the copy precedes them.
Mechanism: the loop-test temp `buf + 2048` must be a loop.c movable scanned after the pos/name
address constants, and cse-after-loop then rewrites the hoisted `s5 = buf+2048` into a copy of the guard temp `$v1`.
loop.c accepts it only if `threshold * savings * lifetime >= insn_count` (1 * 1 * lifetime vs 55 real insns, threshold≈27 because the loop has a call).
A plain test temp has lifetime 1–2 luids and is "not desirable" (`-dL`). Non-line notes get luids, so
block notes inside a statement expression stretch the lifetime:
```c
} while (i < 128 && p < ({ unsigned char *e_ = buf + 2048; { int z_; } e_; }));
```
Here `-dL` shows `regno 188 (life 4), savings 1 moved`. `({ unsigned char *e_; { int z_; e_ = buf + 2048; } e_; })`
(life 3) and a nested `({ e_; })` (life 5) also match. The rejected forms are these. `end` assigned before the do: 4 mismatches.
Plain `p < buf + 2048`: life 1, 96. `({ buf + 2048; })` / `({ T e_ = buf+2048; e_; })`: life 2, 96.
`end = buf + 2048` inside the body: a user var after the first exit (maybe_never), not a candidate, 171.
A static inline `sector_end(buf)`: life 2, 96.
