# func_80070D10 — LINK_EXACT

VRAM `0x80070D10`, size `0x5C` (23 words). Host asm `0x60C1C` ended at the
already-matched `func_80070D6C` (`0x6156C`); the carve consumes that tail.

Retail seeds the lagged-Fibonacci table at `0x80070E0C` (`[16]=1`, `[15]=2`,
then 15 backward sums) and stores index words `0x80070E04=0x40`,
`0x80070E08=0x10` (the second store is the `jr` delay).

`-O1 -G0`. An empty `asm` barrier on the table pointer keeps the stores as
`sw` through `$t0` (`64($8)` / `60($8)`). Two maspsx knobs, both
load-bearing (parent re-link):

- `MASPSX_ORI_SMALL_LI=1` — `li` of `1`, `2`, `14`, `0x40`, `0x10` stays
  `ori $r,$zero,imm`. Without it, 5 words (`addiu` vs `ori`).
- `MASPSX_BNEZ_POSTDEC_FROM_MINUS_ONE=1` — deletes the hoisted `li $v0,-1`
  and turns `addu $n,$n,-1` / `bne $n,$v0` / delay-`sw` into `sw` /
  `bne $n,$zero` / delay-`addu`. Without it, 16 words.

```
MASPSX_ORI_SMALL_LI=1 MASPSX_BNEZ_POSTDEC_FROM_MINUS_ONE=1 \
  python3 tools/analysis/era_link_check.py src/func_80070D10.c 0x80070D10 0x5C -O1 -G0
```

`LINK_EXACT`, `nonzero_pad=0` (linked 96 bytes, declared `0x5C`).
`LEAF_STRONG=EXACT func_80070D10 span=0x5C compiled=0x60`.
Profile `era_o1_g0_bnez_postdec_from_minus_one_ori_small_li`.
