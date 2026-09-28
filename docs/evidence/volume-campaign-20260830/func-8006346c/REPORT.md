# func_8006346C — matched Tier-1 leaf

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: MATCHED, leaf 337. Two bounded phrasings. No pins, inline assembly,
file-scope assembly, volatile scheduling tricks, or padding instructions.

## Function hood and boundaries

The exact span is file `[0x53C6C,0x53CD4)`, VRAM
`[0x8006346C,0x800634D4)`: `0x68` bytes / 26 words. It ends with the
canonical return `jr ra` at file `0x53CCC`, with `move v0,a0` in its delay
slot at `0x53CD0`.

Four distinct instructions call the exact start:

```text
caller PC   file off
80046B94    037394
800477B8    037FB8
8004D2F8    03DAF8
8004D740    03DF40
```

Both boundaries are executable instructions. The preceding real function
ends with `jr ra; nop` at file `0x53C64/0x53C68`. The following function
begins immediately at file `0x53CD4` with `addiu sp,sp,-0x38`
(`0x27BDFFC8`). Therefore:

```text
FUNCTION_HOOD=PROVEN_BY_4_DIRECT_CALLERS_AND_CANONICAL_RETURN
```

This is a genuine callable function, not padding, data, or a mislabeled tail.

## Retail semantics and screens

The argument points to the same record shape used by adjacent matched
`func_80063428`. The routine starts with index `-1`. When the record is
non-null and its word at `+0x44` is nonnegative, it reads the word at `+0x48`;
if that value is nonnegative, the selected index is
`record->multiplier * record->factor + record->base`. It then tests that bit
in the word at `+0x74`, returning the selected index when set and `-1`
otherwise. Retail deliberately reaches the `+0x74` load after the null arm,
so the final C preserves that observable address behavior rather than adding
an unproved early return.

| screen | retail result | implication |
|---|---|---|
| callees / buckets | no `jal`; no callees | leaf; no call-result or clobber bucket |
| frame / save pressure | no stack frame or saved registers | straight-line leaf allocation |
| Stage-0 writers | no symbolic global access or write; reads only `a0+0x34/44/48/74` | no global reader census; argument-owned state only |
| coloring pressure | index in `v1`; base in `a1`; product in `a2`; late result in `a0` | result initialization placement controls the multiplier/result homes |
| `$v0` liveness | multiplier and bit-test temporary use `v0`; final value is copied from `a0` in the return slot | do not force a conventional long-lived `v0` result accumulator |
| address retention | all loads are argument-relative | no symbolic-address or assembler-temp lever |
| optimization signal | scheduled guard slots, load-delay nops, `mult/mflo`, no frame | era GCC 2.7.2 `-O2 -G0` |
| loop/back-edge owner | none | acyclic guarded calculation |

There are no relocatable globals, callees, loops, or callback effects in the
body. The signed branches and arithmetic `srav` require signed `int` fields.

## Bounded phrasing attempts

Attempt 1 initialized both locals at declaration:

```c
int index = -1;
int result = -1;
```

That makes `result` live from entry. cc1 emits 27 content words, begins with
an extra `li v1,-1`, keeps the result in `a2`, and consequently allocates the
multiply result to `a3`:

```text
00000000 2403FFFF  li    v1,-1
00000004 10800010  beqz  a0,0x48
00000008 2406FFFF  li    a2,-1
...
00000034 00430018  mult  v0,v1
00000038 00003812  mflo  a3
...
00000058 10400002  beqz  v0,0x64
0000005C 00000000  nop
00000060 00603021  move  a2,v1
00000064 03E00008  jr    ra
00000068 00C01021  move  v0,a2
```

Attempt 2 moves only `result = -1` to immediately before the final bit test.
That shortens its live range: `a2` becomes available for `mflo`, while `a0`
becomes the late result home. The default assignment then fills the final
guard branch's delay slot exactly. This recovers all 26 words.

Reusable lever:

```text
DELAYED-DEFAULT-RESULT-INIT: when retail uses an argument register as a late
result home and needs another argument register for mflo, delay the default
result assignment until the final selection. This can both shorten the result
live range and supply the late guard's delay-slot constant.
```

## Final C

```c
typedef struct {
    unsigned char pad_00[0x34];
    int multiplier;
    unsigned char pad_38[0x0C];
    int base;
    int factor;
    unsigned char pad_4C[0x28];
    int flags;
} ValueRecord;

int func_8006346C(ValueRecord *record)
{
    int index = -1;
    int result;

    if (record != 0) {
        int base = record->base;

        if (base >= 0) {
            index = record->factor;
            if (index >= 0) {
                index = record->multiplier * index + base;
            } else {
                index = -1;
            }
        }
    }

    result = -1;
    if (((record->flags >> index) & 1) != 0) {
        result = index;
    }

    return result;
}
```

The exact invocation is era GCC 2.7.2 `-O2 -G0`, followed by the ordinary
maspsx/ASPSX 2.21 and little-endian MIPS-I assembler path. No opt-in maspsx
gate is enabled:

```sh
era_compile src/func_8006346C.c build/src/func_8006346C.c.o -O2 -G0
```

## Full 26-word object comparison

The object has one local `R_MIPS_26 .text` relocation. At object offset
`0x38`, raw `08000011` targets local offset `0x44`; linking the object at
`0x8006346C` normalizes it to `08018D2C`, the retail jump to `0x800634B0`.
Every other word is literal.

