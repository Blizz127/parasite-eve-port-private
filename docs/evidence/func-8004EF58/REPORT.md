# func_8004EF58 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x8004EF58`, file offset `0x3F758`, size `0x2E4` (ends on the `jr $ra` before `func_8004F23C` at `0x3FA3C`).
- Profile `era_o2_g8` (`-O2 -G8`); yaml row `- [0x3F758, c, func_8004EF58]` replaces `- [0x3F758, asm]` (the span was exactly this function).
- Source `src/func_8004EF58.c` (starts from the decompA fan-out draft, parked at 1 word).

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_8004EF58
  disc1_preflight: PASS (deep, 2042 c / 217 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_8004EF58
  LEAF_STRONG=EXACT func_8004EF58 span=0x2E4 vram=0x8004EF58 compiled=0x2F0 failures=[none]
python3 tools/analysis/era_link_check.py src/func_8004EF58.c 0x8004EF58 0x2E4 -O2 -G8
  LINK_EXACT
```

`profile_necessity.py` has no `--config` (reads the main yaml), so it was not runnable on the private
config. Manual necessity check: default `-O2 -G0` gives `word mismatches=141, nonzero_pad=5` → `-G8` is load-bearing
(the `D_8009CF0C/1C/30/34` words are `$gp` slots).

## Levers (mismatch ladder)

- Unpinned `l`: 12 words (`$s1`/`$s2` swap between `l` and `dx`).
- `register W *l asm("$17")`: 1 word — inside `if (l == 0)` cse substitutes the known-zero pinned `$s1` for the
  literal `0` second argument of `func_80062A20` (`move a1,s1` vs retail `move a1,zero` at `0x8004EFD4`).
- **New: zero-code barrier `asm("" : "=r"(l) : "0"(l));` as the first statement of the `if (l == 0)` body**: kills
  the branch-derived `l == 0` equivalence, so the constant stays `$zero` → 0 words.
- Carried from the draft: `self` reassigned to the window-53 result (retail reuses `$s3`); `fresh = l != 0` in the
  `sltu` slot; nested ternary `(D_8009CF30 ? 176 : (v->f80 ? 162 : 156)) - w->f18`.
