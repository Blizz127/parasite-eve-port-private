# func_8007CEAC — PARKED (lane exeD3, 2026-09-27)

VRAM `0x8007CEAC`, file `0x6D6AC`, size `0x1A8`. libdma-style channel kick: poll CHCR busy (0x01000000) up to 0x10000 spins with a printf on timeout, then set or clear the channel's DICR enable bit (`D_8009B348[2]`) by `intr`, OR the DPCR enable bit `1 << (ch*4+3)` into `*D_8009B344`, then write MADR/BCR, poll `*D_8009B32C & 0x40`, write CHCR = control, and read it back.

- Best: 71/106 aligned. Draft: build/lanes/exeD3/drafts/func_8007CEAC.c (the archived goalc5 draft scored 50).
- Profile tried: 2.8.1 `-O2 -G0 -mno-split-addresses` + `MASPSX_FILL_CALL_DELAY_SLOT=1` + the drafted K5 `MASPSX_NUMERIC_LOAD_ASPSX=1` (spec in build/lanes/exeD3/KNOB_REQUESTS.md; not landed). 2.7.2 does worse (56).
- Residuals:
  1. K5: the constant-address CHCR loads use ASPSX dest-temp expansion. This alone is 53 → 67.
  2. The intr RMW arms. Retail keeps the compare constant 1 in `$v0` and overwrites it (`sllv $v0,$v0,$s0`), so reorg leaves the `bne` slot empty. cc1 keeps 1 in `$a0` and reorg hoists the shared `sllv` into the slot. Four arm spellings were tried (plain `|=`, pointer local, carried `v = 1`, a common-store form).
  3. The DMA-tail schedule order (bit, dma base, DPCR pointer, `blocks << 16`).

## exeD6 round (2026-09-28): still parked, aligned 40 lines
- Draft `build/lanes/exeD6/drafts/func_8007CEAC.best.c`. The intr arms use a per-arm `p = D_8009B348` and a common
  `p[2] = v;` store at the join. This gives retail's arm structure (`bne $17,$2` with an empty slot, `j join; or` and a
  shared `sb`). One shared `p` before the `if` hoists the load above the compare, which is worse.
- K5 is ported into the private maspsx (`build/lanes/exeD6/maspsx`, env `MASPSX_NUMERIC_LOAD_ASPSX=1`, loads only). It
  is not landed because no leaf closes with it.
- Profile: 2.8.1 `-O2 -G0 -mno-split-addresses` + FILL_CALL_DELAY_SLOT + K5. The aligned difflib score is 40 lines
  (`build/lanes/exeD6/al.py`, branch targets masked).
- Residuals:
  1. The loop-exit `beq` slot. Retail fills it from the fall-through with the loop-invariant `lui $6,0x1` (0x10000).
     cc1 leaves `li $6,65536` after a reorder-mode beq. Patch 6's fidelity gate and patch 19 (split li) do not fire,
     so this needs a single-`lui` variant of patch 19.
  2. The arm registers: retail has p in `$3`, the byte in `$4` and the result in `$2`. Ours has `$5`/`$3`/`$3`, and
     sched hoists the join's `lui/ori 0x1F801080` above the `sb`.
  3. The DMA tail order: retail keeps `t = *D_8009B348` first, then the DPCR bit, the dma base and D_8009B344. Neither a
     `volatile` read of t nor a precomputed `bit` local moves it.
  4. The final `sw t` is scheduled into our epilogue. Retail has it before the restores.
- Tried: v typed int/unsigned/uchar; the bit inline, precomputed or per-arm; `volatile` on p/D_8009B348 (18 variants,
  all tied).

## Lane exeG (2026-09-28): 40 → 28 aligned lines (still parked)
Best draft `build/lanes/exeG/drafts/func_8007CEAC.best.c`. Env: 2.8.1 `-O2 -G0 -mno-split-addresses
-fno-schedule-insns` + `MASPSX_FILL_CALL_DELAY_SLOT=1` + K5 `MASPSX_NUMERIC_LOAD_ASPSX=1` (K5 ported into the
private maspsx `build/lanes/exeG/maspsx`, loads only; run through `ERA_LINK_CHECK_MASPSX`).
1. **`-fno-schedule-insns`** (sched1 off, sched2 on): the retail tail is in source order with unfilled load-delay
   nops, and the loop head/`lui $6,1` slot then matches (40 → 30).
2. Tail source order `t = *(unsigned int *)D_8009B348; bit = 1 << (ch * 4 + 3); dma = …; *D_8009B344 |= bit;`
   (the DPCR pointer loaded after the dma base, as in retail).
3. **Epilogue `sw t` before the restores**: in sched2 `sw t` (priority 3) is ready at T-4, the restores
   (priority 2) only at T-5, so it lands after them. Storing through `volatile unsigned int *tp = &t;`
   makes the restore loads depend on the store and fixes the order. It costs `addiu $6,$29,16`,
   so it is a probe, not a solution (→ 28).
