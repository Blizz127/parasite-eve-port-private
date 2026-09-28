# `func_800506E8` — exact registered callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 361 and Tier-2
continuation rung 20.

## Function hood and retail span

- File `[0x40EE8,0x40F08)`, VRAM `[0x800506E8,0x80050708)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x80050700`, followed by `nop` in the delay
  slot.
- There is no direct `jal` caller. `0x8004EC90–0x8004EC94` constructs this
  exact start and `0x8004EC98–0x8004EC9C` passes it to `func_800638D8` as
  the callback paired with the caller's object.
- Preceding real `func_80050690` ends at file `0x40EE0/0x40EE4`;
  following real `func_80050708` begins immediately at file `0x40F08`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`; the generated
label is supported by executable pointer provenance and is not inferred from
its name.

## Retail body

```text
40EE8 800506E8 xxxxxxxx  addiu sp,sp,-0x18
40EEC 800506EC xxxxxxxx  sw    ra,0x10(sp)
40EF0 800506F0 xxxxxxxx  jal   func_80058AA8
40EF4 800506F4 xxxxxxxx  nop
40EF8 800506F8 xxxxxxxx  lw    ra,0x10(sp)
40EFC 800506FC xxxxxxxx  addiu sp,sp,0x18
40F00 80050700 xxxxxxxx  jr    ra
40F04 80050704 xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_80058AA8` |
| Stage-0 globals | none accessed directly by this wrapper; any state effects belong to the callee |
| Coloring pressure | none beyond preserving `$ra` |
| `$v0` liveness | dead after the call, matching the void source contract |
| Address retention | none |
| `-O` signal | canonical 24-byte call frame, empty call delay slot, and standard restore/teardown sequence are era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
void func_80058AA8(void);

void func_800506E8(void) {
    func_80058AA8();
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_800506E8>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80058AA8
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

The former active span was `[0x40A80,0x41518)` = `0x0A98`:

```text
asm prefix: 0x40EE8 - 0x40A80 = 0x0468
C leaf:     0x40F08 - 0x40EE8 = 0x0020
asm resume: 0x41518 - 0x40F08 = 0x0610
closure:    0x0468 + 0x0020 + 0x0610 = 0x0A98
```

These are boundary-derived text sizes; no aligned object size was used for
the carve.

## Packed span and gates

All sixteen packed words, including both four-word boundaries, equal retail:

```text
40ED8: xxxxxxxx = xxxxxxxx  preceding restore s0
40EDC: xxxxxxxx = xxxxxxxx  preceding teardown
40EE0: xxxxxxxx = xxxxxxxx  preceding return
40EE4: xxxxxxxx = xxxxxxxx  preceding delay
40EE8: xxxxxxxx = xxxxxxxx  leaf 1
40EEC: xxxxxxxx = xxxxxxxx  leaf 2
40EF0: xxxxxxxx = xxxxxxxx  leaf 3
40EF4: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
40EF8: xxxxxxxx = xxxxxxxx  leaf 5
40EFC: xxxxxxxx = xxxxxxxx  leaf 6
40F00: xxxxxxxx = xxxxxxxx  leaf 7
40F04: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
40F08: xxxxxxxx = xxxxxxxx  following function entry
40F0C: xxxxxxxx = xxxxxxxx  following word 2
40F10: xxxxxxxx = xxxxxxxx  following call
40F14: xxxxxxxx = xxxxxxxx  following call delay
PACKED_SPAN=EXACT
```

```text
build/extracted/disc1/SLUS_006.62:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
build/disc1.candidate.exe:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 361
```
