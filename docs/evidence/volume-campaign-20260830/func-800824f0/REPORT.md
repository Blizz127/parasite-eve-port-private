# `func_800824F0` — exact slot-3 forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 368 and Tier-2 continuation rung 27.

## Function hood

- File `[0x72CF0,0x72D14)`, VA `[0x800824F0,0x80082514)`: nine words,
  ending in canonical `jr ra; nop`.
- Seven direct `jal` callers target the exact start: `0x8007A2C8`,
  `0x80080F2C`, `0x800812B4`, `0x8008135C`, `0x8008139C`,
  `0x80081EDC`, and `0x80081EF4`.
- Preceding real `func_800824DC` ends immediately at `0x72CE8/0x72CEC` with
  `jr ra` and a live store delay slot. Following real `func_80082514` begins
  immediately at `0x72D14` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`.

## Retail, screens, and C

```text
72CF0 800824F0 xxxxxxxx  addiu sp,sp,-0x18
72CF4 800824F4 xxxxxxxx  sw    ra,0x10(sp)
72CF8 800824F8 xxxxxxxx  addu  a1,a0,zero
72CFC 800824FC xxxxxxxx  jal   func_80073CF4
72D00 80082500 xxxxxxxx  addiu a0,zero,3
72D04 80082504 xxxxxxxx  lw    ra,0x10(sp)
72D08 80082508 xxxxxxxx  addiu sp,sp,0x18
72D0C 8008250C xxxxxxxx  jr    ra
72D10 80082510 xxxxxxxx  nop
```

| Screen | Result |
|---|---|
| Callee | one unresolved `func_80073CF4` call; original argument becomes argument 2 and constant slot 3 becomes argument 1 |
| Stage-0 | no direct globals; effects belong to the callee |
| Coloring / `$v0` | only `$ra`; call result dead |
| Address retention | none |
| `-O` signal | era `-O2` moves `$a0` to `$a1` before the call and fills its delay slot with constant 3 |
| Loop | none |

```c
void func_80073CF4(int slot, unsigned int value);

void func_800824F0(unsigned int value) {
    func_80073CF4(3, value);
}
```

Flags: era `-O2 -G0`; no maspsx gate.

## Single-leaf object

```text
xxxxxxxx <func_800824F0>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  move  a1,a0
   c: xxxxxxxx  jal   0                 R_MIPS_26 func_80073CF4
  10: xxxxxxxx  li    a0,3
  14: xxxxxxxx  lw    ra,16(sp)
  18: xxxxxxxx  addiu sp,sp,24
  1c: xxxxxxxx  jr    ra
  20: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=9/9
```

GNU as aligns the standalone object to `0x30`; all `0x0C` bytes beyond the
boundary-derived `0x24` body are zero, and the guarded trim accepts only that
zero alignment.

## Carve geometry

Former asm `[0x72ABC,0x72D34)` = `0x0278`:

```text
prefix: 0x72CF0 - 0x72ABC = 0x0234
leaf:   0x72D14 - 0x72CF0 = 0x0024
resume: 0x72D34 - 0x72D14 = 0x0020
close:  0x0234 + 0x0024 + 0x0020 = 0x0278
```

## Packed span and gates

```text
72CE0: xxxxxxxx = xxxxxxxx  preceding address low
72CE4: xxxxxxxx = xxxxxxxx  preceding load
72CE8: xxxxxxxx = xxxxxxxx  preceding return
72CEC: xxxxxxxx = xxxxxxxx  preceding live delay store
72CF0: xxxxxxxx = xxxxxxxx  leaf 1
72CF4: xxxxxxxx = xxxxxxxx  leaf 2
72CF8: xxxxxxxx = xxxxxxxx  leaf 3
72CFC: xxxxxxxx = xxxxxxxx  leaf 4
72D00: xxxxxxxx = xxxxxxxx  leaf 5
72D04: xxxxxxxx = xxxxxxxx  leaf 6
72D08: xxxxxxxx = xxxxxxxx  leaf 7
72D0C: xxxxxxxx = xxxxxxxx  leaf 8
72D10: xxxxxxxx = xxxxxxxx  leaf 9
72D14: xxxxxxxx = xxxxxxxx  following real prologue
72D18: xxxxxxxx = xxxxxxxx  following word 2
72D1C: xxxxxxxx = xxxxxxxx  following call
72D20: xxxxxxxx = xxxxxxxx  following call delay
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 368
```
