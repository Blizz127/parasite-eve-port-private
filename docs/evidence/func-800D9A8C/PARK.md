# func_800D9A8C — PARKED (88 words)

- VRAM 0x800D9A8C, file offset 0xCA28C, size 0x3D0; on the Day 1 route (fan-in 6). Still `asm` in `configs/USA/disc1.yaml`.
- Best draft: `build/lanes/m348/drafts/func_800D9A8C.c` (lane-local; inline-GTE macros + era `-O2 -G0 + MASPSX_EXPAND_DIV=1`).
- Check command: `MASPSX_EXPAND_DIV=1 python3 tools/analysis/era_link_check.py <draft> 0x800D9A8C 0x3D0 0x3D0`

## Residual

The rest is scheduling and register allocation in three places: (a) the state-0 block's store order (x, z, hA, y) and the reassociation of `hA + 0x20 + D_800E27EC*4`, which is fixed by `(short)(e->hA + 0x20)`; (b) `h8 * 3 / 4`, where the product is in $a0 in retail and $v0 in ours; (c) the texture-cell / GetClut arguments block after func_80077A64. The twin **func_800DA1FC** (0xCA9FC, 0x3D8) has the identical body with the D_800E2214.. symbols and will follow once this one closes.

## Levers tried

- Case/return structure as in func_800DD9E4, `D_800BCFA4[0]`/`D_800E11F6[0]` array reads to get retail's address-register form (227 → 90), `D_800F33E0` as `int *`, and `(unsigned char)(h8 / 2)` for the `srl`.
- A 24-permutation store-order sweep of the state-0 block, with and without the `(short)` cast and a hoisted angle temp: best 88.
