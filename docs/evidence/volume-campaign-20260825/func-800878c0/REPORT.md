# `func_800878C0` — exact per-voice SPU bitfield update

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 323, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x780C0,0x780F0)`, VRAM `[0x800878C0,0x800878F0)`, is
`0x30` bytes / twelve words. It ends in canonical `jr ra` with its halfword
store in the return delay slot. The exact direct caller is the `jal` at
`0x80087A48`.

The preceding real `func_8008788C` ends at `0x800878B8/0x800878BC` with
`jr ra` and a halfword-store delay slot. The following real
`func_800878F0` begins exactly at `0x800878F0` with `addiu sp,sp,-0x20`.
Both boundaries are executable instructions and neither belongs to padding.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLER`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; MMIO read/modify/write leaf |
| written-state Stage 0 | no RAM global; writes the per-voice SPU halfword at `0x1F801C0A + voice*0x10` |
| coloring pressure | retail mutates `$a0` into the MMIO address, `$a2` into the shifted field, and combines through `$v0`; `$a1` remains the low field |
| `$v0` liveness | fixed base, then loaded halfword, then masked/final value through the return-slot store |
| address retention | computed MMIO pointer remains in `$a0` from `lhu` through `sh` |
| optimization signal | fixed-address `lui; ori`, in-place argument transforms, grouped OR tree, and return-delay store select era GCC 2.7.2 `-O2 -G0` |
| loop/back-edge owner | none |

The hardware address proves a per-voice SPU control-register update; its
layout is consistent with the second ADSR halfword. No exact Psy-Q routine
name is assigned without symbols. The operation preserves bits 6–15 and
combines `low` with `(mode >> 2) << 5` into the low field.

## C and flags

```c
void func_800878C0(int voice, unsigned int low, unsigned int mode) {
    unsigned short *reg =
        (unsigned short *)(0x1F801C0A + (voice << 4));

    *reg = (unsigned short)((*reg & 0xFFC0) |
                            (((mode >> 2) << 5) | low));
}
```

Era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`; first phrasing, with no pins,
inline assembly, or special maspsx switch. Grouping the two incoming fields
selects retail's `$a2 |= $a1` dataflow. The nonvolatile lvalue allows the
ordinary compiler to place `sh` in the `jr` delay slot; this is source
semantics, not an assembler patch.

## Full twelve-word comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lui v0,0x1F80` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `ori v0,v0,0x1C0A` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `sll a0,a0,4` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `addu a0,a0,v0` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `srl a2,a2,2` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `sll a2,a2,5` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `lhu v0,0(a0)` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `or a2,a2,a1` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `andi v0,v0,0xFFC0` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `or v0,v0,a2` |
| 10 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 11 | `xxxxxxxx` | `xxxxxxxx` | `sh v0,0(a0)` |

Single-leaf object; there are no relocations to normalize:

```text
xxxxxxxx <func_800878C0>:
   0: xxxxxxxx  lui    v0,0x1f80
   4: xxxxxxxx  ori    v0,v0,0x1c0a
   8: xxxxxxxx  sll    a0,a0,0x4
   c: xxxxxxxx  addu   a0,a0,v0
  10: xxxxxxxx  srl    a2,a2,0x2
  14: xxxxxxxx  sll    a2,a2,0x5
  18: xxxxxxxx  lhu    v0,0(a0)
  1c: xxxxxxxx  or     a2,a2,a1
  20: xxxxxxxx  andi   v0,v0,0xffc0
  24: xxxxxxxx  or     v0,v0,a2
  28: xxxxxxxx  jr     ra
  2c: xxxxxxxx  sh     v0,0(a0)
```

The object is exactly `0x30` bytes, so no body padding is trimmed.

## Carve geometry

The prior asm span was `[0x7800C,0x7B31C)`, size `0x3310`:

```text
prefix asm:  0x780C0 - 0x7800C = 0x00B4
C leaf:      0x780F0 - 0x780C0 = 0x0030
resume asm:  0x7B31C - 0x780F0 = 0x322C
closure:     0x00B4 + 0x0030 + 0x322C = 0x3310
```

All sizes come from file-span subtraction, never aligned object sizes.

## Packed-span proof

Retail and candidate disassemble identically across both boundaries:

```text
800878B0: xxxxxxxx  andi   v0,v0,0x3F
800878B4: xxxxxxxx  or     v0,v0,a2
800878B8: xxxxxxxx  jr     ra
800878BC: xxxxxxxx  sh     v0,0(a0)
800878C0: xxxxxxxx  lui    v0,0x1F80
800878C4: xxxxxxxx  ori    v0,v0,0x1C0A
800878C8: xxxxxxxx  sll    a0,a0,4
800878CC: xxxxxxxx  addu   a0,a0,v0
800878D0: xxxxxxxx  srl    a2,a2,2
800878D4: xxxxxxxx  sll    a2,a2,5
800878D8: xxxxxxxx  lhu    v0,0(a0)
800878DC: xxxxxxxx  or     a2,a2,a1
800878E0: xxxxxxxx  andi   v0,v0,0xFFC0
800878E4: xxxxxxxx  or     v0,v0,a2
800878E8: xxxxxxxx  jr     ra
800878EC: xxxxxxxx  sh     v0,0(a0)
800878F0: xxxxxxxx  addiu  sp,sp,-32
800878F4: xxxxxxxx  sw     s0,16(sp)
800878F8: xxxxxxxx  move   s0,a1
800878FC: xxxxxxxx  sw     ra,28(sp)
80087900: xxxxxxxx  sw     s2,24(sp)
```

```text
retail:    <84 bytes, sha1 883439c465a0>
candidate: <84 bytes, sha1 883439c465a0>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
323
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 323 leaves
```

Result: `MATCHED=12/12`, first phrasing; consecutive-park count resets to
zero.
