# func_80051CC4 — resource command-state initializer (0x80051CC4, 0x134)

Status: **matched** (decompA draft, tooling closure by tooling2, 2026-09-24),
`LEAF_STRONG=EXACT` on the live install. Previously excluded at 424a2c3a
("deep-link says it does not match retail").

- YAML: `- [0x424C4, c, func_80051CC4]` (after `[0x42180, asm]`; ends exactly at `[0x425F8, c, func_80051DF8]`).
- Profile: existing `era_o2_g8_dispatch_800111f8` (`-O2 -G8` + `MASPSX_THREE_WORD_SYMBOL_STORE=1` + `MASPSX_DISPATCH_FOLD=jtbl_800111F8`).

## Residual closed by tooling
cc1 hoists the switch table's address out of the loop as `la $14,$L20` and indexes
it with `addu`/`lw 0($2)`; MASPSX_DISPATCH_FOLD only retargeted the indexed
`lw $r,$L<n>($b)` form, leaving 2 words (retail 0x80051D2C/30 `lui $t6,0x8001` /
`addiu $t6,$t6,0x11F8` = jtbl_800111F8). The fold's new `la` form (only for
`$L<n>` defined in .rdata) closes it; the existing dispatch strip then removes
the 32-byte duplicate table.

## Verification
```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_80051CC4
  -> disc1_preflight: PASS (deep, 1887 c / 284 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_80051CC4
  -> dispatch dedup: stripped 32-byte duplicate table
  -> LEAF_STRONG=EXACT func_80051CC4 span=0x134 vram=0x80051CC4 compiled=0x140 failures=[none]
```
