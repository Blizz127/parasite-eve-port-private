# `func_800C9A00` — exact signed-halfword callback twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 326, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0xBA200,0xBA234)`, VRAM `[0x800C9A00,0x800C9A34)`, is
`0x34` bytes / thirteen words and ends in canonical `jr ra; nop`. Its exact
start occurs as a callback-table word at file `0xD11BC`, VRAM `0x800E09BC`:

```text
0xD11BC: 800C9A00
```

The preceding real function ends at `0x800C99F8/0x800C99FC` with
`jr ra; nop`. The following real `func_800C9A34` begins at `0x800C9A34`
with `lhu v0,4(a2)`. Both boundaries are executable function instructions.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_ENTRY`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; callback leaf |
| written-state Stage 0 | argument-relative only: decrements signed halfword `values[2]`; below 20 clamps it to zero and writes byte 2 to `state[1]`; no global writer |
| coloring pressure | `$a2` retains `values`, `$a1` retains `state`; `$v0` carries update, sign extension, predicate, then constant 2 |
| `$v0` liveness | load → decrement → store → signed re-extension → predicate; reused in the branch delay slot |
| address retention | none beyond fixed argument offsets |
| optimization signal | load-delay nop, post-store sign extension, `slti`, and branch-delay constant select era GCC 2.7.2 `-O2 -G0` |
| loop/back-edge owner | none |

## C and flags

```c
void func_800C9A00(void *unused, signed char *state, signed short *values) {
    values[2] -= 20;
    if (values[2] < 20) {
        values[2] = 0;
        state[1] = 2;
    }
}
```

Era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`; first phrasing, no pins,
inline assembly, or special maspsx switch. The signed type is proven by
retail's `sll 16; sra 16` before the comparison.

## Full thirteen-word comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lhu v0,4(a2)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `nop` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,v0,-20` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sh v0,4(a2)` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `sll v0,v0,16` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `sra v0,v0,16` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `slti v0,v0,20` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `beqz v0,+3` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `li v0,2` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sh zero,4(a2)` |
| 10 | `xxxxxxxx` | `xxxxxxxx` | `sb v0,1(a1)` |
| 11 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 12 | `xxxxxxxx` | `xxxxxxxx` | `nop` |

Single-leaf object, with no relocations:

```text
xxxxxxxx <func_800C9A00>:
   0: xxxxxxxx  lhu    v0,4(a2)
   4: xxxxxxxx  nop
   8: xxxxxxxx  addiu  v0,v0,-20
   c: xxxxxxxx  sh     v0,4(a2)
  10: xxxxxxxx  sll    v0,v0,0x10
  14: xxxxxxxx  sra    v0,v0,0x10
  18: xxxxxxxx  slti   v0,v0,20
  1c: xxxxxxxx  beqz   v0,0x2c
  20: xxxxxxxx  li     v0,2
  24: xxxxxxxx  sh     zero,4(a2)
  28: xxxxxxxx  sb     v0,1(a1)
  2c: xxxxxxxx  jr     ra
  30: xxxxxxxx  nop
```

Gas aligns `.text` to `0x40`; the trim guard proved the twelve bytes after
the boundary-derived `0x34` body are zero.

## Carve geometry

The prior asm span was `[0xBA174,0xBA400)`, size `0x28C`:

```text
prefix asm:  0xBA200 - 0xBA174 = 0x08C
C leaf:      0xBA234 - 0xBA200 = 0x034
resume asm:  0xBA400 - 0xBA234 = 0x1CC
closure:     0x08C + 0x034 + 0x1CC = 0x28C
```

All sizes come from span boundaries, not aligned object sizes.

## Packed-span proof

Retail and candidate match across the preceding return, all thirteen words,
and the following function entry:

```text
800C99F8: xxxxxxxx  jr     ra
800C99FC: xxxxxxxx  nop
800C9A00: xxxxxxxx  lhu    v0,4(a2)
800C9A04: xxxxxxxx  nop
800C9A08: xxxxxxxx  addiu  v0,v0,-20
800C9A0C: xxxxxxxx  sh     v0,4(a2)
800C9A10: xxxxxxxx  sll    v0,v0,16
800C9A14: xxxxxxxx  sra    v0,v0,16
800C9A18: xxxxxxxx  slti   v0,v0,20
800C9A1C: xxxxxxxx  beqz   v0,0x800C9A2C
800C9A20: xxxxxxxx  li     v0,2
800C9A24: xxxxxxxx  sh     zero,4(a2)
800C9A28: xxxxxxxx  sb     v0,1(a1)
800C9A2C: xxxxxxxx  jr     ra
800C9A30: xxxxxxxx  nop
800C9A34: xxxxxxxx  lhu    v0,4(a2)
800C9A38: xxxxxxxx  nop
800C9A3C: xxxxxxxx  addiu  v0,v0,-8
800C9A40: xxxxxxxx  sh     v0,4(a2)
```

```text
retail:    <76 bytes, sha1 4024c729441a>
candidate: <76 bytes, sha1 4024c729441a>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
326
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 326 leaves
```

Result: `MATCHED=13/13`, first phrasing; consecutive parks remain zero.
