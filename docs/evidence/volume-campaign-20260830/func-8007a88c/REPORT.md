# `func_8007A88C` — exact fixed-success command wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on the first natural C phrasing under era `-O2 -G0`,
with no maspsx behavior gate. Integrated as matching-C leaf 353 and Tier-2
continuation rung 12.

## Function hood and retail span

- File `[0x6B08C,0x6B0AC)`, VRAM `[0x8007A88C,0x8007A8AC)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x8007A8A4`, with frame teardown in its
  live delay slot at `0x8007A8A8`.
- One executable direct call targets the exact start at `0x80087180`
  (file `0x77980`, word `0x0C01EA23`). That caller passes the address of
  the four-byte command buffer `D_8009D1C8`, then immediately returns the
  wrapper's result.
- Preceding real `func_8007A740` ends immediately with `jr ra` and live
  frame teardown at file `0x6B084/0x6B088`.
- Following real `func_8007A8AC` begins immediately at file `0x6B0AC`
  with a 24-byte call frame.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL_AND_CANONICAL_RETURN`; this is a real
callable function, not padding, data, or a tail fragment.

## Retail body

```text
6B08C 8007A88C xxxxxxxx  addiu sp,sp,-0x18
6B090 8007A890 xxxxxxxx  sw    ra,0x10(sp)
6B094 8007A894 xxxxxxxx  jal   func_8007B964
6B098 8007A898 xxxxxxxx  nop
6B09C 8007A89C xxxxxxxx  lw    ra,0x10(sp)
6B0A0 8007A8A0 xxxxxxxx  addiu v0,zero,1
6B0A4 8007A8A4 xxxxxxxx  jr    ra
6B0A8 8007A8A8 xxxxxxxx  addiu sp,sp,0x18
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_8007B964`; it copies four caller bytes into the CD command-register path and returns zero, while this wrapper deliberately canonicalizes success to 1 |
| Stage-0 globals | none accessed directly; the exact caller supplies `D_8009D1C8`, and all hardware/global effects are inside the callee |
| Coloring pressure | only `$ra` is preserved; the sole argument forwards unchanged and the callee result is intentionally discarded |
| `$v0` liveness | callee zero is dead; retail writes constant 1 into `$v0` after restoring `$ra`, and the caller returns that 1 |
| Address retention | none in the wrapper; the argument pointer is not needed after the call |
| `-O` signal | canonical 24-byte one-call frame, dead callee result, late constant result, and useful return teardown delay slot are era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
int func_8007B964(void *buffer);

int func_8007A88C(void *buffer) {
    func_8007B964(buffer);
    return 1;
}
```

Compile: era GCC 2.7.2 `-O2 -G0`; no three-word-symbol, store-delay,
division, dispatch-fold, pin, or inline-assembly mechanism is enabled.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_8007A88C>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_8007B964
   c: xxxxxxxx  nop
  10: xxxxxxxx  lw    ra,16(sp)
  14: xxxxxxxx  addiu v0,zero,1
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  addiu sp,sp,24

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=8/8
```

## Carve geometry

The former active assembly span was `[0x6ACD0,0x6C930)` = `0x1C60`:

```text
asm prefix: 0x6B08C - 0x6ACD0 = 0x03BC
C leaf:     0x6B0AC - 0x6B08C = 0x0020
asm resume: 0x6C930 - 0x6B0AC = 0x1884
closure:    0x03BC + 0x0020 + 0x1884 = 0x1C60
```

All sizes come from retail boundaries, never aligned object sizes.

## Full packed-span comparison and gates

Retail and rebuilt candidate words are equal over `[0x6B07C,0x6B0BC)`,
including four words on each boundary side:

```text
6B07C: xxxxxxxx = xxxxxxxx  preceding saved-register restore
6B080: xxxxxxxx = xxxxxxxx  preceding saved-register restore
6B084: xxxxxxxx = xxxxxxxx  preceding return
6B088: xxxxxxxx = xxxxxxxx  preceding teardown delay
6B08C: xxxxxxxx = xxxxxxxx  leaf 1
6B090: xxxxxxxx = xxxxxxxx  leaf 2
6B094: xxxxxxxx = xxxxxxxx  leaf 3
6B098: xxxxxxxx = xxxxxxxx  leaf 4
6B09C: xxxxxxxx = xxxxxxxx  leaf 5
6B0A0: xxxxxxxx = xxxxxxxx  leaf 6
6B0A4: xxxxxxxx = xxxxxxxx  leaf 7
6B0A8: xxxxxxxx = xxxxxxxx  leaf 8
6B0AC: xxxxxxxx = xxxxxxxx  following function entry
6B0B0: xxxxxxxx = xxxxxxxx  following saved return
6B0B4: xxxxxxxx = xxxxxxxx  following call
6B0B8: xxxxxxxx = xxxxxxxx  following call delay
PACKED_SPAN=EXACT
```

```text
sha1sum build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

scripts/verify_us.sh: exit 0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: 353 leaves

grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
353
```

No pins, inline assembly, file-scope assembly, fabricated padding, or
mismatch-hiding mechanism is present.
