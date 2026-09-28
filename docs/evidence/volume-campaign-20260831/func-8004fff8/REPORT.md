# `func_8004FFF8` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**408**.

## Hood and screens

- File `[0x407F8,0x40820)`, VA `[0x8004FFF8,0x80050020)`, ten words.
- `0x3C69C/0x3C6A0` materializes the exact start and `0x3C6A4` stores it to
  callback field `0x30($v0)`.
- Canonical return is `0x40818/0x4081C`; real predecessor ends at `0x407F4`
  and already-matched real successor `func_80050020` starts at `0x40820`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
407F8 xxxxxxxx  addiu sp,sp,-0x18
407FC xxxxxxxx  sw    ra,0x10(sp)
40800 xxxxxxxx  lui   a1,%hi(func_80050D20)
40804 xxxxxxxx  addiu a1,a1,%lo(func_80050D20)
40808 xxxxxxxx  jal   func_800638D8
4080C xxxxxxxx  nop
40810 xxxxxxxx  lw    ra,0x10(sp)
40814 xxxxxxxx  addiu sp,sp,0x18
40818 xxxxxxxx  jr    ra
4081C xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and `func_80050D20` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded result |
| retention | none |
| `-O` | compact era `-O2` shape |
| loop | none |

No gp access selects `-G0`.

## C, isolated object, and carve

```c
void func_80050D20(void);
void func_800638D8(int slot, void (*callback)(void));
void func_8004FFF8(int slot) { func_800638D8(slot, func_80050D20); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050D20; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
```

No pins, asm, fabricated nops, or maspsx gates.

```text
old span [0x407F8,0x40820) = 0x28
C:        0x40820 - 0x407F8 = 0x28
resume:   none; next segment already starts at 0x40820
closure:  0x28 = 0x28
```

## Packed span and gates

```text
file    retail   candidate  role
407E8   xxxxxxxx xxxxxxxx   predecessor restore
407EC   xxxxxxxx xxxxxxxx   predecessor teardown
407F0   xxxxxxxx xxxxxxxx   predecessor return
407F4   xxxxxxxx xxxxxxxx   predecessor delay
407F8   xxxxxxxx xxxxxxxx   leaf 1
407FC   xxxxxxxx xxxxxxxx   leaf 2
40800   xxxxxxxx xxxxxxxx   leaf 3
40804   xxxxxxxx xxxxxxxx   leaf 4
40808   xxxxxxxx xxxxxxxx   leaf 5
4080C   xxxxxxxx xxxxxxxx   leaf 6
40810   xxxxxxxx xxxxxxxx   leaf 7
40814   xxxxxxxx xxxxxxxx   leaf 8
40818   xxxxxxxx xxxxxxxx   leaf 9
4081C   xxxxxxxx xxxxxxxx   leaf 10
40820   xxxxxxxx xxxxxxxx   successor entry
40824   xxxxxxxx xxxxxxxx   successor body
40828   xxxxxxxx xxxxxxxx   successor body
4082C   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT

BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 408
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
