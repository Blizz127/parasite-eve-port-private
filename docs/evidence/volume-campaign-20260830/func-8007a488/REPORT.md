# `func_8007A488` — exact `CD_ready` forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on the first natural C phrasing under era `-O2 -G0`,
with no maspsx behavior gate. Integrated as matching-C leaf 352 and the first
Tier-2 continuation rung after the bounded ten-leaf probe.

## Function hood and retail span

- File `[0x6AC88,0x6ACA8)`, VRAM `[0x8007A488,0x8007A4A8)`: `0x20`
  bytes, eight words.
- The body ends in canonical `jr ra; nop` at `0x8007A4A0/0x8007A4A4`.
- One executable direct call targets the exact start, at `0x8007C5E8`
  (file `0x6CDE8`, word `0x0C01E922`). The caller passes a result buffer in
  `$a1 = sp + 0x30`, tests the returned `$v0` against 5, and then consumes
  bytes 0 and 1 of the result buffer.
- Preceding real `func_8007A468` ends immediately with `jr ra; nop` at file
  `0x6AC80/0x6AC84`.
- Following real `func_8007A4A8` begins immediately at file `0x6ACA8` with
  its symbolic callback-global load.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL_AND_CANONICAL_RETURN`; this is a real
callable function, not padding, data, or a mislabeled tail.

## Retail body

```text
6AC88 8007A488 xxxxxxxx  addiu sp,sp,-0x18
6AC8C 8007A48C xxxxxxxx  sw    ra,0x10(sp)
6AC90 8007A490 xxxxxxxx  jal   func_8007B290
6AC94 8007A494 xxxxxxxx  nop
6AC98 8007A498 xxxxxxxx  lw    ra,0x10(sp)
6AC9C 8007A49C xxxxxxxx  addiu sp,sp,0x18
6ACA0 8007A4A0 xxxxxxxx  jr    ra
6ACA4 8007A4A4 xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_8007B290`; the adjacent SDK string map identifies it as PsyQ `CD_ready` |
| Stage-0 globals | none accessed directly by this wrapper; result-buffer and CD state effects belong to the callee |
| Coloring pressure | only `$ra` is preserved; `$a0/$a1` forward unchanged and no local value spans the call |
| `$v0` liveness | the callee result remains in `$v0` through the wrapper return; the direct caller immediately compares it with 5 |
| Address retention | none; the caller-owned result pointer forwards unchanged in `$a1` |
| `-O` signal | canonical 24-byte one-call frame with unchanged argument forwarding and no redundant result copy is era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

The SDK name is independently supported by the embedded `"CD_ready"` string
and the established mapping in `docs/ai_context/sdk_map.md`; the C keeps the
retail symbol because this rung does not rename existing interfaces.

## Minimal C and flags

```c
int func_8007B290(int mode, unsigned char *result);

int func_8007A488(int mode, unsigned char *result) {
    return func_8007B290(mode, result);
}
```

Compile: era GCC 2.7.2 `-O2 -G0`; no three-word-symbol, store-delay, division,
dispatch-fold, pin, or inline-assembly mechanism is enabled.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_8007A488>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_8007B290
   c: xxxxxxxx  nop
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

The former active assembly span was `[0x6AC00,0x6ACA8)` = `0x00A8`:

```text
asm prefix: 0x6AC88 - 0x6AC00 = 0x0088
C leaf:     0x6ACA8 - 0x6AC88 = 0x0020
asm resume: 0x6ACA8 - 0x6ACA8 = 0x0000
closure:    0x0088 + 0x0020 + 0x0000 = 0x00A8
```

All sizes come from retail split boundaries, never from aligned object sizes.
Regenerating the split reduced `6AC00.s` to the proven `0x88`-byte prefix;
the trim guard had correctly refused the stale pre-regeneration object rather
than silently discarding nonzero leaf bytes.

## Full packed-span comparison and gates

Retail and rebuilt candidate words are equal over `[0x6AC78,0x6ACB8)`,
including four words on each boundary side:

```text
6AC78: xxxxxxxx = xxxxxxxx  preceding epilogue load
6AC7C: xxxxxxxx = xxxxxxxx  preceding frame teardown
6AC80: xxxxxxxx = xxxxxxxx  preceding return
6AC84: xxxxxxxx = xxxxxxxx  preceding return delay
6AC88: xxxxxxxx = xxxxxxxx  leaf 1
6AC8C: xxxxxxxx = xxxxxxxx  leaf 2
6AC90: xxxxxxxx = xxxxxxxx  leaf 3
6AC94: xxxxxxxx = xxxxxxxx  leaf 4
6AC98: xxxxxxxx = xxxxxxxx  leaf 5
6AC9C: xxxxxxxx = xxxxxxxx  leaf 6
6ACA0: xxxxxxxx = xxxxxxxx  leaf 7
6ACA4: xxxxxxxx = xxxxxxxx  leaf 8
6ACA8: xxxxxxxx = xxxxxxxx  following function entry
6ACAC: xxxxxxxx = xxxxxxxx  following global load
6ACB0: xxxxxxxx = xxxxxxxx  following store address
6ACB4: xxxxxxxx = xxxxxxxx  following return
PACKED_SPAN=EXACT
```

```text
sha1sum build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

scripts/verify_us.sh: exit 0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: 352 leaves

grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
352
```

No pins, inline assembly, file-scope assembly, fabricated padding, or
mismatch-hiding mechanism is present.
