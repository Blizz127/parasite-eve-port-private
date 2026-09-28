# `func_80021850` — exact 12-byte record swap

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on the first natural C phrasing under era `-O2 -G0`.
Integrated as matching-C leaf 341.

## Function hood and retail span

- File span: `[0x12050,0x120D8)` = `0x88` bytes = 34 words.
- VRAM span: `[0x80021850,0x800218D8)`.
- The body ends in canonical `jr ra; nop` at `0x800218D0/0x800218D4`.
- The preceding word at file `0x1204C` is the real `nop` return delay slot
  ending `func_800216E4`.
- The following word at file `0x120D8` is the real `lui t2,0x000F` entry of
  `func_800218D8`.
- Three direct exact-start `jal` references target `0x80021850`:

| caller file PC | caller VA | word |
|---:|---:|---:|
| `0x11F40` | `0x80021740` | `0x0C008614` |
| `0x11FB0` | `0x800217B0` | `0x0C008614` |
| `0x11FE8` | `0x800217E8` | `0x0C008614` |

`FUNCTION_HOOD=PROVEN`. This is a callable function, not padding, data, or a
mis-split tail.

## Screens

| Screen | Result |
|---|---|
| Frame decomposition | args 0 + local aggregate 12 + alignment 4 = `0x10`; no saved registers |
| Callee buckets | none; there is no `jal` in the body |
| Stage-0 written globals | none; all reads/writes are through argument `records` |
| Coloring pressure | first record address retained in `$v1`; second in `$v0`; three words shuttle through `$a0-$a2/$a3` and the stack temporary |
| `$v0` liveness | not a return value; `$v0` is the second record address and remains live through the final restore stores |
| Address retention | two argument-derived `index * 12` addresses; no symbolic-address or assembler-temp gate |
| `-O` signal | strength reduction `index * 12` as shift/add/shift and unrolled three-word aggregate copies identify era `-O2` |
| Loop/back-edge owner | none |
| Relocations | none |

Flags: era `-O2 -G0`. There are no GP-relative globals, symbolic expansion
knobs, division guards, dispatch folds, pins, or inline assembly.

## Minimal C

```c
typedef signed char s8;

typedef struct {
    unsigned int word0;
    unsigned int word1;
    unsigned int word2;
} Record12;

void func_80021850(Record12 *records, s8 first, s8 second) {
    Record12 temporary = records[first];

    records[first] = records[second];
    records[second] = temporary;
}
```

The signed-byte formals account for the two entry `sll 24; sra 24`
normalizations. The by-value 12-byte temporary accounts for the retail
`0x10` frame and its three stack words.

## Single-leaf object: all 34 words

```text
xxxxxxxx <func_80021850>:
   0: xxxxxxxx  addiu sp,sp,-16
   4: xxxxxxxx  sll   a1,a1,24
   8: xxxxxxxx  sra   a1,a1,24
   c: xxxxxxxx  sll   v1,a1,1
  10: xxxxxxxx  addu  v1,v1,a1
  14: xxxxxxxx  sll   v1,v1,2
  18: xxxxxxxx  addu  v1,v1,a0
  1c: xxxxxxxx  lw    v0,0(v1)
  20: xxxxxxxx  lw    a1,4(v1)
  24: xxxxxxxx  lw    a3,8(v1)
  28: xxxxxxxx  sw    v0,0(sp)
  2c: xxxxxxxx  sw    a1,4(sp)
  30: xxxxxxxx  sw    a3,8(sp)
  34: xxxxxxxx  sll   a2,a2,24
  38: xxxxxxxx  sra   a2,a2,24
  3c: xxxxxxxx  sll   v0,a2,1
  40: xxxxxxxx  addu  v0,v0,a2
  44: xxxxxxxx  sll   v0,v0,2
  48: xxxxxxxx  addu  v0,v0,a0
  4c: xxxxxxxx  lw    a0,0(v0)
  50: xxxxxxxx  lw    a1,4(v0)
  54: xxxxxxxx  lw    a2,8(v0)
  58: xxxxxxxx  sw    a0,0(v1)
  5c: xxxxxxxx  sw    a1,4(v1)
  60: xxxxxxxx  sw    a2,8(v1)
  64: xxxxxxxx  lw    v1,0(sp)
  68: xxxxxxxx  lw    a0,4(sp)
  6c: xxxxxxxx  lw    a1,8(sp)
  70: xxxxxxxx  sw    v1,0(v0)
  74: xxxxxxxx  sw    a0,4(v0)
  78: xxxxxxxx  sw    a1,8(v0)
  7c: xxxxxxxx  addiu sp,sp,16
  80: xxxxxxxx  jr    ra
  84: xxxxxxxx  nop
```

