# `func_8004FF30` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED**, natural phrasing 1, era `-O2 -G0`; matching-C leaf
**403**.

## Hood, retail, and screens

- File `[0x40730,0x40758)`, VA `[0x8004FF30,0x8004FF58)`, ten words.
- `0x3B5E8/0x3B5EC` materializes its exact start and `0x3B5F4` stores it to
  callback field `0x30($s1)` in the `func_80062CB8` call delay slot.
- Body ends `jr ra; nop` at `0x40750/0x40754`.
- Real `func_8004FEEC` ends at `0x40728/0x4072C`; real `func_8004FF58`
  begins at `0x40758`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
40730 xxxxxxxx  addiu sp,sp,-0x18
40734 xxxxxxxx  sw    ra,0x10(sp)
40738 xxxxxxxx  lui   a1,%hi(func_80050C50)
4073C xxxxxxxx  addiu a1,a1,%lo(func_80050C50)
40740 xxxxxxxx  jal   func_800638D8
40744 xxxxxxxx  nop
40748 xxxxxxxx  lw    ra,0x10(sp)
4074C xxxxxxxx  addiu sp,sp,0x18
40750 xxxxxxxx  jr    ra
40754 xxxxxxxx  nop
```

| screen | result |
|---|---|
| callee | one call to `func_800638D8`, forwarding `$a0` and `func_80050C50` |
| Stage-0 globals | none |
| coloring | minimal |
| `$v0` liveness | discarded result; void wrapper |
| address retention | none |
| optimization | compact era `-O2` frame/materialization |
| loop | none |

No gp access selects `-G0`.

## C and object

```c
void func_80050C50(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004FF30(int slot) {
    func_800638D8(slot, func_80050C50);
}
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: R_MIPS_HI16/R_MIPS_LO16 func_80050C50; R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10; PHRASINGS_USED=1/2
```

No pins, asm, fabricated nops, or maspsx behavior gates.

## Carve, packed span, and gates

```text
old span: [0x401A0,0x40820) = 0x680
prefix:   0x40730 - 0x401A0 = 0x590
C:        0x40758 - 0x40730 = 0x028
resume:   0x40820 - 0x40758 = 0x0C8
closure:  0x590 + 0x028 + 0x0C8 = 0x680
```

```text
file    retail   candidate  role
40720   xxxxxxxx xxxxxxxx   predecessor restore
40724   xxxxxxxx xxxxxxxx   predecessor teardown
40728   xxxxxxxx xxxxxxxx   predecessor return
4072C   xxxxxxxx xxxxxxxx   predecessor delay
40730   xxxxxxxx xxxxxxxx   leaf 1
40734   xxxxxxxx xxxxxxxx   leaf 2
40738   xxxxxxxx xxxxxxxx   leaf 3
4073C   xxxxxxxx xxxxxxxx   leaf 4
40740   xxxxxxxx xxxxxxxx   leaf 5
40744   xxxxxxxx xxxxxxxx   leaf 6
40748   xxxxxxxx xxxxxxxx   leaf 7
4074C   xxxxxxxx xxxxxxxx   leaf 8
40750   xxxxxxxx xxxxxxxx   leaf 9
40754   xxxxxxxx xxxxxxxx   leaf 10
40758   xxxxxxxx xxxxxxxx   successor entry
4075C   xxxxxxxx xxxxxxxx   successor body
40760   xxxxxxxx xxxxxxxx   successor body
40764   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; compare: EXACT MATCH; matching-C count: 403
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
