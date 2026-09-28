# `func_800CE118` — exact six-argument callback forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **390**.

## Function hood first

- File `[0xBE918,0xBE944)`, VA `[0x800CE118,0x800CE144)`: `0x2C`, eleven
  words.
- Canonical return is `jr ra` at `0x800CE13C`; frame teardown at
  `0x800CE140` is its live delay slot.
- Exact-start callback-table reference: file `0xD17E8`, VA `0x800E0FE8`, is
  `.word func_800CE118` inside `D_800E0FD8`.
- Preceding real `func_800CE084` ends at `0xBE910/0xBE914` with `jr ra` and
  its live 24-byte frame teardown.
- Following real `func_800CE144` starts at `0xBE944` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_REFERENCE_AND_CANONICAL_RETURN`.
This span is callable code by independent retail evidence, not by its label or
similarity to earlier members.

## Retail body and screens

```text
BE918 800CE118 xxxxxxxx  addiu sp,sp,-0x20
BE91C 800CE11C xxxxxxxx  lw    v0,0x30(sp)
BE920 800CE120 xxxxxxxx  lw    v1,0x34(sp)
BE924 800CE124 xxxxxxxx  sw    ra,0x18(sp)
BE928 800CE128 xxxxxxxx  sw    v0,0x10(sp)
BE92C 800CE12C xxxxxxxx  jal   func_800C2AF0
BE930 800CE130 xxxxxxxx  sw    v1,0x14(sp)
BE934 800CE134 xxxxxxxx  lw    ra,0x18(sp)
BE938 800CE138 xxxxxxxx  addu  v0,zero,zero
BE93C 800CE13C xxxxxxxx  jr    ra
BE940 800CE140 xxxxxxxx  addiu sp,sp,0x20
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, matched `func_800C2AF0`; all six callback arguments are forwarded, then zero is returned |
| Stage-0 globals | no symbolic global in the wrapper; accepted callee evidence proves mutable `unsigned int *D_800E2248`, published and indexed by the callee |
| Coloring pressure | low; register arguments remain in `$a0`-`$a3`; stack arguments 5/6 use `$v0/$v1` |
| `$v0` liveness | carries incoming argument 5 before the call, is call-clobbered, then receives zero |
| Address retention | none in the wrapper; the callee owns base advancement and access |
| `-O` signal | compact 32-byte call frame, call-delay stack store, and return-delay teardown are established era `-O2` output |
| Loop/back-edge | none |

The paired stack copies prove a six-formal callback ABI even though the
callee's matched semantic body currently ignores those last two arguments.
No `$gp` access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
int func_800C2AF0(unsigned int *base, int unused, int index,
                  unsigned int value, int arg4, int arg5);

int func_800CE118(unsigned int *base, int unused, int index,
                  unsigned int value, int arg4, int arg5) {
    func_800C2AF0(base, unused, index, value, arg4, arg5);
    return 0;
}
```

```text
xxxxxxxx <func_800CE118>:
   0: xxxxxxxx  addiu sp,sp,-32
   4: xxxxxxxx  lw    v0,48(sp)
   8: xxxxxxxx  lw    v1,52(sp)
   c: xxxxxxxx  sw    ra,24(sp)
  10: xxxxxxxx  sw    v0,16(sp)
  14: xxxxxxxx  jal   0                 R_MIPS_26 func_800C2AF0
  18: xxxxxxxx  sw    v1,20(sp)
  1c: xxxxxxxx  lw    ra,24(sp)
  20: xxxxxxxx  move  v0,zero
  24: xxxxxxxx  jr    ra
  28: xxxxxxxx  addiu sp,sp,32

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_26 func_800C2AF0
BYTE_EXACT=11/11
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The former active asm span was `[0xBE74C,0xBE9DC)` = `0x290`:

```text
asm prefix: 0xBE918 - 0xBE74C = 0x1CC
C leaf:     0xBE944 - 0xBE918 = 0x02C
asm resume: 0xBE9DC - 0xBE944 = 0x098
closure:    0x1CC + 0x02C + 0x098 = 0x290
```

The carve creates `BE944.s`; every size is retail boundary arithmetic, not
aligned object size.

## Packed span and gates

```text
file    VA        retail   candidate  role
BE908 800CE108  xxxxxxxx xxxxxxxx   preceding ra restore
BE90C 800CE10C  xxxxxxxx xxxxxxxx   preceding zero result
BE910 800CE110  xxxxxxxx xxxxxxxx   preceding return
BE914 800CE114  xxxxxxxx xxxxxxxx   preceding frame teardown
BE918 800CE118  xxxxxxxx xxxxxxxx   leaf 1
BE91C 800CE11C  xxxxxxxx xxxxxxxx   leaf 2
BE920 800CE120  xxxxxxxx xxxxxxxx   leaf 3
BE924 800CE124  xxxxxxxx xxxxxxxx   leaf 4
BE928 800CE128  xxxxxxxx xxxxxxxx   leaf 5
BE92C 800CE12C  xxxxxxxx xxxxxxxx   leaf 6
BE930 800CE130  xxxxxxxx xxxxxxxx   leaf 7
BE934 800CE134  xxxxxxxx xxxxxxxx   leaf 8
BE938 800CE138  xxxxxxxx xxxxxxxx   leaf 9
BE93C 800CE13C  xxxxxxxx xxxxxxxx   leaf 10
BE940 800CE140  xxxxxxxx xxxxxxxx   leaf 11
BE944 800CE144  xxxxxxxx xxxxxxxx   following real entry
BE948 800CE148  xxxxxxxx xxxxxxxx   following prologue save
BE94C 800CE14C  xxxxxxxx xxxxxxxx   following global address
BE950 800CE150  xxxxxxxx xxxxxxxx   following global address low
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
C conversion: Phase 5HD-12850 — 390 leaves

grep matching-C count: 390
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
