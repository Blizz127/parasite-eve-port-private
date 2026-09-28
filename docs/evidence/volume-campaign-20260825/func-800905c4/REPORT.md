# `func_800905C4` — exact stream-byte flag helper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 313, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x80DC4,0x80DEC)`, VRAM `0x800905C4`, is ten words and ends
in canonical `jr ra` plus a halfword-store delay slot. Exact-start witnesses:

```text
file 0x812CC / VA 0x80090ACC: jal func_800905C4
file 0x8D3AC / VA 0x8009CBAC: .word func_800905C4
```

The preceding real `func_8009059C` ends at VA `0x800905BC/0x800905C0`
(`0x80DBC/0x80DC0` file offsets); the following real `func_800905EC` starts
at file `0x80DEC` with `lw v1,0(a0)`. Ownership is exact, with no padding at
either edge.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL_AND_CALLBACK_TABLE_REFERENCE`.

## Screens

| screen | result |
|---|---|
| callee buckets | no `jal`; stream-byte consumer/state mutator |
| written-global Stage 0 | no globals; argument-relative writes at `+0`, `+0xF4`, `+0x112` |
| coloring pressure | old cursor retained in `$v1`; increment and flag word use `$v0` |
| `$v0` liveness | no return value; cursor increment then flag RMW |
| address retention | old cursor remains in `$v1` through the cursor update and byte load |
| optimization signal | load-delay nop, useful `lbu` in a later load delay, final `sh` in return slot; exact GCC 14 `-O1` family |
| loop/back-edge owner | none |

## C and flags

```c
void func_800905C4(void *arg0) {
    unsigned int v0;
    unsigned char *v1;
    unsigned char byte;

    v1 = *(unsigned char **)arg0;
    *(unsigned char **)arg0 = v1 + 1;
    v0 = *(unsigned int *)((unsigned char *)arg0 + 0xF4);
    byte = *v1;
    v0 |= 0x8000u;
    *(unsigned int *)((unsigned char *)arg0 + 0xF4) = v0;
    *(unsigned short *)((unsigned char *)arg0 + 0x112) = byte;
}
```

GCC 14.2 production `CFLAGS_LEAF`:

```text
-EL -mips1 -mfp32 -mabi=32 -G0 -fno-pic -mno-abicalls
-ffreestanding -fno-builtin -O1
```

The flags and source form come from independently verified sibling
`func_8009059C`/`func_800906B4`. No pins, assembly, maspsx switch, or
relocation normalization is involved.

## Full ten-word comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lw v1,0(a0)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `nop` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,v1,1` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0(a0)` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `lw v0,0xF4(a0)` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `lbu v1,0(v1)` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `ori v0,v0,0x8000` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0xF4(a0)` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sh v1,0x112(a0)` |

Object disassembly:

```text
xxxxxxxx <func_800905C4>:
   0: xxxxxxxx  lw     v1,0(a0)
   4: xxxxxxxx  nop
   8: xxxxxxxx  addiu  v0,v1,1
   c: xxxxxxxx  sw     v0,0(a0)
  10: xxxxxxxx  lw     v0,244(a0)
  14: xxxxxxxx  lbu    v1,0(v1)
  18: xxxxxxxx  ori    v0,v0,0x8000
  1c: xxxxxxxx  sw     v0,244(a0)
  20: xxxxxxxx  jr     ra
  24: xxxxxxxx  sh     v1,274(a0)
```

## Carve and packed-span proof

The prior active asm span was `[0x80DC4,0x80EB4)`, size `0xF0`:

```text
prefix asm: 0x80DC4 - 0x80DC4 = 0x00
C leaf:     0x80DEC - 0x80DC4 = 0x28
resume asm: 0x80EB4 - 0x80DEC = 0xC8
closure:    0x00 + 0x28 + 0xC8 = 0xF0
```

The packed candidate and retail disassemble identically across both real
function boundaries:

```text
800905bc: xxxxxxxx  jr     ra
800905c0: xxxxxxxx  sh     v1,272(a0)
800905c4: xxxxxxxx  lw     v1,0(a0)
800905c8: xxxxxxxx  nop
800905cc: xxxxxxxx  addiu  v0,v1,1
800905d0: xxxxxxxx  sw     v0,0(a0)
800905d4: xxxxxxxx  lw     v0,244(a0)
800905d8: xxxxxxxx  lbu    v1,0(v1)
800905dc: xxxxxxxx  ori    v0,v0,0x8000
800905e0: xxxxxxxx  sw     v0,244(a0)
800905e4: xxxxxxxx  jr     ra
800905e8: xxxxxxxx  sh     v1,274(a0)
800905ec: xxxxxxxx  lw     v1,0(a0)
800905f0: xxxxxxxx  nop
```

```text
retail:    <40 bytes, sha1 79dbd04e97a0>
candidate: <40 bytes, sha1 79dbd04e97a0>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
313
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 313 leaves
```

Result: `MATCHED=10/10`, first phrasing; consecutive-park count remains zero.
