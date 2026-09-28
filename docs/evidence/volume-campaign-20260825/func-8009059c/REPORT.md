# `func_8009059C` — exact stream-byte flag helper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 312, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x80D9C,0x80DC4)`, VRAM `0x8009059C`, is exactly `0x28`
bytes / ten words. It ends in the canonical `jr ra` with its final halfword
store in the return delay slot.

There are two independent exact-start witnesses:

```text
file 0x812C0 / VA 0x80090AC0: jal func_8009059C
file 0x8D3A8 / VA 0x8009CBA8: .word func_8009059C
```

The preceding real function `func_80090574` ends at file `0x80D94/0x80D98`
with `jr ra; sh v1,0x10E(a0)`. The following real function
`func_800905C4` begins at `0x80DC4` with `lw v1,0(a0)`. There is no
padding or ambiguous ownership at either edge.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL_AND_CALLBACK_TABLE_REFERENCE`.

## Retail screens

| screen | result |
|---|---|
| callee buckets | no `jal`; stream-byte consumer/state mutator |
| written-global Stage 0 | no globals; writes only argument-relative state at `+0`, `+0xF4`, and `+0x110` |
| coloring pressure | low but constrained: old cursor remains in `$v1`; cursor increment and flag word use `$v0` |
| `$v0` liveness | no C return; first holds `cursor+1`, then is reused for the `+0xF4` flag RMW |
| address retention | old stream cursor in `$v1` survives the `arg0+0` update and supplies the consumed byte |
| optimization signal | explicit load-delay nop, independent `lbu` in the flag-load delay, and final `sh` in the return slot match the modern GCC 14 `-O1` family |
| loop/back-edge owner | none |

The body consumes one unsigned byte from the old cursor, advances the cursor,
ORs `0x1000` into the state word at `+0xF4`, and stores the zero-extended byte
as a halfword at `+0x110`.

## C and flags

```c
void func_8009059C(void *arg0) {
    unsigned int v0;
    unsigned char *v1;
    unsigned char byte;

    v1 = *(unsigned char **)arg0;
    *(unsigned char **)arg0 = v1 + 1;
    v0 = *(unsigned int *)((unsigned char *)arg0 + 0xF4);
    byte = *v1;
    v0 |= 0x1000u;
    *(unsigned int *)((unsigned char *)arg0 + 0xF4) = v0;
    *(unsigned short *)((unsigned char *)arg0 + 0x110) = byte;
}
```

Compiler: GCC 14.2, production `CFLAGS_LEAF`:

```text
-EL -mips1 -mfp32 -mabi=32 -G0 -fno-pic -mno-abicalls
-ffreestanding -fno-builtin -O1
```

These flags are selected by the exact adjacent family precedent
`func_800906B4`; no pins, inline assembly, file-scope assembly, special
maspsx gate, or relocation normalization is involved.

## Full single-leaf comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lw v1,0(a0)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `nop` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,v1,1` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0(a0)` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `lw v0,0xF4(a0)` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `lbu v1,0(v1)` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `ori v0,v0,0x1000` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0xF4(a0)` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sh v1,0x110(a0)` |

Object disassembly before link:

```text
xxxxxxxx <func_8009059C>:
   0: xxxxxxxx  lw     v1,0(a0)
   4: xxxxxxxx  nop
   8: xxxxxxxx  addiu  v0,v1,1
   c: xxxxxxxx  sw     v0,0(a0)
  10: xxxxxxxx  lw     v0,244(a0)
  14: xxxxxxxx  lbu    v1,0(v1)
  18: xxxxxxxx  ori    v0,v0,0x1000
  1c: xxxxxxxx  sw     v0,244(a0)
  20: xxxxxxxx  jr     ra
  24: xxxxxxxx  sh     v1,272(a0)
```

## Carve and packed-span proof

The prior asm span was `[0x80CC4,0x80EB4)`, size `0x1F0`:

```text
prefix asm: 0x80D9C - 0x80CC4 = 0xD8
C leaf:     0x80DC4 - 0x80D9C = 0x28
resume asm: 0x80EB4 - 0x80DC4 = 0xF0
closure:    0xD8 + 0x28 + 0xF0 = 0x1F0
```

All sizes derive from boundary arithmetic, not aligned object sizes. The
packed disassembly includes the preceding return and first two words of the
following real function; candidate and retail are identical:

```text
80090594: xxxxxxxx  jr     ra
80090598: xxxxxxxx  sh     v1,270(a0)
8009059c: xxxxxxxx  lw     v1,0(a0)
800905a0: xxxxxxxx  nop
800905a4: xxxxxxxx  addiu  v0,v1,1
800905a8: xxxxxxxx  sw     v0,0(a0)
800905ac: xxxxxxxx  lw     v0,244(a0)
800905b0: xxxxxxxx  lbu    v1,0(v1)
800905b4: xxxxxxxx  ori    v0,v0,0x1000
800905b8: xxxxxxxx  sw     v0,244(a0)
800905bc: xxxxxxxx  jr     ra
800905c0: xxxxxxxx  sh     v1,272(a0)
800905c4: xxxxxxxx  lw     v1,0(a0)
800905c8: xxxxxxxx  nop
```

Full packed leaf bytes, file `0x80D9C..0x80DC3`:

```text
retail:    <40 bytes, sha1 43db2143780f>
candidate: <40 bytes, sha1 43db2143780f>
```

## Gates

Fresh production build tail:

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
```

Raw hash and count:

```text
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
312
```

`scripts/verify_us.sh` exits zero and ends with:

```text
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 312 leaves
```

Result: `MATCHED=10/10`, first phrasing. This match resets the consecutive
park count from one to zero.
