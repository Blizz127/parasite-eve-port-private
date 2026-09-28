# func_80081A7C — EXACT (lane exeD4, 2026-09-27)

VRAM `0x80081A7C`, file `0x7227C`, size `0x29C` (`asm/disc1/7227C.s`). CD directory loader.
It reads the directory sector for `dir` into `D_800A52B0`, then walks the records into the
64-entry `D_800A36B0` file table (CdlLOC via `func_80080B44`, size, name). Entry 0 is ".",
entry 1 is "..", and the rest are copied names. It prints debug output when `D_8009AFC0 >= 2`.

- Profile (new): `era_o2_g0_symbol_load_dest_temp_fill_call_epilogue_jump_reg_store` = 2.7.2
  `-O2 -G0` + `MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_CALL_DELAY_SLOT=1
  MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 MASPSX_FILL_JUMP_REG_STORE=1` (K6, landed this session).
- YAML: `- [0x7227C, c, func_80081A7C]`, which replaces `[0x7227C, asm]`. The next row is `[0x72518, c, func_80081D18]`.

## Verification (private config build/lanes/exeD4/)
```
disc1_preflight.py --config build/lanes/exeD4/disc1.yaml --profiles build/lanes/exeD4/disc1_build_profiles.json --deep --only func_80081A7C
  disc1_preflight: PASS (deep, 2089 c / 189 asm / 2 rodata)
leaf_strong_check.py --config ... func_80081A7C
  LEAF_STRONG=EXACT func_80081A7C span=0x29C vram=0x80081A7C compiled=0x2A0 failures=[none]
era_link_check without MASPSX_FILL_JUMP_REG_STORE: word mismatches=83 (K6 is load-bearing)
```
profile_necessity.py has no `--config`, so it was skipped. The lead's gate covers it.

## Load-bearing levers (exeD3 best was 157/167, a $s4/$s5 base-vs-giv swap)
1. Cases 0/1 are 2- and 3-byte block copies of the "." / ".." name images:
   `__builtin_memcpy(D_800A36B0[0].name, D_80012014, 2)` and `(…[1].name, D_80012018, 3)`, with
   `extern char D_8001201x[] __attribute__((aligned(4)))`. The 3-byte copy is one `movstrsi` insn,
   so the loop-invariant table base has 9 refs instead of 11. Its global-alloc priority is
   3*9/108 = 0.25, which is below the `e` giv's 0.275, so the giv gets $s4 and the base gets $s5,
   as in retail. The earlier `int` temp stores gave 11 refs (0.289 > 0.259) and the swapped
   registers. `strcpy(…, "."/"..")` gives the same code. The builder's RODATA_FOLD stripper
   rejects those literals because the pool blocks are `.short`/`.byte`, which is why the
   extern-symbol memcpy form is used.
2. Because the block move is one insn, reorg cannot split its final `sb` into the following `j`
   slot. ASPSX does, so K6 `MASPSX_FILL_JUMP_REG_STORE` moves it (0x80081BE8 `j` / `sb $v1,34($s5)`).
3. Kept from exeD3: a real `switch (i)` with 0/1/default cases, `D_800A36B0[i]` indexing
   throughout, `asm volatile` barriers on `one` and `pos` for the prologue order, and 2.7.2 (2.8.1
   uses `$t0` for the unaligned-copy scratch register).
