# `func_80050038` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**409**.

## Hood and screens

- File `[0x40838,0x40860)`, VA `[0x80050038,0x80050060)`, ten words.
- `0x38C14/0x38C18` materializes its exact start and `0x38C1C` stores it to
  callback field `0x30($a0)`.
- Canonical return is `0x40858/0x4085C`; real predecessor `func_80050020`
  ends at `0x40830/0x40834`, and real successor starts at `0x40860`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
40838 xxxxxxxx  addiu sp,sp,-0x18
4083C xxxxxxxx  sw    ra,0x10(sp)
40840 xxxxxxxx  lui   a1,%hi(func_80050DC0)
40844 xxxxxxxx  addiu a1,a1,%lo(func_80050DC0)
40848 xxxxxxxx  jal   func_800638D8
4084C xxxxxxxx  nop
40850 xxxxxxxx  lw    ra,0x10(sp)
40854 xxxxxxxx  addiu sp,sp,0x18
40858 xxxxxxxx  jr    ra
4085C xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and `func_80050DC0` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded result |
| retention | none |
| `-O` | compact era `-O2` shape |
| loop | none |

No gp access selects `-G0`.

## C, object, carve, and packed span

```c
void func_80050DC0(void);
void func_800638D8(int slot, void (*callback)(void));
void func_80050038(int slot) { func_800638D8(slot, func_80050DC0); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050DC0; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
old span [0x40838,0x40888)=0x50; C 0x28 + resume 0x28 = 0x50
```

No pins, asm, fabricated nops, or maspsx gates.

```text
file    retail   candidate  role
40828   xxxxxxxx xxxxxxxx   predecessor body
4082C   xxxxxxxx xxxxxxxx   predecessor body
40830   xxxxxxxx xxxxxxxx   predecessor return
40834   xxxxxxxx xxxxxxxx   predecessor delay
40838   xxxxxxxx xxxxxxxx   leaf 1
4083C   xxxxxxxx xxxxxxxx   leaf 2
40840   xxxxxxxx xxxxxxxx   leaf 3
40844   xxxxxxxx xxxxxxxx   leaf 4
40848   xxxxxxxx xxxxxxxx   leaf 5
4084C   xxxxxxxx xxxxxxxx   leaf 6
40850   xxxxxxxx xxxxxxxx   leaf 7
40854   xxxxxxxx xxxxxxxx   leaf 8
40858   xxxxxxxx xxxxxxxx   leaf 9
4085C   xxxxxxxx xxxxxxxx   leaf 10
40860   xxxxxxxx xxxxxxxx   successor entry
40864   xxxxxxxx xxxxxxxx   successor body
40868   xxxxxxxx xxxxxxxx   successor body
4086C   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT
BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 409
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
