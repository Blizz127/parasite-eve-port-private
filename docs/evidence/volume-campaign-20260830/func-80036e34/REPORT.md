# `func_80036E34` — exact three-state initializer

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 377 and Tier-2 continuation rung 36.

## Function hood and screens

- File `[0x27634,0x27658)`, VA `[0x80036E34,0x80036E58)`: nine words,
  ending in canonical `jr ra; nop`.
- Exact direct caller `0x80036DD8` targets the start.
- Preceding real `func_80036DF8` ends at `0x2762C/0x27630`; following real
  twin `func_80036E58` starts immediately at `0x27658`.
- No callees, loop, address retention, or `$v0` result liveness. Coloring
  pressure is only the constant-one temporary used by the final store.
- Stage-0 finds `D_800A76BC` read at `0x8005C2B8` and written at
  `0x8005C3FC` and `0x8005D8E4`; `D_800A76B8` and `D_800A76C0` have no
  other exact-symbol executable references. This leaf is a three-word state
  initializer, not padding.
- Retail's three absolute `lui $at` store pairs prove `-G0`; there is no
  constant-materialization ambiguity and no maspsx gate is needed.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`.

## C and object

```c
extern int D_800A76B8;
extern int D_800A76BC;
extern int D_800A76C0;

void func_80036E34(void) {
    D_800A76BC = 0;
    D_800A76C0 = 0;
    D_800A76B8 = 1;
}
```

Flags: era `-O2 -G0`; no maspsx gate.

```text
27634: xxxxxxxx  addiu v0,zero,1
27638: xxxxxxxx  lui   at,%hi(D_800A76BC)
2763C: xxxxxxxx  sw    zero,%lo(D_800A76BC)(at)
27640: xxxxxxxx  lui   at,%hi(D_800A76C0)
27644: xxxxxxxx  sw    zero,%lo(D_800A76C0)(at)
27648: xxxxxxxx  lui   at,%hi(D_800A76B8)
2764C: xxxxxxxx  sw    v0,%lo(D_800A76B8)(at)
27650: xxxxxxxx  jr    ra
27654: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=three R_MIPS_HI16/R_MIPS_LO16 symbol pairs
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x26C48,0x278A8)` = `0x0C60`:

```text
prefix: 0x27634 - 0x26C48 = 0x09EC
leaf:   0x27658 - 0x27634 = 0x0024
resume: 0x278A8 - 0x27658 = 0x0250
close:  0x09EC + 0x0024 + 0x0250 = 0x0C60
```

```text
27624: xxxxxxxx = xxxxxxxx  preceding real body
27628: xxxxxxxx = xxxxxxxx
2762C: xxxxxxxx = xxxxxxxx  preceding return
27630: xxxxxxxx = xxxxxxxx
27634: xxxxxxxx = xxxxxxxx  leaf 1
27638: xxxxxxxx = xxxxxxxx  leaf 2
2763C: xxxxxxxx = xxxxxxxx  leaf 3
27640: xxxxxxxx = xxxxxxxx  leaf 4
27644: xxxxxxxx = xxxxxxxx  leaf 5
27648: xxxxxxxx = xxxxxxxx  leaf 6
2764C: xxxxxxxx = xxxxxxxx  leaf 7
27650: xxxxxxxx = xxxxxxxx  leaf 8
27654: xxxxxxxx = xxxxxxxx  leaf 9
27658: xxxxxxxx = xxxxxxxx  following real twin
2765C: xxxxxxxx = xxxxxxxx
27660: xxxxxxxx = xxxxxxxx
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/build_us.sh: exit 0; Compare: EXACT SHA-1 MATCH
matching-C count: 377
```
