# `func_800816F4` — exact fixed-length equality wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 357 and Tier-2
continuation rung 16.

## Function hood and retail span

- File `[0x71EF4,0x71F14)`, VRAM `[0x800816F4,0x80081714)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x8008170C`, with frame teardown in its live
  delay slot at `0x80081710`.
- One exact direct caller exists at `0x8008161C`. It passes a global-backed
  pointer in `$a0` and a 12-byte stack buffer in `$a1`.
- Preceding real `func_80081414` ends at file `0x71EEC/0x71EF0`; following
  real `func_80081714` begins immediately at file `0x71F14`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`; this is not padding, data, or a tail
entry.

## Retail body

```text
71EF4 800816F4 xxxxxxxx  addiu sp,sp,-0x18
71EF8 800816F8 xxxxxxxx  sw    ra,0x10(sp)
71EFC 800816FC xxxxxxxx  jal   func_80071A04
71F00 80081700 xxxxxxxx  addiu a2,zero,0xC
71F04 80081704 xxxxxxxx  lw    ra,0x10(sp)
71F08 80081708 xxxxxxxx  sltiu v0,v0,1
71F0C 8008170C xxxxxxxx  jr    ra
71F10 80081710 xxxxxxxx  addiu sp,sp,0x18
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one handwritten BIOS-vector wrapper, `func_80071A04`; it selects A0-table operation `0x18`, consumes `$a0/$a1/$a2`, and callers use it as a bounded compare |
| Stage-0 globals | none accessed directly by this wrapper |
| Coloring pressure | none beyond preserving `$ra`; the first two arguments flow through unchanged and constant `12` is supplied in the call delay slot |
| `$v0` liveness | callee result stays in `$v0` and is canonicalized in place by `sltiu v0,v0,1` |
| Address retention | none |
| `-O` signal | canonical 24-byte one-call frame, useful constant call-delay slot, in-place boolean result, and teardown return slot are era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
int func_80071A04(const void *left, const void *right, unsigned int size);

int func_800816F4(const void *left, const void *right) {
    return func_80071A04(left, right, 12) == 0;
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_800816F4>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80071A04
   c: xxxxxxxx  addiu a2,zero,12
  10: xxxxxxxx  lw    ra,16(sp)
  14: xxxxxxxx  sltiu v0,v0,1
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  addiu sp,sp,24

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=8/8
```

## Carve geometry

The former active span was `[0x71B10,0x72AAC)` = `0x0F9C`:

```text
asm prefix: 0x71EF4 - 0x71B10 = 0x03E4
C leaf:     0x71F14 - 0x71EF4 = 0x0020
asm resume: 0x72AAC - 0x71F14 = 0x0B98
closure:    0x03E4 + 0x0020 + 0x0B98 = 0x0F9C
```

These are boundary-derived text sizes; no aligned object size was used for
the carve.

## Packed span and gates

All sixteen words below were read from both the rebuilt packed executable and
retail. Each pair is equal:

```text
71EE4: xxxxxxxx = xxxxxxxx  preceding restore s1
71EE8: xxxxxxxx = xxxxxxxx  preceding restore s0
71EEC: xxxxxxxx = xxxxxxxx  preceding return
71EF0: xxxxxxxx = xxxxxxxx  preceding teardown delay
71EF4: xxxxxxxx = xxxxxxxx  leaf 1
71EF8: xxxxxxxx = xxxxxxxx  leaf 2
71EFC: xxxxxxxx = xxxxxxxx  leaf 3
71F00: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
71F04: xxxxxxxx = xxxxxxxx  leaf 5
71F08: xxxxxxxx = xxxxxxxx  leaf 6
71F0C: xxxxxxxx = xxxxxxxx  leaf 7
71F10: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
71F14: xxxxxxxx = xxxxxxxx  following function entry
71F18: xxxxxxxx = xxxxxxxx  following word 2
71F1C: xxxxxxxx = xxxxxxxx  following word 3
71F20: xxxxxxxx = xxxxxxxx  following word 4
PACKED_SPAN=EXACT
```

```text
build/extracted/disc1/SLUS_006.62:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
build/disc1.candidate.exe:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 357
```
