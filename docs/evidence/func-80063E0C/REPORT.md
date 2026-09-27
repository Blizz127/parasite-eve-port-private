# func_80063E0C — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x80063E0C`, file offset `0x5460C`, size `0x9C4` (625 words); yaml `- [0x5460C, c, func_80063E0C]` replaces
  `- [0x5460C, asm]` exactly (next row is the `0x54FD0` list-view row recompute leaf already in the yaml).
- Profile `era_o2_g8` (`-O2 -G8`). Fresh draft (never drafted before). `-O2 -G0` → 621 words + size change.
- Menu input handler over a menu struct (`col/row` at `+0x44/+0x48`, saved selection `+0x4C/+0x50`, `ncol/nrow`
  `+0x54/+0x58`, `top/anim` `+0x5C/+0x60`, flags `+0x64`, left/right panes `+0x78/+0x7C`, callbacks `+0x84/+0x88`),
  focus in `D_8009D15C`, pad state via `func_8005E038` when `D_8009D0E8`.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_80063E0C
  disc1_preflight: PASS (deep, 2055 c / 207 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_80063E0C
  LEAF_STRONG=EXACT func_80063E0C span=0x9C4 vram=0x80063E0C compiled=0x9D0 failures=[none]
```

## Levers (ladder; aligned-instruction diffs from build/lanes/exeC/adiff.py)

1. First draft from the retail control flow, with inlines `idx()` (`r = -1; if (...) r = cols*row + col`),
   `sel()` (early `return -1`), `pick()`/`restore()` for the linked panes, `show_row()`, `set_top()` → 98.
2. `unsigned int flags` (retail `srl`), `goto out` for every `return r` that retail routes through the shared
   `move $v0,$s2`, pad test as `u = 0; if (D_8009D0E8) u = (…) != 0`, page-down `b = top + vis` before the key test → 77.
3. `x - (e + 1)` spelled with a separate `e1 = e + 1` temp (cc1 otherwise folds to `(x - 1) - e`; retail adds first)
   and if/else defaults `if (c == 6) v = 4; else v = c - 1;` (the ternary inverts the branch) → 44.
4. Page-down clamp written out with the recompute in the `lim < v` arm (retail recomputes `e`) and pinned temps
   `lim` `$2` / `v` `$4`; `& 0x5000` result through a `$2`-pinned temp; `u` pinned `$3` → 3.
5. Cross-jump control (retail kept copies cc1 wanted to merge): `asm("")` in the left-arrow tail, an explicit `rtail:`
   label shared by the right arrow and the cancel path, the left tail jumping to a distinct `out2:` label (separated
   from `out:` by an empty `asm`), and the final `key & 0x20` test written as `if (!(key & 0x20)) goto out;` so its
   branch still steals `move $v0,$s2` from `out` → 0.
