# `func_80050728` — exact offset callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 363 and Tier-2
continuation rung 22.

## Function hood and retail span

- File `[0x40F28,0x40F48)`, VRAM `[0x80050728,0x80050748)`: eight words.
- Canonical `jr ra; nop` return at `0x80050740/0x80050744`.
- There is no direct `jal` caller. `0x8004F2EC–0x8004F2F0` constructs this
  exact start in `$a1`, then `0x8004F2F4` passes it to callback registrar
  `func_800638D8`.
- Preceding real `func_80050708` ends at file `0x40F20/0x40F24`;
  following real `func_80050748` begins immediately at `0x40F48`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`.

## Retail body

```text
40F28 80050728 xxxxxxxx  addiu sp,sp,-0x18
40F2C 8005072C xxxxxxxx  sw    ra,0x10(sp)
40F30 80050730 xxxxxxxx  jal   func_80064C54
40F34 80050734 xxxxxxxx  addiu a0,a0,0x5D
40F38 80050738 xxxxxxxx  lw    ra,0x10(sp)
40F3C 8005073C xxxxxxxx  addiu sp,sp,0x18
40F40 80050740 xxxxxxxx  jr    ra
40F44 80050744 xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_80064C54` |
| Stage-0 globals | none directly accessed; any state effect belongs to the callee |
| Coloring pressure | none beyond `$ra`; incoming `$a0` is mutated in place |
| `$v0` liveness | dead after the call |
| Address retention | none |
| `-O` signal | era `-O2` places the sole argument adjustment in the call delay slot and emits the canonical 24-byte frame |
| Loop/back-edge | none |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
void func_80064C54(int value);

void func_80050728(int value) {
    func_80064C54(value + 0x5D);
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80050728>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80064C54
   c: xxxxxxxx  addiu a0,a0,93
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

The former active span was `[0x40F28,0x41518)` = `0x05F0`:

```text
C leaf:     0x40F48 - 0x40F28 = 0x0020
asm resume: 0x41518 - 0x40F48 = 0x05D0
closure:    0x0020 + 0x05D0 = 0x05F0
```

The sizes come only from span boundaries, never aligned object sizes.

## Packed span and gates

```text
40F18: xxxxxxxx = xxxxxxxx  preceding restore
40F1C: xxxxxxxx = xxxxxxxx  preceding teardown
40F20: xxxxxxxx = xxxxxxxx  preceding return
40F24: xxxxxxxx = xxxxxxxx  preceding delay
40F28: xxxxxxxx = xxxxxxxx  leaf 1
40F2C: xxxxxxxx = xxxxxxxx  leaf 2
40F30: xxxxxxxx = xxxxxxxx  leaf 3
40F34: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
40F38: xxxxxxxx = xxxxxxxx  leaf 5
40F3C: xxxxxxxx = xxxxxxxx  leaf 6
40F40: xxxxxxxx = xxxxxxxx  leaf 7
40F44: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
40F48: xxxxxxxx = xxxxxxxx  following entry
40F4C: xxxxxxxx = xxxxxxxx  following word 2
40F50: xxxxxxxx = xxxxxxxx  following word 3
40F54: xxxxxxxx = xxxxxxxx  following word 4
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 363
```
