# `func_80051060` — exact gp-backed state forwarding callback

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G8`, with no
maspsx behavior gate. Integrated as matching-C leaf **392**.

## Function hood first

- File `[0x41860,0x41884)`, VA `[0x80051060,0x80051084)`: `0x24`, nine
  words.
- `func_80050204` materializes the exact start at file `0x40A0C/0x40A10`
  (`lui a1,%hi(func_80051060)` / `addiu a1,a1,%lo(func_80051060)`) and
  passes it to `func_800638D8`. This is an executable callback registration,
  not a label-derived inference.
- The body has a canonical `jr ra` at file `0x4187C` and `nop` delay slot at
  `0x41880`.
- Preceding real `func_8005100C` ends at `0x41858/0x4185C` with `jr ra; nop`.
- Following real `func_80051084` starts at `0x41884`; its absolute-address
  load/store body is already exact C.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION_AND_CANONICAL_RETURN`.

## Retail body and screens

```text
41860 80051060 xxxxxxxx  lw    a0,0x1E8(gp)
41864 80051064 xxxxxxxx  addiu sp,sp,-0x18
41868 80051068 xxxxxxxx  sw    ra,0x10(sp)
4186C 8005106C xxxxxxxx  jal   func_80053648
41870 80051070 xxxxxxxx  nop
41874 80051074 xxxxxxxx  lw    ra,0x10(sp)
41878 80051078 xxxxxxxx  addiu sp,sp,0x18
4187C 8005107C xxxxxxxx  jr    ra
41880 80051080 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct unresolved callee, `func_80053648`; it consumes the forwarded state pointer and reads byte fields `+4`, `+5`, and `+6` before downstream calls |
| Stage-0 globals | gp base `0x8009CD70` plus `0x1E8` resolves to `D_8009CF58`; retail initializes it from pointer-returning `func_800532B4`, and users dereference byte fields, proving pointer state rather than a scalar |
| Coloring pressure | minimal; the gp load goes directly to argument register `$a0` and no value must survive the call |
| `$v0` liveness | the callee result is discarded; the void wrapper has no result canonicalization |
| Address retention | no computed address; the sole state address is a direct gp-relative pointer load |
| `-O` signal | compact 24-byte call frame and established era call scheduling select `-O2` |
| Loop/back-edge | none |

The retail `lw ...($gp)` requires `-G8`; `_gp=0x8009CD70` gives
`D_8009CF58 - _gp = 0x1E8`.

## Minimal C and single-leaf comparison

```c
/* gp base 0x8009CD70; gp+0x1E8 resolves to D_8009CF58. */
extern unsigned char *D_8009CF58;

void func_80053648(unsigned char *state);

void func_80051060(void) {
    func_80053648(D_8009CF58);
}
```

```text
xxxxxxxx <func_80051060>:
   0: xxxxxxxx  lw    a0,0(gp)     R_MIPS_GPREL16 D_8009CF58
   4: xxxxxxxx  addiu sp,sp,-24
   8: xxxxxxxx  sw    ra,16(sp)
   c: xxxxxxxx  jal   0            R_MIPS_26 func_80053648
  10: xxxxxxxx  nop
  14: xxxxxxxx  lw    ra,16(sp)
  18: xxxxxxxx  addiu sp,sp,24
  1c: xxxxxxxx  jr    ra
  20: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_GPREL16 D_8009CF58; R_MIPS_26 func_80053648
BYTE_EXACT=9/9
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G8`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The former active asm span was `[0x41520,0x41884)` = `0x364`:

```text
asm prefix: 0x41860 - 0x41520 = 0x340
C leaf:     0x41884 - 0x41860 = 0x024
closure:    0x340 + 0x024 = 0x364
```

The leaf closes directly against existing C `func_80051084`; no zero-size or
invented resume span is created. Sizes are retail boundary arithmetic, not
aligned object sizes.

## Packed span and gates

```text
file    VA        retail   candidate  role
41850 80051050  xxxxxxxx xxxxxxxx   preceding s0 restore
41854 80051054  xxxxxxxx xxxxxxxx   preceding frame teardown
41858 80051058  xxxxxxxx xxxxxxxx   preceding return
4185C 8005105C  xxxxxxxx xxxxxxxx   preceding delay slot
41860 80051060  xxxxxxxx xxxxxxxx   leaf 1
41864 80051064  xxxxxxxx xxxxxxxx   leaf 2
41868 80051068  xxxxxxxx xxxxxxxx   leaf 3
4186C 8005106C  xxxxxxxx xxxxxxxx   leaf 4
41870 80051070  xxxxxxxx xxxxxxxx   leaf 5
41874 80051074  xxxxxxxx xxxxxxxx   leaf 6
41878 80051078  xxxxxxxx xxxxxxxx   leaf 7
4187C 8005107C  xxxxxxxx xxxxxxxx   leaf 8
41880 80051080  xxxxxxxx xxxxxxxx   leaf 9
41884 80051084  xxxxxxxx xxxxxxxx   following real entry
41888 80051088  xxxxxxxx xxxxxxxx   following address low
4188C 8005108C  xxxxxxxx xxxxxxxx   following gp store
41890 80051090  xxxxxxxx xxxxxxxx   following return
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0
RESULT: EXACT MATCH
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

VERIFY_RC=0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 392 leaves

grep matching-C count: 392
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
