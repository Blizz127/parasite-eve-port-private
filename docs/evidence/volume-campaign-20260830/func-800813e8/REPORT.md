# `func_800813E8` — exact registered callback wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 367 and Tier-2 continuation rung 26.

## Function hood

- File `[0x71BE8,0x71C08)`, VA `[0x800813E8,0x80081408)`: eight words,
  ending in canonical `jr ra; nop`.
- There is no direct `jal` caller. `func_80081314` constructs the exact start
  at `0x80081364–0x80081368` and passes it at `0x8008136C` to
  `func_800824C8`, the proven callback-pointer exchange for `D_800B8AB4`.
- Real predecessor `func_80081314` ends at file `0x71BE0/0x71BE4`.
- The leaf is followed by three explicit alignment nops at
  `0x71C08..0x71C10`; real `func_80081414` begins at `0x71C14` with a
  nontrivial prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`. The generated
label is not being used as function proof.

## Retail, screens, and C

```text
71BE8 800813E8 xxxxxxxx  addiu sp,sp,-0x18
71BEC 800813EC xxxxxxxx  sw    ra,0x10(sp)
71BF0 800813F0 xxxxxxxx  jal   func_8007C564
71BF4 800813F4 xxxxxxxx  nop
71BF8 800813F8 xxxxxxxx  lw    ra,0x10(sp)
71BFC 800813FC xxxxxxxx  addiu sp,sp,0x18
71C00 80081400 xxxxxxxx  jr    ra
71C04 80081404 xxxxxxxx  nop
```

| Screen | Result |
|---|---|
| Callee | one unresolved `func_8007C564` call |
| Stage-0 | no direct global access; effects belong to the callee |
| Coloring / `$v0` | only `$ra`; call result is dead |
| Address retention | none |
| `-O` signal | canonical era `-O2` one-call frame; empty call delay slot |
| Loop | none |

```c
void func_8007C564(void);

void func_800813E8(void) {
    func_8007C564();
}
```

Flags: era `-O2 -G0`; no maspsx gate.

## Single-leaf object

```text
xxxxxxxx <func_800813E8>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_8007C564
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

Former asm `[0x71B10,0x71EF4)` = `0x03E4`:

```text
prefix: 0x71BE8 - 0x71B10 = 0x00D8
leaf:   0x71C08 - 0x71BE8 = 0x0020
resume: 0x71EF4 - 0x71C08 = 0x02EC
close:  0x00D8 + 0x0020 + 0x02EC = 0x03E4
```

These are boundary-derived sizes, never aligned object sizes.

## Packed span and gates

The packed candidate and retail words are identical:

```text
71BD8: xxxxxxxx = xxxxxxxx  preceding restore s1
71BDC: xxxxxxxx = xxxxxxxx  preceding restore s0
71BE0: xxxxxxxx = xxxxxxxx  preceding return
71BE4: xxxxxxxx = xxxxxxxx  preceding teardown delay
71BE8: xxxxxxxx = xxxxxxxx  leaf 1
71BEC: xxxxxxxx = xxxxxxxx  leaf 2
71BF0: xxxxxxxx = xxxxxxxx  leaf 3
71BF4: xxxxxxxx = xxxxxxxx  leaf 4
71BF8: xxxxxxxx = xxxxxxxx  leaf 5
71BFC: xxxxxxxx = xxxxxxxx  leaf 6
71C00: xxxxxxxx = xxxxxxxx  leaf 7
71C04: xxxxxxxx = xxxxxxxx  leaf 8
71C08: xxxxxxxx = xxxxxxxx  alignment nop 1
71C0C: xxxxxxxx = xxxxxxxx  alignment nop 2
71C10: xxxxxxxx = xxxxxxxx  alignment nop 3
71C14: xxxxxxxx = xxxxxxxx  following real prologue
71C18: xxxxxxxx = xxxxxxxx  following word 2
71C1C: xxxxxxxx = xxxxxxxx  following word 3
71C20: xxxxxxxx = xxxxxxxx  following word 4
71C24: xxxxxxxx = xxxxxxxx  following word 5
PACKED_SPAN=EXACT
```

```text
retail SHA-1:    452fb033f2eaa4b18aa20a5bca60b8125af3a37b
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 367
```
