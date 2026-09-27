# func_8003D94C — PARKED (lane exeB, 2026-09-27)

- VRAM `0x8003D94C`, file offset `0x2E14C`, size `0x298`; GT4/GT3 texture-page and CLUT
  rebase over `a0->prims` (two `n4`/`n3` loops, each prim ×2 double buffer).
- Profile tried: `era_o2_g8` (plain `-O2 -G8`).
- Best draft: `build/lanes/exeB/drafts/func_8003D94C.c`, **141** word mismatches
  (down from 156 in `build/archive/fanout/decompA/drafts/func_8003D94C.c`).
  Command: `tools/analysis/era_link_check.py src/func_8003D94C.c 0x8003D94C 0x298 -O2 -G8`.

## What changed (156 → 141)
- `short flag = (y == 0x80);`: retail copies the flag into a fresh register for
  loop 1 only (`addu $t4,$t5,$0`). A `short` flag reproduces that copy; `int`
  gives 156 and `unsigned char` gives 144. `c`, `tp` and `d` types
  (int/short/unsigned short × int/short) move nothing further. A 36-combination sweep is in
  `build/lanes/exeB/v/sweep_d94c.txt`.
- `int d` together with `(short)tp >> 1` gets the `x != 0x3C0` arm laid out as the
  fallthrough with a `j` over the `== 0x3C0` arm, which is the retail layout. But it
  costs +19 words elsewhere, so the best draft keeps `short d`.

## Residual classes
1. **Sign-extension of `tp` in the `x == 0x3C0` arm.** Retail keeps a copy of `tp`
   (`addu $a2,$a1,$0`) and uses `sll 16; sra 17` on it in the `==` arm, and a plain
   `srl $t1,$a1,1` in the `!=` arm. cc1 always proves `tp` has ≥17 sign-bit copies
   (it is built from `(x>>6)`, `(y>>7)`, `(y>>8)*30` shift chains) and emits a bare
   `sra 1`. None of these keep the extension: `(short)tp >> 1`, `(tp << 16) >> 17`,
   `d = (short)tp; d >>= 1`, `short tp`, or building `tp` over two or three statements
   (so it is set more than once and `reg_sign_bit_copies` is not recorded).
2. **Param copy of `c`.** Retail keeps the stack arg `c` in `$a3` and copies it to `$t2`.
   The `!=` arm computes `c*64` from `$a3` and the `==` arm from `$t2`; they share only
   the `addiu -0x7080`, via cross-jump. Neither `short c` (gives `lhu`, retail uses `lw`)
   nor computing `clut` in both arms reproduces that.
3. Loop registers then shift throughout (ptr `$a1` vs `$a2`, `d+1` `$a2` vs `$a3`, etc.).

