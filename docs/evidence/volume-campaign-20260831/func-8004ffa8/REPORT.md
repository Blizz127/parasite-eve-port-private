# `func_8004FFA8` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**406**.

## Hood and screens

- File `[0x407A8,0x407D0)`, VA `[0x8004FFA8,0x8004FFD0)`, ten words.
- One of several independent references materializes its exact start at
  `0x38908/0x3890C` and stores it to callback field `0x30($s1)` at `0x38914`.
- Canonical return is `0x407C8/0x407CC`; real wrappers end at `0x407A4` and
  start at `0x407D0`.

`FUNCTION_HOOD=PROVEN_BY_INDEPENDENT_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
407A8 xxxxxxxx  addiu sp,sp,-0x18
407AC xxxxxxxx  sw    ra,0x10(sp)
407B0 xxxxxxxx  lui   a1,%hi(func_80050CF8)
407B4 xxxxxxxx  addiu a1,a1,%lo(func_80050CF8)
407B8 xxxxxxxx  jal   func_800638D8
407BC xxxxxxxx  nop
407C0 xxxxxxxx  lw    ra,0x10(sp)
407C4 xxxxxxxx  addiu sp,sp,0x18
407C8 xxxxxxxx  jr    ra
407CC xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and `func_80050CF8` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded result |
| retention | none |
| `-O` | compact era `-O2` shape |
| loop | none |

No gp access selects `-G0`.

## C, isolated object, and carve

```c
void func_80050CF8(void);
void func_800638D8(int slot, void (*callback)(void));
void func_8004FFA8(int slot) { func_800638D8(slot, func_80050CF8); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050CF8; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
```

No pins, asm, fabricated nops, or maspsx gates.

```text
old span [0x407A8,0x40820) = 0x78
C:      0x407D0 - 0x407A8 = 0x28
resume: 0x40820 - 0x407D0 = 0x50
closure: 0x28 + 0x50 = 0x78
```

## Packed span and gates

```text
file    retail   candidate  role
40798   xxxxxxxx xxxxxxxx   predecessor restore
4079C   xxxxxxxx xxxxxxxx   predecessor teardown
407A0   xxxxxxxx xxxxxxxx   predecessor return
407A4   xxxxxxxx xxxxxxxx   predecessor delay
407A8   xxxxxxxx xxxxxxxx   leaf 1
407AC   xxxxxxxx xxxxxxxx   leaf 2
407B0   xxxxxxxx xxxxxxxx   leaf 3
407B4   xxxxxxxx xxxxxxxx   leaf 4
407B8   xxxxxxxx xxxxxxxx   leaf 5
407BC   xxxxxxxx xxxxxxxx   leaf 6
407C0   xxxxxxxx xxxxxxxx   leaf 7
407C4   xxxxxxxx xxxxxxxx   leaf 8
407C8   xxxxxxxx xxxxxxxx   leaf 9
407CC   xxxxxxxx xxxxxxxx   leaf 10
407D0   xxxxxxxx xxxxxxxx   successor entry
407D4   xxxxxxxx xxxxxxxx   successor body
407D8   xxxxxxxx xxxxxxxx   successor body
407DC   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT

BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 406
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
