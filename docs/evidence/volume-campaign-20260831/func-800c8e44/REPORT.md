# `func_800C8E44` — exact six-argument callback forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **384**.

## Function hood first

- File `[0xB9644,0xB9670)`, VA `[0x800C8E44,0x800C8E70)`: `0x2C`, eleven
  words.
- Canonical return is `jr ra` at `0x800C8E68`; frame teardown at
  `0x800C8E6C` is its live delay slot.
- Exact-start callback-table reference: file `0xD11DC`, VA `0x800E09DC`,
  contains `.word func_800C8E44` in `D_800E09C8`.
- Preceding real `func_800C8D34` ends at `0xB963C/0xB9640` with `jr ra` and
  frame teardown.
- Following real `func_800C8E70` begins at `0xB9670` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_TABLE_REFERENCE_AND_CANONICAL_RETURN`.
The proof is independent of the analogous callback at `0xB8500`; this is not
padding, data, or label-derived function hood.

## Retail body and screens

```text
B9644 800C8E44 xxxxxxxx  addiu sp,sp,-0x20
B9648 800C8E48 xxxxxxxx  lw    v0,0x30(sp)
B964C 800C8E4C xxxxxxxx  lw    v1,0x34(sp)
B9650 800C8E50 xxxxxxxx  sw    ra,0x18(sp)
B9654 800C8E54 xxxxxxxx  sw    v0,0x10(sp)
B9658 800C8E58 xxxxxxxx  jal   func_800C2AF0
B965C 800C8E5C xxxxxxxx  sw    v1,0x14(sp)
B9660 800C8E60 xxxxxxxx  lw    ra,0x18(sp)
B9664 800C8E64 xxxxxxxx  addu  v0,zero,zero
B9668 800C8E68 xxxxxxxx  jr    ra
B966C 800C8E6C xxxxxxxx  addiu sp,sp,0x20
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, matched `func_800C2AF0`; all six incoming arguments are forwarded, then the callback returns zero |
| Stage-0 globals | none in this wrapper; accepted callee evidence establishes `D_800E2248` as the mutable `unsigned int *` base it publishes and indexes |
| Coloring pressure | low; `$a0`-`$a3` stay in argument homes while stack arguments 5/6 pass through `$v0/$v1` |
| `$v0` liveness | carries incoming stack argument 5 before the call, is call-clobbered, then receives the explicit zero result |
| Address retention | none in the wrapper; the matched callee owns base advancement and indexed addressing |
| `-O` signal | compact 32-byte nonleaf frame, call-delay stack store, and return-delay teardown match established era `-O2` output |
| Loop/back-edge | none |

The six-formal declaration is structural evidence, not speculative naming:
retail copies both incoming stack arguments to the outgoing argument area.
No `$gp` access occurs, so the campaign rule selects `-G0`.

## Minimal C and single-leaf comparison

```c
int func_800C2AF0(unsigned int *base, int unused, int index,
                  unsigned int value, int arg4, int arg5);

int func_800C8E44(unsigned int *base, int unused, int index,
                  unsigned int value, int arg4, int arg5) {
    func_800C2AF0(base, unused, index, value, arg4, arg5);
    return 0;
}
```

```text
xxxxxxxx <func_800C8E44>:
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

The former active asm span was `[0xB9480,0xB9708)` = `0x288`:

```text
asm prefix: 0xB9644 - 0xB9480 = 0x1C4
C leaf:     0xB9670 - 0xB9644 = 0x02C
asm resume: 0xB9708 - 0xB9670 = 0x098
closure:    0x1C4 + 0x02C + 0x098 = 0x288
```

The carve creates `B9670.s`; all sizes derive from retail split boundaries,
not aligned object size.

## Packed span and gates

```text
file    VA        retail   candidate  role
B9634 800C8E34  xxxxxxxx xxxxxxxx   preceding epilogue load
B9638 800C8E38  xxxxxxxx xxxxxxxx   preceding zero result
B963C 800C8E3C  xxxxxxxx xxxxxxxx   preceding return
B9640 800C8E40  xxxxxxxx xxxxxxxx   preceding return delay
B9644 800C8E44  xxxxxxxx xxxxxxxx   leaf 1
B9648 800C8E48  xxxxxxxx xxxxxxxx   leaf 2
B964C 800C8E4C  xxxxxxxx xxxxxxxx   leaf 3
B9650 800C8E50  xxxxxxxx xxxxxxxx   leaf 4
B9654 800C8E54  xxxxxxxx xxxxxxxx   leaf 5
B9658 800C8E58  xxxxxxxx xxxxxxxx   leaf 6
B965C 800C8E5C  xxxxxxxx xxxxxxxx   leaf 7
B9660 800C8E60  xxxxxxxx xxxxxxxx   leaf 8
B9664 800C8E64  xxxxxxxx xxxxxxxx   leaf 9
B9668 800C8E68  xxxxxxxx xxxxxxxx   leaf 10
B966C 800C8E6C  xxxxxxxx xxxxxxxx   leaf 11
B9670 800C8E70  xxxxxxxx xxxxxxxx   following real entry
B9674 800C8E74  xxxxxxxx xxxxxxxx   following prologue save
B9678 800C8E78  xxxxxxxx xxxxxxxx   following global address
B967C 800C8E7C  xxxxxxxx xxxxxxxx   following global address low
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
C conversion: Phase 5HD-12850 — 384 leaves

grep matching-C count: 384
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
