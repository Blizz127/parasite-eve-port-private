# func_800710A4 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x800710A4`, file offset `0x618A4`, size `0x2C8`; yaml `- [0x618A4, c, func_800710A4]` +
  resume `- [0x61B6C, asm]` (the old `[0x618A4, asm]` span also holds `func_8007136C`, `0x61B6C..0x61EA4`).
- Profile: default `era_o2_g0` (`-O2 -G0`), no assignment. Outside the 2.8.1 band.
- Player walk step: `D_8009D1A0 & 2` → run twin `func_8007136C`; else state `0x16`, speed via two
  `func_8003708C` fixed-point multiplies, analog (atan2 `func_80079FB4`) or 8-way d-pad facing `+0x3A`, camera
  adjust, `func_80078934` matrix apply.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_800710A4
  disc1_preflight: PASS (deep, 2048 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_800710A4
  LEAF_STRONG=EXACT func_800710A4 span=0x2C8 vram=0x800710A4 compiled=0x2D0 failures=[none]
```

## Levers (ladder)

decompB draft 58 words (retail shares `$s0` between the `state` pointer and the speed, `$s1` between the `0x16`
constant and the analog angle).
1. **Variable sharing, as in retail**: the `state` parameter is an `int` that is reassigned the speed
   (`state = func_8003708C(0x50000, …)`, derefs as `*(int *)state`), and `ang = 0x16` is the state-id constant
   before it becomes the angle. Alone 63; with `register int ang asm("$17")` → 46.
2. Tail angle in its own local `r` (retail keeps it in `$v1`) → 42; **unpinned `e` parameter** (drop the `$18` pin
   copy; the incoming-arg copy lands right after `sw $s2`) → 37.
3. `r += 0x800; r += (… >> 7) & 0xC00;` (retail's association; the single expression reassociates) → 12;
   `r &= 0xFFF; *(short *)(e + 0x3A) = r;` → 7.
4. **`asm volatile("");` between `d = func_80077CF4(ang);` and `state = -state;`**: keeps retail's `negu $s0,$s0`
   after the call (cc1 otherwise hoists the callee-saved negate above the `jal`) → 0. The operand form
   `asm volatile("" : "=r"(state) : "0"(state))` is much worse (127).
