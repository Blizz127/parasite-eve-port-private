# `func_8007FC88` — exact SDK `CD_ready` mode-1 wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **382**.

## Function hood first

- File `[0x70488,0x704AC)`, VA `[0x8007FC88,0x8007FCAC)`: nine words.
- Canonical return is `jr ra; nop` at `0x8007FCA4/0x8007FCA8`.
- Exact direct caller `0x8007F618` targets the start and passes zero in `$a0`.
- Preceding real `func_8007FC64` ends immediately at `0x70480/0x70484`.
- Following real `func_8007FCAC` begins at `0x704AC` with `lui/lw`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL_AND_CANONICAL_RETURN`. Hood was checked
independently despite the adjacent wrapper having the same shape.

## Retail body and screens

```text
70488 8007FC88 xxxxxxxx  addiu sp,sp,-0x18
7048C 8007FC8C xxxxxxxx  sw    ra,0x10(sp)
70490 8007FC90 xxxxxxxx  addu  a1,a0,zero
70494 8007FC94 xxxxxxxx  jal   func_8007B290
70498 8007FC98 xxxxxxxx  addiu a0,zero,1
7049C 8007FC9C xxxxxxxx  lw    ra,0x10(sp)
704A0 8007FCA0 xxxxxxxx  addiu sp,sp,0x18
704A4 8007FCA4 xxxxxxxx  jr    ra
704A8 8007FCA8 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, `func_8007B290`; retail string evidence and `sdk_map.md` identify PsyQ `CD_ready` |
| Stage-0 globals | none in the wrapper; CD state and result-buffer effects belong to the callee |
| Coloring pressure | only `$ra` is preserved; `$a0` moves directly to `$a1` |
| `$v0` liveness | callee result returns directly in `$v0` |
| Address retention | none |
| `-O` signal | established era `-O2` 24-byte one-call frame and fixed delay-slot argument |
| Loop/back-edge | none |

No `$gp` access is present, so the campaign rule selects `-G0`.

## Minimal C and single-leaf comparison

```c
int func_8007B290(int mode, unsigned char *result);

int func_8007FC88(unsigned char *result) {
    return func_8007B290(1, result);
}
```

```text
xxxxxxxx <func_8007FC88>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  addu  a1,a0,zero
   c: xxxxxxxx  jal   0                 R_MIPS_26 func_8007B290
  10: xxxxxxxx  addiu a0,zero,1
  14: xxxxxxxx  lw    ra,16(sp)
  18: xxxxxxxx  addiu sp,sp,24
  1c: xxxxxxxx  jr    ra
  20: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_26 func_8007B290
BYTE_EXACT=9/9
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The active asm span was exactly `[0x70488,0x704AC)`:

```text
asm prefix: 0x70488 - 0x70488 = 0x00
C leaf:     0x704AC - 0x70488 = 0x24
asm resume: 0x704AC - 0x704AC = 0x00
closure:    0x00 + 0x24 + 0x00 = 0x24
```

This consumes `70488.s`; sizes are retail boundary arithmetic, not object
alignment.

## Packed-span and gates

```text
file    VA        retail   candidate  role
70478 8007FC78  xxxxxxxx xxxxxxxx   preceding epilogue load
7047C 8007FC7C  xxxxxxxx xxxxxxxx   preceding frame teardown
70480 8007FC80  xxxxxxxx xxxxxxxx   preceding return
70484 8007FC84  xxxxxxxx xxxxxxxx   preceding return delay
70488 8007FC88  xxxxxxxx xxxxxxxx   leaf 1
7048C 8007FC8C  xxxxxxxx xxxxxxxx   leaf 2
70490 8007FC90  xxxxxxxx xxxxxxxx   leaf 3
70494 8007FC94  xxxxxxxx xxxxxxxx   leaf 4
70498 8007FC98  xxxxxxxx xxxxxxxx   leaf 5
7049C 8007FC9C  xxxxxxxx xxxxxxxx   leaf 6
704A0 8007FCA0  xxxxxxxx xxxxxxxx   leaf 7
704A4 8007FCA4  xxxxxxxx xxxxxxxx   leaf 8
704A8 8007FCA8  xxxxxxxx xxxxxxxx   leaf 9
704AC 8007FCAC  xxxxxxxx xxxxxxxx   following real entry
704B0 8007FCB0  xxxxxxxx xxxxxxxx   following global load
704B4 8007FCB4  xxxxxxxx xxxxxxxx   following return
704B8 8007FCB8  xxxxxxxx xxxxxxxx   following return delay
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

VERIFY_RC=0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 382 leaves

grep matching-C count: 382
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
