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