Residual (28): the intr-arm registers (retail p `$3` / byte `$4` / result `$2`, ours `$4`/`$3`/`$3`;
`v = 1 << ch; v = p[2] | v`, `v = p[2] | (v = …)`, `(1 << ch) | p[2]` tried: 33–38); the DPCR RMW/`blocks << 16`
interleave (sched2 fill); the `tp` address word; the t-chain nops (retail sched2 leaves them unfilled).
Toolchain sweep on this draft: 2.8.1 best (2.8.0-plain 38, 2.7.2 73, cdk 87).

## Lane exeH (2026-09-28): 28 → 16 aligned lines (still parked)
Best draft `build/lanes/exeH/drafts/func_8007CEAC.best.c`. Same env as exeG: 2.8.1 `-O2 -G0 -mno-split-addresses
-fno-schedule-insns`, FILL_CALL_DELAY_SLOT, and K5 from the private `build/lanes/exeG/maspsx`.
1. **Intr-arm registers solved** by reusing the loop counter `i` as the byte temp:
   `p = D_8009B348; i = p[2]; p[2] = i | (1 << ch);` (and `& ~` in the else arm). `i` is live in the poll loop too,
   so it is a global pseudo. Local-alloc then cannot tie the `or` result to operand 1 (combine_regs rejects a ureg
   with no qty) and ties it to the shift instead. The result goes to `$2` (the reused `li $2,1`), p to `$3`, and
   global-alloc puts `i` in `$4`, as retail does. Each arm does its own `sb`, and post-reload cross-jumping merges
   the two stores into retail's shared `sb $2,2($3)` (28 → 19).
2. **t-chain nops solved**: put `t = *(unsigned int *)D_8009B348` (through `tp`) inside **both** arms. sched2 runs
   before cross-jumping, so the t-chain sits at the end of each arm block and sched2 has nothing to fill its load
   delays with. Cross-jumping then merges `sb; lui; lw; lw; sw t` into one tail. This is retail's unfilled
   `lw / nop / lw / nop / sw` (19 → 16).
Remaining 16: the `tp` probe word (`addiu $6,$29,16`; `tp` is still needed for the epilogue store order, and
dropping it also makes reorg fill the head `beq` slot from the target, `li $2,1`, instead of retail's `lui $6,1`,
probably because `$6` is then never written again); the DMA-tail register allocation (retail shift amount `$6`,
bit `$3`, const/dma `$5`, `ch<<4` `$2`, DPCR value `$6`; ours `$2`/`$3`/`$2`/`$5`/`$4`/`$2`), which also leaves the
`blocks << 16` / `| words` pair out of the load-delay slots. Tried on the new draft: every legal order of the 5
tail statements (plus a `words |= blocks << 16` pre-merge), each with sched1 on and off (best is the current
order with sched1 off); bit written inline into the RMW (4 forms); 6 `dma` address spellings (all identical after cse).

## Lane exeI (2026-09-28): 16 → 15 aligned lines; the `tp` probe is gone (still parked)
Best draft `build/lanes/exeI/drafts/func_8007CEAC.best.c`. Env as exeH (`build/lanes/exeH/rc.sh`: 2.8.1 `-O2 -G0 -mno-split-addresses
-fno-schedule-insns`, FILL_CALL_DELAY_SLOT, K5 from exeG's private maspsx).
1. **Epilogue order without `tp`**: plain `t = …` stores, with `t = *dma; __asm__ volatile("");` at the end. The empty volatile asm
   is a sched2 barrier, so the restores cannot be scheduled above the final `sw t`. That gives retail's `sw $2,16($29)` before the
   `lw $31…` restores and drops the `addiu $6,$29,16` probe word (16 → 15).
2. **The head `beq` slot and the tail allocation are one residual.** Without `tp`, reorg fills the loop-exit `beq` slot from the
   target (`li $2,1`), not with retail's fall-through `lui $6,1`. Reorg's `mark_target_live_regs` only proves `$6` dead at the
   target when it finds a write to `$6` there. In retail that write is the tail's `sll $6,$16,2` (shift amount in `$6`). So once the
   DMA tail allocates `ch*4+3` into `$6`, the head slot should follow.
3. Tail allocation reasoning: retail's s=`$6` means `$2..$5` were all occupied by local qtys during s's life (h `$2`, one/bit `$3`,
   D_8009B344 pointer `$4`, const `$5`). Its DPCR value in `$6` needs `$2` held by the `blocks<<16|words` value across the RMW,
   and sched2 (backward) then sinks it into the load-delay slots. Brute-forced 288 tail orders (`build/lanes/exeI/permc2.py`,
   `permc3.py`): statements dma / bit / `q = D_8009B344` / early `w = (blocks<<16)|words` / RMW, the RMW spelled 3 ways, and bit
   either inline or via a reused `v = ch*4+3` (one pseudo covering s and the DPCR value). Best is still 15 (TAPR and TAR orders).
   Reusing `v` does move the DPCR value off `$2` (to `$5`, because `dma` is a global pseudo and local-alloc does not see it).
   sched1 on: 29.

## RESOLVED (lane exeJ, 2026-09-28): EXACT, see REPORT.md
The fix: sched1 ON, a global `v`, do-while(0) ch-ref weighting, `bit = 1; bit <<= v;`, and the K5 knob (landed).
