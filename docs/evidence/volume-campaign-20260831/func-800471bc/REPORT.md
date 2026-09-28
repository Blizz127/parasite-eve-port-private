# `func_800471BC` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **394**.

## Function hood first

- File `[0x379BC,0x379E4)`, VA `[0x800471BC,0x800471E4)`: `0x28`, ten
  words.
- Two independent exact-start references materialize `func_800471BC` at
  files `0x37650/0x37654` and `0x3F594/0x3F598`, then store it into callback
  field `+0x30` of separate allocated objects.
- Canonical return is `jr ra` at `0x379DC` with `nop` at `0x379E0`.
- Preceding real `func_80047040` ends at `0x379B4/0x379B8` with `jr ra; nop`.
- Following real `func_800471E4` starts at `0x379E4` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_TWO_INDEPENDENT_EXACT_START_CALLBACK_STORES_AND_CANONICAL_RETURN`.

## Retail body and screens

```text
379BC 800471BC xxxxxxxx  addiu sp,sp,-0x18
379C0 800471C0 xxxxxxxx  sw    ra,0x10(sp)
379C4 800471C4 xxxxxxxx  lui   a1,%hi(func_80047040)
379C8 800471C8 xxxxxxxx  addiu a1,a1,%lo(func_80047040)
379CC 800471CC xxxxxxxx  jal   func_800638D8
379D0 800471D0 xxxxxxxx  nop
379D4 800471D4 xxxxxxxx  lw    ra,0x10(sp)
379D8 800471D8 xxxxxxxx  addiu sp,sp,0x18
379DC 800471DC xxxxxxxx  jr    ra
379E0 800471E0 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct unresolved callee, `func_800638D8`; incoming slot `$a0` is forwarded and `func_80047040` is supplied as callback `$a1` |
| Stage-0 globals | none in the wrapper; callback ownership is proven by the exact-start stores in its object users |
| Coloring pressure | minimal; `$a0` is untouched and the symbolic callback occupies `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact frame and stable symbolic function-pointer materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_80047040(void);
void func_800638D8(int slot, void (*callback)(void));

void func_800471BC(int slot) {
    func_800638D8(slot, func_80047040);
}
```

```text
xxxxxxxx <func_800471BC>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_80047040
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_80047040
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_80047040; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The former active asm span was `[0x340EC,0x3C708)` = `0x861C`:

```text
asm prefix: 0x379BC - 0x340EC = 0x38D0
C leaf:     0x379E4 - 0x379BC = 0x0028
asm resume: 0x3C708 - 0x379E4 = 0x4D24
closure:    0x38D0 + 0x0028 + 0x4D24 = 0x861C
```

The carve creates `379E4.s`; sizes derive only from retail boundaries.

## Packed span and gates

```text
file    VA        retail   candidate  role
379AC 800471AC  xxxxxxxx xxxxxxxx   preceding s0 restore
379B0 800471B0  xxxxxxxx xxxxxxxx   preceding teardown
379B4 800471B4  xxxxxxxx xxxxxxxx   preceding return
379B8 800471B8  xxxxxxxx xxxxxxxx   preceding delay
379BC 800471BC  xxxxxxxx xxxxxxxx   leaf 1
379C0 800471C0  xxxxxxxx xxxxxxxx   leaf 2
379C4 800471C4  xxxxxxxx xxxxxxxx   leaf 3
379C8 800471C8  xxxxxxxx xxxxxxxx   leaf 4
379CC 800471CC  xxxxxxxx xxxxxxxx   leaf 5
379D0 800471D0  xxxxxxxx xxxxxxxx   leaf 6
379D4 800471D4  xxxxxxxx xxxxxxxx   leaf 7
379D8 800471D8  xxxxxxxx xxxxxxxx   leaf 8
379DC 800471DC  xxxxxxxx xxxxxxxx   leaf 9
379E0 800471E0  xxxxxxxx xxxxxxxx   leaf 10
379E4 800471E4  xxxxxxxx xxxxxxxx   following real entry
379E8 800471E8  xxxxxxxx xxxxxxxx   following body
379EC 800471EC  xxxxxxxx xxxxxxxx   following body
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
C conversion: Phase 5HD-12850 — 394 leaves

grep matching-C count: 394
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
