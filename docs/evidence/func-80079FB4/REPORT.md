# func_80079FB4 — LEAF_STRONG=EXACT (lane exeD, 2026-09-27)

Resolves the earlier `PARK.md` (which was only tried on cc1 2.7.2).

- VRAM `0x80079FB4`, file `0x6A7B4`, size `0x174` (93 words), `asm/disc1/695FC.s`.
- Semantics: libgte-style `ratan2(y, x)`. It takes the two signs, returns 0 when both are zero, and takes one of two ratio arms (with guarded `div` by `>>10`) into the `D_8009A6EC` short table. It applies `0x800 - c` and a negate from the sign flags.
- Profile `era_o2_g0_cc1_281_div_no_reuse_nop_expand_div_symbol_load_dest_temp`:
  `-O2 -G0 -mno-split-addresses`, `ERA_CC1_VER=2.8.1`, `MASPSX_EXPAND_DIV=1`,
  `MASPSX_DIV_NO_REUSE_NOP=1`, `MASPSX_SYMBOL_LOAD_DEST_TEMP=1`.
- YAML rows: `- [0x6A7B4, c, func_80079FB4]`, `- [0x6A928, asm]` (carved from `[0x695FC, asm]`).

## Verify

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json --deep --only func_80079FB4
  -> disc1_preflight: PASS (deep, 2042 c / 219 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD/disc1.yaml --profiles build/lanes/exeD/disc1_build_profiles.json func_80079FB4
  -> LEAF_STRONG=EXACT func_80079FB4 span=0x174 vram=0x80079FB4 compiled=0x180 failures=[none]
```
(`compiled=0x180` = 0x174 body + gas 16-byte pad, nonzero_pad=0.)
`profile_necessity.py` has no `--config` option and rejects the private profiles
file (it reads the main YAML), so it was skipped per the lane brief.

## Levers and their mismatch counts (era_link_check, word mismatches)

| form | mismatches |
|---|---|
| first draft, cc1 2.7.2 | 82 |
| first draft, cc1 2.8.1 + expand-div | 83 (size 96 words) |
| + `MASPSX_DIV_NO_REUSE_NOP` (patch 16: its ROM evidence already names 0x8007A060/0x8007A0E8) + `MASPSX_SYMBOL_LOAD_DEST_TEMP` | 19 |
| single result variable `r` (no early `return 0`; 2.8.1 otherwise emits a mid-function `jr ra`) | 92 → 4 after `s1 = 0; s2 = 0;` are written together at the top |
| `r = 0` put inside the condition: `if (x != 0 \|\| (r = 0, y != 0))`. This puts `addu v0,zero,zero` in the `beqz a0` slot, and reorg duplicates the `slt` into the `bnez a1` slot | 1 |
| empty `asm("" : "=r"(s1) : "0"(s1))` after `s1 = 0`, which stops cse turning `s2 = 0` into `addu a3,a2,zero` | **0** |

Knob necessity (on the final C, 2.8.1): without EXPAND_DIV 39 mismatches, without
DIV_NO_REUSE_NOP 51, without SYMBOL_LOAD_DEST_TEMP 6. The same C on 2.7.2 gives 2.
