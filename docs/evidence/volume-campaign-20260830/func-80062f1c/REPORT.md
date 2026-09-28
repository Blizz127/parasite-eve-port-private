# `func_80062F1C` — exact node-removal forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 347 and Tier-2 probe rung
6.

## Function hood and retail span

- File `[0x5371C,0x5373C)`, VRAM `[0x80062F1C,0x80062F3C)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra; nop` at `0x80062F34/0x80062F38`.
- A raw executable scan finds **57 unique direct `jal` targets** at the exact
  start. Representative callers are `0x80044768`, `0x80044C20`,
  `0x80045150`, `0x8004C2D8`, `0x8004D390`, and `0x8004F10C`.
- The preceding real `func_80062D2C` ends with `jr ra; nop` at file
  `0x53714/0x53718`.
- The following real `func_80062F3C` begins immediately at file `0x5373C`
  with its stack-frame prologue.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`. This is not padding, data, or a tail
entry.

## Retail body

```text
5371C 80062F1C xxxxxxxx  addiu sp,sp,-0x18
53720 80062F20 xxxxxxxx  sw    ra,0x10(sp)
53724 80062F24 xxxxxxxx  jal   func_8006269C
53728 80062F28 xxxxxxxx  nop
5372C 80062F2C xxxxxxxx  lw    ra,0x10(sp)
53730 80062F30 xxxxxxxx  addiu sp,sp,0x18
53734 80062F34 xxxxxxxx  jr    ra
53738 80062F38 xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_8006269C`; its body recursively detaches the supplied node from a GP-backed list, releases descendants, and clears references to it |
| Caller/reference census | 57 exact direct calls in the executable; no function-hood inference rests on the generated label |
| Stage-0 globals | none accessed by the wrapper; all global/list effects belong to the callee |
| Coloring pressure | none; the incoming node pointer remains naturally in `$a0` through the call |
| `$v0` liveness | wrapper specifies no result and does not consume or rewrite the callee's incidental `$v0`; retail likewise performs only epilogue operations after the call |
| Address retention | none |
| `-O` signal | canonical 24-byte one-call frame and ordinary epilogue are the established era `-O2` wrapper shape |
| Loop/back-edge | none in the wrapper; recursion and loops are wholly owned by the callee |
| Relocations | one normalized `R_MIPS_26` call relocation |

The wrapper adds no policy or state of its own. Its semantic contract is the
forwarding call; a more specific public name is not asserted without symbols.

## Minimal C and flags

```c
void func_8006269C(void *node);

void func_80062F1C(void *node) {
    func_8006269C(node);
}
```

Compile: era cc1, `-O2 -G0`; no three-word, store-delay, div, or dispatch
gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80062F1C>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_8006269C
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

The former active span was `[0x534E4,0x53958)` = `0x0474`:

```text
asm prefix: 0x5371C - 0x534E4 = 0x0238
C leaf:     0x5373C - 0x5371C = 0x0020
asm resume: 0x53958 - 0x5373C = 0x021C
closure:    0x0238 + 0x0020 + 0x021C = 0x0474
```

All sizes come from split boundaries, never aligned object sizes.

## Packed span and gates

```text
5370C: xxxxxxxx = xxxxxxxx  preceding epilogue
53710: xxxxxxxx = xxxxxxxx  preceding teardown
53714: xxxxxxxx = xxxxxxxx  preceding return
53718: xxxxxxxx = xxxxxxxx  preceding delay slot
5371C: xxxxxxxx = xxxxxxxx  leaf 1
53720: xxxxxxxx = xxxxxxxx  leaf 2
53724: xxxxxxxx = xxxxxxxx  leaf 3
53728: xxxxxxxx = xxxxxxxx  leaf 4
5372C: xxxxxxxx = xxxxxxxx  leaf 5
53730: xxxxxxxx = xxxxxxxx  leaf 6
53734: xxxxxxxx = xxxxxxxx  leaf 7
53738: xxxxxxxx = xxxxxxxx  leaf 8
5373C: xxxxxxxx = xxxxxxxx  following function entry
53740: xxxxxxxx = xxxxxxxx  following function word 2
53744: xxxxxxxx = xxxxxxxx  following function word 3
53748: xxxxxxxx = xxxxxxxx  following function word 4
PACKED_SPAN=EXACT
```

```text
Compare:  EXACT SHA-1 MATCH
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 347
```
