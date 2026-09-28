# `func_800474A8` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **396**.

## Function hood first

- File `[0x37CA8,0x37CD0)`, VA `[0x800474A8,0x800474D0)`: `0x28`, ten
  words.
- Files `0x37C34/0x37C38` materialize the exact start and file `0x37C40`
  stores it into callback field `+0x30`.
- Canonical return is `jr ra` at `0x37CC8`, with `nop` at `0x37CCC`.
- Preceding real `func_800473E4` ends at `0x37CA0/0x37CA4` with `jr ra; nop`.
- Following real `func_800474D0` starts at `0x37CD0` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

## Retail body and screens

```text
37CA8 800474A8 xxxxxxxx  addiu sp,sp,-0x18
37CAC 800474AC xxxxxxxx  sw    ra,0x10(sp)
37CB0 800474B0 xxxxxxxx  lui   a1,%hi(func_80050308)
37CB4 800474B4 xxxxxxxx  addiu a1,a1,%lo(func_80050308)
37CB8 800474B8 xxxxxxxx  jal   func_800638D8
37CBC 800474BC xxxxxxxx  nop
37CC0 800474C0 xxxxxxxx  lw    ra,0x10(sp)
37CC4 800474C4 xxxxxxxx  addiu sp,sp,0x18
37CC8 800474C8 xxxxxxxx  jr    ra
37CCC 800474CC xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct unresolved callee, `func_800638D8`; incoming slot `$a0` is forwarded and `func_80050308` is callback `$a1` |
| Stage-0 globals | none; this wrapper only forwards a callback and slot |
| Coloring pressure | minimal; unchanged `$a0`, symbolic function pointer in `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact frame and stable callback materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_80050308(void);
void func_800638D8(int slot, void (*callback)(void));

void func_800474A8(int slot) {
    func_800638D8(slot, func_80050308);
}
```

```text
xxxxxxxx <func_800474A8>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_80050308
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_80050308
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_80050308; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The prior active asm span was `[0x37B54,0x3C708)` = `0x4BB4`:

```text
asm prefix: 0x37CA8 - 0x37B54 = 0x0154
C leaf:     0x37CD0 - 0x37CA8 = 0x0028
asm resume: 0x3C708 - 0x37CD0 = 0x4A38
closure:    0x0154 + 0x0028 + 0x4A38 = 0x4BB4
```

The carve creates `37CD0.s`; sizes derive only from retail boundaries.

## Packed span and gates

```text
file    VA        retail   candidate  role
37C98 80047498  xxxxxxxx xxxxxxxx   preceding s0 restore
37C9C 8004749C  xxxxxxxx xxxxxxxx   preceding teardown
37CA0 800474A0  xxxxxxxx xxxxxxxx   preceding return
37CA4 800474A4  xxxxxxxx xxxxxxxx   preceding delay
37CA8 800474A8  xxxxxxxx xxxxxxxx   leaf 1
37CAC 800474AC  xxxxxxxx xxxxxxxx   leaf 2
37CB0 800474B0  xxxxxxxx xxxxxxxx   leaf 3
37CB4 800474B4  xxxxxxxx xxxxxxxx   leaf 4
37CB8 800474B8  xxxxxxxx xxxxxxxx   leaf 5
37CBC 800474BC  xxxxxxxx xxxxxxxx   leaf 6
37CC0 800474C0  xxxxxxxx xxxxxxxx   leaf 7
37CC4 800474C4  xxxxxxxx xxxxxxxx   leaf 8
37CC8 800474C8  xxxxxxxx xxxxxxxx   leaf 9
37CCC 800474CC  xxxxxxxx xxxxxxxx   leaf 10
37CD0 800474D0  xxxxxxxx xxxxxxxx   following real entry
37CD4 800474D4  xxxxxxxx xxxxxxxx   following body
37CD8 800474D8  xxxxxxxx xxxxxxxx   following body
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
C conversion: Phase 5HD-12850 — 396 leaves

grep matching-C count: 396
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
