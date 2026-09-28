# func_800740D0 — not carved

VRAM `0x800740D0`, size `0x148` (82 words). Next ordinary C-expressible
function after the recorded instruction blockers (`multu`-only
`func_80078CC4`, `mfhi`-before-`mflo`, trapping `addi`, `jr`/`jalr $t2`).

Best link on era `-O2 -G0`, base pinned `asm("$5")` and the slot pointer
pinned `asm("$4")`: **64 words**, linked 320 bytes (retail 328).
`MASPSX_FILL_EPILOGUE_DELAY_SLOT` did not change that count. cc1 2.8.1
`-mno-split-addresses` was 70.

The first five words match (frame, `$s1`/`$s2` moves). The next mismatch is
at `0x800740EC`: retail computes `slot = &D_800945E8[idx]` and only then
saves `$ra`/`$s4`/`$s3`/`$s0`; cc1 saves those four before the `sll`/`addu`.
The guard halfword at `D_800945E8-4` and the mask at `+0x2C` are the same
retail loads when the base stays in `$a1`. A separate `andi $s3,$v1,0xFFFF`
after the `I_MASK` `lhu` is still dropped, and the two mask stores (through
`base-4+0x30` and `base+0x2C`) collapse to one `sh`.

Not integrated. No exact-rebuild was run for this attempt.


## exeD round 2 (2026-09-27)

- Best: 12 diff lines (59 mismatches) under 2.8.1 -O2 -G0 -mno-split-addresses -fno-schedule-insns.
- Draft: `build/lanes/exeD/drafts/func_800740D0.c` (if present).
- Semantics: libetc interrupt-callback setter over D_800945E4 {u16 active; void *cb[11]; u16 mask @0x30}, I_MASK through the volatile pointer D_80095674. Levers (PARK had 64w on 2.7.2): 2.8.1 + -fno-schedule-insns (prologue order); `register` pins idx $17 / cb $18 / m $19; tab pinned $5 and slot pinned $4 with an empty asm barrier on slot (cse otherwise replaces the $4 var with a temp); `IntrTab *t = tab - 4` for the flag and the set-arm mask (retail's `addiu a2,a1,-4`); single `out: return old` via gotos. Left: the shift temp (`sll v0` vs `sll a0`), retail's separate `andi s3,v1,0xffff` zero-extend of the volatile I_MASK read, and the clear arm (`sw zero` where cse substitutes the known-zero cb register; `li v0,1` rematerialised vs hoisted).

## exeD5 round (2026-09-28)
- Natural global-struct forms (`extern IntrEnv D_800945E4;` direct fields, or `IntrEnv *e = &D_800945E4;
  void **slot = &e->cb[idx]`) score 40–57/82 on 2.7.2 and on 2.8.1 ± `-mno-split-addresses`. None reproduces
  retail's `la $5,D_800945E8` base with the flag at `-4($5)` and `$6 = $5-4` for the set-arm mask. The pinned exeD draft
  (70/82, 2.8.1 `-fno-schedule-insns`) is still the best. Not pursued further.
