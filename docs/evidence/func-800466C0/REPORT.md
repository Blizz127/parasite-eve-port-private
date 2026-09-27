# func_800466C0 — menu window key handler (confirm / cancel)

- VRAM `0x800466C0`, file offset `0x36EC0`, size `0x3FC` (single `jr $ra`; next span
  `[0x372BC, c, func_80046ABC]`).
- Profile `era_o2_g8` (`-O2 -G8`; plain `-O2 -G0` → 231 mismatches).
- Lane exeB, 2026-09-27. Draft base `build/archive/fanout/decompA/drafts/func_800466C0.c`
  (previously PARKED: "retail uses 5 callee-saved regs, cc1 4", 250 mismatches).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_800466C0
  -> disc1_preflight: PASS (deep, 2045 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_800466C0
  -> LEAF_STRONG=EXACT func_800466C0 span=0x3FC vram=0x800466C0 compiled=0x400 failures=[none]
```

## Levers (mismatch ladder)

| step | mismatches |
|---|---|
| prior draft (`r = 1; return r;` at each exit) | 250 |
| single exit: every path `r = 1; goto end;` / `end: return r;` (retail returns `move $v0,$s2` at each join) | 122 |
| `if (func_8005B89C() != 0) { big block; goto done; } func_80046574(self, 1); goto done;` (retail places the short arm after the big block) | 71 |
| `void *fp = func_80046574;` before the first width, `D_8009CFA8 = fp;` (retail hoists the address into `$s3` across the width calls) | 115 → with the pins below |
| `register int r asm("$18")` + separate `width2`, written `if (f() >= 120) width2 = f(); else width2 = 120;` | 8 |
| second `func_80062A34(1, 41)` result stored back into **`self`** (retail reuses `$s3`) | 2 |
| `x = func_80062D2C(41, w, 0, 1); l = func_8006322C(41, x, x); w = x;` (retail sets `$a0 = 41` before the `$s0` copy) | **0** |

Necessity on the final form: dropping the `r` pin → 15, dropping `fp` → 23. The
`width2` pin used along the way is not needed in the final form (still exact without it).
