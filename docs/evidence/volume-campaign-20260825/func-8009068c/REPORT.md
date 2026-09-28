# `func_8009068C` — exact stream-byte word-output helper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 318, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x80E8C,0x80EB4)`, VRAM `0x8009068C`, is ten words and ends
in canonical `jr ra` plus a word-store delay slot. Its exact-start witness is:

```text
file 0x8D3EC / VA 0x8009CBEC: .word func_8009068C
```

The preceding real `func_80090664` ends at VA `0x80090684/0x80090688`;
the following already-matched real `func_800906B4` begins at VA `0x800906B4`.
Both boundary words are real instructions, and no padding belongs to this
span. The pool's stale `0/0 no caller/ref` row is removed from Tier 3.

`FUNCTION_HOOD=PROVEN_BY_CALLBACK_TABLE_REFERENCE`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; stream-byte consumer/state mutator |
| written-global Stage 0 | no globals; argument-relative writes at `+0`, `+0xF4`, `+0x108` |
| coloring pressure | old cursor retained in `$v1`; increment and flag word use `$v0` |
| `$v0` liveness | no return value; cursor increment then flag read/modify/write |
| address retention | old cursor remains in `$v1` through cursor update and byte load |
| optimization signal | load-delay nop, useful `lbu` in a later load delay, final `sw` in return slot; exact GCC 14 `-O1` family |
| loop/back-edge owner | none |

## C and flags

```c
void func_8009068C(void *arg0) {
    unsigned int v0;
    unsigned char *v1;
    unsigned char byte;

    v1 = *(unsigned char **)arg0;
    *(unsigned char **)arg0 = v1 + 1;
    v0 = *(unsigned int *)((unsigned char *)arg0 + 0xF4);
    byte = *v1;
    v0 |= 0x400u;
    *(unsigned int *)((unsigned char *)arg0 + 0xF4) = v0;
    *(unsigned int *)((unsigned char *)arg0 + 0x108) = byte;
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
| 6 | `xxxxxxxx` | `xxxxxxxx` | `ori v0,v0,0x400` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0xF4(a0)` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sw v1,0x108(a0)` |

```text
xxxxxxxx <func_8009068C>:
   0: xxxxxxxx  lw     v1,0(a0)
   4: xxxxxxxx  nop
   8: xxxxxxxx  addiu  v0,v1,1
   c: xxxxxxxx  sw     v0,0(a0)
  10: xxxxxxxx  lw     v0,244(a0)
  14: xxxxxxxx  lbu    v1,0(v1)
  18: xxxxxxxx  ori    v0,v0,0x400
  1c: xxxxxxxx  sw     v0,244(a0)
  20: xxxxxxxx  jr     ra
  24: xxxxxxxx  sw     v1,264(a0)
```

## Carve and packed-span proof

The complete remaining asm span was `[0x80E8C,0x80EB4)`, size `0x28`:

```text
prefix asm: 0x80E8C - 0x80E8C = 0x00
C leaf:     0x80EB4 - 0x80E8C = 0x28
resume asm: 0x80EB4 - 0x80EB4 = 0x00
closure:    0x00 + 0x28 + 0x00 = 0x28
```

Packed candidate and retail disassemble identically across both boundaries:

```text
80090684: xxxxxxxx  jr     ra
80090688: xxxxxxxx  sw     v1,260(a0)
8009068c: xxxxxxxx  lw     v1,0(a0)
80090690: xxxxxxxx  nop
80090694: xxxxxxxx  addiu  v0,v1,1
80090698: xxxxxxxx  sw     v0,0(a0)
8009069c: xxxxxxxx  lw     v0,244(a0)
800906a0: xxxxxxxx  lbu    v1,0(v1)
800906a4: xxxxxxxx  ori    v0,v0,0x400
800906a8: xxxxxxxx  sw     v0,244(a0)
800906ac: xxxxxxxx  jr     ra
800906b0: xxxxxxxx  sw     v1,264(a0)
800906b4: xxxxxxxx  lw     v0,56(a0)
800906b8: xxxxxxxx  lw     v1,0(a0)
```

```text
retail:    <40 bytes, sha1 6b01b8ab994b>
candidate: <40 bytes, sha1 6b01b8ab994b>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
318
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 318 leaves
```

Result: `MATCHED=10/10`, first phrasing; consecutive-park count remains zero.
