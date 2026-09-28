# `func_8008777C` — exact matching-C hardware halfword setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 297, matched on the first bounded phrasing using the proven one-base
volatile MMIO idiom.

## Function hood and retail screens

Retail span `[0x77F7C,0x77F98)`, VRAM `0x8008777C`, seven words. It ends in
`jr ra` plus `nop`; both boundaries are real instructions. The pool records
one direct caller/reference in the contiguous hardware-setter cluster.
`FUNCTION_HOOD=PROVEN`.

Retail body:

```text
lui at,0x1F80
sh  a0,0x1D90(at)
srl a0,a0,16
lui at,0x1F80
sh  a0,0x1D92(at)
jr  ra
nop
```

## C and flags

```c
void func_8008777C(unsigned int a0) {
    volatile unsigned short *base = (volatile unsigned short *)0x1F800000;
    base[0xEC8] = (unsigned short)a0;
    base[0xEC9] = (unsigned short)(a0 >> 16);
}
```

Compiled with `era_compile ... -O2 -G0`; the proven one-base MMIO form
reproduced the `$at` materialization and both stores on the first attempt.

## Packed span and carve

```text
candidate: 801f013c xxxxxxxx xxxxxxxx 801f013c xxxxxxxx xxxxxxxx xxxxxxxx
retail:   801f013c xxxxxxxx xxxxxxxx 801f013c xxxxxxxx xxxxxxxx xxxxxxxx
```

```text
asm prefix: 0x77F7C - 0x77F60 = 0x1C
C leaf:     0x77F98 - 0x77F7C = 0x1C
resume asm: 0x77FBC - 0x77F98 = 0x24
closure:    0x1C + 0x1C + 0x24 = 0x5C
```

Build candidate SHA-1:
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
`scripts/verify_us.sh` passed; matching-C count is 297.