```text
off   object normalized  retail    instruction
00    xxxxxxxx           xxxxxxxx  beqz  a0,0x44
04    xxxxxxxx           xxxxxxxx  li    v1,-1
08    xxxxxxxx           xxxxxxxx  lw    a1,0x44(a0)
0C    xxxxxxxx           xxxxxxxx  nop
10    xxxxxxxx           xxxxxxxx  bltz  a1,0x44
14    xxxxxxxx           xxxxxxxx  nop
18    xxxxxxxx           xxxxxxxx  lw    v1,0x48(a0)
1C    xxxxxxxx           xxxxxxxx  nop
20    xxxxxxxx           xxxxxxxx  bltz  v1,0x40
24    xxxxxxxx           xxxxxxxx  nop
28    xxxxxxxx           xxxxxxxx  lw    v0,0x34(a0)
2C    xxxxxxxx           xxxxxxxx  nop
30    xxxxxxxx           xxxxxxxx  mult  v0,v1
34    xxxxxxxx           xxxxxxxx  mflo  a2
38    xxxxxxxx           xxxxxxxx  j     0x800634B0 (raw xxxxxxxx + R_MIPS_26)
3C    xxxxxxxx           xxxxxxxx  addu  v1,a2,a1
40    xxxxxxxx           xxxxxxxx  li    v1,-1
44    xxxxxxxx           xxxxxxxx  lw    v0,0x74(a0)
48    xxxxxxxx           xxxxxxxx  nop
4C    xxxxxxxx           xxxxxxxx  srav  v0,v0,v1
50    xxxxxxxx           xxxxxxxx  andi  v0,v0,1
54    xxxxxxxx           xxxxxxxx  beqz  v0,0x60
58    xxxxxxxx           xxxxxxxx  li    a0,-1
5C    xxxxxxxx           xxxxxxxx  move  a0,v1
60    xxxxxxxx           xxxxxxxx  jr    ra
64    xxxxxxxx           xxxxxxxx  move  v0,a0
```

Result: `26/26` after ordinary local-relocation normalization, zero
mismatches.

## Carve geometry

The former active assembly subsegment was `[0x53C6C,0x55248)`, size
`0x15DC`. Sizes come only from file-boundary arithmetic:

```text
prefix asm:  0x53C6C - 0x53C6C = 0x0000
C leaf:      0x53CD4 - 0x53C6C = 0x0068
resume asm:  0x55248 - 0x53CD4 = 0x1574
closure:     0x0000 + 0x0068 + 0x1574 = 0x15DC
```

The trim guard reports `.text 0x70 -> 0x68 (-8)` and verifies that all eight
removed bytes are alignment zeros. No body byte is truncated.

## Full packed-span comparison

This independently compares the rebuilt executable against retail from the
preceding function's return through the first word of the following function:

```text
file    VRAM       candidate  retail    status
53C64   80063464   xxxxxxxx   xxxxxxxx  MATCH
53C68   80063468   xxxxxxxx   xxxxxxxx  MATCH
53C6C   8006346C   xxxxxxxx   xxxxxxxx  MATCH
53C70   80063470   xxxxxxxx   xxxxxxxx  MATCH
53C74   80063474   xxxxxxxx   xxxxxxxx  MATCH
53C78   80063478   xxxxxxxx   xxxxxxxx  MATCH
53C7C   8006347C   xxxxxxxx   xxxxxxxx  MATCH
53C80   80063480   xxxxxxxx   xxxxxxxx  MATCH
53C84   80063484   xxxxxxxx   xxxxxxxx  MATCH
53C88   80063488   xxxxxxxx   xxxxxxxx  MATCH
53C8C   8006348C   xxxxxxxx   xxxxxxxx  MATCH
53C90   80063490   xxxxxxxx   xxxxxxxx  MATCH
53C94   80063494   xxxxxxxx   xxxxxxxx  MATCH
53C98   80063498   xxxxxxxx   xxxxxxxx  MATCH
53C9C   8006349C   xxxxxxxx   xxxxxxxx  MATCH
53CA0   800634A0   xxxxxxxx   xxxxxxxx  MATCH
53CA4   800634A4   xxxxxxxx   xxxxxxxx  MATCH
53CA8   800634A8   xxxxxxxx   xxxxxxxx  MATCH
53CAC   800634AC   xxxxxxxx   xxxxxxxx  MATCH
53CB0   800634B0   xxxxxxxx   xxxxxxxx  MATCH
53CB4   800634B4   xxxxxxxx   xxxxxxxx  MATCH
53CB8   800634B8   xxxxxxxx   xxxxxxxx  MATCH
53CBC   800634BC   xxxxxxxx   xxxxxxxx  MATCH
53CC0   800634C0   xxxxxxxx   xxxxxxxx  MATCH
53CC4   800634C4   xxxxxxxx   xxxxxxxx  MATCH
53CC8   800634C8   xxxxxxxx   xxxxxxxx  MATCH
53CCC   800634CC   xxxxxxxx   xxxxxxxx  MATCH
53CD0   800634D0   xxxxxxxx   xxxxxxxx  MATCH
53CD4   800634D4   xxxxxxxx   xxxxxxxx  MATCH
```

`cmp` returns zero for all `0x74` displayed bytes. Candidate and retail slice
SHA-256 are both
`888e17eabe5cceebb550938a307ec63b91d8d7024b08135eaa556a7f8b0441ec`.
The packed leaf therefore agrees with both retail and the normalized isolated
object.

## Full gates

```text
$ sha1sum build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

$ scripts/verify_us.sh
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 337 leaves

$ grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
337
```

The refreshed active pool is now 1,092 spans: Tier 1 12, Tier 2 214, Tier 3
99, and SKIP/suppressed 767. This match resets consecutive bounded parks from
one to zero.
