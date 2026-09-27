# func_800634D4 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x800634D4`, file offset `0x53CD4`, size `0x278`; yaml `- [0x53CD4, c, func_800634D4]` replaces
  `- [0x53CD4, asm]` (the span was exactly this function; next row `[0x53F4C, c, func_8006374C]`).
- Profile `era_o2_g8` (`-O2 -G8`). Menu row draw: draw-pos push/pop on the `D_8009D12C` stack, per-column
  enable callback + flag bit, highlight flag, flashing-cursor box via `func_80062090`, x advance.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_800634D4
  disc1_preflight: PASS (deep, 2043 c / 216 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_800634D4
  LEAF_STRONG=EXACT func_800634D4 span=0x278 vram=0x800634D4 compiled=0x280 failures=[none]
```
`profile_necessity.py` has no `--config`; manual check: `-O2 -G0` → `word mismatches=141, nonzero_pad=29`, so `-G8`
is load-bearing (`D_8009D124/128/12C/10C` are `$gp` slots).

## Levers (ladder)

42 (first draft) → 8 (separate `D_8009D124`/`D_8009D128` ints) → 3 (menu field read before the draw-pos loads in
row-advance/pop blocks; pinned `$3`/`$5` push/pop pointers) — decompB draft.
Residual 3 words in the flashing-cursor block: retail loads `m->w`, x, y, then `m->h`; cc1 hoisted `m->h` above x/y.
Plain locals in every order, inline `box()` helper (3 forms), `m->h` through a pointer (5): all stay at 3.
**Closing lever: volatile-cast reads of the three sequenced loads** —
`x = *(volatile int *)&D_8009D124; y = *(volatile int *)&D_8009D128; h = *(volatile int *)&m->h;` then
`D_8009D124 = x - 2; D_8009D128 = y - 2; func_80062090(w, h, 0);`. Volatile memory refs are not reordered among
themselves by the scheduler, so the source order x, y, h survives → 0 words. Only `y`/`h` volatile gives 2
(x must be volatile too).
