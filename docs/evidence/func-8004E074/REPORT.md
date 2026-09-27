# func_8004E074 — name-entry window handler

- VRAM `0x8004E074`, size `0x270`, profile `era_o2_g8_force_d800b0cd8_absolute`. Source: `src/func_8004E074.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8004E074 0x270`.


## Levers
1. Profile era_o2_g8_force_d800b0cd8_absolute (existing): gp for D_8009CF54/D_8009D004, D_800B0CD8 absolute.
2. Row routing written as nested signed tests `if (pos >= 0) { if (pos < 3) {...; goto done;} if (pos < 5) {...; goto done;} } ...; done:` (a combined `pos >= 0 && pos < 3` is range-folded into `sltiu`).
3. `register W *w asm("$16")` (retail keeps the window in $s0 and the key/pos in $s1; unpinned they swap: 31 words).
4. Glyph-page arg `t == 0 ? 117 : t->f48 + 115` (default-first polarity).
5. Blank-name scan `for (q = p; *q != 255; ) ok &= (*q++ == 15);` gives retail's `andi a1,a1,1` / `move a1,zero` arms; confirm only `if (p < q && !ok)`.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8004E074
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8004E074
disc1_preflight: PASS (deep, 1860 c / 292 asm / 2 rodata)
LEAF_STRONG=EXACT func_8004E074 span=0x270 vram=0x8004E074 compiled=0x270 failures=[none]
```
