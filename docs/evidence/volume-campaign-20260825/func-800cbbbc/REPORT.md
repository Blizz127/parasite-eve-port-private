# `func_800CBBBC` — exact signed-halfword callback twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 328, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0xBC3BC,0xBC3F0)`, VRAM `[0x800CBBBC,0x800CBBF0)`, is
thirteen words and ends in canonical `jr ra; nop`. Exact-start callback-table
word `0x800E0BA8` at file `0xD13A8` targets it.

The preceding real function ends at `0x800CBBB4/0x800CBBB8` with
`jr ra; nop`; following real `func_800CBBF0` begins with `lhu v0,4(a2)`.
Both boundaries are executable instructions.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_ENTRY`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; callback leaf |
| written-state Stage 0 | argument-relative only: signed halfword decrement/clamp and byte state transition; no global writer |
| coloring pressure | `$a2` retains values, `$a1` retains state, `$v0` carries update/predicate/constant |
| `$v0` liveness | load → decrement/store → sign extension → predicate → constant 2 |
| address retention | fixed argument offsets only |
| optimization signal | load-delay nop, post-store sign extension, `slti`, and branch-delay constant select era `-O2 -G0` |
| loop/back-edge owner | none |

## C and flags

```c
void func_800CBBBC(void *unused, signed char *state, signed short *values) {
    values[2] -= 20;
    if (values[2] < 20) {
        values[2] = 0;
        state[1] = 2;
    }
}
```

Era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`; first phrasing, no pins,
inline assembly, or special switch. Signed halfword is proven by retail's
post-store `sll 16; sra 16`.

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
xxxxxxxx <func_800CBBBC>:
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

The trim guard proves all gas-alignment bytes beyond the `0x34` body zero.

## Carve and packed-span proof

The prior asm span was `[0xBC330,0xBC7A4)`, size `0x474`:

```text
prefix asm:  0xBC3BC - 0xBC330 = 0x08C
C leaf:      0xBC3F0 - 0xBC3BC = 0x034
resume asm:  0xBC7A4 - 0xBC3F0 = 0x3B4
closure:     0x08C + 0x034 + 0x3B4 = 0x474
```

Retail and candidate match across both real boundaries:

```text
800CBBB4: xxxxxxxx  jr     ra
800CBBB8: xxxxxxxx  nop
800CBBBC: xxxxxxxx  lhu    v0,4(a2)
800CBBC0: xxxxxxxx  nop
800CBBC4: xxxxxxxx  addiu  v0,v0,-20
800CBBC8: xxxxxxxx  sh     v0,4(a2)
800CBBCC: xxxxxxxx  sll    v0,v0,16
800CBBD0: xxxxxxxx  sra    v0,v0,16
800CBBD4: xxxxxxxx  slti   v0,v0,20
800CBBD8: xxxxxxxx  beqz   v0,0x800CBBE8
800CBBDC: xxxxxxxx  li     v0,2
800CBBE0: xxxxxxxx  sh     zero,4(a2)
800CBBE4: xxxxxxxx  sb     v0,1(a1)
800CBBE8: xxxxxxxx  jr     ra
800CBBEC: xxxxxxxx  nop
800CBBF0: xxxxxxxx  lhu    v0,4(a2)
800CBBF4: xxxxxxxx  nop
800CBBF8: xxxxxxxx  addiu  v0,v0,-8
800CBBFC: xxxxxxxx  sh     v0,4(a2)
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
328
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 328 leaves
```

Result: `MATCHED=13/13`, first phrasing; consecutive parks remain zero.
