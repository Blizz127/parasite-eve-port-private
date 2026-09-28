# `func_80080AC4` — exact fixed-success command-wrapper twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on the first natural C phrasing under era `-O2 -G0`,
with no maspsx behavior gate. Integrated as matching-C leaf 355 and Tier-2
continuation rung 14.

## Function hood and retail span

- File `[0x712C4,0x712E4)`, VRAM `[0x80080AC4,0x80080AE4)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x80080ADC`, with frame teardown in its
  live delay slot at `0x80080AE0`.
- One executable direct call targets the exact start at `0x80016894`
  (file `0x7094`, word `0x0C0202B1`). The caller builds a four-byte
  command buffer at `sp + 0x20`, passes that exact address, and continues
  with an independently established result of 1.
- Preceding real `func_800809E0` ends with `jr ra` and live teardown at
  file `0x712B4/0x712B8`; the two intervening zero words at
  `0x712BC/0x712C0` are explicit alignment, not part of either function.
- Following real `func_80080AE4` begins immediately at file `0x712E4`
  with a 24-byte call frame.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL_AND_CANONICAL_RETURN`. The preceding
alignment is called out rather than misrepresented as executable function
body; the exact-start call independently excludes padding or data.

## Retail body

```text
712C4 80080AC4 xxxxxxxx  addiu sp,sp,-0x18
712C8 80080AC8 xxxxxxxx  sw    ra,0x10(sp)
712CC 80080ACC xxxxxxxx  jal   func_8007B964
712D0 80080AD0 xxxxxxxx  nop
712D4 80080AD4 xxxxxxxx  lw    ra,0x10(sp)
712D8 80080AD8 xxxxxxxx  addiu v0,zero,1
712DC 80080ADC xxxxxxxx  jr    ra
712E0 80080AE0 xxxxxxxx  addiu sp,sp,0x18
```

The body is word-identical to independently hood-proven
`func_8007A88C`; this rung still carries its own caller and boundary proof.

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_8007B964`; it copies four caller bytes into the CD command-register path and returns zero |
| Stage-0 globals | none accessed directly; the exact caller supplies a four-byte stack buffer and all hardware/global effects belong to the callee |
| Coloring pressure | only `$ra` is preserved; the sole pointer forwards unchanged and no local survives the call |
| `$v0` liveness | callee zero is dead; retail replaces it with constant 1 before returning |
| Address retention | none |
| `-O` signal | canonical 24-byte call frame, late fixed result, and useful teardown delay slot are era `-O2` |
| Loop/back-edge | none |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
int func_8007B964(void *buffer);

int func_80080AC4(void *buffer) {
    func_8007B964(buffer);
    return 1;
}
```

Compile: era GCC 2.7.2 `-O2 -G0`; no three-word-symbol, store-delay,
division, dispatch-fold, pin, or inline-assembly mechanism is enabled.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80080AC4>:
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

The current active assembly span before this carve was
`[0x71150,0x712E4)` = `0x0194`:

```text
asm prefix: 0x712C4 - 0x71150 = 0x0174
C leaf:     0x712E4 - 0x712C4 = 0x0020
asm resume: 0x712E4 - 0x712E4 = 0x0000
closure:    0x0174 + 0x0020 + 0x0000 = 0x0194
```

All sizes come from retail boundaries, never aligned object sizes.

## Full packed-span comparison and gates

Retail and rebuilt candidate words are equal over `[0x712B4,0x712F4)`,
including the real predecessor epilogue, explicit alignment, and following
function words:

```text
712B4: xxxxxxxx = xxxxxxxx  preceding return
712B8: xxxxxxxx = xxxxxxxx  preceding teardown delay
712BC: xxxxxxxx = xxxxxxxx  explicit alignment
712C0: xxxxxxxx = xxxxxxxx  explicit alignment
712C4: xxxxxxxx = xxxxxxxx  leaf 1
712C8: xxxxxxxx = xxxxxxxx  leaf 2
712CC: xxxxxxxx = xxxxxxxx  leaf 3
712D0: xxxxxxxx = xxxxxxxx  leaf 4
712D4: xxxxxxxx = xxxxxxxx  leaf 5
712D8: xxxxxxxx = xxxxxxxx  leaf 6
712DC: xxxxxxxx = xxxxxxxx  leaf 7
712E0: xxxxxxxx = xxxxxxxx  leaf 8
712E4: xxxxxxxx = xxxxxxxx  following function entry
712E8: xxxxxxxx = xxxxxxxx  following saved return
712EC: xxxxxxxx = xxxxxxxx  following call
712F0: xxxxxxxx = xxxxxxxx  following call delay
PACKED_SPAN=EXACT
```

```text
sha1sum build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

scripts/verify_us.sh: exit 0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: 355 leaves

grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
355
```

No pins, inline assembly, file-scope assembly, fabricated padding, or
mismatch-hiding mechanism is present.
