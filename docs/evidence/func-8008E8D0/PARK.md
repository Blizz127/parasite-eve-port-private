# func_8008E8D0 — PARKED (lane exeE3, 2026-09-28)

VRAM `0x8008E8D0`, file `0x7F0D0`, size `0x800`. AKAO sequencer step (opcode loop + note
dispatch). Outside the 2.8.1 band; stock cc1 2.7.2 `-O2 -G0` + `MASPSX_EXPAND_DIV=1` is the
best toolchain (2.7.2: 319, 2.7.2-cdk 497, 2.7.2.3 460, 2.8.0 491, 2.8.1 491, 2.6.3 435).
Best draft `build/lanes/exeE3/drafts/func_8008E8D0.best.c` (decompB draft + `add` temp for the
`D_8009B8DC[op % 11]` copy: 337 → 319 raw, 158 diff lines; count is shift-inflated).

## Blocking residual: multiply/divide result register (toolchain-level)
Every `multu`/`mult` site differs the same way: retail `mfhi a3` (÷11 ×3) / `mflo a1` / `mflo a3`,
stock 2.7.2 `mfhi t0` / `mflo t0`. `-dg` shows the `umulsi3_highpart` output pseudo allocated to
`hi` itself, and reload inserts `(set t0 hi)` (insn code -1) for the `srl` — reload's
`order_regs_for_reload` prefers never-used `t0` over the pseudo-used `a3`. Retail instead has
the value in an a-register, i.e. the pseudo got a GPR and `mfhi` is an output reload. `a3` is
never explicitly used in the RTL, so this is not `regs_explicitly_used`; it is the regclass
preference of the hi/lo-part pseudo. `gcc-2.7.2-cdk-psx` reproduces `mfhi a3` at all sites
(0 `t0`), but its `la`/prologue scheduling differs everywhere else, so it is not the retail
compiler either. No C lever found; would need a cc1-level difference or a maspsx rename pass.
Other residuals (secondary): `lh 210` delay-slot fill, `ent` (retail `s2`) allocation, the
`D_800B2900` address/value CSE split, `U16(0xE2)` arithmetic order in the portamento arm.

## Lane exeE4 (2026-09-28): new-idea attempts, no progress
- `-mhard-float` (raises n_non_fixed_regs; load-bearing in func_80071A84): 460, still `mfhi t0`.
- `-mfp32`: identical (319).
- Explicit 64-bit multiply-high (`(unsigned)(((unsigned long long)op * 0xBA2E8BA3u) >> 32) >> 3`)
  for the `% 11` / `/ 11` sites: umulsidi3 → both `mflo t1` and `mfhi t0` (360 / 420).
Retail `mfhi a3` still needs the hi-part pseudo to prefer GR_REGS (as cdk does); unchanged verdict.

## Lane exeG (2026-09-28): 158 → 43 aligned diff lines; the "toolchain-level" mfhi residual is solved in C
Best draft `build/lanes/exeG/drafts/func_8008E8D0.best.c` (stock 2.7.2 `-O2 -G0` + `MASPSX_EXPAND_DIV=1`;
raw `word mismatches=330` is shift-inflated, text is 2048 vs 0x800 = 4 words long). Score = aligned difflib
lines (`build/lanes/exeG/al.py`, branch targets masked).
**The `mfhi t0` vs `mfhi a3` residual is not a toolchain difference.** Reload takes the first *unused*
call-used register. Retail never allocates any pseudo to `$7`, so its reload reg is `$7`. Ours put `w`
(`U16(0x5A)`) in `$7`, which pushed the reload reg to `$8`. `w` could not take `$4` because local-alloc gave
`$4` to the `PTR(0x14)` temp while `w` was live (the `-dg` conflict list shows `89 conflicts: … 2 3 4 5 6`).
Ladder (aligned lines):
1. Split the reused `add` variable: the `D_8009B8DC[op % 11]` site and the `-(f != 0) & 0x30` site are
   separate locals (158 → 154).
2. `ent = PTR(0x14); ent += (n % 12) * 6;` loads the base straight into `ent`, as retail's `lw $18` does. No `$4`
   temp, so `w` gets `$4` and every `mfhi`/`mflo` reload lands in `$7` (154 → 151).
3. asm-label alias `extern int D_800B2900_w[] asm("D_800B2900")` for the `[x * 16]` word load, so cse no
   longer shares it with the `D_800B2900 + x * 64` argument (retail `lw $6,%lo(sym)($1)` + separate `la`)
   (151 → 136).
