# `func_80036E58` — exact adjacent three-state initializer

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 378 and Tier-2 continuation rung 37.

## Function hood and screens

- File `[0x27658,0x2767C)`, VA `[0x80036E58,0x80036E7C)`: nine words,
  ending in canonical `jr ra; nop`.
- Exact direct caller `0x80036DE0` targets this start independently.
- Preceding accepted C `func_80036E34` ends immediately at `0x27654`;
  following real `func_80036E7C` starts immediately at `0x2767C`.
- No callees, loop, address retention, or result liveness. Coloring pressure
  is only the constant-one `$v0` used by the final store.
- Stage-0 finds another `D_800A76B0` writer at `0x8005D8DC`;
  `D_800A76AC` and `D_800A76B4` have no other exact-symbol executable
  references. The three absolute store pairs prove `-G0`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`.

## C and object

```c
extern int D_800A76AC;
extern int D_800A76B0;
extern int D_800A76B4;

void func_80036E58(void) {
    D_800A76B0 = 0;
    D_800A76B4 = 0;
    D_800A76AC = 1;
}
```

Flags: era `-O2 -G0`; no maspsx gate.

```text
27658: xxxxxxxx  addiu v0,zero,1
2765C: xxxxxxxx  lui   at,%hi(D_800A76B0)
27660: xxxxxxxx  sw    zero,%lo(D_800A76B0)(at)
27664: xxxxxxxx  lui   at,%hi(D_800A76B4)
27668: xxxxxxxx  sw    zero,%lo(D_800A76B4)(at)
2766C: xxxxxxxx  lui   at,%hi(D_800A76AC)
27670: xxxxxxxx  sw    v0,%lo(D_800A76AC)(at)
27674: xxxxxxxx  jr    ra
27678: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=three R_MIPS_HI16/R_MIPS_LO16 symbol pairs
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x27658,0x278A8)` = `0x0250`:

```text
leaf:   0x2767C - 0x27658 = 0x0024
resume: 0x278A8 - 0x2767C = 0x022C
close:  0x0024 + 0x022C = 0x0250
```

```text
27648: xxxxxxxx = xxxxxxxx  preceding accepted leaf
2764C: xxxxxxxx = xxxxxxxx
27650: xxxxxxxx = xxxxxxxx
27654: xxxxxxxx = xxxxxxxx
27658: xxxxxxxx = xxxxxxxx  leaf 1
2765C: xxxxxxxx = xxxxxxxx  leaf 2
27660: xxxxxxxx = xxxxxxxx  leaf 3
27664: xxxxxxxx = xxxxxxxx  leaf 4
27668: xxxxxxxx = xxxxxxxx  leaf 5
2766C: xxxxxxxx = xxxxxxxx  leaf 6
27670: xxxxxxxx = xxxxxxxx  leaf 7
27674: xxxxxxxx = xxxxxxxx  leaf 8
27678: xxxxxxxx = xxxxxxxx  leaf 9
2767C: xxxxxxxx = xxxxxxxx  following real body
27680: xxxxxxxx = xxxxxxxx
27684: xxxxxxxx = xxxxxxxx
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/build_us.sh: exit 0; Compare: EXACT SHA-1 MATCH
matching-C count: 378
```
