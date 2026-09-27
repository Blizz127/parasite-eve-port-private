# func_80055760 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x80055760`, file offset `0x45F60`, size `0x6B4`; yaml `- [0x45F60, c, func_80055760]` replaces
  `- [0x45F60, asm]` exactly (next row `[0x46614, c, func_80055E14]`).
- Profile `era_o2_g8_force_descriptor_absolute` (existing, same as `func_80052E30`: `-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800A1F84,D_800C0E48,D_8009D05C`). Note: the leaf checks also pass on plain
  `era_o2_g8` (these three are only address-taken here); the force list is kept for the same whole-image small-data
  reason recorded for `func_80052E30`. `-O2 -G0` → 411 words.
- Item-slot availability mask rebuild: two passes over both item tables (inline `sel()` = `func_80052E30` body,
  inline `get()` = record lookup), clears and rebuilds the `D_8009D058` bitmask, restores the selection.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_80055760
  disc1_preflight: PASS (deep, 2053 c / 209 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_80055760
  LEAF_STRONG=EXACT func_80055760 span=0x6B4 vram=0x80055760 compiled=0x6C0 failures=[none]
```

## Levers (ladder; aligned-instruction diff via build/lanes/exeC/adiff.py)

decompA draft 417 words.
1. Loop-body-local `unsigned short *h = &D_800C0E06[1]` → 300; moved to function scope before `loop2` (retail hoists
   `la $s2,D_800C0E08` once) → 224 (with 2).
2. `x = p[5]` loaded once before the `D_8009D028` test (retail shares one `lbu` for both arms, `sra`) → 234/224.
3. `get()` in the `func_80054F58` `rec()` shape: `w = v` copy for the third range test, inner `res = 0; done:
   return res;` and a separate outer `return 0` → 43; loop-1 tests via `static inline int a6(j) { p = get(j); return p ?
   p[6] : 0; }` (keeps the result in `$v0`) → part of 188 → 43.
4. `asm volatile("" : : : "memory")` before the final `sel(was)` (retail reloads `D_8009D04C` instead of reusing loop-2's
   copy) and `i = 0;` before `h = …` → 6.
5. **The mask-clear loop counter shares the record pointer's variable**: `for (p = 0; (int)p < D_8009D064; p++)
   ((int *)D_8009D058)[(int)p] = 0;` — retail keeps that counter in `$a0`, the register `p` holds for the rest of
   loop 2; every separate int counter (`k`, block-scoped, `j`, `b`, `x`, pins, do/while) gave 5–16 → 0.
