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
