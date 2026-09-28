# `func_80050260` — exact callback forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 360 and Tier-2
continuation rung 19.

## Function hood and retail span

- File `[0x40A60,0x40A80)`, VRAM `[0x80050260,0x80050280)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x80050278`, followed by `nop` in the delay
  slot.
- There is no direct `jal` caller. Six independent sites construct the exact
  start and store it into callback slot `+0x88`: `0x8004C400–0x8004C40C`,
  `0x8004EB08–0x8004EB14`, `0x80044200–0x8004420C`,
  `0x80046FA4–0x80046FB0`, `0x80048774–0x8004877C`, and
  `0x8004E8BC–0x8004E8C4`.
- Preceding real `func_8005022C` ends at file `0x40A58/0x40A5C`;
  following real `func_80050280` begins immediately at file `0x40A80`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATIONS`; the generated
label is supported by six executable pointer registrations and is not
inferred from its name.

## Retail body

```text
40A60 80050260 xxxxxxxx  addiu sp,sp,-0x18
40A64 80050264 xxxxxxxx  sw    ra,0x10(sp)
40A68 80050268 xxxxxxxx  jal   func_80055760
40A6C 8005026C xxxxxxxx  nop
40A70 80050270 xxxxxxxx  lw    ra,0x10(sp)
40A74 80050274 xxxxxxxx  addiu sp,sp,0x18
40A78 80050278 xxxxxxxx  jr    ra
40A7C 8005027C xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_80055760`; five registration sites also invoke it immediately after publishing this wrapper, while the sixth only publishes the callback |
| Stage-0 globals | none accessed directly by this wrapper; any state effects belong to the callee |
| Coloring pressure | none beyond preserving `$ra`; no argument or result needs a new register home |
| `$v0` liveness | dead after the call, matching the void source contract |
| Address retention | none |
| `-O` signal | canonical 24-byte call frame, empty call delay slot, and standard restore/teardown sequence are era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
void func_80055760(void);

void func_80050260(void) {
    func_80055760();
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80050260>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80055760
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

The former active span was `[0x408A8,0x41518)` = `0x0C70`:

```text
asm prefix: 0x40A60 - 0x408A8 = 0x01B8
C leaf:     0x40A80 - 0x40A60 = 0x0020
asm resume: 0x41518 - 0x40A80 = 0x0A98
closure:    0x01B8 + 0x0020 + 0x0A98 = 0x0C70
```

These are boundary-derived text sizes; no aligned object size was used for
the carve.

## Packed span and gates

All sixteen words below were read from both the rebuilt packed executable and
retail. Each pair is equal:

```text
40A50: xxxxxxxx = xxxxxxxx  preceding restore ra
40A54: xxxxxxxx = xxxxxxxx  preceding teardown
40A58: xxxxxxxx = xxxxxxxx  preceding return
40A5C: xxxxxxxx = xxxxxxxx  preceding delay
40A60: xxxxxxxx = xxxxxxxx  leaf 1
40A64: xxxxxxxx = xxxxxxxx  leaf 2
40A68: xxxxxxxx = xxxxxxxx  leaf 3
40A6C: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
40A70: xxxxxxxx = xxxxxxxx  leaf 5
40A74: xxxxxxxx = xxxxxxxx  leaf 6
40A78: xxxxxxxx = xxxxxxxx  leaf 7
40A7C: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
40A80: xxxxxxxx = xxxxxxxx  following function entry
40A84: xxxxxxxx = xxxxxxxx  following word 2
40A88: xxxxxxxx = xxxxxxxx  following word 3
40A8C: xxxxxxxx = xxxxxxxx  following word 4
PACKED_SPAN=EXACT
```

```text
build/extracted/disc1/SLUS_006.62:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
build/disc1.candidate.exe:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 360
```
