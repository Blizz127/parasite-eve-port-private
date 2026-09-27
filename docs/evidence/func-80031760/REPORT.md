# func_80031760 — matching C leaf

- VRAM `0x80031760`, file offset `0x21F60`, size `0x60C` (387 words), frame `0x120`
- Source: `src/func_80031760.c`; YAML row `[0x21F60, c, func_80031760]` (resume `[0x2256C, c, func_80031D6C]`)
- Profile: `era_o2_g0_rodata_fold_80010de4` (`-O2 -G0`, `MASPSX_RODATA_FOLD=D_80010DE4`), cc1 2.7.2
- Battle target-marker builder: rodata SVECTOR[3] triangle, `memset`-initialised SVECTOR/VECTOR
  (cc1 emits `jal memset` → BIOS `A(2Bh)` stub `0x80071A44` via `configs/USA/libcall_aliases.ld`),
  libgte rotate/translate/project, POLY_F3 + LinePair + Rec48 records at `D_8009E4D8`/`D_8009E498`/
  `D_8009E360` indexed by `D_8009CDDC`, three `func_80077AC4` (addPrim) calls.

## Verification (main config, 2026-09-27)

```
MASPSX_RODATA_FOLD=D_80010DE4 python3 tools/analysis/era_link_check.py src/func_80031760.c 0x80031760 0x60C -O2 -G0
  -> linked .text 1552 bytes, target 0x60c, word mismatches=0, nonzero_pad=0 / LINK_EXACT
python3 tools/build/disc1_preflight.py --deep --only func_80031760   -> PASS (deep, 2042 c / 217 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py func_80031760            -> LEAF_STRONG=EXACT span=0x60C compiled=0x610 failures=[none]
python3 tools/analysis/profile_necessity.py --only func_80031760     -> PROFILE_NECESSITY=PASS
```

## Load-bearing levers (aligned ladder 317 → 338 → 351 → 353 → 368 → 387/387)

Starting point: the decompC PARK draft (316/387; `build/archive/fanout/decompC/drafts/`), blocked
on three residuals plus the then-unresolved `memset`.

1. **Colour if/else — defeat the cross-jump with an asm-label alias read** (35 words). Retail
   keeps both six-store arms and cross-jumps only the final `D_8009E4D8[i].b0` store's
   `sll/addu/sll/sb`; the arm's own last `lw D_8009CDDC` is NOT merged. cc1's jump2
   `find_cross_jump` merges every identical tail insn, so identical arms collapse back to the
   `li` constants (1408-byte link). The else-arm's final index read is written through
   `extern int D_8009CDDC_b asm("D_8009CDDC");` — same assembled `lui/lw`, but the SYMBOL_REF
   string differs (`*D_8009CDDC`), so `rtx_renumbered_equal_p` fails on that `lw` and the merge
   stops exactly where retail's does. A `*(volatile int *)&D_8009CDDC` read also blocks the merge
   (351/387) but materialises the address in a register (`lui/addiu/lw 0(v0)`); the alias keeps
   the direct `%lo` load. Putting the alias in the then-arm instead gives the same bytes.
2. **y-block else-arm: compute `x + 35` once** (`x = e->f64 + 35;` then store `x` to
   `D_8009E360[..].x` and both `.x1` fields). Writing `x + 35` per store let sched1 hoist the
   `idx*3` product early and swapped the `v0`/`v1` and `a0`/`a1` homes (the PARK's residual 2).
   Pinning an index local to `$5` got 359; the once-computed value alone gets 368.
3. **z-block both arms: same lever** (`x = e->f66 ± 35;`) — closes residual 3 and the stray word
   near `+0x488` (387/387).

Kept from the draft: `register short t asm("$2")` for `y + 8` in the y-else arm,
`pb = D_8009E4D8; p = &pb[idx]`, and the 32-byte `int pad[8]` block that reproduces the retail
`0x120` frame.
