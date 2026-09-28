# `func_8004FF58` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**404**.

## Hood and screens

- File `[0x40758,0x40780)`, VA `[0x8004FF58,0x8004FF80)`, ten words.
- `0x3B780/0x3B784` materializes the exact start; `0x3B78C` stores it to
  callback field `0x30($a0)` in a real registrar call's delay slot.
- Canonical return is `0x40778/0x4077C`; real neighbors end at `0x40754`
  and start at `0x40780`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
40758 xxxxxxxx  addiu sp,sp,-0x18
4075C xxxxxxxx  sw    ra,0x10(sp)
40760 xxxxxxxx  lui   a1,%hi(func_80050C70)
40764 xxxxxxxx  addiu a1,a1,%lo(func_80050C70)
40768 xxxxxxxx  jal   func_800638D8
4076C xxxxxxxx  nop
40770 xxxxxxxx  lw    ra,0x10(sp)
40774 xxxxxxxx  addiu sp,sp,0x18
40778 xxxxxxxx  jr    ra
4077C xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and `func_80050C70` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded call result |
| address retention | none |
| `-O` | compact era `-O2` shape |
| loop | none |

No gp access selects `-G0`.

## C, object, and carve

```c
void func_80050C70(void);
void func_800638D8(int slot, void (*callback)(void));
void func_8004FF58(int slot) { func_800638D8(slot, func_80050C70); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050C70; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
```

No pins, asm, fabricated nops, or maspsx gates.

```text
old span [0x40758,0x40820) = 0xC8
C:      0x40780 - 0x40758 = 0x28
resume: 0x40820 - 0x40780 = 0xA0
closure: 0x28 + 0xA0 = 0xC8
```

## Packed span and gates

```text
file    retail   candidate  role
40748   xxxxxxxx xxxxxxxx   predecessor restore
4074C   xxxxxxxx xxxxxxxx   predecessor teardown
40750   xxxxxxxx xxxxxxxx   predecessor return
40754   xxxxxxxx xxxxxxxx   predecessor delay
40758   xxxxxxxx xxxxxxxx   leaf 1
4075C   xxxxxxxx xxxxxxxx   leaf 2
40760   xxxxxxxx xxxxxxxx   leaf 3
40764   xxxxxxxx xxxxxxxx   leaf 4
40768   xxxxxxxx xxxxxxxx   leaf 5
4076C   xxxxxxxx xxxxxxxx   leaf 6
40770   xxxxxxxx xxxxxxxx   leaf 7
40774   xxxxxxxx xxxxxxxx   leaf 8
40778   xxxxxxxx xxxxxxxx   leaf 9
4077C   xxxxxxxx xxxxxxxx   leaf 10
40780   xxxxxxxx xxxxxxxx   successor entry
40784   xxxxxxxx xxxxxxxx   successor body
40788   xxxxxxxx xxxxxxxx   successor body
4078C   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT

BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 404
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