```text
ROM words:
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx

C words:
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
xxxxxxxx xxxxxxxx

RELOCS_NORMALIZED=none
BYTE_EXACT=34/34
```

The standalone object was `0x90` bytes only because ELF section alignment
added eight trailing zero bytes. The trim guard explicitly accepted
`0x90 -> 0x88`; span size comes from retail boundaries, not object size.

## Carve geometry

Prior active asm span: `[0x11718,0x19B88)` = `0x8470`.

```text
asm prefix:  0x12050 - 0x11718 = 0x0938
C leaf:      0x120D8 - 0x12050 = 0x0088
resume asm:  0x19B88 - 0x120D8 = 0x7AB0
closure:     0x0938 + 0x0088 + 0x7AB0 = 0x8470
```

Splat regenerated `11718.s` and `120D8.s`; the trim guard accepted exactly
those boundary-derived sizes.

## Packed span and full gates

Packed candidate and retail compare equal over
`[0x1204C,0x120DC)`, including both boundary words:

```text
1204C: xxxxxxxx = xxxxxxxx  preceding real return delay slot
12050: xxxxxxxx = xxxxxxxx  leaf word 1
12054: xxxxxxxx = xxxxxxxx  leaf word 2
12058: xxxxxxxx = xxxxxxxx  leaf word 3
1205C: xxxxxxxx = xxxxxxxx  leaf word 4
12060: xxxxxxxx = xxxxxxxx  leaf word 5
12064: xxxxxxxx = xxxxxxxx  leaf word 6
12068: xxxxxxxx = xxxxxxxx  leaf word 7
1206C: xxxxxxxx = xxxxxxxx  leaf word 8
12070: xxxxxxxx = xxxxxxxx  leaf word 9
12074: xxxxxxxx = xxxxxxxx  leaf word 10
12078: xxxxxxxx = xxxxxxxx  leaf word 11
1207C: xxxxxxxx = xxxxxxxx  leaf word 12
12080: xxxxxxxx = xxxxxxxx  leaf word 13
12084: xxxxxxxx = xxxxxxxx  leaf word 14
12088: xxxxxxxx = xxxxxxxx  leaf word 15
1208C: xxxxxxxx = xxxxxxxx  leaf word 16
12090: xxxxxxxx = xxxxxxxx  leaf word 17
12094: xxxxxxxx = xxxxxxxx  leaf word 18
12098: xxxxxxxx = xxxxxxxx  leaf word 19
1209C: xxxxxxxx = xxxxxxxx  leaf word 20
120A0: xxxxxxxx = xxxxxxxx  leaf word 21
120A4: xxxxxxxx = xxxxxxxx  leaf word 22
120A8: xxxxxxxx = xxxxxxxx  leaf word 23
120AC: xxxxxxxx = xxxxxxxx  leaf word 24
120B0: xxxxxxxx = xxxxxxxx  leaf word 25
120B4: xxxxxxxx = xxxxxxxx  leaf word 26
120B8: xxxxxxxx = xxxxxxxx  leaf word 27
120BC: xxxxxxxx = xxxxxxxx  leaf word 28
120C0: xxxxxxxx = xxxxxxxx  leaf word 29
120C4: xxxxxxxx = xxxxxxxx  leaf word 30
120C8: xxxxxxxx = xxxxxxxx  leaf word 31
120CC: xxxxxxxx = xxxxxxxx  leaf word 32
120D0: xxxxxxxx = xxxxxxxx  leaf word 33
120D4: xxxxxxxx = xxxxxxxx  leaf word 34
120D8: xxxxxxxx = xxxxxxxx  following real function entry
PACKED_SPAN=EXACT
```

```text
sha1sum build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

scripts/verify_us.sh (tail)
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 341 leaves

grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
341
```

No pins, inline assembly, file-scope assembly, fabricated padding, or
mismatch-hiding mechanism is present.
