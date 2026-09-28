# `func_80052558` — exact signed-byte return twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 371 and Tier-2 continuation rung 30.

## Function hood

- File `[0x42D58,0x42D7C)`, VA `[0x80052558,0x8005257C)`: nine words,
  ending in canonical `jr ra` with stack teardown in the delay slot.
- Three unique direct callers target the exact start: `0x80044A28`,
  `0x80045D74`, and `0x80050A04`.
- Preceding independently proven `func_80052534` and following already-C
  `func_8005257C` meet this leaf at the exact boundaries.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`.

## Retail, screens, and C

```text
42D58 80052558 xxxxxxxx  addiu sp,sp,-0x18
42D5C 8005255C xxxxxxxx  sw    ra,0x10(sp)
42D60 80052560 xxxxxxxx  jal   func_800210D4
42D64 80052564 xxxxxxxx  nop
42D68 80052568 xxxxxxxx  sll   v0,v0,24
42D6C 8005256C xxxxxxxx  lw    ra,0x10(sp)
42D70 80052570 xxxxxxxx  sra   v0,v0,24
42D74 80052574 xxxxxxxx  jr    ra
42D78 80052578 xxxxxxxx  addiu sp,sp,0x18
```

| Screen | Result |
|---|---|
| Callee | one unresolved `func_800210D4` call |
| Stage-0 | no direct globals; effects belong to the callee |
| Coloring / `$v0` | callee result stays in `$v0`; signed-byte canonicalization is live to return |
| Address retention | none |
| `-O` signal | era `-O2` schedules restore between `sll`/`sra` and teardown in return delay |
| Loop | none |

```c
int func_800210D4(void);

signed char func_80052558(void) {
    return func_800210D4();
}
```

The signed return is retail-proven by `sll 24; sra 24`. Flags: era `-O2
-G0`; no maspsx gate.

## Single-leaf object

```text
xxxxxxxx <func_80052558>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_800210D4
   c: xxxxxxxx  nop
  10: xxxxxxxx  sll   v0,v0,0x18
  14: xxxxxxxx  lw    ra,16(sp)
  18: xxxxxxxx  sra   v0,v0,0x18
  1c: xxxxxxxx  jr    ra
  20: xxxxxxxx  addiu sp,sp,24

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=9/9
```

The standalone object's bytes beyond the boundary-derived `0x24` body are
zero alignment only and pass the guarded trim.

## Carve geometry

The active asm span was exactly `[0x42D58,0x42D7C)`:

```text
leaf: 0x42D7C - 0x42D58 = 0x0024
close: 0x0024 = 0x0024
```

No aligned object size and no zero padding are included in the YAML leaf.

## Packed span and gates

```text
42D48: xxxxxxxx = xxxxxxxx  preceding restore
42D4C: xxxxxxxx = xxxxxxxx  preceding sign extension
42D50: xxxxxxxx = xxxxxxxx  preceding return
42D54: xxxxxxxx = xxxxxxxx  preceding teardown delay
42D58: xxxxxxxx = xxxxxxxx  leaf 1
42D5C: xxxxxxxx = xxxxxxxx  leaf 2
42D60: xxxxxxxx = xxxxxxxx  leaf 3
42D64: xxxxxxxx = xxxxxxxx  leaf 4
42D68: xxxxxxxx = xxxxxxxx  leaf 5
42D6C: xxxxxxxx = xxxxxxxx  leaf 6
42D70: xxxxxxxx = xxxxxxxx  leaf 7
42D74: xxxxxxxx = xxxxxxxx  leaf 8
42D78: xxxxxxxx = xxxxxxxx  leaf 9
42D7C: xxxxxxxx = xxxxxxxx  following real function entry
42D80: xxxxxxxx = xxxxxxxx  following load
42D84: xxxxxxxx = xxxxxxxx  following load-delay nop
42D88: xxxxxxxx = xxxxxxxx  following shift
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 371
```
