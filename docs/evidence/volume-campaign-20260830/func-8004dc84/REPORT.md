# `func_8004DC84` — exact constant forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 351 and Tier-2 probe rung
10.

## Function hood and retail span

- File `[0x3E484,0x3E4A4)`, VRAM `[0x8004DC84,0x8004DCA4)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra; nop` at `0x8004DC9C/0x8004DCA0`.
- One exact direct caller exists at `0x8004272C`; its next operations call
  `func_8004CDAC` and `func_80042910`.
- Preceding real `func_8004DAA4` ends at file `0x3E47C/0x3E480` with
  `jr ra; nop`. Following real `func_8004DCA4` starts immediately at
  `0x3E4A4` with a stack-frame prologue.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`; this is not padding, data, or a tail
entry.

## Retail body

```text
3E484 8004DC84 xxxxxxxx  addiu sp,sp,-0x18
3E488 8004DC88 xxxxxxxx  sw    ra,0x10(sp)
3E48C 8004DC8C xxxxxxxx  jal   func_80062F3C
3E490 8004DC90 xxxxxxxx  addiu a0,zero,0x2A
3E494 8004DC94 xxxxxxxx  lw    ra,0x10(sp)
3E498 8004DC98 xxxxxxxx  addiu sp,sp,0x18
3E49C 8004DC9C xxxxxxxx  jr    ra
3E4A0 8004DCA0 xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved asm callee, `func_80062F3C`; its body traverses GP-backed node/list state and calls `func_8006269C`, but no semantic name is asserted |
| Stage-0 globals | none read or written directly by this wrapper; state effects belong to the callee |
| Coloring pressure | none beyond preserving `$ra`; the only argument is constant `0x2A` |
| `$v0` liveness | none; wrapper return value is unused |
| Address retention | none |
| `-O` signal | canonical 24-byte one-call frame and constant-filled call delay slot are era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
void func_80062F3C(int value);

void func_8004DC84(void) {
    func_80062F3C(0x2A);
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_8004DC84>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80062F3C
   c: xxxxxxxx  addiu a0,zero,42
  10: xxxxxxxx  lw    ra,16(sp)
  14: xxxxxxxx  addiu sp,sp,24
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=8/8
```

## Carve geometry

The former active span was `[0x3E2A4,0x3F170)` = `0x0ECC`:

```text
asm prefix: 0x3E484 - 0x3E2A4 = 0x01E0
C leaf:     0x3E4A4 - 0x3E484 = 0x0020
asm resume: 0x3F170 - 0x3E4A4 = 0x0CCC
closure:    0x01E0 + 0x0020 + 0x0CCC = 0x0ECC
```

These sizes come from boundary arithmetic, not aligned object sizes.

## Packed span and gates

```text
3E474: xxxxxxxx = xxxxxxxx  preceding epilogue
3E478: xxxxxxxx = xxxxxxxx  preceding stack restore
3E47C: xxxxxxxx = xxxxxxxx  preceding return
3E480: xxxxxxxx = xxxxxxxx  preceding delay
3E484: xxxxxxxx = xxxxxxxx  leaf 1
3E488: xxxxxxxx = xxxxxxxx  leaf 2
3E48C: xxxxxxxx = xxxxxxxx  leaf 3
3E490: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
3E494: xxxxxxxx = xxxxxxxx  leaf 5
3E498: xxxxxxxx = xxxxxxxx  leaf 6
3E49C: xxxxxxxx = xxxxxxxx  leaf 7
3E4A0: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
3E4A4: xxxxxxxx = xxxxxxxx  following function entry
3E4A8: xxxxxxxx = xxxxxxxx  following function word 2
3E4AC: xxxxxxxx = xxxxxxxx  following function word 3
3E4B0: xxxxxxxx = xxxxxxxx  following call
PACKED_SPAN=EXACT
```

```text
Compare:  EXACT SHA-1 MATCH
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 351
```
