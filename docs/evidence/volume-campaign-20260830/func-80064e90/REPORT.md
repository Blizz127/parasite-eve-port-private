# `func_80064E90` — exact nested-state clear wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 379 and Tier-2 continuation rung 38.

## Function hood and screens

- File `[0x55690,0x556B4)`, VA `[0x80064E90,0x80064EB4)`: nine words,
  ending in canonical `jr ra; nop`.
- Exact direct caller `0x80064924` targets the start.
- Preceding real `func_80064D08` ends at `0x55688/0x5568C`; following real
  `func_80064EB4` starts immediately at `0x556B4`.
- One unresolved callee, `func_8006269C`; no globals or loop. The field-34
  pointer remains in `$v0` only until its nested `+0x80` store in the call
  delay slot. The call result is dead and there is no result-register pressure.
- Typed offsets `0x34` and `0x80` are directly retail-proven. No symbolic
  addressing or constant-materialization signal requires a maspsx gate;
  established era `-O2 -G0` call scheduling is sufficient.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`.

## C and object

```c
typedef struct {
    unsigned char pad00[0x34];
    int *state;
} Func80064E90Object;

void func_8006269C(Func80064E90Object *object);

void func_80064E90(Func80064E90Object *object) {
    object->state[0x20] = 0;
    func_8006269C(object);
}
```

Flags: era `-O2 -G0`; no maspsx gate.

```text
55690: xxxxxxxx  addiu sp,sp,-24
55694: xxxxxxxx  sw    ra,16(sp)
55698: xxxxxxxx  lw    v0,52(a0)
5569C: xxxxxxxx  jal   func_8006269C
556A0: xxxxxxxx  sw    zero,128(v0)
556A4: xxxxxxxx  lw    ra,16(sp)
556A8: xxxxxxxx  addiu sp,sp,24
556AC: xxxxxxxx  jr    ra
556B0: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_26 func_8006269C
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x55454,0x55BB8)` = `0x0764`:

```text
prefix: 0x55690 - 0x55454 = 0x023C
leaf:   0x556B4 - 0x55690 = 0x0024
resume: 0x55BB8 - 0x556B4 = 0x0504
close:  0x023C + 0x0024 + 0x0504 = 0x0764
```

```text
55680: xxxxxxxx = xxxxxxxx  preceding epilogue
55684: xxxxxxxx = xxxxxxxx
55688: xxxxxxxx = xxxxxxxx
5568C: xxxxxxxx = xxxxxxxx
55690: xxxxxxxx = xxxxxxxx  leaf 1
55694: xxxxxxxx = xxxxxxxx  leaf 2
55698: xxxxxxxx = xxxxxxxx  leaf 3
5569C: xxxxxxxx = xxxxxxxx  leaf 4
556A0: xxxxxxxx = xxxxxxxx  leaf 5
556A4: xxxxxxxx = xxxxxxxx  leaf 6
556A8: xxxxxxxx = xxxxxxxx  leaf 7
556AC: xxxxxxxx = xxxxxxxx  leaf 8
556B0: xxxxxxxx = xxxxxxxx  leaf 9
556B4: xxxxxxxx = xxxxxxxx  following real prologue
556B8: xxxxxxxx = xxxxxxxx
556BC: xxxxxxxx = xxxxxxxx
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/build_us.sh: exit 0; Compare: EXACT SHA-1 MATCH
matching-C count: 379
```
