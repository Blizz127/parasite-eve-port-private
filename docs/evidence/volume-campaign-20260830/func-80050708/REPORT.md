# `func_80050708` — exact offset callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 362 and Tier-2
continuation rung 21.

## Function hood and retail span

- File `[0x40F08,0x40F28)`, VRAM `[0x80050708,0x80050728)`: eight words.
- Canonical `jr ra; nop` return at `0x80050720/0x80050724`.
- There is no direct `jal` caller. `0x8004EF38–0x8004EF3C` constructs this
  exact start in `$a1`, then `0x8004EF40` passes it to callback registrar
  `func_800638D8`.
- Preceding real `func_800506E8` ends at file `0x40F00/0x40F04`;
  following real `func_80050728` begins immediately at `0x40F28`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`.

## Retail body

```text
40F08 80050708 xxxxxxxx  addiu sp,sp,-0x18
40F0C 8005070C xxxxxxxx  sw    ra,0x10(sp)
40F10 80050710 xxxxxxxx  jal   func_80064C54
40F14 80050714 xxxxxxxx  addiu a0,a0,0x1F
40F18 80050718 xxxxxxxx  lw    ra,0x10(sp)
40F1C 8005071C xxxxxxxx  addiu sp,sp,0x18
40F20 80050720 xxxxxxxx  jr    ra
40F24 80050724 xxxxxxxx  nop
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

void func_80050708(int value) {
    func_80064C54(value + 0x1F);
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80050708>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80064C54
   c: xxxxxxxx  addiu a0,a0,31
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

The former active span was `[0x40F08,0x41518)` = `0x0610`:

```text
C leaf:     0x40F28 - 0x40F08 = 0x0020
asm resume: 0x41518 - 0x40F28 = 0x05F0
closure:    0x0020 + 0x05F0 = 0x0610
```

The sizes come only from span boundaries, never aligned object sizes.

## Packed span and gates

```text
40EF8: xxxxxxxx = xxxxxxxx  preceding restore
40EFC: xxxxxxxx = xxxxxxxx  preceding teardown
40F00: xxxxxxxx = xxxxxxxx  preceding return
40F04: xxxxxxxx = xxxxxxxx  preceding delay
40F08: xxxxxxxx = xxxxxxxx  leaf 1
40F0C: xxxxxxxx = xxxxxxxx  leaf 2
40F10: xxxxxxxx = xxxxxxxx  leaf 3
40F14: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
40F18: xxxxxxxx = xxxxxxxx  leaf 5
40F1C: xxxxxxxx = xxxxxxxx  leaf 6
40F20: xxxxxxxx = xxxxxxxx  leaf 7
40F24: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
40F28: xxxxxxxx = xxxxxxxx  following entry
40F2C: xxxxxxxx = xxxxxxxx  following word 2
40F30: xxxxxxxx = xxxxxxxx  following call
40F34: xxxxxxxx = xxxxxxxx  following call delay
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 362
```
