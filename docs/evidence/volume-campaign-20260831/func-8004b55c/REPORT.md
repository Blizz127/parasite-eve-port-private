# `func_8004B55C` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **398**.

## Function hood first

- File `[0x3BD5C,0x3BD84)`, VA `[0x8004B55C,0x8004B584)`: `0x28`, ten words.
- Files `0x3B9C4/0x3B9C8` independently materialize its exact start for a
  callback-field store.
- Canonical return is `jr ra` at `0x3BD7C`, with `nop` at `0x3BD80`.
- Preceding real `func_8004B534` ends at `0x3BD54/0x3BD58`; following real
  `func_8004B584` starts at `0x3BD84` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_INDEPENDENT_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

## Retail body and screens

```text
3BD5C 8004B55C xxxxxxxx  addiu sp,sp,-0x18
3BD60 8004B560 xxxxxxxx  sw    ra,0x10(sp)
3BD64 8004B564 xxxxxxxx  lui   a1,%hi(func_800504BC)
3BD68 8004B568 xxxxxxxx  addiu a1,a1,%lo(func_800504BC)
3BD6C 8004B56C xxxxxxxx  jal   func_800638D8
3BD70 8004B570 xxxxxxxx  nop
3BD74 8004B574 xxxxxxxx  lw    ra,0x10(sp)
3BD78 8004B578 xxxxxxxx  addiu sp,sp,0x18
3BD7C 8004B57C xxxxxxxx  jr    ra
3BD80 8004B580 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct unresolved callee, `func_800638D8`; forwards slot `$a0` and callback `func_800504BC` in `$a1` |
| Stage-0 globals | none in the wrapper |
| Coloring pressure | minimal; unchanged `$a0`, symbolic callback in `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact frame and callback materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_800504BC(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004B55C(int slot) {
    func_800638D8(slot, func_800504BC);
}
```

```text
xxxxxxxx <func_8004B55C>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_800504BC
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_800504BC
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_800504BC; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The prior active asm span was `[0x3BD5C,0x3C708)` = `0x9AC`:

```text
asm prefix: 0x3BD5C - 0x3BD5C = 0x000
C leaf:     0x3BD84 - 0x3BD5C = 0x028
asm resume: 0x3C708 - 0x3BD84 = 0x984
closure:    0x000 + 0x028 + 0x984 = 0x9AC
```

The asm start is consumed and replaced by C plus `3BD84.s`; sizes derive
only from retail boundaries.

## Packed span and gates

```text
file    VA        retail   candidate  role
3BD4C 8004B54C  xxxxxxxx xxxxxxxx   preceding ra restore
3BD50 8004B550  xxxxxxxx xxxxxxxx   preceding teardown
3BD54 8004B554  xxxxxxxx xxxxxxxx   preceding return
3BD58 8004B558  xxxxxxxx xxxxxxxx   preceding delay
3BD5C 8004B55C  xxxxxxxx xxxxxxxx   leaf 1
3BD60 8004B560  xxxxxxxx xxxxxxxx   leaf 2
3BD64 8004B564  xxxxxxxx xxxxxxxx   leaf 3
3BD68 8004B568  xxxxxxxx xxxxxxxx   leaf 4
3BD6C 8004B56C  xxxxxxxx xxxxxxxx   leaf 5
3BD70 8004B570  xxxxxxxx xxxxxxxx   leaf 6
3BD74 8004B574  xxxxxxxx xxxxxxxx   leaf 7
3BD78 8004B578  xxxxxxxx xxxxxxxx   leaf 8
3BD7C 8004B57C  xxxxxxxx xxxxxxxx   leaf 9
3BD80 8004B580  xxxxxxxx xxxxxxxx   leaf 10
3BD84 8004B584  xxxxxxxx xxxxxxxx   following real entry
3BD88 8004B588  xxxxxxxx xxxxxxxx   following body
3BD8C 8004B58C  xxxxxxxx xxxxxxxx   following body
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
C conversion: Phase 5HD-12850 — 398 leaves

grep matching-C count: 398
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
