# `func_80090664` — exact stream-byte word-output helper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 317, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x80E64,0x80E8C)`, VRAM `0x80090664`, is ten words and ends
in canonical `jr ra` plus a word-store delay slot. Its exact-start witness is:

```text
file 0x8D3DC / VA 0x8009CBDC: .word func_80090664
```

The preceding real `func_8009063C` ends at VA `0x8009065C/0x80090660`;
the following real `func_8009068C` begins at VA `0x8009068C`. Both boundary
words are real instructions, with no padding owned by this function. The
pool's stale `0/0 no caller/ref` row is removed from Tier 3.

`FUNCTION_HOOD=PROVEN_BY_CALLBACK_TABLE_REFERENCE`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; stream-byte consumer/state mutator |
| written-global Stage 0 | no globals; argument-relative writes at `+0`, `+0xF4`, `+0x104` |
| coloring pressure | old cursor retained in `$v1`; increment and flag word use `$v0` |
| `$v0` liveness | no return value; cursor increment then flag read/modify/write |
| address retention | old cursor remains in `$v1` through cursor update and byte load |
| optimization signal | load-delay nop, useful `lbu` in a later load delay, final `sw` in return slot; exact GCC 14 `-O1` family |
| loop/back-edge owner | none |

## C and flags

```c
void func_80090664(void *arg0) {
    unsigned int v0;
    unsigned char *v1;
    unsigned char byte;

    v1 = *(unsigned char **)arg0;
    *(unsigned char **)arg0 = v1 + 1;
    v0 = *(unsigned int *)((unsigned char *)arg0 + 0xF4);
    byte = *v1;
    v0 |= 0x200u;
    *(unsigned int *)((unsigned char *)arg0 + 0xF4) = v0;
    *(unsigned int *)((unsigned char *)arg0 + 0x104) = byte;
}
```

GCC 14.2 production `CFLAGS_LEAF`:

```text
-EL -mips1 -mfp32 -mabi=32 -G0 -fno-pic -mno-abicalls
-ffreestanding -fno-builtin -O1
```

No pins, assembly, maspsx switch, or relocations are involved.

## Full ten-word comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lw v1,0(a0)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `nop` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,v1,1` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0(a0)` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `lw v0,0xF4(a0)` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `lbu v1,0(v1)` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `ori v0,v0,0x200` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0xF4(a0)` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sw v1,0x104(a0)` |

```text
xxxxxxxx <func_80090664>:
   0: xxxxxxxx  lw     v1,0(a0)
   4: xxxxxxxx  nop
   8: xxxxxxxx  addiu  v0,v1,1
   c: xxxxxxxx  sw     v0,0(a0)
  10: xxxxxxxx  lw     v0,244(a0)
  14: xxxxxxxx  lbu    v1,0(v1)
  18: xxxxxxxx  ori    v0,v0,0x200
  1c: xxxxxxxx  sw     v0,244(a0)
  20: xxxxxxxx  jr     ra
  24: xxxxxxxx  sw     v1,260(a0)
```

## Carve and packed-span proof

The active asm span was `[0x80E64,0x80EB4)`, size `0x50`:

```text
prefix asm: 0x80E64 - 0x80E64 = 0x00
C leaf:     0x80E8C - 0x80E64 = 0x28
resume asm: 0x80EB4 - 0x80E8C = 0x28
closure:    0x00 + 0x28 + 0x28 = 0x50
```

Packed candidate and retail disassemble identically across both boundaries:

```text
8009065c: xxxxxxxx  jr     ra
80090660: xxxxxxxx  sw     v1,256(a0)
80090664: xxxxxxxx  lw     v1,0(a0)
80090668: xxxxxxxx  nop
8009066c: xxxxxxxx  addiu  v0,v1,1
80090670: xxxxxxxx  sw     v0,0(a0)
80090674: xxxxxxxx  lw     v0,244(a0)
80090678: xxxxxxxx  lbu    v1,0(v1)
8009067c: xxxxxxxx  ori    v0,v0,0x200
80090680: xxxxxxxx  sw     v0,244(a0)
80090684: xxxxxxxx  jr     ra
80090688: xxxxxxxx  sw     v1,260(a0)
8009068c: xxxxxxxx  lw     v1,0(a0)
80090690: xxxxxxxx  nop
```

```text
retail:    <40 bytes, sha1 b031c1039d37>
candidate: <40 bytes, sha1 b031c1039d37>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
317
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 317 leaves
```

Result: `MATCHED=10/10`, first phrasing; consecutive-park count remains zero.
