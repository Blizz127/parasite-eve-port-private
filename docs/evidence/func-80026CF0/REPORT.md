# func_80026CF0 — EXACT (lane exeA5, 2026-09-27)

- VRAM `0x80026CF0`, file offset `0x174F0`, size `0x2E0` (ends at `0x177D0`, next row `[0x177D0, c, func_80026FD0]`)
- Menu-cursor group rewind over `D_800BE830[]` (8-byte slots), `D_8009D2D8` group clear loop.
- Profile: existing `era_o2_g8_force_d8009d278_absolute` (`-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278`) — add
  `func_80026CF0` to its assignment list.
- Source: `src/func_80026CF0.c` (copy in `build/lanes/exeA5/drafts/`)
- Yaml row: `- [0x174F0, c, func_80026CF0]` (replaces `- [0x174F0, asm]`; no resume row needed)

## Verification (private lane config build/lanes/exeA5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json --deep --only func_80026CF0
  disc1_preflight: PASS (deep, 2094 c / 184 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json func_80026CF0
  LEAF_STRONG=EXACT func_80026CF0 span=0x2E0 vram=0x80026CF0 compiled=0x2E0 failures=[none]
tools/analysis/era_link_check.py src/func_80026CF0.c 0x80026CF0 0x2E0 -O2 -G8  (MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278, subshell)
  linked .text 736 bytes, target 0x2e0, word mismatches=0
```
profile_necessity.py has no `--config` (run after integration).

## Levers (on top of the exeA4 176/184 draft, `c` pinned `$2`)
The residual was reorg filling the `bne` slot by stealing B's `addiu $2,$5,-1` (retail fills from A's fall-through).
It follows from A's register picture; getting retail's A registers exactly makes reorg do what retail did:
1. A written as `{ register unsigned char x asm("$3"); x = n - 1; D_8009CE3C = x;
   { register unsigned char t asm("$2") = D_8009D2D8; t = t + 1; D_8009D2D8 = t; } c = x; }`
   (retail: x in `$3`, the `D_8009D2D8` temp and sum in `$2`). 176 → 179 (x pin) → 182 (t pin) → 167 (t+1 in `$2`
   frees `$4`, so global-alloc moves `n` from `$5` to `$4`).
2. A zero-code `asm("" ::: "$4");` at the `check:` label (where `n` is live) makes `n` conflict with `$4` and
   restores `n` in `$5`. Clobbers placed inside A perturb sched (183); inside the `D_8009CE60 != 0` arm also exact.
   → 184/184, 0 word mismatches.
