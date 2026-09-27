# func_8002B94C — EXACT (lane exeA3, 2026-09-27)

- VRAM `0x8002B94C`, file offset `0x1C14C`, size `0x344` (compiled `.text` 0x350 incl. gas pad)
- Profile `era_o2_g8_force_d800b0d88_actor`: era cc1 2.7.2, `-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D20C,D_800B0D88,D_800B0D8A,D_8009D254,D_800B0CEC`
- YAML rows: `- [0x1C14C, c, func_8002B94C]`, `- [0x1C490, asm]`
- Supersedes `PARK.md` (95 mismatches).

## Verify (private lane config build/lanes/exeA3/)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA3/disc1.yaml --profiles build/lanes/exeA3/disc1_build_profiles.json --deep --only func_8002B94C
  disc1_preflight: PASS (deep, 2080 c / 194 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA3/disc1.yaml --profiles build/lanes/exeA3/disc1_build_profiles.json func_8002B94C
  LEAF_STRONG=EXACT func_8002B94C span=0x344 vram=0x8002B94C compiled=0x350 failures=[none]
python3 tools/analysis/era_link_check.py src/func_8002B94C.c 0x8002B94C 0x344 -O2 -G8   (with the profile env)
  linked .text 848 bytes, target 0x344, word mismatches=0, nonzero_pad=0
```
`profile_necessity.py` has no `--config` (reads the main yaml), so it was skipped; default `-O2 -G0` gives 199
mismatches and `-O2 -G8` without the env does not link (gp window), so the profile is load-bearing.

## Levers (mismatch ladder: 95 -> 80 -> 13 -> 0)

1. `D_800B0D88`/`D_800B0D8A` are `D_800B0B38.f250`/`.f252` (the second actor record). Declared as 2-/1-byte
   single-member structs (`extern struct { unsigned short v; } D_800B0D88;`): the in-struct MEM keeps the
   `sh D88` store ahead of the `D_8009D254->f250` load (retail order), while the small size keeps cc1 from
   hoisting the address into a callee-saved register (accessing it as `D_800B0B38.f250` hoists `la` into `$s1`).
2. `D_8009D254` is a plain `Actor *` (the `[]` array declaration hoists `la D_8009D254`).
3. `t->f98 |= 0x1000;` is written BEFORE the three `t->f68/f6C/f70 = 0` stores. The in-struct zero stores make
   2.7.2 cse invalidate the in-struct memory table, so the following `t->f98 & 0x40000000` test reloads `f98`
   (retail's `lw $3,152($16)`) and the `|= 0x10` reuses that reload; sched1 still moves the zero stores above
   the `ori`/`sw`. (A volatile read gave 13.)
