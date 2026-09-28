# `func_80050C50` — exact offset callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 365 and Tier-2 continuation rung 24.

## Function hood and boundaries

- File `[0x41450,0x41470)`, VA `[0x80050C50,0x80050C70)`: eight words,
  ending in canonical `jr ra; nop`.
- No direct `jal` caller. `0x8004FF38–0x8004FF3C` constructs this exact start
  in `$a1`; `0x8004FF40` passes it to callback registrar `func_800638D8`.
- Real predecessor `func_80050C08` ends at `0x41448/0x4144C`; real successor
  `func_80050C70` starts immediately at `0x41470`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`.

## Retail, screens, and source

```text
41450 80050C50 xxxxxxxx  addiu sp,sp,-0x18
41454 80050C54 xxxxxxxx  sw    ra,0x10(sp)
41458 80050C58 xxxxxxxx  jal   func_80064C54
4145C 80050C5C xxxxxxxx  addiu a0,a0,0x28
41460 80050C60 xxxxxxxx  lw    ra,0x10(sp)
41464 80050C64 xxxxxxxx  addiu sp,sp,0x18
41468 80050C68 xxxxxxxx  jr    ra
4146C 80050C6C xxxxxxxx  nop
```

| Screen | Result |
|---|---|
| Callee | one unresolved `func_80064C54` call |
| Stage-0 | no direct globals; effects belong to callee |
| Coloring / `$v0` | only `$ra`; call result dead |
| Address retention | none |
| `-O` signal | `-O2` mutates `$a0` in the call slot and uses canonical frame |
| Loop | none |

```c
void func_80064C54(int value);

void func_80050C50(int value) {
    func_80064C54(value + 0x28);
}
```

Flags: era `-O2 -G0`; no maspsx gate.

## Single-leaf object

```text
xxxxxxxx <func_80050C50>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80064C54
   c: xxxxxxxx  addiu a0,a0,40
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

Former asm `[0x41408,0x41518)` = `0x0110`:

```text
prefix: 0x41450 - 0x41408 = 0x0048
leaf:   0x41470 - 0x41450 = 0x0020
resume: 0x41518 - 0x41470 = 0x00A8
close:  0x0048 + 0x0020 + 0x00A8 = 0x0110
```

Boundary arithmetic only; no aligned object size was used.

## Packed span and gates

```text
41440: xxxxxxxx = xxxxxxxx  preceding restore s0
41444: xxxxxxxx = xxxxxxxx  preceding teardown
41448: xxxxxxxx = xxxxxxxx  preceding return
4144C: xxxxxxxx = xxxxxxxx  preceding delay
41450: xxxxxxxx = xxxxxxxx  leaf 1
41454: xxxxxxxx = xxxxxxxx  leaf 2
41458: xxxxxxxx = xxxxxxxx  leaf 3
4145C: xxxxxxxx = xxxxxxxx  leaf 4
41460: xxxxxxxx = xxxxxxxx  leaf 5
41464: xxxxxxxx = xxxxxxxx  leaf 6
41468: xxxxxxxx = xxxxxxxx  leaf 7
4146C: xxxxxxxx = xxxxxxxx  leaf 8
41470: xxxxxxxx = xxxxxxxx  following entry
41474: xxxxxxxx = xxxxxxxx  following word 2
41478: xxxxxxxx = xxxxxxxx  following word 3
4147C: xxxxxxxx = xxxxxxxx  following word 4
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 365
```
