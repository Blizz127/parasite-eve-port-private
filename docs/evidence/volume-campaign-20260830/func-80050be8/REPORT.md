# `func_80050BE8` — exact registered callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 364 and Tier-2 continuation rung 23.

## Function hood and retail span

- File `[0x413E8,0x41408)`, VA `[0x80050BE8,0x80050C08)`: eight words.
- Canonical `jr ra; nop` return at `0x80050C00/0x80050C04`.
- No direct `jal` caller. `0x8004FD80–0x8004FD84` constructs this exact start
  in `$a1`; `0x8004FD88` passes it to callback registrar `func_800638D8`.
- Real predecessor `func_80050B94` ends at `0x413E0/0x413E4`; real successor
  `func_80050C08` begins immediately at `0x41408`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`.

## Retail, screens, and C

```text
413E8 80050BE8 xxxxxxxx  addiu sp,sp,-0x18
413EC 80050BEC xxxxxxxx  sw    ra,0x10(sp)
413F0 80050BF0 xxxxxxxx  jal   func_80057F14
413F4 80050BF4 xxxxxxxx  nop
413F8 80050BF8 xxxxxxxx  lw    ra,0x10(sp)
413FC 80050BFC xxxxxxxx  addiu sp,sp,0x18
41400 80050C00 xxxxxxxx  jr    ra
41404 80050C04 xxxxxxxx  nop
```

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_80057F14` |
| Stage-0 globals | none directly accessed; effects belong to the callee |
| Coloring pressure | only `$ra` preservation |
| `$v0` liveness | dead after call; void contract |
| Address retention | none |
| `-O` signal | canonical era `-O2` 24-byte call frame and empty call slot |
| Loop/back-edge | none |

```c
void func_80057F14(void);

void func_80050BE8(void) {
    func_80057F14();
}
```

Flags: era `-O2 -G0`; no maspsx gate.

## Single-leaf comparison

```text
xxxxxxxx <func_80050BE8>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_80057F14
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

Former asm `[0x40F48,0x41518)` = `0x05D0`:

```text
prefix: 0x413E8 - 0x40F48 = 0x04A0
leaf:   0x41408 - 0x413E8 = 0x0020
resume: 0x41518 - 0x41408 = 0x0110
close:  0x04A0 + 0x0020 + 0x0110 = 0x05D0
```

All sizes are boundary arithmetic, not aligned object sizes.

## Packed span and gates

```text
413D8: xxxxxxxx = xxxxxxxx  preceding restore s0
413DC: xxxxxxxx = xxxxxxxx  preceding teardown
413E0: xxxxxxxx = xxxxxxxx  preceding return
413E4: xxxxxxxx = xxxxxxxx  preceding delay
413E8: xxxxxxxx = xxxxxxxx  leaf 1
413EC: xxxxxxxx = xxxxxxxx  leaf 2
413F0: xxxxxxxx = xxxxxxxx  leaf 3
413F4: xxxxxxxx = xxxxxxxx  leaf 4
413F8: xxxxxxxx = xxxxxxxx  leaf 5
413FC: xxxxxxxx = xxxxxxxx  leaf 6
41400: xxxxxxxx = xxxxxxxx  leaf 7
41404: xxxxxxxx = xxxxxxxx  leaf 8
41408: xxxxxxxx = xxxxxxxx  following entry
4140C: xxxxxxxx = xxxxxxxx  following word 2
41410: xxxxxxxx = xxxxxxxx  following word 3
41414: xxxxxxxx = xxxxxxxx  following word 4
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 364
```
