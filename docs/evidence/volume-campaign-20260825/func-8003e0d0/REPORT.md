# `func_8003E0D0` — exact conditional state initializer

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 319, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x2E8D0,0x2E8FC)`, VRAM `0x8003E0D0`, is eleven words and
ends in canonical `jr ra; nop`. The exact direct caller is:

```text
file 0x9A70 / VA 0x80019270: jal func_8003E0D0
```

The preceding real `func_8003E0A4` ends at VA `0x8003E0C8/0x8003E0CC`
with `jr ra` and a halfword-store delay slot. The following real
`func_8003E0FC` begins at VA `0x8003E0FC` with `addiu sp,sp,-8`. No padding
belongs to the function span.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLER`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; argument-relative state initializer |
| written-global Stage 0 | no globals; writes argument fields `+0x24` and `+0x28` |
| coloring pressure | loaded data pointer/value share `$v0`; tag byte stays in `$v1` |
| `$v0` liveness | pointer, comparison constant, then selected halfword value on the equal path; no return value |
| address retention | base argument remains `$a0`; loaded pointer is consumed by the byte load |
| optimization signal | load-delay store, branch-delay constant, jump-delay halfword store; era `-O2 -G0` |
| loop/back-edge owner | none |

## C and flags

```c
void func_8003E0D0(void *arg0) {
    unsigned char *data;

    data = *(unsigned char **)arg0;
    *(int *)((unsigned char *)arg0 + 0x24) = 0;
    if (data[2] == 2) {
        *(short *)((unsigned char *)arg0 + 0x28) = 2;
    } else {
        *(short *)((unsigned char *)arg0 + 0x28) = 0;
    }
}
```

Era GCC 2.7.2-psx plus maspsx 2.21 behavior, `-O2 -G0`. The ROM's useful
load/branch/jump delay slots and adjacent era-compiled code select these flags.
No pins, inline assembly, or special maspsx switch is used.

## Full eleven-word comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lw v0,0(a0)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `sw zero,0x24(a0)` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `lbu v1,2(v0)` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,zero,2` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `bne v1,v0,.Lelse` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,zero,2` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `j .Lreturn` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `sh v0,0x28(a0)` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `sh zero,0x28(a0)` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 10 | `xxxxxxxx` | `xxxxxxxx` | `nop` |

The object carries one local `R_MIPS_26 .text` relocation on word 6. Linking
at `0x8003E0D0` normalizes it to retail target `0x8003E0F4`:

```text
00000000 <func_8003E0D0>:
   0: 8c820000  lw     v0,0(a0)
   4: ac800024  sw     zero,36(a0)
   8: 90430002  lbu    v1,2(v0)
   c: 24020002  li     v0,2
  10: 14620003  bne    v1,v0,20
  14: 24020002  li     v0,2
  18: 08000009  j      24                 R_MIPS_26 .text
  1c: a4820028  sh     v0,40(a0)
  20: a4800028  sh     zero,40(a0)
  24: 03e00008  jr     ra
  28: 00000000  nop
```

## Carve and packed-span proof

The active asm span was `[0x2E7D8,0x2EE10)`, size `0x638`:

```text
prefix asm: 0x2E8D0 - 0x2E7D8 = 0x0F8
C leaf:     0x2E8FC - 0x2E8D0 = 0x02C
resume asm: 0x2EE10 - 0x2E8FC = 0x514
closure:    0x0F8 + 0x02C + 0x514 = 0x638
```

The era object contains an alignment nop at offset `0x2C`; trim validation
removes that zero pad and retains exactly the boundary-derived `0x2C` body.

Packed candidate and retail disassemble identically across both boundaries:

```text
8003e0c8: xxxxxxxx  jr     ra
8003e0cc: xxxxxxxx  sh     a2,42(a0)
8003e0d0: xxxxxxxx  lw     v0,0(a0)
8003e0d4: xxxxxxxx  sw     zero,36(a0)
8003e0d8: xxxxxxxx  lbu    v1,2(v0)
8003e0dc: xxxxxxxx  li     v0,2
8003e0e0: xxxxxxxx  bne    v1,v0,0x8003e0f0
8003e0e4: xxxxxxxx  li     v0,2
8003e0e8: xxxxxxxx  j      0x8003e0f4
8003e0ec: xxxxxxxx  sh     v0,40(a0)
8003e0f0: xxxxxxxx  sh     zero,40(a0)
8003e0f4: xxxxxxxx  jr     ra
8003e0f8: xxxxxxxx  nop
8003e0fc: xxxxxxxx  addiu  sp,sp,-8
8003e100: xxxxxxxx  lw     v0,0(a0)
```

```text
retail:    <44 bytes, sha1 78c33f3e89e6>
candidate: <44 bytes, sha1 78c33f3e89e6>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
319
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 319 leaves
```

Result: `MATCHED=11/11`, first phrasing; consecutive-park count resets to zero.
