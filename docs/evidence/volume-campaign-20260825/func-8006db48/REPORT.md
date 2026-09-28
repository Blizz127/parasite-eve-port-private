# func_8006DB48 — matched Tier-1 leaf

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: MATCHED, leaf 335. No pins or inline assembly.

## Function hood and boundaries

The span is file [0x5E348,0x5E39C), VRAM [0x8006DB48,0x8006DB9C), 0x54
bytes / 21 words. It ends with jr ra at 0x8006DB94 and its delay-slot nop
at 0x8006DB98. Four distinct direct jal callers are present at 0x80015EC0,
0x80015F68, 0x80016158, and 0x8006DB00. The preceding boundary is the real
jr ra/nop pair at 0x8006DB40/0x8006DB44; the following function starts at
0x8006DB9C with addu a1,zero,zero. The span is therefore a callable function,
not padding or a mislabeled tail.

## Retail semantics and screens

The function writes value0 and value1 to byte fields
D_800B0CD8[index*2+0xDC] and +0xDD, and value2 to D_800B0CD8[index+0xFE].
For index zero it ORs word flag 0x40; for index one it ORs word flag 0x80;
it returns zero in all cases.

There are no callees, no loop/back-edge, and no global other than this
word-backed overlay. The screen predicts one shared symbolic base in $v1,
scaled/index-derived addresses in $v0, a zero result accumulator, and era
-O2 -G0 compilation. The shared base is retained across byte stores and flag
loads/stores. A direct byte-global spelling was rejected as a shape probe
because it repeated symbol materialization; the union overlay is the final
minimal representation.

## Final C

```c
typedef union {
    unsigned int flags;
    unsigned char bytes[0x100];
} State;

extern State D_800B0CD8;

int func_8006DB48(int index, signed char value0, unsigned char value1,
                  unsigned char value2) {
    State *base = &D_800B0CD8;
    int result = 0;

    base->bytes[index * 2 + 0xDC] = value0;
    base->bytes[index * 2 + 0xDD] = value1;
    base->bytes[index + 0xFE] = value2;

    if (index == 0) {
        base->flags |= 0x40;
    } else if (index == 1) {
        base->flags |= 0x80;
    }

    return result;
}
```

The era compiler invocation was `-O2 -G0` with the repository's established
freestanding MIPS-1 flags. No inline asm, volatile trick, pin, or forged nop
is used.

## Object comparison

Single-leaf `mipsel-linux-gnu-objdump -dr` output:

```
xxxxxxxx <func_8006DB48>:
xxxxxxxx: xxxxxxxx  lui v1,0x0       R_MIPS_HI16 D_800B0CD8
xxxxxxxx: xxxxxxxx  addiu v1,v1,0    R_MIPS_LO16 D_800B0CD8
xxxxxxxx: xxxxxxxx  sll v0,a0,0x1
xxxxxxxx: xxxxxxxx  addu v0,v0,v1
xxxxxxxx: xxxxxxxx  sb a1,220(v0)
xxxxxxxx: xxxxxxxx  sb a2,221(v0)
xxxxxxxx: xxxxxxxx  addu v0,a0,v1
xxxxxxxx: xxxxxxxx  bnez a0,0x30
xxxxxxxx: xxxxxxxx  sb a3,254(v0)
xxxxxxxx: xxxxxxxx  lw v0,0(v1)
xxxxxxxx: xxxxxxxx  j 0x48             R_MIPS_26 .text
xxxxxxxx: xxxxxxxx  ori v0,v0,0x40
xxxxxxxx: xxxxxxxx  li v0,1
xxxxxxxx: xxxxxxxx  bne a0,v0,0x4c
xxxxxxxx: xxxxxxxx  nop
xxxxxxxx: xxxxxxxx  lw v0,0(v1)
xxxxxxxx: xxxxxxxx  nop
xxxxxxxx: xxxxxxxx  ori v0,v0,0x80
xxxxxxxx: xxxxxxxx  sw v0,0(v1)
xxxxxxxx: xxxxxxxx  jr ra
xxxxxxxx: xxxxxxxx  move v0,zero
```

The 21 words after relocation normalization are:

```
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx
```

Every non-relocatable word agrees with retail. The two symbol words and the
local jump normalize to D_800B0CD8 and 0x8006DB90; no mismatch remains.

## Carve and packed span

The former assembly segment was [0x5B1E4,0x5EFE8):

```
prefix:  0x5E348 - 0x5B1E4 = 0x3164
C leaf:  0x5E39C - 0x5E348 = 0x0054
resume:  0x5EFE8 - 0x5E39C = 0x0C4C
closure: 0x3164 + 0x0054 + 0x0C4C = 0x3E04
```

Packed-span objdump words, from two preceding boundary words through four
following words:

```
0x5E340: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
0x5E350: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
0x5E360: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
0x5E370: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
0x5E380: xxxxxxxx xxxxxxxx xxxxxxxx 80004234
0x5E390: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
0x5E3A0: xxxxxxxx
```

The preceding jr ra/nop, all 21 leaf words, and the following function's
first instruction are identical to retail.

## Gates

```
candidate SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT SHA-1 MATCH
scripts/verify_us.sh: exit 0
C conversion: Phase 5HD-12850 — 335 leaves
```

The authoritative YAML count is 335 matching-C spans; the refreshed pool has
1081 total candidates and 21 Tier-1 candidates remaining.
