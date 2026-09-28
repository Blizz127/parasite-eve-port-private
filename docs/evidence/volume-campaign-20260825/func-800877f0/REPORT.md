# `func_800877F0` — exact matching-C per-voice SPU setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 299, matched on the first bounded phrasing as the structural twin of
`func_800877D4`.

## Function hood and retail screens

Retail span `[0x77FF0,0x7800C)`, VRAM `0x800877F0`, seven words. It ends in
`jr ra` plus `nop`; both boundaries are real instructions. The pool records
one direct caller/reference. `FUNCTION_HOOD=PROVEN`.

```text
sll  a0,a0,4
srl  a1,a1,3
lui  at,0x1F80
addu at,a0,at
sh   a1,0x1C0E(at)
jr   ra
nop
```

## C and flags

```c
void func_800877F0(int a0, unsigned int a1) {
    *(volatile unsigned short *)(0x1F801C0E + (a0 << 4)) =
        (unsigned short)(a1 >> 3);
}
```

Compiled with `era_compile ... -O2 -G0`; the proven natural indexed volatile
store matched all seven words on the first attempt.

## Packed span and carve

```text
candidate: xxxxxxxx xxxxxxxx 801f013c xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
retail:   xxxxxxxx xxxxxxxx 801f013c xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
```

```text
C leaf:     0x7800C - 0x77FF0 = 0x1C
resume asm: 0x7B31C - 0x7800C = 0x3310
closure:    0x1C + 0x3310 = 0x332C
```

Build candidate SHA-1:
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
`scripts/verify_us.sh` passed; matching-C count is 299.
