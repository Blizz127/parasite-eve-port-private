# `func_80050204` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**411**.

## Hood and screens

- File `[0x40A04,0x40A2C)`, VA `[0x80050204,0x8005022C)`, ten words.
- `0x3FD1C/0x3FD20` materializes its exact start and `0x3FD28` stores it to
  callback field `0x30($s2)`.
- Canonical return is `0x40A24/0x40A28`; real predecessor ends at
  `0x409FC/0x40A00`, and real `func_8005022C` starts at `0x40A2C`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
40A04 xxxxxxxx  addiu sp,sp,-0x18
40A08 xxxxxxxx  sw    ra,0x10(sp)
40A0C xxxxxxxx  lui   a1,%hi(func_80051060)
40A10 xxxxxxxx  addiu a1,a1,%lo(func_80051060)
40A14 xxxxxxxx  jal   func_800638D8
40A18 xxxxxxxx  nop
40A1C xxxxxxxx  lw    ra,0x10(sp)
40A20 xxxxxxxx  addiu sp,sp,0x18
40A24 xxxxxxxx  jr    ra
40A28 xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and already-matched `func_80051060` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded result |
| retention | none |
| `-O` | compact era `-O2` shape |
| loop | none |

No gp access selects `-G0`.

## C, isolated object, and carve

```c
void func_80051060(void);
void func_800638D8(int slot, void (*callback)(void));
void func_80050204(int slot) { func_800638D8(slot, func_80051060); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80051060; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
```

No pins, asm, fabricated nops, or maspsx gates.

```text
old span [0x408A8,0x40A60) = 0x1B8
prefix: 0x40A04 - 0x408A8 = 0x15C
C:      0x40A2C - 0x40A04 = 0x028
resume: 0x40A60 - 0x40A2C = 0x034
closure: 0x15C + 0x028 + 0x034 = 0x1B8
```

## Packed span and gates

```text
file    retail   candidate  role
409F4   xxxxxxxx xxxxxxxx   predecessor restore
409F8   xxxxxxxx xxxxxxxx   predecessor teardown
409FC   xxxxxxxx xxxxxxxx   predecessor return
40A00   xxxxxxxx xxxxxxxx   predecessor delay
40A04   xxxxxxxx xxxxxxxx   leaf 1
40A08   xxxxxxxx xxxxxxxx   leaf 2
40A0C   xxxxxxxx xxxxxxxx   leaf 3
40A10   xxxxxxxx xxxxxxxx   leaf 4
40A14   xxxxxxxx xxxxxxxx   leaf 5
40A18   xxxxxxxx xxxxxxxx   leaf 6
40A1C   xxxxxxxx xxxxxxxx   leaf 7
40A20   xxxxxxxx xxxxxxxx   leaf 8
40A24   xxxxxxxx xxxxxxxx   leaf 9
40A28   xxxxxxxx xxxxxxxx   leaf 10
40A2C   xxxxxxxx xxxxxxxx   successor entry
40A30   xxxxxxxx xxxxxxxx   successor gp load
40A34   xxxxxxxx xxxxxxxx   successor body
40A38   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT

BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 411
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
