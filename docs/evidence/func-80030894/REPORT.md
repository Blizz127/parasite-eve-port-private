# func_80030894 — EXACT (lane exeB4, 2026-09-27)

- VRAM `0x80030894`, file offset `0x21094`, size `0xC50` (788 words): the HUD/menu primitive builder
  for both draw buffers (FT4 record, the 2×10×5 sprite array, TILE/G4/SPRT/F3 records and 11 loops).
  The structure follows the semantic decode in `pc_port/game/boot/func_80030894_port.c` and PARK.md.
- Profile: default `era_o2_g0` (`-O2 -G0`), so no assignment is needed.
- YAML: `- [0x21094, asm]` → `- [0x21094, c, func_80030894]` (the next row `0x21CE4` is already c).
- Source: `src/func_80030894.c` (copy in `build/lanes/exeB4/drafts/`).

## Verification (private lane config build/lanes/exeB4/)
- `python3 tools/analysis/era_link_check.py src/func_80030894.c 0x80030894 0xC50 -O2 -G0` → `word mismatches=0`
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeB4/disc1.yaml --profiles build/lanes/exeB4/disc1_build_profiles.json --deep --only func_80030894`
  → `disc1_preflight: PASS (deep, 2090 c / 188 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_80030894`
  → `LEAF_STRONG=EXACT func_80030894 span=0xC50 vram=0x80030894 compiled=0xC50 failures=[none]`

## Levers (ladder in word mismatches / opcode-aligned diff)
1. **3-byte struct copy** `Col3 col = D_8009CD90;` (`typedef struct { unsigned char c[3]; } Col3`):
   the block move gives retail's `la` base plus `lb`/`sb` ×3. Byte loads give `lbu` and separate `lui`s.
2. `int tp = GetTPage(..) & 0xFFFF` with `func_800370DC(Spr *, int)`, and explicit `& 0xFFFF` at the
   GetTPage-direct call sites. With an unsigned-short `tp`, a second hoisted zero-extended copy steals a register.
3. **One loop counter `i`** for the outer j loop and every later loop (retail's s6), plus a separate inner counter.
   Declaration order `buf` then `clut` gives the spill slots 24/32.
4. **A fresh block-scoped pointer per record block**: reused `s`/`t`/`g` variables become long-lived pseudos,
   and then `tp` is spilled (762 → 675).
5. **Sub-struct pointer as symbol+field plus a byte offset**:
   `s = (SPRT *)((char *)&D_X[0][0].s + (i * sizeof(Spr) + buf * sizeof(D_X[0])));`
   (and `(char *)&D_X[0].s + buf * sizeof(Spr)` for single records). This gives retail's
   `off + (sym+8)` with a hoisted `sym+8`. The natural `&D_X[buf][i].s` is folded by c-typeck into
   `(&D_X[buf][i]) + 8`, so cse reuses the call argument (675 → 445, opcode diff 96 → 19).
   In L6/L8/L11 the offset order is `buf * .. + i * ..` (the 3 last words).
6. The second element of the G4 pair and of the TILE pair is computed **after the first call** in the same
   symbol+offset form, and `col.c[i]` is read once into a temp (`unsigned char c`).
