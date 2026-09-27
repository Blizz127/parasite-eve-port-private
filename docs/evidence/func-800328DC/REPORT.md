# func_800328DC — number sprite row (decimal digits → SPRT records)

- VRAM `0x800328DC`, file offset `0x230DC`, size `0x230` (single `jr $ra`; resume row
  `[0x2330C, asm]`, which holds `func_80032B0C`).
- Profile: default `era_o2_g0` (`-O2 -G0`), no knobs.
- Lane exeB, 2026-09-27. Draft base `build/archive/fanout/decompA/drafts/func_800328DC.c`
  (previously PARKED at ~122 words: "loop-hoisted /10 magic, &buf and mode not shared").

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_800328DC
  -> disc1_preflight: PASS (deep, 2046 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_800328DC
  -> LEAF_STRONG=EXACT func_800328DC span=0x230 vram=0x800328DC compiled=0x230 failures=[none]
```
No profile assignment (the default profile reproduces it).

## Levers (mismatch ladder)

| variant | mismatches |
|---|---|
| prior draft (`q = v/10; buf[0] = ...; while (q) { n++; buf[n] = q % 10; q /= 10; }`) | 122 |
| digit loop with the division inside the condition: `while (q = v / 10, buf[n] = v - q * 10, (v = q) != 0) { n++; }` | 72 |
| per-sprite `unsigned char c = buf[n];` read at the top of the draw loop | 6 |
| + `Ent *e = tbl + n;` declared before `c` (sprite address first, as in retail) | 1 |
| `Ent *e = (Ent *)(n * 28 + (int)tbl);` (retail `addu $a1,$v0,$s5`, offset operand first) | **0** |

1. **Division in the `while` condition.** cc1 duplicates the whole exit test (division and
   first digit store) in front of the loop. The `/10` magic constant, `&buf` and the stack
   arg `mode` then come out in the pre-loop registers that retail shares with the loop body
   (`$t1`, `$a2`, `$t2`). The same comma-condition spelling also appears in the
   `func_80032B0C` draft.
2. **Early `buf[n]` read.** Retail loads `buf[n]` before the first sprite store. A later
   `buf[n] << 3` cannot move above the stores, which may alias.
3. **Operand order of the sprite address.** `tbl + n`, `&tbl[n]`, `n + tbl` and char-pointer
   forms all emit `addu $a1,$s5,$v0`. Only the integer sum with the scaled offset first gives
   retail's `addu $a1,$v0,$s5`.
