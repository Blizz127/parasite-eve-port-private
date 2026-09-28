# func_8004A9A0 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x8004A9A0`, file offset `0x3B1A0`, size `0x3FC` (255 words), `asm/disc1/3B1A0.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "profile": "era_o2_g8 + DISPATCH_FOLD=jtbl_80010FF4,jtbl_80011014", "levers": "two jump tables folded (multi-table form works); 32-byte 2-aligned Item struct copy gives the lwl/lwr block move; plus/minus/confirm arms decoded", "note": "~150 words: frame 96 vs retail 88 (retail locals 56 bytes) and the minus-switch tail sharing (retail: each stat case 'h[i]--' in the slot of 'j tail' with CFD8 += 100 shared with the level case's fall-through; cc1 cross-jumps the stat compares instead). Draft drafts/func_8004A9A0.c"}

---

# func_8004A9A0 — PARKED (cc1 cross-jump residual)

- vram `0x8004A9A0`, file `0x3B1A0` (inside the `[0x3AECC, asm]` span), size `0x3FC`.
- Role: the input callback that `func_8004A570` installs at `obj+0x2C`
  (`func_8004A9A0(obj, pad)`): buy (`pad & 0x4000`), sell (`pad & 0x1000`),
  confirm (`pad & 0x10000`, 32-byte `Blk` copy `*blk = D_800A1A00`), cancel
  (`pad & 0x40`). Globals are the `func_8004A570` set (`D_8009CFAC/CFD0/CFD8/
  CFDC/CF68`, all `$gp` slots → `-G8`), two 7-entry switches on `D_8009CFD0`.
- Tooling now in place: multi-table fold
  `MASPSX_DISPATCH_FOLD=jtbl_80010FF4,jtbl_80011014` (+`MASPSX_THREE_WORD_SYMBOL_STORE=1`);
  both pool tables are 7 entries + one `.align 3` zero word, which the
  multi-table strip accepts. `D_800C0E20`/`D_800C0E22` must be unsized
  `signed char[]` (sized → `-G8` gp-relative → `relocation truncated`).
- Best draft: era 2.7.2 `-O2 -G8`, 149 word mismatches, 0x3C0 compiled vs 0x3FC.
  Command (draft path in the orchestrator scratchpad `drafts/func_8004A9A0.PARK.c`):
  `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010FF4,jtbl_80011014 python3 tools/analysis/era_link_check.py <draft> 0x8004A9A0 0x3FC -O2 -G8`

## Residual
Every path matches instruction-for-instruction modulo placement, but the
tail sharing differs:
- **Retail**: sell cases 0–2 each keep `lw $v0,D_8009CFD8` / `addiu $v1,$a0,-1`
  and jump to `.L8004AC14` (`addiu $v0,$v0,100` / `sw` / `jal func_8005267C` /
  return), i.e. only the identical suffix is cross-jumped into the case-5/6
  block. Buy cases 0–2 keep their own `lui/addiu/lh/slti` and share only
  `addu`/`sh`.
- **Our cc1 2.7.2**: with duplicated per-case tails in source it cross-jumps
  *nothing* (0x430 bytes); with a shared label it merges *more* than retail
  (0x3C0 bytes). No source spelling tried lands in between.
- Flags tried with no effect on either variant: `-fschedule-insns`,
  `-fno-schedule-insns2`, `-fno-thread-jumps`. The 2.7.2 cc1 has no
  cross-jump flag.
This is the `jump.c` cross-jump / follow_jumps class that the patch-queue notes
list as not reachable from the assembler, so no maspsx knob is proposed.

## Lane exeC3 (2026-09-27) — 163 raw / 115 aligned → 6 aligned-diff insns

Best draft `build/lanes/exeC3/drafts/func_8004A9A0.c` (era `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`,
`MASPSX_DISPATCH_FOLD=jtbl_80010FF4,jtbl_80011014`). Levers that landed:
1. **Frame 88**: the per-case `D_800A1A00.h[k]--` post-decrements each reserve an 8-byte spilled
   pseudo slot (vars 40 → 56 = retail); `= x - 1` forms give vars 32/40. The two-call ternary alone
   adds 8 (drop it into the `p2` form below).
2. Sell cases: `if (rec->h[k] >= D_800A1A00.h[k]) goto fail; v = D_8009CFD8_k; D_800A1A00.h[k]--; goto tail;`
   with **per-case asm-label aliases** `extern int D_8009CFD8_k asm("D_8009CFD8");` (distinct SYMBOL_REF
   pointers stop jump2 cross-jumping the per-case `lw v0,CFD8`), `register int v asm("$2")`, and
   `tail: D_8009CFD8 = v + 100; func_8005267C(); return 1;` shared with case 5/6.
