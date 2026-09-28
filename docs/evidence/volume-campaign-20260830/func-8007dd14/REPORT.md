# `func_8007DD14` — exact slot-4 forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 369 and Tier-2 continuation rung 28.

## Function hood

- File `[0x6E514,0x6E538)`, VA `[0x8007DD14,0x8007DD38)`: nine words,
  ending in canonical `jr ra; nop`.
- Two unique direct callers target the exact start: `0x8007D184` and
  `0x800859B8`.
- Preceding real `func_8007DCAC` returns at `0x6E500/0x6E504`, followed by
  three explicit alignment nops. After this leaf are three more alignment
  nops, then real handwritten tail helper `func_8007DD44` at `0x6E544`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`; alignment is kept outside the leaf.

## Retail, screens, and C

```text
6E514 8007DD14 xxxxxxxx  addiu sp,sp,-0x18
6E518 8007DD18 xxxxxxxx  sw    ra,0x10(sp)
6E51C 8007DD1C xxxxxxxx  addu  a1,a0,zero
6E520 8007DD20 xxxxxxxx  jal   func_80073CF4
6E524 8007DD24 xxxxxxxx  addiu a0,zero,4
6E528 8007DD28 xxxxxxxx  lw    ra,0x10(sp)
6E52C 8007DD2C xxxxxxxx  addiu sp,sp,0x18
6E530 8007DD30 xxxxxxxx  jr    ra
6E534 8007DD34 xxxxxxxx  nop
```

| Screen | Result |
|---|---|
| Callee | one unresolved `func_80073CF4` call; original argument becomes argument 2 and constant slot 4 becomes argument 1 |
| Stage-0 | no direct globals; effects belong to the callee |
| Coloring / `$v0` | only `$ra`; call result dead |
| Address retention | none |
| `-O` signal | era `-O2` argument shuffle and call-delay constant materialization |
| Loop | none |

```c
void func_80073CF4(int slot, unsigned int value);

void func_8007DD14(unsigned int value) {
    func_80073CF4(4, value);
}
```

Flags: era `-O2 -G0`; no maspsx gate.

## Single-leaf object

```text
xxxxxxxx <func_8007DD14>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  move  a1,a0
   c: xxxxxxxx  jal   0                 R_MIPS_26 func_80073CF4
  10: xxxxxxxx  li    a0,4
  14: xxxxxxxx  lw    ra,16(sp)
  18: xxxxxxxx  addiu sp,sp,24
  1c: xxxxxxxx  jr    ra
  20: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=9/9
```

The standalone object has `0x0C` zero alignment bytes after the
boundary-derived `0x24` body; the guarded trim accepted only those zeros.

## Carve geometry

Former asm `[0x6CD60,0x6E6A4)` = `0x1944`:

```text
prefix: 0x6E514 - 0x6CD60 = 0x17B4
leaf:   0x6E538 - 0x6E514 = 0x0024
resume: 0x6E6A4 - 0x6E538 = 0x016C
close:  0x17B4 + 0x0024 + 0x016C = 0x1944
```

## Packed span and gates

```text
6E500: xxxxxxxx = xxxxxxxx  preceding return
6E504: xxxxxxxx = xxxxxxxx  preceding teardown delay
6E508: xxxxxxxx = xxxxxxxx  preceding alignment 1
6E50C: xxxxxxxx = xxxxxxxx  preceding alignment 2
6E510: xxxxxxxx = xxxxxxxx  preceding alignment 3
6E514: xxxxxxxx = xxxxxxxx  leaf 1
6E518: xxxxxxxx = xxxxxxxx  leaf 2
6E51C: xxxxxxxx = xxxxxxxx  leaf 3
6E520: xxxxxxxx = xxxxxxxx  leaf 4
6E524: xxxxxxxx = xxxxxxxx  leaf 5
6E528: xxxxxxxx = xxxxxxxx  leaf 6
6E52C: xxxxxxxx = xxxxxxxx  leaf 7
6E530: xxxxxxxx = xxxxxxxx  leaf 8
6E534: xxxxxxxx = xxxxxxxx  leaf 9
6E538: xxxxxxxx = xxxxxxxx  following alignment 1
6E53C: xxxxxxxx = xxxxxxxx  following alignment 2
6E540: xxxxxxxx = xxxxxxxx  following alignment 3
6E544: xxxxxxxx = xxxxxxxx  following real helper entry
6E548: xxxxxxxx = xxxxxxxx  following tail jump
6E54C: xxxxxxxx = xxxxxxxx  following delay slot
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 369
```
