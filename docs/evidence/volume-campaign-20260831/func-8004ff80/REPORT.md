# `func_8004FF80` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**405**.

## Hood and screens

- File `[0x40780,0x407A8)`, VA `[0x8004FF80,0x8004FFA8)`, ten words.
- `0x3B880/0x3B884` materializes the exact start and `0x3B88C` stores it
  into callback field `0x30($a0)`.
- Canonical return is `0x407A0/0x407A4`; real neighboring wrappers end at
  `0x4077C` and start at `0x407A8`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
40780 xxxxxxxx  addiu sp,sp,-0x18
40784 xxxxxxxx  sw    ra,0x10(sp)
40788 xxxxxxxx  lui   a1,%hi(func_80050CB4)
4078C xxxxxxxx  addiu a1,a1,%lo(func_80050CB4)
40790 xxxxxxxx  jal   func_800638D8
40794 xxxxxxxx  nop
40798 xxxxxxxx  lw    ra,0x10(sp)
4079C xxxxxxxx  addiu sp,sp,0x18
407A0 xxxxxxxx  jr    ra
407A4 xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and `func_80050CB4` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded call result |
| address retention | none |
| `-O` | compact era `-O2` frame/materialization |
| loop | none |

No gp access selects `-G0`.

## C, object, and carve

```c
void func_80050CB4(void);
void func_800638D8(int slot, void (*callback)(void));
void func_8004FF80(int slot) { func_800638D8(slot, func_80050CB4); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050CB4; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
```

No pins, asm, fabricated nops, or maspsx gates.

```text
old span [0x40780,0x40820) = 0xA0
C:      0x407A8 - 0x40780 = 0x28
resume: 0x40820 - 0x407A8 = 0x78
closure: 0x28 + 0x78 = 0xA0
```

## Packed span and gates

```text
file    retail   candidate  role
40770   xxxxxxxx xxxxxxxx   predecessor restore
40774   xxxxxxxx xxxxxxxx   predecessor teardown
40778   xxxxxxxx xxxxxxxx   predecessor return
4077C   xxxxxxxx xxxxxxxx   predecessor delay
40780   xxxxxxxx xxxxxxxx   leaf 1
40784   xxxxxxxx xxxxxxxx   leaf 2
40788   xxxxxxxx xxxxxxxx   leaf 3
4078C   xxxxxxxx xxxxxxxx   leaf 4
40790   xxxxxxxx xxxxxxxx   leaf 5
40794   xxxxxxxx xxxxxxxx   leaf 6
40798   xxxxxxxx xxxxxxxx   leaf 7
4079C   xxxxxxxx xxxxxxxx   leaf 8
407A0   xxxxxxxx xxxxxxxx   leaf 9
407A4   xxxxxxxx xxxxxxxx   leaf 10
407A8   xxxxxxxx xxxxxxxx   successor entry
407AC   xxxxxxxx xxxxxxxx   successor body
407B0   xxxxxxxx xxxxxxxx   successor body
407B4   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT

BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 405
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
