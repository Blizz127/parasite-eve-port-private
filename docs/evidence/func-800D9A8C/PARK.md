# func_800D9A8C — PARKED (8 words)

- VRAM 0x800D9A8C, file offset 0xCA28C, size 0x3D0; on the Day 1 route (fan-in 6). Still `asm` in `configs/USA/disc1.yaml`.
- Best draft: `build/lanes/m348/drafts/func_800D9A8C.c` (lane-local; inline-GTE macros + era `-O2 -G0 + MASPSX_EXPAND_DIV=1`).
- Check command: `MASPSX_EXPAND_DIV=1 python3 tools/analysis/era_link_check.py <draft> 0x800D9A8C 0x3D0 0x3D0`

## Residual

Eight words, all in the \`state != 0\` branch: \`h8 = h8 * 3 / 4\`. Retail keeps the \`(h8 << 1) + h8\` product in \$a0 and reloads D_800E27EC into \$v0. Ours puts the product in \$v0 and the reload in \$v1, so the next five instructions differ by register. The twin **func_800DA1FC** (0xCA9FC, 0x3D8) has the identical body with the D_800E2214.. symbols and will follow once this one closes.

## Levers tried

- Case/return structure as in func_800DD9E4, `D_800BCFA4[0]`/`D_800E11F6[0]` array reads to get retail's address-register form (227 → 90), `D_800F33E0` as `int *`, and `(unsigned char)(h8 / 2)` for the `srl`.
- A 24-permutation store-order sweep of the state-0 block, with and without the `(short)` cast and a hoisted angle temp: best 88.
- 2026-09-29 retry: D_800E220C..10 as one \`unsigned short[]\` (both sides are then struct/array references, so the global loads stay behind the \`e->x\` store as in retail) with the x, z, hA, y store order: 88 → 74. Passing the texture-cell expression inline as the GetClut argument (so it is computed straight into \$a1): 74 → 8.
- For the product: \`(h8*2 + h8)\`, a named \`t\` (also pinned \`asm("$4")\`), \`h8 *= 3; h8 /= 4\`, a \`(short)\` cast, moving the \`done\` statement, and early returns in place of \`done\`. All give 8 or worse.
