# `func_800C2AF0` — exact base-publishing indexed setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 348 and Tier-2 probe rung
7.

## Function hood and retail span

- File `[0xB32F0,0xB3310)`, VRAM `[0x800C2AF0,0x800C2B10)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x800C2B08`, with the indexed store as its
  live delay slot at `0x800C2B0C`.
- A raw executable scan finds eight unique exact direct callers:
  `0x800C7D14`, `0x800C8E58`, `0x800C9B50`, `0x800CA6E8`,
  `0x800CBEF4`, `0x800CCED0`, `0x800CD8B0`, and `0x800CE12C`.
- The preceding real `func_800C2758` ends with `jr ra; nop` at file
  `0xB32E8/0xB32EC`.
- The following real `func_800C2B10` begins immediately at file `0xB3310`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`. This is not padding, data, or a tail
entry.

## Retail body

```text
B32F0 800C2AF0 xxxxxxxx  addiu a0,a0,0xC
B32F4 800C2AF4 xxxxxxxx  sll   a2,a2,2
B32F8 800C2AF8 xxxxxxxx  addu  a2,a2,a0
B32FC 800C2AFC xxxxxxxx  addu  v0,zero,zero
B3300 800C2B00 xxxxxxxx  lui   at,%hi(D_800E2248)
B3304 800C2B04 xxxxxxxx  sw    a0,%lo(D_800E2248)(at)
B3308 800C2B08 xxxxxxxx  jr    ra
B330C 800C2B0C xxxxxxxx  sw    a3,0x48(a2)
```

## Screens

| Screen | Result |
|---|---|
| Callee buckets | no calls or frame |
| Stage-0 global | `D_800E2248` is a mutable base pointer with four earlier direct writers in this active asm region and numerous field readers; adjacent accessors read offsets `+0x08`, `+0x48`, and `+0x70`, independently confirming base-pointer use |
| Coloring pressure | low; scaled index remains in `$a2`, advanced base remains in `$a0`, and value remains in `$a3` |
| `$v0` liveness | explicit zero result is established before either store and remains live through return |
| Address retention | the advanced base is both published globally and retained in `$a0` for indexed address formation |
| `-O` signal | frame-free strength reduction, explicit zero result, and useful return delay slot are the era `-O2` shape |
| Loop/back-edge | none |
| Relocations | one normalized `HI16/LO16` pair for `D_800E2248` |

The second formal is unused. The eight callers supply the remaining arguments
through the common callback wrapper shape; no stronger semantic names are
asserted.

## Minimal C and flags

```c
extern unsigned int *D_800E2248;

int func_800C2AF0(unsigned int *base, int unused, int index,
                  unsigned int value) {
    base += 3;
    D_800E2248 = base;
    base[index + 18] = value;
    return 0;
}
```

Compile: era cc1, `-O2 -G0`; no three-word, store-delay, div, or dispatch
gate. Cc1 itself schedules the independent indexed store into the return
delay slot.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_800C2AF0>:
   0: xxxxxxxx  addiu a0,a0,12
   4: xxxxxxxx  sll   a2,a2,2
   8: xxxxxxxx  addu  a2,a2,a0
   c: xxxxxxxx  move  v0,zero
  10: xxxxxxxx  lui   at,0             R_MIPS_HI16 D_800E2248
  14: xxxxxxxx  sw    a0,0(at)         R_MIPS_LO16 D_800E2248
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  sw    a3,72(a2)

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one HI16/LO16 pair
BYTE_EXACT=8/8
```

## Carve geometry

The former active span was `[0xB2AF8,0xB3340)` = `0x0848`:

```text
asm prefix: 0xB32F0 - 0xB2AF8 = 0x07F8
C leaf:     0xB3310 - 0xB32F0 = 0x0020
asm resume: 0xB3340 - 0xB3310 = 0x0030
closure:    0x07F8 + 0x0020 + 0x0030 = 0x0848
```

All sizes come from split boundaries, never aligned object sizes.

## Packed span and gates

```text
B32E0: xxxxxxxx = xxxxxxxx  preceding epilogue
B32E4: xxxxxxxx = xxxxxxxx  preceding teardown
B32E8: xxxxxxxx = xxxxxxxx  preceding return
B32EC: xxxxxxxx = xxxxxxxx  preceding delay slot
B32F0: xxxxxxxx = xxxxxxxx  leaf 1
B32F4: xxxxxxxx = xxxxxxxx  leaf 2
B32F8: xxxxxxxx = xxxxxxxx  leaf 3
B32FC: xxxxxxxx = xxxxxxxx  leaf 4
B3300: xxxxxxxx = xxxxxxxx  leaf 5
B3304: xxxxxxxx = xxxxxxxx  leaf 6
B3308: xxxxxxxx = xxxxxxxx  leaf 7
B330C: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
B3310: xxxxxxxx = xxxxxxxx  following function entry
B3314: xxxxxxxx = xxxxxxxx  following function word 2
B3318: xxxxxxxx = xxxxxxxx  following function word 3
B331C: xxxxxxxx = xxxxxxxx  following function word 4
PACKED_SPAN=EXACT
```

```text
Compare:  EXACT SHA-1 MATCH
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 348
```
