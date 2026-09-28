# `func_800C8C4C` — exact signed-halfword callback

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 325, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0xB944C,0xB9480)`, VRAM `[0x800C8C4C,0x800C8C80)`, is
`0x34` bytes / thirteen words and ends in canonical `jr ra; nop`. Its exact
start occurs as a callback-table word at file `0xD1048`, VRAM `0x800E0848`:

```text
0xD1048: 800C8C4C
```

The preceding real function ends at `0x800C8C44/0x800C8C48` with
`jr ra; nop`. The following real `func_800C8C80` begins exactly at
`0x800C8C80` with `lhu v0,4(a2)`. Both sides contain executable function
instructions; there is no padding ambiguity.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_ENTRY`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; callback leaf |
| written-state Stage 0 | argument-relative only: decrements signed halfword `values[2]`; below 20 clamps it to zero and writes byte 2 to `state[1]`; no global writer |
| coloring pressure | `$a2` retains `values`, `$a1` retains `state`; `$v0` carries the halfword update, signed re-extension, predicate, then constant 2 |
| `$v0` liveness | load → decrement → store → signed re-extension → threshold predicate; reused as constant 2 in the branch delay slot |
| address retention | none beyond fixed argument offsets |
| optimization signal | load-delay nop, post-store sign extension, `slti`, and useful branch-delay constant select era GCC 2.7.2 `-O2 -G0` |
| loop/back-edge owner | none |

There are no callees, relocations, symbolic addresses, or global provenance
questions in this leaf.

## C and flags

```c
void func_800C8C4C(void *unused, signed char *state, signed short *values) {
    values[2] -= 20;
    if (values[2] < 20) {
        values[2] = 0;
        state[1] = 2;
    }
}
```

Era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`; first phrasing, no pins,
inline assembly, or special maspsx switch. The signed-halfword type is proven
by retail's `sll 16; sra 16` before `slti`.

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

Single-leaf object; there are no relocations to normalize:

```text
xxxxxxxx <func_800C8C4C>:
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

Gas aligns `.text` to `0x40`; the trim guard proved the twelve bytes beyond
the boundary-derived `0x34` body are zero before removing them.

## Carve geometry

The prior asm span was `[0xB93C0,0xB9708)`, size `0x348`:

```text
prefix asm:  0xB944C - 0xB93C0 = 0x08C
C leaf:      0xB9480 - 0xB944C = 0x034
resume asm:  0xB9708 - 0xB9480 = 0x288
closure:     0x08C + 0x034 + 0x288 = 0x348
```

All sizes come from file-span subtraction, never aligned object sizes.

## Packed-span proof

Retail and candidate disassemble identically across both real boundaries:

```text
800C8C44: xxxxxxxx  jr     ra
800C8C48: xxxxxxxx  nop
800C8C4C: xxxxxxxx  lhu    v0,4(a2)
800C8C50: xxxxxxxx  nop
800C8C54: xxxxxxxx  addiu  v0,v0,-20
800C8C58: xxxxxxxx  sh     v0,4(a2)
800C8C5C: xxxxxxxx  sll    v0,v0,16
800C8C60: xxxxxxxx  sra    v0,v0,16
800C8C64: xxxxxxxx  slti   v0,v0,20
800C8C68: xxxxxxxx  beqz   v0,0x800C8C78
800C8C6C: xxxxxxxx  li     v0,2
800C8C70: xxxxxxxx  sh     zero,4(a2)
800C8C74: xxxxxxxx  sb     v0,1(a1)
800C8C78: xxxxxxxx  jr     ra
800C8C7C: xxxxxxxx  nop
800C8C80: xxxxxxxx  lhu    v0,4(a2)
800C8C84: xxxxxxxx  nop
800C8C88: xxxxxxxx  addiu  v0,v0,-8
800C8C8C: xxxxxxxx  sh     v0,4(a2)
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
325
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 325 leaves
```

Result: `MATCHED=13/13`, first phrasing; consecutive parks remain zero.
