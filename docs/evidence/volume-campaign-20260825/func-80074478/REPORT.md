# `func_80074478` — exact indexed callback-table setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 321, matched on the second bounded phrasing.

## Function hood and boundaries

Retail span `[0x64C78,0x64CA4)`, VRAM `0x80074478`, is eleven words and
ends in canonical `jr ra; nop`. It has no direct `jal`, but its exact start is
constructed as a function pointer:

```text
0x800743F4: lui    v0,%hi(func_80074478)
0x800743F8: addiu  v0,v0,%lo(func_80074478)
```

`func_800743B4` returns that pointer. Its exact caller at `0x80073EBC` stores
the returned value into a callback slot at `0x80073ECC`. This is one semantic
exact-start reference represented by the relocation pair, not two callers.

The preceding real `func_8007440C` ends at `0x80074470/0x80074474` with
`jr ra` and stack restoration. The following already-matched
`func_800744A4` starts at `0x800744A4` with `beqz a1`; both boundary words
are executable instructions and there is no padding in the leaf.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REFERENCE`.

## Screens and state provenance

| screen | result |
|---|---|
| callee buckets | no `jal`; conditional table setter |
| written-state Stage 0 | conditionally writes one of eight words in `D_8009568C`; `func_8007440C` reads the same eight entries and invokes each nonzero word with `jalr`, passing its index in `$a0` |
| coloring pressure | retail converts `$a0` from index to retained element address; `$a1` stays the proposed value; `$v0` changes from table base to old entry |
| `$v0` liveness | table base through address formation, then old value through the equality branch |
| address retention | computed element address remains in `$a0` across load, comparison, and optional store |
| optimization signal | explicit `lui/addiu`, load-delay nop, empty branch delay, and era register allocation select GCC 2.7.2 `-O2 -G0` |
| loop/back-edge owner | none; the eight-entry traversal belongs to reader `func_8007440C` |

The indirect calls prove that the stored words are callback addresses. The C
keeps an integer representation because the exact callback prototype is not
yet proven; pointers and integers are both 32-bit in this target ABI.

## C and bounded phrasings

Final C:

```c
extern int D_8009568C[];

void func_80074478(int index, int value) {
    if (value != D_8009568C[index]) {
        D_8009568C[index] = value;
    }
}
```

Era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G0`; no pins, inline assembly,
or special maspsx switch.

Attempt 1 spelled the condition as `D_8009568C[index] != value`. It produced
the complete retail instruction shape and 10/11 matching words, but encoded
word 6 as `10450002` (`beq v0,a1`) instead of retail `10A20002`
(`beq a1,v0`). Reversing the commutative C operands in attempt 2 selected the
retail branch operand order and changed no other word.

## Full eleven-word comparison

| word | retail | candidate after relocation | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lui v0,%hi(D_8009568C)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,v0,%lo(D_8009568C)` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `sll a0,a0,2` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `addu a0,a0,v0` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `lw v0,0(a0)` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `nop` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `beq a1,v0,+2` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `nop` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `sw a1,0(a0)` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 10 | `xxxxxxxx` | `xxxxxxxx` | `nop` |

Single-leaf object before relocation:

```text
xxxxxxxx <func_80074478>:
   0: xxxxxxxx  lui    v0,0x0       R_MIPS_HI16 D_8009568C
   4: xxxxxxxx  addiu  v0,v0,0      R_MIPS_LO16 D_8009568C
   8: xxxxxxxx  sll    a0,a0,0x2
   c: xxxxxxxx  addu   a0,a0,v0
  10: xxxxxxxx  lw     v0,0(a0)
  14: xxxxxxxx  nop
  18: xxxxxxxx  beq    a1,v0,0x24
  1c: xxxxxxxx  nop
  20: xxxxxxxx  sw     a1,0(a0)
  24: xxxxxxxx  jr     ra
  28: xxxxxxxx  nop
```

The object alignment word at `0x2C` is outside the boundary-derived body;
trim validation removes that zero word.

## Carve and packed-span proof

The prior asm span was `[0x64B54,0x64CA4)`, size `0x150`:

```text
prefix asm: 0x64C78 - 0x64B54 = 0x124
C leaf:     0x64CA4 - 0x64C78 = 0x02C
resume asm: 0x64CA4 - 0x64CA4 = 0x000
closure:    0x124 + 0x02C + 0x000 = 0x150
```

The zero-length resume is intentional: the leaf closes directly against the
existing C boundary for `func_800744A4`. Sizes come from file subtraction,
not aligned object size.

Packed candidate and retail match across both real-function boundaries:

```text
80074468: xxxxxxxx  lw     s1,20(sp)
8007446C: xxxxxxxx  lw     s0,16(sp)
80074470: xxxxxxxx  jr     ra
80074474: xxxxxxxx  addiu  sp,sp,32
80074478: xxxxxxxx  lui    v0,0x8009
8007447C: xxxxxxxx  addiu  v0,v0,0x568C
80074480: xxxxxxxx  sll    a0,a0,2
80074484: xxxxxxxx  addu   a0,a0,v0
80074488: xxxxxxxx  lw     v0,0(a0)
8007448C: xxxxxxxx  nop
80074490: xxxxxxxx  beq    a1,v0,0x8007449C
80074494: xxxxxxxx  nop
80074498: xxxxxxxx  sw     a1,0(a0)
8007449C: xxxxxxxx  jr     ra
800744A0: xxxxxxxx  nop
800744A4: xxxxxxxx  beqz   a1,0x800744C0
800744A8: xxxxxxxx  addiu  v0,a1,-1
800744AC: xxxxxxxx  addiu  v1,zero,-1
800744B0: xxxxxxxx  sw     zero,0(a0)
```

```text
retail:    <44 bytes, sha1 95d6c65e2bde>
candidate: <44 bytes, sha1 95d6c65e2bde>
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
321
Split verification (Phase 4E): OK.
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 321 leaves
```

Result: `MATCHED=11/11`, second phrasing; consecutive-park count resets to
zero.
