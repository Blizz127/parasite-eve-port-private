# `func_8004B534` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **397**.

## Function hood first

- File `[0x3BD34,0x3BD5C)`, VA `[0x8004B534,0x8004B55C)`: `0x28`, ten words.
- Files `0x3B9A8/0x3B9AC` materialize the exact start and its owner stores it
  into callback field `+0x30`.
- Canonical return is `jr ra` at `0x3BD54`, with `nop` at `0x3BD58`.
- Preceding real `func_8004B394` ends at `0x3BD2C/0x3BD30` with `jr ra; nop`.
- Following real `func_8004B55C` starts at `0x3BD5C` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

## Retail body and screens

```text
3BD34 8004B534 xxxxxxxx  addiu sp,sp,-0x18
3BD38 8004B538 xxxxxxxx  sw    ra,0x10(sp)
3BD3C 8004B53C xxxxxxxx  lui   a1,%hi(func_80050438)
3BD40 8004B540 xxxxxxxx  addiu a1,a1,%lo(func_80050438)
3BD44 8004B544 xxxxxxxx  jal   func_800638D8
3BD48 8004B548 xxxxxxxx  nop
3BD4C 8004B54C xxxxxxxx  lw    ra,0x10(sp)
3BD50 8004B550 xxxxxxxx  addiu sp,sp,0x18
3BD54 8004B554 xxxxxxxx  jr    ra
3BD58 8004B558 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct unresolved callee, `func_800638D8`; forwards slot `$a0` and callback `func_80050438` in `$a1` |
| Stage-0 globals | none in the wrapper |
| Coloring pressure | minimal; unchanged `$a0`, symbolic callback in `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact call frame and symbolic callback materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_80050438(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004B534(int slot) {
    func_800638D8(slot, func_80050438);
}
```

```text
xxxxxxxx <func_8004B534>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_80050438
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_80050438
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_80050438; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The prior active asm span was `[0x37CD0,0x3C708)` = `0x4A38`:

```text
asm prefix: 0x3BD34 - 0x37CD0 = 0x4064
C leaf:     0x3BD5C - 0x3BD34 = 0x0028
asm resume: 0x3C708 - 0x3BD5C = 0x09AC
closure:    0x4064 + 0x0028 + 0x09AC = 0x4A38
```

The carve creates `3BD5C.s`; sizes derive only from retail boundaries.

## Packed span and gates

```text
file    VA        retail   candidate  role
3BD24 8004B524  xxxxxxxx xxxxxxxx   preceding s0 restore
3BD28 8004B528  xxxxxxxx xxxxxxxx   preceding teardown
3BD2C 8004B52C  xxxxxxxx xxxxxxxx   preceding return
3BD30 8004B530  xxxxxxxx xxxxxxxx   preceding delay
3BD34 8004B534  xxxxxxxx xxxxxxxx   leaf 1
3BD38 8004B538  xxxxxxxx xxxxxxxx   leaf 2
3BD3C 8004B53C  xxxxxxxx xxxxxxxx   leaf 3
3BD40 8004B540  xxxxxxxx xxxxxxxx   leaf 4
3BD44 8004B544  xxxxxxxx xxxxxxxx   leaf 5
3BD48 8004B548  xxxxxxxx xxxxxxxx   leaf 6
3BD4C 8004B54C  xxxxxxxx xxxxxxxx   leaf 7
3BD50 8004B550  xxxxxxxx xxxxxxxx   leaf 8
3BD54 8004B554  xxxxxxxx xxxxxxxx   leaf 9
3BD58 8004B558  xxxxxxxx xxxxxxxx   leaf 10
3BD5C 8004B55C  xxxxxxxx xxxxxxxx   following real entry
3BD60 8004B560  xxxxxxxx xxxxxxxx   following prologue
3BD64 8004B564  xxxxxxxx xxxxxxxx   following body
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0
RESULT: EXACT MATCH
Compare: EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

VERIFY_RC=0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 397 leaves

grep matching-C count: 397
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
