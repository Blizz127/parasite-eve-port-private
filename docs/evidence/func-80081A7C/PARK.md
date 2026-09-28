
## exeD3 round (2026-09-27): 43 diff lines → 157/167 aligned (10 words, one residual class)

- Best draft: `build/lanes/exeD3/drafts/func_80081A7C.c`.
- Profile: **2.7.2** `-O2 -G0` with `MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_CALL_DELAY_SLOT=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`. 2.7.2 beats 2.8.1 here: under 2.8.1 the unaligned memcpy scratch register is `$t0` where retail uses `$v0/$v1`.
- Levers that landed:
  - The name dispatch is a real `switch (i) { case 0: … case 1: … default: … }`. Retail emits `beq i,0` / `li 1` / `beq i,1` / `j default`.
  - All table accesses go through `D_800A36B0[i]`, so the loop creates retail's `off`, `name` and `e` giv registers after the top test. Only the default case uses a `name = D_800A36B0[i].name` temp, which gives `name[p[32]] = 0` as `$19+x`.
  - The case-1 `".."` copy is done through `int` temps, which gives `lh/lb`.
  - Prologue order:
    - `asm volatile("" : "=r"(one) : "0"(1))` puts `li a0,1` in the first `beq` slot.
    - `pos = D_800A3C8C[dir].pos; asm volatile("" : "=r"(pos) : "0"(pos));` comes before `p = D_800A52B0`, so `lui $s0` lands just before the `jal`.
- Residual: the loop-invariant table base (`D_800A36B0`) takes `$s4` and the `e` giv (the `func_80080B44` arg) takes `$s5`. Retail has them the other way round, costing 10 words. Tried without success:
  - a `$20` pin on an explicit `e` (121)
  - a `$21` pin on an explicit base local (108)
  - an unpinned base local in three placements (118/119)
  - an explicit `e` walking pointer (142)
