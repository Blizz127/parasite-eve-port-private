# func_8007136C — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x8007136C`, file offset `0x61B6C`, size `0x338`; yaml `- [0x61B6C, c, func_8007136C]` replaces the
  `- [0x61B6C, asm]` resume row added for `func_800710A4` (next row `[0x61EA4, c, func_800716A4]`).
  **Integrate together with func_800710A4** (whose rows are `[0x618A4, c, func_800710A4]` + `[0x61B6C, asm]`).
- Profile: default `era_o2_g0`. Running twin of `func_800710A4` (speed base `0x168000`/`0xB4000`/`0x21C000` from the
  model's animation flags, state `0x17` or the model's `+0x13`).

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_8007136C
  disc1_preflight: PASS (deep, 2052 c / 210 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_8007136C
  LEAF_STRONG=EXACT func_8007136C span=0x338 vram=0x8007136C compiled=0x340 failures=[none]
```

## Levers (ladder)

decompB draft 87. The func_800710A4 levers (tail `r` with `r += 0x800; r += …; r &= 0xFFF`, `asm volatile("")` between
`func_80077CF4` and the negate, barrier on `ang` before the `+0x1000` wrap) → 49; `ang` as its own `$17`-pinned local
(retail: `state` in `$s1`, then the angle in `$s1`; speed `$s0`; `0x17` in `$s3`) → head exact.
1. **No pin on `sp`**: `register int sp asm("$16")` let cc1 negate the hard-reg variable in place (`subu $16,$0,$16`) and
   cross-jump the `-sp` d-pad arms into the `+sp` tails (97). Plain `int sp` still lands in `$s0` → 20.
2. **Separate variable for the head's animation-flag word** (`g = *(unsigned *)(*(u8 **)e + 0x4C)`) instead of reusing
   the d-pad flags `f`: the shared variable swapped `D_8009D26C`/`D_800BD020` into `$a0`/`$v1` (retail `$v1`/`$a0`) → 0.
