# `func_8004EF30` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **399**.

## Function hood first

- File `[0x3F730,0x3F758)`, VA `[0x8004EF30,0x8004EF58)`: `0x28`, ten words.
- Files `0x391BC/0x391C0` materialize its exact start and `0x391C4` stores it
  to callback field `0x30($s4)` before the owner is passed to
  `func_80062CB8`.
- Canonical return is `jr ra` at `0x3F750`, with `nop` at `0x3F754`.
- Preceding real `func_8004ECB4` ends at `0x3F728/0x3F72C`; following real
  `func_8004EF58` starts at `0x3F758` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
391BC 800489BC 3C028005  lui   v0,%hi(func_8004EF30)
391C0 800489C0 2442EF30  addiu v0,v0,%lo(func_8004EF30)
391C4 800489C4 AE820030  sw    v0,0x30(s4)
391C8 800489C8 0C018B2E  jal   func_80062CB8
391CC 800489CC 02802021  addu  a0,s4,zero
```

## Retail body and screens

```text
3F730 8004EF30 xxxxxxxx  addiu sp,sp,-0x18
3F734 8004EF34 xxxxxxxx  sw    ra,0x10(sp)
3F738 8004EF38 xxxxxxxx  lui   a1,%hi(func_80050708)
3F73C 8004EF3C xxxxxxxx  addiu a1,a1,%lo(func_80050708)
3F740 8004EF40 xxxxxxxx  jal   func_800638D8
3F744 8004EF44 xxxxxxxx  nop
3F748 8004EF48 xxxxxxxx  lw    ra,0x10(sp)
3F74C 8004EF4C xxxxxxxx  addiu sp,sp,0x18
3F750 8004EF50 xxxxxxxx  jr    ra
3F754 8004EF54 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, `func_800638D8`; forwards slot `$a0` and callback `func_80050708` in `$a1` |
| Stage-0 globals | none in the wrapper |
| Coloring pressure | minimal; unchanged `$a0`, symbolic callback in `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact frame and callback materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_80050708(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004EF30(int slot) {
    func_800638D8(slot, func_80050708);
}
```

```text
xxxxxxxx <func_8004EF30>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_80050708
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_80050708
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_80050708; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The prior active asm span was `[0x3F17C,0x3FC48)` = `0xACC`:

```text
asm prefix: 0x3F730 - 0x3F17C = 0x5B4
C leaf:     0x3F758 - 0x3F730 = 0x028
asm resume: 0x3FC48 - 0x3F758 = 0x4F0
closure:    0x5B4 + 0x028 + 0x4F0 = 0xACC
```

All sizes derive from retail boundaries, not aligned object sizes.

## Packed span and gates

```text
file    VA        retail   candidate  role
3F720 8004EF20  xxxxxxxx xxxxxxxx   preceding restore
3F724 8004EF24  xxxxxxxx xxxxxxxx   preceding teardown
3F728 8004EF28  xxxxxxxx xxxxxxxx   preceding return
3F72C 8004EF2C  xxxxxxxx xxxxxxxx   preceding delay
3F730 8004EF30  xxxxxxxx xxxxxxxx   leaf 1
3F734 8004EF34  xxxxxxxx xxxxxxxx   leaf 2
3F738 8004EF38  xxxxxxxx xxxxxxxx   leaf 3
3F73C 8004EF3C  xxxxxxxx xxxxxxxx   leaf 4
3F740 8004EF40  xxxxxxxx xxxxxxxx   leaf 5
3F744 8004EF44  xxxxxxxx xxxxxxxx   leaf 6
3F748 8004EF48  xxxxxxxx xxxxxxxx   leaf 7
3F74C 8004EF4C  xxxxxxxx xxxxxxxx   leaf 8
3F750 8004EF50  xxxxxxxx xxxxxxxx   leaf 9
3F754 8004EF54  xxxxxxxx xxxxxxxx   leaf 10
3F758 8004EF58  xxxxxxxx xxxxxxxx   following real entry
3F75C 8004EF5C  xxxxxxxx xxxxxxxx   following body
3F760 8004EF60  xxxxxxxx xxxxxxxx   following body
3F764 8004EF64  xxxxxxxx xxxxxxxx   following body
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0
RESULT: EXACT MATCH
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

VERIFY_RC=0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 399 leaves

grep matching-C count: 399
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
