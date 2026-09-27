# func_80028574

- VRAM `0x80028574`, file offset `0x18D74`, size `0x6D4` (437 words)
- Profile: `era_o2_g0_expand_div_rodata_fold_800108e4` (`-O2 -G0`, `MASPSX_EXPAND_DIV=1`,
  `MASPSX_RODATA_FOLD=D_800108E4`); cc1 2.7.2
- Source: `src/func_80028574.c`; carved from `[0x18D74, asm]` (resume `[0x19448, asm]` = `func_80028C48`).

## Verification (decompC private config, session 2026-09-24)

```
python3 build/fanout/decompC/elc.py src/func_80028574.c 0x80028574 0x6D4 -O2 -G0   (private copy of
    era_link_check.py that also resolves the libgcc names below)             -> LINK_EXACT, 0 mismatches
```
**Gate status: BLOCKED on tooling, not on the C.** cc1 emits the soft-float libcalls itself
(`jal __floatsidf/__muldf3/__adddf3/__divdf3/__fixdfsi`); retail's copies are
`0x80073454` (matched leaf `func_80073454` = Math_Int32ToDouble), `0x800735C4`, `0x80072A74`,
`0x80072F64`, `0x80073354`. `era_link_check.py` / `leaf_strong_check.py` / the abs_syms link only
resolve address-suffixed names, so the tree's gates report the five as unresolved until an alias
table exists (same class as `memset` = `0x80071A44` needed by `func_80031760`).

## What it does

Enemy-turn damage step (khasinski TU `Battle_StepEnemyMovement`): computes `base` three ways from
the `D_8009D278` record — (1) `f4C & 0x80000`: double math
`(double)((f1E-25+f4)*6) * ((double)(D_8009D2B0-1)*0.1 + 1.0) / 7.0`; (2) `f4C & 0x100000`:
`q = (u16)(f1E/5); q += f68->f0; q*((f4*7+120)/160)*fA/f2A + q`; (3) else a rodata
`unsigned char tbl[11]` lookup on `f68->f10 & 0xF`, `q*tbl[k]/100` — then
`amt = base - rec->f8C[/ (f10 & 0xF)]`, applies the 0x38000 element modifiers, the `fl` flag gates
(0x400/0x100/0x200/0x800/0x1000 with `rand()` chances setting `rec->f0` bits and a `mode` that scales
`amt` by /5 or *3/2), clamps `amt` to 1/2 when non-positive, subtracts it from `rec->f10`, calls
`func_80028C48(e)` unless `f4C & 0x80000`, and rewrites `rec->f0`'s 0x6000 field to 0x4000.

## Load-bearing levers (ladder: link error -> 239 -> 302 -> 345 -> 373 -> 394 -> 415 -> 437 aligned)

1. Three pointer locals on the record: `rec` (`$s4`), `fw = &rec->f0` (`$s2`, every flag-word
   access) and `pcc = &rec->fCC` (`$s3`).
2. **`D_8009D278` re-read per statement, no local**: retail keeps the record in `$a1` for branches
   1–2 (CSE across the fall-through path) but reloads into `$a0` in branch 3 — two pseudos; a
   single `st` variable spans the branch-3 temporaries and is pushed to `$a2`.
3. **Block-local `q`/`t`** (`{ int q; … }` per branch): a shared `q` becomes a global pseudo
   (9 refs) that wins `$a1` by priority and displaces the record pointer.
4. `q = (unsigned short)(f1E / 5); q += f68->f0;` — two statements: the sum lands in the
   quotient's register (`addu a0,a0,v0`) and `f0` is loaded early.
5. `switch` for every out-of-line dispatch (`(*fw>>18)&3` with cases 2/3/default, the three
   `rand()` gates written **`case 0: if (rand() & 1) break; case 2: SET; break;`** — the
   fallthrough gives case 2's label two predecessors, so CSE cannot carry the loaded `*fw` into the
   SET and no cross-jump is needed; the two `mode` setters; the final `switch (mode)`).
6. `(f4*7+120)/160`, `/100` via magic multiplies; `(unsigned short)` on the `/5` quotient (`andi 0xffff`).
7. `register Rec *rec asm("$20")`: cc1 ranks `mode` (7 refs/137 insns) a hair above `rec`
   (9/292) for `$s4`; retail has `rec`→`$s4`, `mode`→`$s5`. Pin only; every other register
   falls out naturally.
8. `MASPSX_EXPAND_DIV` (two trap-checked `div`s) and `MASPSX_RODATA_FOLD=D_800108E4`
   (`{0,100,60,41,0,25,0,18,0,0,13}`, copied with lwl/lwr + `lb`/`sb` tail).
