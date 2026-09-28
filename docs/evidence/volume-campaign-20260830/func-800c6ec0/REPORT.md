# `func_800C6EC0` — exact dual unsigned-halfword setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`.
Integrated as matching-C leaf 343 and Tier-2 probe rung 2.

## Function hood and retail span

- File `[0xB76C0,0xB76D8)`, VRAM `[0x800C6EC0,0x800C6ED8)`: `0x18`
  bytes, six words.
- Canonical return: `jr ra; nop` at `0x800C6ED0/0x800C6ED4`.
- Preceding real boundary: `func_800C6D5C` ends with `jr ra; nop` at file
  `0xB76B8/0xB76BC`.
- Following real boundary: the already proven `func_800C6ED8` starts with
  `lui at,0x800F` at file `0xB76D8`.
- A raw scan for the exact `jal` word `0x0C031BB0` found seven callers:

```text
file       VA
C70B4      800D68B4
CA5C0      800D9DC0
CAD30      800DA530
CCB64      800DC364
CCD3C      800DC53C
CEC18      800DE418
CED20      800DE520
```

`FUNCTION_HOOD=PROVEN`; this is not padding, data, or a tail fragment.

## Screens

| Screen | Result |
|---|---|
| Frame/callees | no frame, saved registers, calls, or callee buckets |
| Stage-0 globals | this is the sole `sh` writer of `D_800F346C` and `D_800F3414`; `func_800C71E4` reads both with `lhu` |
| Coloring pressure | none; incoming `$a0/$a1` go directly to their stores |
| `$v0` liveness | none; `void` return and no `$v0` write |
| Address retention | none; each direct symbolic halfword store uses transient assembler `$at` |
| `-O` signal | frame-free ordered stores plus canonical return; established era `-O2` setter shape |
| Loop/back-edge | none |
| Relocations | two ordinary `HI16/LO16` symbol pairs |

Flags are era `-O2 -G0`, with no maspsx opt-in gate, pins, or assembly.

## Minimal C

```c
extern unsigned short D_800F346C;
extern unsigned short D_800F3414;

void func_800C6EC0(unsigned int first, unsigned int second) {
    D_800F346C = (unsigned short)first;
    D_800F3414 = (unsigned short)second;
}
```

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_800C6EC0>:
   0: xxxxxxxx  lui at,0x0       R_MIPS_HI16 D_800F346C
   4: xxxxxxxx  sh  a0,0(at)     R_MIPS_LO16 D_800F346C
   8: xxxxxxxx  lui at,0x0       R_MIPS_HI16 D_800F3414
   c: xxxxxxxx  sh  a1,0(at)     R_MIPS_LO16 D_800F3414
  10: xxxxxxxx  jr  ra
  14: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=two HI16/LO16 pairs
BYTE_EXACT=6/6
```

## Carve geometry

The prior active prefix was `[0xB3368,0xB76D8)` = `0x4370`:

```text
asm prefix: 0xB76C0 - 0xB3368 = 0x4358
C leaf:     0xB76D8 - 0xB76C0 = 0x0018
closure:    0x4358 + 0x0018 = 0x4370
```

These are boundary-derived sizes; the aligned object size was not used.

## Packed span and gates

```text
B76B8: xxxxxxxx = xxxxxxxx  preceding return
B76BC: xxxxxxxx = xxxxxxxx  preceding real delay slot
B76C0: xxxxxxxx = xxxxxxxx  leaf 1
B76C4: xxxxxxxx = xxxxxxxx  leaf 2
B76C8: xxxxxxxx = xxxxxxxx  leaf 3
B76CC: xxxxxxxx = xxxxxxxx  leaf 4
B76D0: xxxxxxxx = xxxxxxxx  leaf 5
B76D4: xxxxxxxx = xxxxxxxx  leaf 6
B76D8: xxxxxxxx = xxxxxxxx  following function entry
B76DC: xxxxxxxx = xxxxxxxx  following function word 2
PACKED_SPAN=EXACT
```

```text
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 343
```

No pin, inline/file-scope assembly, fabricated padding, or mismatch-hiding
mechanism is present.
