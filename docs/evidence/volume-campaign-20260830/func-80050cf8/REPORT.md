# `func_80050CF8` — exact constant callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 366 and Tier-2 continuation rung 25.

## Function hood

- File `[0x414F8,0x41518)`, VA `[0x80050CF8,0x80050D18)`: eight words,
  ending in canonical `jr ra; nop`.
- No direct `jal` caller. `0x8004FFB0–0x8004FFB4` constructs the exact start
  and `0x8004FFB8` passes it in `$a1` to callback registrar `func_800638D8`.
- Real predecessor `func_80050CB4` ends at `0x414F0/0x414F4`; already-proven
  real `func_80050D18` starts immediately at `0x41518`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`.

## Retail, screens, and C

```text
414F8 80050CF8 xxxxxxxx  addiu sp,sp,-0x18
414FC 80050CFC xxxxxxxx  sw    ra,0x10(sp)
41500 80050D00 xxxxxxxx  jal   func_80064C54
41504 80050D04 xxxxxxxx  addiu a0,zero,8
41508 80050D08 xxxxxxxx  lw    ra,0x10(sp)
4150C 80050D0C xxxxxxxx  addiu sp,sp,0x18
41510 80050D10 xxxxxxxx  jr    ra
41514 80050D14 xxxxxxxx  nop
```

| Screen | Result |
|---|---|
| Callee | one unresolved `func_80064C54` call |
| Stage-0 | no direct globals; effects belong to callee |
| Coloring / `$v0` | only `$ra`; call result dead |
| Address retention | none |
| `-O` signal | `-O2` materializes constant 8 in the call delay slot |
| Loop | none |

```c
void func_80064C54(int value);

void func_80050CF8(void) {
    func_80064C54(8);
}
```

Flags: era `-O2 -G0`; no maspsx gate.

## Object and ROM

```text
xxxxxxxx <func_80050CF8>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80064C54
   c: xxxxxxxx  li    a0,8
  10: xxxxxxxx  lw    ra,16(sp)
  14: xxxxxxxx  addiu sp,sp,24
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=8/8
```

## Carve

Former asm `[0x41470,0x41518)` = `0x00A8`:

```text
prefix: 0x414F8 - 0x41470 = 0x0088
leaf:   0x41518 - 0x414F8 = 0x0020
close:  0x0088 + 0x0020 = 0x00A8
```

Boundary arithmetic only; no aligned object size.

## Packed span and gates

```text
414E8: xxxxxxxx = xxxxxxxx  preceding restore s0
414EC: xxxxxxxx = xxxxxxxx  preceding teardown
414F0: xxxxxxxx = xxxxxxxx  preceding return
414F4: xxxxxxxx = xxxxxxxx  preceding delay
414F8: xxxxxxxx = xxxxxxxx  leaf 1
414FC: xxxxxxxx = xxxxxxxx  leaf 2
41500: xxxxxxxx = xxxxxxxx  leaf 3
41504: xxxxxxxx = xxxxxxxx  leaf 4
41508: xxxxxxxx = xxxxxxxx  leaf 5
4150C: xxxxxxxx = xxxxxxxx  leaf 6
41510: xxxxxxxx = xxxxxxxx  leaf 7
41514: xxxxxxxx = xxxxxxxx  leaf 8
41518: xxxxxxxx = xxxxxxxx  following `func_80050D18` entry
4151C: xxxxxxxx = xxxxxxxx  following return delay
41520: xxxxxxxx = xxxxxxxx  next real prologue
41524: xxxxxxxx = xxxxxxxx  next real word 2
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 366
```