4. `op /= 11` in place of a separate `n`, then use `op` for the rest of that arm (retail keeps op and n in one
   pseudo, `$17`; it fixes the `$17/$18` swap with `ent`) (136 → 122); reuse `op` for `fl = U32(0x38)`
   as well (→ 119).
5. Block-local `unsigned int xq = U16(0x94)` (a local pseudo, not the shared `x`) (→ 118); else-arm-first
   polarity `if (!(xq & 0x8000)) m = d * ((k*15) >> 8); else m = d * k;` (→ 113).
6. Store order in the `fl & 1/2/4` blocks (brute-forced): `U16(0x92)=…; I32(0x1C)=…; U16(0x8E)=1;
   U16(0x8A)=U16(0x88);`, then `I32(0x20)=…; U16(0x9E)=U16(0x9C); U16(0xA2)=1;`, then `I32(0x24)=…; U16(0xB0)=1;`
   (113 → 57).
7. `U16(0x6A) * (ent[3] * 256 + ent[2]) * 4` (→ 54). Block-local `unsigned int xq = U16(0x84)` for the
   tail flag swap (→ 43).
Remaining (≈10 sites): the `lh 210` / `andi $18` beq-slot choice (retail fills from the fall-through, ours
takes the andi); the x-path `andi $3,$2,0xff` copy; the `ent[3]/ent[2]` register pair and `mflo $5`;
the portamento-arm reload pattern (statement permutations tried, current order is best); the tail
`beq $4 / addu $3,$4,$0` t copy (5 spellings tried); the fl&1 `lhu 144`/`sh 138/142` order.
Later in the same lane: 43 → 37 (best draft updated, `build/lanes/exeG/drafts/func_8008E8D0.best.c`).
8. x-path: `{ unsigned int b = ent[0]; x = (unsigned char)b; if (b >= 0x20) x += add2; }` reproduces
   retail's `lbu $2 / andi $3,$2,0xff / sltiu $2,$2,32` (43 → 41).
9. Beq-slot copy at 0x8008EA28: `{ int a = S16(0xD2); unsigned short t2 = a; if (a != 0) U16(0x56) =
   U16(0x58) = t2; }`. The narrow copy `t2` is emitted before the branch, so reorg fills the slot with it
   (retail `beq $3 / addu $2,$3,$0`) and the `andi $18` stays in the `lh` load-delay slot (41 → 37).
   The same shape at the tail (`S16(0xE4)`) does not keep its copy: its single use is folded into the add.
   Tried there: int/short/ushort copies, the global `t`, `if ((t = …))`, and a static-inline helper with a
   narrow parameter (49–55).
Final state this lane: **43 raw word mismatches, text size exact (2048 = 0x800)**; the aligned metric no longer
applies. Draft `build/lanes/exeG/drafts/func_8008E8D0.best.c`.
10. Portamento arm source order `U16(0x7E) = U16(0x82); U16(0xE4) = …; U16(0xE2) = …; op = U16(0x80) + S16(0xE6);`,
    so the new `op` is computed last. cse re-reads `U16(0x80)` after every store, which gives retail's 4× `0x80` /
    2× `0xDE` / 3× `0xE6` load pattern, and the block now matches exactly (raw 170 → 59 once the shift is gone).
11. Frame 56 → 64: the unused `int pad[2]` becomes `int pad[4]` (vars 16), matching retail's `addiu $29,-64` and every save
    offset (59 → 43).
Remaining 43 words:
- 0x8008ECF0 (8 words): the `I32(0x44) = U16(0x6A) * (ent[3]*256 + ent[2]) * 4` block. Retail loads `U16(0x6A)` first,
  ent[3]→`$3`, ent[2]→`$4`, `mflo $5`. 10 spellings tried.
- 0x8008EF58 (4 words): the fl&1 `srl`/`lhu 144` order and the `sh 138`/`sh 142` order. All 24 permutations
  tried; the current order is best.
- Tail from 0x8008F010 (~31 words, mostly a one-insn shift): retail keeps a `t = S16(0xE4)` copy *before*
  the `beq` (`beq $4 / addu $3,$4,$0`). Ours folds it: combine merges the copy when copy and use share a block,
  and cse propagates it when the copy sits in the test block. Tried narrow/global/multi-set/inline forms.

## Lane exeH (2026-09-28): 43 → 12 raw word mismatches (size exact)
Best draft `build/lanes/exeH/drafts/func_8008E8D0.best.c` (stock 2.7.2 `-O2 -G0` + `MASPSX_EXPAND_DIV=1`).
12. Tail divisor: a block-local `int dv = U16(0x7E);` loaded after the call and used for both `U16(0x7A) = dv` and
    the `/ dv` (retail loads `lhu 126` once; the double read reloaded it after the `mflo`). `int`, not `unsigned`
    (`unsigned` gives `divu`).
