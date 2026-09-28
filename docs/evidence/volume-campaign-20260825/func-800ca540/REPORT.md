# `func_800CA540` — exact signed-halfword callback twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 327, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0xBAD40,0xBAD74)`, VRAM `[0x800CA540,0x800CA574)`, is
thirteen words and ends in canonical `jr ra; nop`. Exact-start callback-table
word `0x800E0AAC` at file `0xD12AC` targets it.

The preceding real function ends at `0x800CA538/0x800CA53C` with
`jr ra; nop`. The following real function starts at `0x800CA574` with
`addiu sp,sp,-24`. Both boundary words are executable instructions.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_ENTRY`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; callback leaf |
| written-state Stage 0 | argument-relative only: decrement signed `values[2]`, then clamp and set `state[1]` on the threshold; no global writer |
| coloring pressure | `$a2` retains values, `$a1` retains state, `$v0` carries update/predicate/constant |
| `$v0` liveness | load → decrement/store → signed re-extension → predicate → constant 2 |
| address retention | fixed argument offsets only |
| optimization signal | load-delay nop, post-store sign extension, `slti`, and branch-delay constant select era `-O2 -G0` |
| loop/back-edge owner | none |

## C and flags

```c
void func_800CA540(void *unused, signed char *state, signed short *values) {
    values[2] -= 20;
    if (values[2] < 20) {
        values[2] = 0;
        state[1] = 2;
    }
}
```

Era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`; first phrasing, no pins,
inline assembly, or special switch. Signed halfword is proven by the
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

Single-leaf object (no relocations):

```text
xxxxxxxx <func_800CA540>:
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

Gas alignment grows the object to `0x40`; the trim guard verifies all bytes
beyond the boundary-derived `0x34` are zero.

## Carve and packed-span proof

The prior asm span was `[0xBACB4,0xBAF98)`, size `0x2E4`:

```text
prefix asm:  0xBAD40 - 0xBACB4 = 0x08C
C leaf:      0xBAD74 - 0xBAD40 = 0x034
resume asm:  0xBAF98 - 0xBAD74 = 0x224
closure:     0x08C + 0x034 + 0x224 = 0x2E4
```

Retail and candidate match across both neighboring real functions:

```text
800CA538: xxxxxxxx  jr     ra
800CA53C: xxxxxxxx  nop
800CA540: xxxxxxxx  lhu    v0,4(a2)
800CA544: xxxxxxxx  nop
800CA548: xxxxxxxx  addiu  v0,v0,-20
800CA54C: xxxxxxxx  sh     v0,4(a2)
800CA550: xxxxxxxx  sll    v0,v0,16
800CA554: xxxxxxxx  sra    v0,v0,16
800CA558: xxxxxxxx  slti   v0,v0,20
800CA55C: xxxxxxxx  beqz   v0,0x800CA56C
800CA560: xxxxxxxx  li     v0,2
800CA564: xxxxxxxx  sh     zero,4(a2)
800CA568: xxxxxxxx  sb     v0,1(a1)
800CA56C: xxxxxxxx  jr     ra
800CA570: xxxxxxxx  nop
800CA574: xxxxxxxx  addiu  sp,sp,-24
800CA578: xxxxxxxx  sw     ra,16(sp)
800CA57C: xxxxxxxx  jal    0x800C22F8
800CA580: xxxxxxxx  nop
```

```text
retail:    <76 bytes, sha1 a44e764129f8>
candidate: <76 bytes, sha1 a44e764129f8>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
327
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 327 leaves
```

Result: `MATCHED=13/13`, first phrasing; consecutive parks remain zero.
