# `func_80080B04` — exact zero-status wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 356 and Tier-2
continuation rung 15.

## Function hood and retail span

- File `[0x71304,0x71324)`, VRAM `[0x80080B04,0x80080B24)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x80080B1C`, with frame teardown in its live
  delay slot at `0x80080B20`.
- One exact direct caller exists at `0x80080FF0`; it loads two state words into
  `$a0/$a1` at `0x80080FE8/0x80080FEC` and calls this exact start.
- Preceding real `func_80080AE4` ends at file `0x712FC/0x71300`; following
  real `func_80080B24` begins immediately at file `0x71324`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`; this is not padding, data, or a tail
entry.

## Retail body

```text
71304 80080B04 xxxxxxxx  addiu sp,sp,-0x18
71308 80080B08 xxxxxxxx  sw    ra,0x10(sp)
7130C 80080B0C xxxxxxxx  jal   func_8007C044
71310 80080B10 xxxxxxxx  nop
71314 80080B14 xxxxxxxx  lw    ra,0x10(sp)
71318 80080B18 xxxxxxxx  sltiu v0,v0,1
7131C 80080B1C xxxxxxxx  jr    ra
71320 80080B20 xxxxxxxx  addiu sp,sp,0x18
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_8007C044`; it performs an ordered hardware/global transaction using both forwarded arguments and returns zero on its normal completion path |
| Stage-0 globals | none accessed directly by the wrapper; all global and hardware effects belong to the callee |
| Coloring pressure | none beyond preserving `$ra`; `$a0/$a1` flow through unchanged |
| `$v0` liveness | callee result stays in `$v0` and is canonicalized in place by `sltiu v0,v0,1` |
| Address retention | none |
| `-O` signal | canonical 24-byte one-call frame, in-place boolean canonicalization, and teardown return slot are era `-O2` |
| Loop/back-edge | none in the wrapper; the callee owns its hardware-status polling loop |
| Relocations | one normalized `R_MIPS_26` call relocation |

The body and source shape independently reproduce the adjacent proven
`func_80080AE4` zero-status wrapper, but function hood was established from
this leaf's own caller and boundaries before using that twin as codegen
evidence.

## Minimal C and flags

```c
int func_8007C044(void *buffer, int value);

int func_80080B04(void *buffer, int value) {
    return func_8007C044(buffer, value) == 0;
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80080B04>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_8007C044
   c: xxxxxxxx  nop
  10: xxxxxxxx  lw    ra,16(sp)
  14: xxxxxxxx  sltiu v0,v0,1
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  addiu sp,sp,24

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=8/8
```

## Carve geometry

The former active span was `[0x71304,0x714C8)` = `0x01C4`:

```text
C leaf:     0x71324 - 0x71304 = 0x0020
asm resume: 0x714C8 - 0x71324 = 0x01A4
closure:    0x0020 + 0x01A4 = 0x01C4
```

These are boundary-derived text sizes; no aligned object size was used for
the carve.

## Packed span and gates

All sixteen words below were read from both the rebuilt packed executable and
retail. Each pair is equal:

```text
712F4: xxxxxxxx = xxxxxxxx  preceding epilogue word 1
712F8: xxxxxxxx = xxxxxxxx  preceding result canonicalization
712FC: xxxxxxxx = xxxxxxxx  preceding return
71300: xxxxxxxx = xxxxxxxx  preceding teardown delay
71304: xxxxxxxx = xxxxxxxx  leaf 1
71308: xxxxxxxx = xxxxxxxx  leaf 2
7130C: xxxxxxxx = xxxxxxxx  leaf 3
71310: xxxxxxxx = xxxxxxxx  leaf 4
71314: xxxxxxxx = xxxxxxxx  leaf 5
71318: xxxxxxxx = xxxxxxxx  leaf 6
7131C: xxxxxxxx = xxxxxxxx  leaf 7
71320: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
71324: xxxxxxxx = xxxxxxxx  following function entry
71328: xxxxxxxx = xxxxxxxx  following function word 2
7132C: xxxxxxxx = xxxxxxxx  following call
71330: xxxxxxxx = xxxxxxxx  following call delay
PACKED_SPAN=EXACT
```

```text
build/extracted/disc1/SLUS_006.62:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
build/disc1.candidate.exe:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 356
```