3. Case 5/6: `p2 = &D_800A18D8[D_8009CFD0]; { register int c asm("$5") = D_8009CFDC; if (*p2 >= c) goto fail; }
   { register int c asm("$5"); int f = func_8005BBE4(D_8009CFD0); c = D_8009CFDC;
   D_8009CFDC = (c - f < *p2) ? D_800A18D8[D_8009CFD0] : D_8009CFDC - func_8005BBE4(D_8009CFD0); } v = D_8009CFD8;`
4. `rec == func_8005332C(...)` operand order (fixes `beq s1,v0` ×2).

Residual (6 aligned insns): jump2 jump-to-jump cross-jumps the identical `sh v1,0(a1)` of sell cases
0 and 1 into case 2's (retail keeps `j AC14; sh` in all three). At jump2 each case ends
`lw alias; addiu; sh; j` — the last insn is identical. Tried: distinct `asm volatile("#k")` after the sh
(stops the merge but blocks the delay-slot fill: 15), `asm("")` in two cases (merges those two),
stacked distinct labels tail0/1/2 (no effect), statement order h--/v (sched1 reorders anyway),
unpinned v (no merge, but v → a0 and the whole sell block rotates registers: 26), full per-case
tails (merge into case 0: 27). The spilled post-dec pseudo's `(use (reg))` lands at each case head;
a zero-code per-case distinct insn just before the jump that reorg can skip is what is missing.

## Lane exeC4 (2026-09-27) — still 6 aligned (158 raw); mechanism pinned down

Dumped `cc1 -dJRSdlgc` for the exeC3 draft. Findings:
- jump2 merges the sell cases' identical `addu v1,a0,-1; sh v1,0(a1)` tail into case 2 (both insns,
  not just the `sh`); reorg then steals the `addu` into case 0/1's `j` slot. Retail keeps
  `lw; addiu; j AC14; sh` per case, i.e. retail's jump2 saw a *distinct* last insn before each `j`.
- The per-case distinct zero-code insns in the **buy** cases are `(use (reg N))` insns emitted by
  combine's `distribute_notes` (REG_DEAD of the dead shift intermediate of the sign-extending `lh`,
  placed after the CODE_LABEL); they later become `(use (mem sp+N))` (the frame-slot lever) and stop
  jump2 there. reorg skips USE/CLOBBER when filling slots, so a USE after the `sh` would give
  retail exactly. In the **sell** cases that USE lands at the case head (the `lh` is in the head
  block), not in the tail block after the `beq`.
- sched1 knows `D_800A1A00+k` vs `D_8009CFD8` don't alias (reg_known_value), so it hoists the alias
  `lw` above the `sh`; with `-fno-schedule-insns` the `sh` is not merged but sched2 (no alias
  knowledge after reload) keeps `sh; lw` and the slot takes the target insn (23 aligned).
- Tried, all still 6 (or worse): `h[k] = h[k] - 1`, `--h[k]`, `h[k] -= 1`, `v` before the decrement,
  short/int/unsigned-short locals of the old value (int: 28), `(short)(x - 1)`, pointer local
  `p = &h[k]` (44), `!(a < b)` polarity, if-body form, the 3D94C cse boundary `Lk: z = 0; if (z) goto Lk;`
  before/after the decrement or the alias read (6; with `-fno-rerun-cse-after-loop` 44).
- Needed: a tail-block combine that leaves a REG_DEAD note with no home (so a USE is emitted after the
  `beq`), with no code. Not found.

## Lane exeC5 (2026-09-28): merge stopped, 6 words (size now exact)

- `asm("" : : "r"(v));` after `v = D_8009CFD8_k;` in each sell case (draft
  `build/lanes/exeC5/drafts/func_8004A9A0_asmuse.c`): the three asm_operands carry distinct
  source lines, so jump2 no longer cross-jumps the `addiu; sh` tails — link size is now 0x3FC and
  word mismatches drop 158 → 6. Residual (3×2 words): each case emits `addiu v1,a0,-1; lw v0,CFD8`
  where retail has `lw v0,CFD8; addiu v1,a0,-1`. sched1 hoists the `lw` in the no-asm draft (it
  has no consumer in the block); giving it a consumer (the asm) changes its priority and the
  `addiu` wins. Tried (all 6 words / 7 aligned): asm before/after the decrement, `v` first,
  two-operand use, output barrier `asm("" : "=r"(v) : "0"(v))`, use of `rec` instead of `v`,
  `volatile int` read of the alias, an extra asm on `rec` before the case body; `"memory"` clobber
  and `asm("")` bring the merge back. `-fno-schedule-insns2` 21, `-fno-schedule-insns` 160.
- Still needed: a distinct zero-code insn before each `j` that is neither a consumer of the `lw`
  nor an asm (a combine-made `(use)` as exeC4 found).
