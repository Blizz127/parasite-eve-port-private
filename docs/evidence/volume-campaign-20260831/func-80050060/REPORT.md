# `func_80050060` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**410**.

## Hood and screens

- File `[0x40860,0x40888)`, VA `[0x80050060,0x80050088)`, ten words.
- `0x39480/0x39484` materializes its exact start; branch-delay instruction
  `0x3948C` stores it to callback field `0x30($s4)`.
- Canonical return is `0x40880/0x40884`; real predecessor ends at `0x4085C`
  and already-matched real successor `func_80050088` starts at `0x40888`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
40860 xxxxxxxx  addiu sp,sp,-0x18
40864 xxxxxxxx  sw    ra,0x10(sp)
40868 xxxxxxxx  lui   a1,%hi(func_80050E70)
4086C xxxxxxxx  addiu a1,a1,%lo(func_80050E70)
40870 xxxxxxxx  jal   func_800638D8
40874 xxxxxxxx  nop
40878 xxxxxxxx  lw    ra,0x10(sp)
4087C xxxxxxxx  addiu sp,sp,0x18
40880 xxxxxxxx  jr    ra
40884 xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one `func_800638D8`; forwards `$a0` and `func_80050E70` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` | discarded result |
| retention | none |
| `-O` | compact era `-O2` shape |
| loop | none |

No gp access selects `-G0`.

## C, object, carve, and packed span

```c
void func_80050E70(void);
void func_800638D8(int slot, void (*callback)(void));
void func_80050060(int slot) { func_800638D8(slot, func_80050E70); }
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050E70; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
old span [0x40860,0x40888)=0x28; C consumes all 0x28; no resume
```

No pins, asm, fabricated nops, or maspsx gates.

```text
file    retail   candidate  role
40850   xxxxxxxx xxxxxxxx   predecessor restore
40854   xxxxxxxx xxxxxxxx   predecessor teardown
40858   xxxxxxxx xxxxxxxx   predecessor return
4085C   xxxxxxxx xxxxxxxx   predecessor delay
40860   xxxxxxxx xxxxxxxx   leaf 1
40864   xxxxxxxx xxxxxxxx   leaf 2
40868   xxxxxxxx xxxxxxxx   leaf 3
4086C   xxxxxxxx xxxxxxxx   leaf 4
40870   xxxxxxxx xxxxxxxx   leaf 5
40874   xxxxxxxx xxxxxxxx   leaf 6
40878   xxxxxxxx xxxxxxxx   leaf 7
4087C   xxxxxxxx xxxxxxxx   leaf 8
40880   xxxxxxxx xxxxxxxx   leaf 9
40884   xxxxxxxx xxxxxxxx   leaf 10
40888   xxxxxxxx xxxxxxxx   successor entry
4088C   xxxxxxxx xxxxxxxx   successor body
40890   xxxxxxxx xxxxxxxx   successor call
40894   xxxxxxxx xxxxxxxx   successor delay
PACKED_SPAN=EXACT
BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 410
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