13. Tail `t` copy **solved**: `U16(0xE2) += U16(0xE4);` inside `if (S16(0xE4) != 0)`. The HImode `U16` read of the
    same slot is a different RTL expression from the `S16` test's sign-extended load, so cse gives the add a subreg
    of the tested register (a real copy that survives) and reorg puts it in the `beq` slot: `beq $4 / addu $3,$4,$0`.
    `U16(0xE2) = U16(0xE2) + U16(0xE4)`, `+= (unsigned short)S16(0xE4)`, `u = U16(0xE4)` all work equally.
14. With the copy restored the frame is 8 bytes larger: `int pad[4]` → `int pad[2]` (frame 64 again) (→ 12).
Remaining 12 words:
- 0x8008ECF0 (8 words), the `I32(0x44)` multiply. Retail `lhu $5 / lbu $3 / lbu $4 / sll / addu $4,$4,$3 / mult $5,$4 /
  mflo $5 / sll $5,$5,2`. Ours computes the product into a LO pseudo (regclass `pref LO_REG`), so the `sll` input reload
  takes spill reg `$7`. `{ int s = U16(0x6A); s *= ent[2] + ent[3] * 256; I32(0x44) = s * 4; }` reproduces the retail
  block exactly (s gets GR, mult output reload `mflo $5`), **but** that output reload makes LO a spill reg, which evicts
  the fl&1 `m` (a global pseudo, `LO_REG or none`) to the stack (+1 frame slot, `sw/lw $7,32($sp)`). Retail keeps `m` in LO
  with one `mflo $7` at the join, so retail has no LO reload anywhere; the `mflo $5` there must be a real move or a
  non-spill reload. Tried: 14 operand spellings, product into each function-level local (`m`, `d`, `x`, `w`, `y`, `fl`,
  `n`, `t`), `long long` products (cc1 narrows them back to `mulsi3`), `register … asm("$5")` (ignored),
  an empty-asm barrier on the product (LO spill again).
- 0x8008EF58 (4 words), fl&1 `srl`/`lhu 144` and `sh 138`/`sh 142` order. All 24 statement permutations × 4 temp spellings
  re-run on the new draft; the current order is still best.
Later in lane exeH: 12 → **3 raw words** (size exact; `build/lanes/exeH/drafts/func_8008E8D0.best.c`).
15. `I32(0x44)`: `{ int s = U16(0x6A); s *= ent[2] + ent[3] * 256; I32(0x44) = s * 4; }`. `s` is set twice
    (load, then multiply), so regclass gives it GR_REGS, local-alloc ties the multiply result to the dead `U16(0x6A)`
    register, and reload emits `mult $5,$4 / mflo $5 / sll $5,$5,2` exactly like retail (ent[2] first gives
    `addu $4,$4,$3`) (12 → 114, because of the next item).
16. The output reload into LO makes LO a spill reg for the whole function, which sends the fl&1 `m` (`LO_REG or none`)
    to the stack. `m >>= 7; U16(0x92) = m;` makes `m` a GR pseudo too: each arm gets `mult; mflo $3`, and post-reload
    cross-jumping merges the two `mflo`s into one at the join, as in retail (→ 5).
17. fl&1 store order re-brute-forced on the new draft: `m >>= 7; U16(0x92) = m; I32(0x1C) = …; U16(0x8A) = U16(0x88);
    U16(0x8E) = 1;` (→ 3).
Remaining 3 words at 0x8008EF54: retail `mflo $7 / srl $3,$7,7 / lhu $2,144`, ours `mflo $3 / lhu $2,144 / srl $3,$3,7`.
Retail keeps the product and the shifted value in different registers, with the product in `$7`. Evidence for where
that comes from: with `U16(0x92) = m >> 7` and no `pad`, `m` goes to the stack and the result is retail's code exactly
(frame 64 with no pad) except for an extra `sw $7,24($sp) / lw $7,24($sp)` pair after the merged `mflo $7`. So retail looks
like a spilled `m` with its store/reload removed, which stock 2.7.2 does not do here (the output reloads are in the
arms and the input reload is after the join label, so reload inheritance cannot connect them). 2.7.2.3 / 2.8.x / cdk / 2.6.3
are all far worse on this draft (482–494 vs 99 raw).

## RESOLVED (lane exeI, 2026-09-28): EXACT — see REPORT.md
The last 3 words came from where the shift sat. `m = (d * …) >> 7;` in each arm plus a single `U16(0x92) = m;`
after the join reproduces retail's `mflo $7 / srl $3,$7,7 / lhu $2,144 / sh $3,146`.
