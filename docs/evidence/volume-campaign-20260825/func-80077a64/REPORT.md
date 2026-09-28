# `func_80077A64` — exact Psy-Q `GetTPage` bit packer

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 330, matched on the first bounded phrasing.

## Function hood and boundary ownership

Retail span `[0x68264,0x682A0)`, VRAM `[0x80077A64,0x80077AA0)`, is
`0x3C` bytes / fifteen words. It ends with canonical `jr ra` and a live OR
in its delay slot at `0x80077A98/0x80077A9C`.

The executable contains 36 direct `jal 0x80077A64` references:

```text
0x800308EC  0x800309BC  0x80030AD0  0x800313AC  0x80031444
0x80037384  0x8005F118  0x80067128  0x80074834  0x800C2FD0
0x800C3114  0x800C3304  0x800CECD4  0x800CED08  0x800CEEE0
0x800CF758  0x800D0630  0x800D0D90  0x800D1280  0x800D19D4
0x800D1C2C  0x800D200C  0x800D227C  0x800D2468  0x800D2A94
0x800D2DA8  0x800D2F38  0x800D33C0  0x800D3DC8  0x800D684C
0x800D9D4C  0x800DA4BC  0x800DC2F0  0x800DC4C0  0x800DE3B0
0x800DE4B0
```

The preceding real `func_80077A54` is a BIOS trampoline ending with
`jr t2; li t1,73` at `0x80077A58/0x80077A5C`; the nop at `0x80077A60`
is alignment. The target owns its live return delay slot through
`0x80077A9C`. The nop at `0x80077AA0` is alignment, and matched real
`func_80077AA4` begins at `0x80077AA4`.

`FUNCTION_HOOD=PROVEN_BY_36_DIRECT_CALLERS_AND_CANONICAL_RETURN`.

## Semantic proof and screens

Existing retail/native evidence in
`docs/evidence/pe-b54i-gpu-primitive-leaves/REPORT.md` identifies this
operation as Psy-Q `GetTPage(tp, abr, x, y)` and proves the expression:

```text
((tp & 3) << 7) | ((abr & 3) << 5) |
((y & 0x100) >> 4) | ((x & 0x3FF) >> 6) | ((y & 0x200) << 2)
```

The independent known oracle `(0, 1, 256, 480) -> 0x34` agrees.

| screen | result |
|---|---|
| callee buckets | no `jal`; pure bit-packing leaf |
| written-state Stage 0 | no stores and no globals |
| coloring pressure | retail mutates `$a1`, `$a2`, and `$a3` in place; `$v1` holds the first `y` term; `$v0` accumulates the result |
| `$v0` liveness | `tp` term becomes a five-term result; the final OR occupies the return delay slot |
| address retention | none |
| optimization signal | sequential in-place masks/shifts and the live return delay slot select era GCC `-O2 -G0` |
| loop/back-edge owner | none |

## Final C and flags

```c
int func_80077A64(int tp, int abr, int x, int y) {
    return ((tp & 3) << 7) |
           ((abr & 3) << 5) |
           ((y & 0x100) >> 4) |
           ((x & 0x3FF) >> 6) |
           ((y & 0x200) << 2);
}
```

Compiler: era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`. No pins,
inline assembly, symbolic-address switch, or relocation normalization is
required. The first phrasing is byte-exact.

## Full fifteen-word comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `andi v0,a0,3` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `sll v0,v0,7` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `andi a1,a1,3` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sll a1,a1,5` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `or v0,v0,a1` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `andi v1,a3,0x100` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `sra v1,v1,4` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `or v0,v0,v1` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `andi a2,a2,0x3ff` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sra a2,a2,6` |
| 10 | `xxxxxxxx` | `xxxxxxxx` | `or v0,v0,a2` |
| 11 | `xxxxxxxx` | `xxxxxxxx` | `andi a3,a3,0x200` |
| 12 | `xxxxxxxx` | `xxxxxxxx` | `sll a3,a3,2` |
| 13 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 14 | `xxxxxxxx` | `xxxxxxxx` | `or v0,v0,a3` |

Single-leaf object:

```text
xxxxxxxx <func_80077A64>:
   0: xxxxxxxx  andi   v0,a0,0x3
   4: xxxxxxxx  sll    v0,v0,0x7
   8: xxxxxxxx  andi   a1,a1,0x3
   c: xxxxxxxx  sll    a1,a1,0x5
  10: xxxxxxxx  or     v0,v0,a1
  14: xxxxxxxx  andi   v1,a3,0x100
  18: xxxxxxxx  sra    v1,v1,0x4
  1c: xxxxxxxx  or     v0,v0,v1
  20: xxxxxxxx  andi   a2,a2,0x3ff
  24: xxxxxxxx  sra    a2,a2,0x6
  28: xxxxxxxx  or     v0,v0,a2
  2c: xxxxxxxx  andi   a3,a3,0x200
  30: xxxxxxxx  sll    a3,a3,0x2
  34: xxxxxxxx  jr     ra
  38: xxxxxxxx  or     v0,v0,a3
```

## Carve geometry and packed-span proof

The prior asm span was `[0x6824C,0x682A4)`, size `0x58`:

```text
prefix asm:  0x68264 - 0x6824C = 0x18
C leaf:      0x682A0 - 0x68264 = 0x3C
resume asm:  0x682A4 - 0x682A0 = 0x04
closure:     0x18 + 0x3C + 0x04 = 0x58
```

The prefix retains the preceding real trampoline and its alignment; the
resume is exactly the one trailing alignment nop. Retail and packed candidate
are identical through both real boundaries:

```text
retail:
00068254: a0000a24 08004001 49000924 00000000
00068264: 03008230 c0110200 0300a530 40290500
00068274: 25104500 0001e330 03190300 25104300
00068284: ff03c630 83310600 25104600 0002e730
00068294: 80380700 0800e003 25104700 00000000
000682a4: 80110500 03210400 3f008430

candidate:
00068254: a0000a24 08004001 49000924 00000000
00068264: 03008230 c0110200 0300a530 40290500
00068274: 25104500 0001e330 03190300 25104300
00068284: ff03c630 83310600 25104600 0002e730
00068294: 80380700 0800e003 25104700 00000000
000682a4: 80110500 03210400 3f008430
```

The packed target span is also identical to the single-leaf object:

```text
<32 bytes, sha1 8f9f32389f02>
<28 bytes, sha1 c5bda5e313e0>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
volume file 0x68264 (77A64): cand=<60 bytes, sha1 7aa74b146bbe> orig=<60 bytes, sha1 7aa74b146bbe>
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 330 leaves
330
```

Result: `MATCHED=15/15`, first phrasing; consecutive parks reset from two
to zero.