## Second pass (exeB, 2026-09-27)
Also tried, all ≥141: an int `tp` plus a short copy `ts = tp` used only in the `== 0x3C0` arm
(retail's `addu $a2,$a1,$0` copy); the same with `ts` set a second time so the pseudo is
multi-set; `short cs = c` for the stack arg, with `clut` computed from it in one or both arms.
cse still folds `(short)ts >> 1` to `sra 1` through the equivalence.

## Third pass (lane exeB2, 2026-09-27): 141 → 2
Best draft `build/lanes/exeB2/drafts/func_8003D94C.c`: **2** word mismatches (one swapped pair: the
`addu $10,$7,$0` copy of `c` sits *before* the `beq` in cc1's output, and in the `beq` delay slot in retail).
Command: `tools/analysis/era_link_check.py src/func_8003D94C.c 0x8003D94C 0x298 -O2 -G8`.

Levers, with the ladder:
1. **Empty-asm copy barriers for the two EQ-arm copies** (141 → 40 → 34):
   `asm("" : "=r"(ts) : "0"(tp));` (ts is used only as `(short)ts >> 1` in the `== 0x3C0` arm, so
   cse and combine can no longer prove the sign-bit copies and the `sll 16; sra 17` survives), and
   `asm("" : "=r"(cc) : "0"(c));`, with `clut = c*64 - 0x7080` in the NE arm and `clut = cc*64 - 0x7080`
   in the EQ arm (retail computes clut in both arms from different registers and cross-jumps only the `addiu`).
   Order matters: the cc barrier goes before the ts barrier (34, the others 37–39).
2. **Two `asm("" : "=r"(ptr) : "0"(ptr));` ref barriers** between the loops (34 → 22). global.c priority is
   floor_log2(refs)*refs/live_length: `d` scored 16 refs / 131 insns (0.489) against `ptr` 18 / 158 (0.456),
   so `d` took `$8`. Each barrier adds refs to `ptr` (22 refs, 0.55). Now `ptr` is `$8` and `d` is `$9`, as in retail.
   Put before the `beq`, they pin the load to the entry (42). Between the loops or in the EQ arm they cost nothing.
3. **`ptr += sizeof(GT4)` moved into the for-increment** (`for (k = 0; k < 2; k++, ptr += sizeof(GT4))`),
   and the same for GT3 (22 → 2). Retail does the `k+1` temp before `ptr += 52` in both k-loop tails.

Residual (2 words): the empty asm insn for `cc` sits between its reload copy and the `beq`, and
reorg's `stop_search_p` will not look backward past an asm. A natural copy (`cc = c;`) reaches the slot only
with `-fno-rerun-cse-after-loop` (5 mismatches, register homes then swapped). Under plain -O2, cse2 folds it
because it canonicalises the longer-lived pseudo. Also tried, all ≥ 5:
- asm barrier in the EQ head (10; the slot is filled from the fallthrough `srl`, since reorg predicts `EQ` not taken);
- cc barrier first (5) or after `flag` or after `tp` (2–6);
- swapped arm usage (3);
- `move %0,%1` asm templates (149; an asm cannot go into a delay slot and blocks sched);
- `register int cc asm("$10")` (146);
- `short cc = c` (13; no copy, and a `sll/sra` appears);
- a cse-foldable dead `goto eq` to stop cse following into the EQ arm (146–148);
- empty-asm uses of `c` or `cc` at the join to change which pseudo cse2 canonicalises (145–147);
- the combined `x == 0x3C0 && tpage == 0x1F` return with a second if or a ternary (147).

## Fourth pass (lane exeB3, 2026-09-27): 2 → 1 with `-fno-rerun-cse-after-loop`; plain -O2 stays 2
Best drafts: `build/lanes/exeB3/drafts/func_8003D94C.c` (**1** mismatch under
`-O2 -G8 -fno-rerun-cse-after-loop`, 144 under plain -O2 because rerun-cse changes the layout) and
`build/lanes/exeB3/drafts/func_8003D94C_plainO2_2mm.c` (the exeB2 draft, 2 under plain `-O2 -G8`).
1-mismatch shape: natural `cc = c;` as the **first** statement (before `flag`), `ts` asm barrier kept,
NE arm `asm("" : "=r"(c) : "0"(cc)); clut = c * 64 - 0x7080;`, EQ arm `clut = cc * 64 - 0x7080;`.
The copy `addu $10,$7,$0` now lands in the `beq` delay slot (backward fill, as in retail) and every
register home matches except one: the EQ arm reads `$7` (the load) where retail reads `$10` (the copy).
Cause (from the `-dr/-ds` dumps): cse1 following the jump into the EQ path puts both pseudos in one
equivalence class whose head is the load pseudo, because `make_regs_eqv` only promotes the copy pseudo
when it lives outside the current cse block; inside the entry→EQ path it does not, so EQ's `cc` is
rewritten to `c`.
Ladder: `cc = c` after ptr / after tp barrier: 5 (homes swapped); at top: 2 (homes of c/cc swapped);
+ NE tie from cc: 1. Tried, all worse: register pins `$6/$7/$10` on ts/cc (28–146); swapped arm usage
(142–145, the arms cross-jump to a 656-byte body); extra `c`/`cc` self-ties in the EQ arm, at the join, or
`asm("" :: "r"(cc))` at the join (8); `volatile int c` (2 with -fno-rerun); `int *cp = &c` (145);
moving the cc tie into the EQ head plus an `asm("")` at NE head (10 / 146, slot filled from fallthrough).
Next idea not tried: get a cse block boundary between `cc = c` and the `if` with no emitted code, so
cse never sees the equivalence.
