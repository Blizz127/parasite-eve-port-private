# func_80043DA4 — main-menu category key handler

- VRAM `0x80043DA4`, file offset `0x345A4`, size `0x3D0` (next span `[0x34974, c, func_80044174]`).
- Profile `era_o2_g8_dispatch_80010fcc` = `-O2 -G8` + `MASPSX_THREE_WORD_SYMBOL_STORE=1` +
  `MASPSX_DISPATCH_FOLD=jtbl_80010FCC` (the fold only fires with the three-word gate on;
  without the fold → 2 mismatches, plain `-O2 -G8` → table stays local).
- Lane exeB, 2026-09-27 (was PARKED at 197 → 16 earlier the same day).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_80043DA4
  -> disc1_preflight: PASS (deep, 2048 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_80043DA4
  -> LEAF_STRONG=EXACT func_80043DA4 span=0x3D0 vram=0x80043DA4 compiled=0x3D0 failures=[none]
```

## Levers (mismatch ladder)

| lever | mismatches |
|---|---|
| prior draft | 197 |
| popcount walk as explicit gotos (`k = -1; goto test; loop: if (sel < 0) goto out; …; k++; test: if (k < 9) goto loop; out:`) — retail's entry `j test` + bottom test | 157 |
| asm-label alias on case 2's last call (`extern void func_80046378_b(W *, int) asm("func_80046378");`) stops the case 2/3 cross-jump | 61 |
| single exit (`r = 1; goto end;` / `end: return r;`), case 7 / case 8 each ending in their own `func_80048918(id, -1, -1); D_8009CEF8 = 0; break;` | 120 → 21 |
| case 4 `r = 1; func_800526C4(); goto end;`, done path `r = 1; func_800525EC();` (`jal` slots filled from before) | 16 |
| `default: goto rset;` with `rset:` before the 0x40 arm's `r = 1;` (the default shares that tail, as retail's range check does) | 16 |
| **empty `__asm__ volatile("");` right after that `r = 1;`** | **0** |

The final lever: retail keeps `addiu $s0,1` before the arm's `j` and fills the `j` slot from
the target (`addu $v0,$s0,$0`, retargeted to the epilogue). cc1 instead moves `r = 1`
into the slot. An empty volatile asm between the two stops reorg's backward scan, so the slot
is filled from the target. A line-order search found the same shape (a real call between
`r = 1` and the jump → 13) but that reorders the calls. Removing the `rset` label while keeping
the barrier → 49.
