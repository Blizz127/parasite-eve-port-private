# `func_800CCEBC` — exact six-argument callback forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **388**.

## Function hood first

- File `[0xBD6BC,0xBD6E8)`, VA `[0x800CCEBC,0x800CCEE8)`: `0x2C`, eleven
  words.
- Canonical return is `jr ra` at `0x800CCEE0`; frame teardown at
  `0x800CCEE4` is its live delay slot.
- Exact-start callback-table reference: file `0xD16A4`, VA `0x800E0EA4`, is
  `.word func_800CCEBC` inside `D_800E0E90`.
- Preceding real `func_800CCBA8` restores `$s0` and a 72-byte frame, then ends
  at `0xBD6B4/0xBD6B8` with `jr ra; nop`.
- Following real `func_800CCEE8` starts at `0xBD6E8` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_REFERENCE_AND_CANONICAL_RETURN`.
This span is callable code by independent retail evidence, not by its label or
similarity to earlier members.

## Retail body and screens

```text
BD6BC 800CCEBC xxxxxxxx  addiu sp,sp,-0x20
BD6C0 800CCEC0 xxxxxxxx  lw    v0,0x30(sp)
BD6C4 800CCEC4 xxxxxxxx  lw    v1,0x34(sp)
BD6C8 800CCEC8 xxxxxxxx  sw    ra,0x18(sp)
BD6CC 800CCECC xxxxxxxx  sw    v0,0x10(sp)
BD6D0 800CCED0 xxxxxxxx  jal   func_800C2AF0
BD6D4 800CCED4 xxxxxxxx  sw    v1,0x14(sp)
BD6D8 800CCED8 xxxxxxxx  lw    ra,0x18(sp)
BD6DC 800CCEDC xxxxxxxx  addu  v0,zero,zero
BD6E0 800CCEE0 xxxxxxxx  jr    ra
BD6E4 800CCEE4 xxxxxxxx  addiu sp,sp,0x20
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, matched `func_800C2AF0`; all six callback arguments are forwarded, then zero is returned |
| Stage-0 globals | no symbolic global in the wrapper; accepted callee evidence proves mutable `unsigned int *D_800E2248`, published and indexed by the callee |
| Coloring pressure | low; register arguments remain in `$a0`-`$a3`; stack arguments 5/6 use `$v0/$v1` |
| `$v0` liveness | carries incoming argument 5 before call, is call-clobbered, then receives zero |
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

int func_800CCEBC(unsigned int *base, int unused, int index,
                  unsigned int value, int arg4, int arg5) {
    func_800C2AF0(base, unused, index, value, arg4, arg5);
    return 0;
}
```

```text
xxxxxxxx <func_800CCEBC>:
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

The former active asm span was `[0xBC7C4,0xBD780)` = `0xFBC`:

```text
asm prefix: 0xBD6BC - 0xBC7C4 = 0xEF8
C leaf:     0xBD6E8 - 0xBD6BC = 0x02C
asm resume: 0xBD780 - 0xBD6E8 = 0x098
closure:    0xEF8 + 0x02C + 0x098 = 0xFBC
```

The carve creates `BD6E8.s`; every size is retail boundary arithmetic, not
aligned object size.

## Packed span and gates

```text
file    VA        retail   candidate  role
BD6AC 800CCEAC  xxxxxxxx xxxxxxxx   preceding saved-register restore
BD6B0 800CCEB0  xxxxxxxx xxxxxxxx   preceding frame teardown
BD6B4 800CCEB4  xxxxxxxx xxxxxxxx   preceding return
BD6B8 800CCEB8  xxxxxxxx xxxxxxxx   preceding return delay
BD6BC 800CCEBC  xxxxxxxx xxxxxxxx   leaf 1
BD6C0 800CCEC0  xxxxxxxx xxxxxxxx   leaf 2
BD6C4 800CCEC4  xxxxxxxx xxxxxxxx   leaf 3
BD6C8 800CCEC8  xxxxxxxx xxxxxxxx   leaf 4
BD6CC 800CCECC  xxxxxxxx xxxxxxxx   leaf 5
BD6D0 800CCED0  xxxxxxxx xxxxxxxx   leaf 6
BD6D4 800CCED4  xxxxxxxx xxxxxxxx   leaf 7
BD6D8 800CCED8  xxxxxxxx xxxxxxxx   leaf 8
BD6DC 800CCEDC  xxxxxxxx xxxxxxxx   leaf 9
BD6E0 800CCEE0  xxxxxxxx xxxxxxxx   leaf 10
BD6E4 800CCEE4  xxxxxxxx xxxxxxxx   leaf 11
BD6E8 800CCEE8  xxxxxxxx xxxxxxxx   following real entry
BD6EC 800CCEEC  xxxxxxxx xxxxxxxx   following prologue save
BD6F0 800CCEF0  xxxxxxxx xxxxxxxx   following global address
BD6F4 800CCEF4  xxxxxxxx xxxxxxxx   following global address low
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
C conversion: Phase 5HD-12850 — 388 leaves

grep matching-C count: 388
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
