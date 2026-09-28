# `func_8007C544` — exact three-word state setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0` with the
established `MASPSX_FILL_STORE_DELAY_SLOT=1` gate. Integrated as matching-C
leaf 345 and Tier-2 probe rung 4.

## Function hood and retail span

- File `[0x6CD44,0x6CD60)`, VRAM `[0x8007C544,0x8007C560)`: `0x1C`
  bytes, seven words.
- Canonical return: `jr ra` at `0x8007C558`; the final `sw` is its live delay
  slot at `0x8007C55C`.
- One exact direct caller exists: `jal func_8007C544` at `0x8007C324`, with
  `a0 = 1` in its delay slot.
- The preceding real `func_8007C484` returns at file `0x6CD34/0x6CD38`,
  followed by two explicit alignment nops at `0x6CD3C/0x6CD40`.
- One alignment nop at `0x6CD60` separates this leaf from the real prologue of
  `func_8007C564` at `0x6CD64`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`; the alignment words are not folded into
the body. This is not padding, data, or a tail entry.

## Retail body

```text
6CD44 8007C544 xxxxxxxx  lui  at,%hi(D_800C0DC0)
6CD48 8007C548 xxxxxxxx  sw   a0,%lo(D_800C0DC0)(at)
6CD4C 8007C54C xxxxxxxx  lui  at,%hi(D_800B6918)
6CD50 8007C550 xxxxxxxx  sw   a1,%lo(D_800B6918)(at)
6CD54 8007C554 xxxxxxxx  lui  at,%hi(D_800C0DBC)
6CD58 8007C558 xxxxxxxx  jr   ra
6CD5C 8007C55C xxxxxxxx  sw   a2,%lo(D_800C0DBC)(at)
```

## Screens

| Screen | Result |
|---|---|
| Frame/callees | no frame, saved registers, calls, or callee buckets |
| Stage-0 globals | all three are zero-initialized 32-bit state; `D_800C0DC0` is read at `0x8007C838` and has later zero/computed writers, `D_800B6918` is read at `0x8007C84C`, and `D_800C0DBC` is read at `0x8007C4B4/CA0C/CB08` |
| Coloring pressure | none; `$a0/$a1/$a2` flow directly to ordered symbolic stores |
| `$v0` liveness | none; void return and no `$v0` use |
| Address retention | none; each absolute store gets a transient assembler `$at` materialization |
| `-O` signal | frame-free ordered stores and a useful return delay slot are the established era `-O2` setter pattern |
| Loop/back-edge | none |
| Relocations | three normalized `HI16/LO16` symbol pairs |

The retail `jr ra; sw` ending selects the existing default-off store-delay
gate. Without that gate, the same C emits `sw; jr ra; nop` (eight content
words); with it, only the independent last store moves into the return delay
slot and all seven retail words match. No compiler/maspsx code changed.

## Minimal C

```c
extern unsigned int D_800C0DC0;
extern unsigned int D_800B6918;
extern unsigned int D_800C0DBC;

void func_8007C544(unsigned int first, unsigned int second,
                   unsigned int third) {
    D_800C0DC0 = first;
    D_800B6918 = second;
    D_800C0DBC = third;
}
```

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_8007C544>:
   0: xxxxxxxx  lui at,0       R_MIPS_HI16 D_800C0DC0
   4: xxxxxxxx  sw  a0,0(at)   R_MIPS_LO16 D_800C0DC0
   8: xxxxxxxx  lui at,0       R_MIPS_HI16 D_800B6918
   c: xxxxxxxx  sw  a1,0(at)   R_MIPS_LO16 D_800B6918
  10: xxxxxxxx  lui at,0       R_MIPS_HI16 D_800C0DBC
  14: xxxxxxxx  jr  ra
  18: xxxxxxxx  sw  a2,0(at)   R_MIPS_LO16 D_800C0DBC

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=three HI16/LO16 pairs
BYTE_EXACT=7/7
```

## Carve geometry

The former active span was `[0x6C93C,0x6E6A4)` = `0x1D68`:

```text
asm prefix: 0x6CD44 - 0x6C93C = 0x0408
C leaf:     0x6CD60 - 0x6CD44 = 0x001C
asm resume: 0x6E6A4 - 0x6CD60 = 0x1944
closure:    0x0408 + 0x001C + 0x1944 = 0x1D68
```

All sizes are derived from boundaries, never aligned object sizes.

## Packed span and gates

```text
6CD34: xxxxxxxx = xxxxxxxx  preceding return
6CD38: xxxxxxxx = xxxxxxxx  preceding delay slot
6CD3C: xxxxxxxx = xxxxxxxx  alignment
6CD40: xxxxxxxx = xxxxxxxx  alignment
6CD44: xxxxxxxx = xxxxxxxx  leaf 1
6CD48: xxxxxxxx = xxxxxxxx  leaf 2
6CD4C: xxxxxxxx = xxxxxxxx  leaf 3
6CD50: xxxxxxxx = xxxxxxxx  leaf 4
6CD54: xxxxxxxx = xxxxxxxx  leaf 5
6CD58: xxxxxxxx = xxxxxxxx  leaf 6
6CD5C: xxxxxxxx = xxxxxxxx  leaf 7 / return delay
6CD60: xxxxxxxx = xxxxxxxx  alignment
6CD64: xxxxxxxx = xxxxxxxx  following function entry
6CD68: xxxxxxxx = xxxxxxxx  following function word 2
PACKED_SPAN=EXACT
```

```text
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 345
```
