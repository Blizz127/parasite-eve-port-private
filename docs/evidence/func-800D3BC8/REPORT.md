# func_800D3BC8 — EXE, inline GTE (C candidate + native port)

- VRAM 0x800D3BC8, file offset 0xC43C8, size 0x39C. It stays inside the
  `[0xC43C8, asm]` span of `configs/USA/disc1.yaml` (not carved).
- Role: camera-facing textured sprite, a single POLY_FT4.
  - Projects `pos` with the camera RT (`D_800BCFA4`, RTPS).
  - The quad is centred on the screen point. Its half-extents are
    `D_800F3376 * sx / 8192` x `D_800F3378 * sy / 8192`.
  - The texture is a 16x16 cell (`u = (cell & 15) << 4`,
    `v = (cell / 16) << 4`). With `D_800F336E` set, cells 8..15 of a row
    fold to `u - 128`, `v + 32`.
  - Colour is `color[i] * bright / 128`, or `bright` when `color` is NULL.
  - `abr` 255 means opaque (code 0x2C, tpage `D_800F3370`). Any other
    value gives code 0x2E and `D_800F3370 | GetTPage(0, abr, 0, 0)`.
  - Depth is `SZ3 >> 2 - D_800F3374`. A nonzero `lerp` pins the depth to 14
    and pulls the centre towards (160,120) via func_800783E4 with weights
    `4096 - 2*lerp` and `2*lerp`.
  - The prim is taken from the packet arena up front. It is linked only when
    the depth is below 0x1000 (unsigned).
- Callers:
  - room m0123i `func_80192F0C` (Day 3 boss-fight effect, four billboards
    per frame);
  - room m0123i `func_80193618` (3 sites, still asm);
  - rooms m0429i (5 sites) and m0432i (3 sites), still asm.
- Candidate C: [`candidate.c`](candidate.c) (inline GTE macros, Psy-Q
  `inline_c.h` shape; the camera slot goes through the func_800CF5B0 register
  barrier).
- Native port: `pc_port/game/boot/func_800D3BC8_port.c`, with the unit test
  `D3BC8_sprite` (`pc_port/tests/test_d3bc8_sprite.h`).

## Match status: not carved (prologue residual)

```
$ python3 tools/analysis/era_link_check.py <candidate as func_800D3BC8.c> 0x800D3BC8 0x39C
linked .text 928 bytes, target 0x39c, word mismatches=24, nonzero_pad=0
```

All 24 words are prologue scheduling. Retail stores every callee save
(`$31,$21,$20,$18,$17,$16`) before the first stack-argument load
(`lw $21,120($sp)`). era cc1 instead hoists `sw $21; lw $21,120` to the top
and sinks the other saves below the `center` stores. The body, epilogue and
code size match word for word; the extra word is the trailing alignment nop.

This is the `prologue save-batching` mechanism of
`docs/acceptance/MATCHING_RESIDUAL_POLICY.md` (func_8001220C).

Levers tried, none of which change the prologue:
- `lerp` spelled as `*= 2`, `<<= 1`, a block temp, or inline `*2`.
- An early `return` vs. an `if` block.
- Local declaration order.
- `-O1`, `-fno-schedule-insns`, `-fno-schedule-insns2` (62 off), `-G8`.
- maspsx EXPAND_DIV / FILL_STORE / FILL_EPILOGUE.

Levers that did matter, and are kept in the candidate:
- The r/g/b temps, with `r = g = b = bright`.
- Chained UV/XY stores in retail order.
- `center` initialised before the packet allocation.
- The render parameters read as the `D_800F3368` struct: their in-struct
  loads keep the clut store ahead of the `D_800F3376` load, so `clut` and
  `w` share `$s3`.

Not registered in YAML or `disc1_nonmatching_sources.json`. The candidate
lives here only, so the exact EXE build is unchanged.
