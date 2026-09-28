# func_8003DF50 — matched Tier-1 leaf

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: MATCHED, leaf 339. Exact on the first natural-C phrasing. No pins,
inline assembly, file-scope assembly, volatile scheduling tricks, or forged
padding.

## Function hood and boundaries

The exact span is file `[0x2E750,0x2E7C8)`, VRAM
`[0x8003DF50,0x8003DFC8)`: `0x78` bytes / 30 words. It ends with canonical
`jr ra` at file `0x2E7C0`, with a live halfword store in its delay slot at
`0x2E7C4`.

The instruction at `0x800151C8` (file `0x59C8`, encoded `0C00F7D4`) calls the
exact start. Its setup loads the third argument with signed `lh` and supplies
the first argument in the call delay slot.

Both boundaries are executable instructions. The preceding real
`func_8003DD08` ends immediately with `jr ra` and a live word store at file
`0x2E748/0x2E74C`. The following already-matched `func_8003DFC8` begins
immediately at file `0x2E7C8` with its own `jr ra` (`03E00008`). Therefore:

```text
FUNCTION_HOOD=PROVEN_BY_1_DIRECT_CALLER_AND_CANONICAL_RETURN
```

This is a genuine callable function, not padding, data, or a tail fragment.

## Retail semantics and screens

The routine stores its signed 16-bit index and source pointer into the
destination, clears destination word zero, and treats `source+0x18` as a
pointer to 16-byte records. It copies unsigned halfwords at record offsets
`+6`, `+0`, and `+2` into destination offsets `+0x70`, `+0x2C`, and `+0x2E`.
It writes the sum of record `+4` and destination `+0x70` to destination
`+0x30`.

The sole caller proves the signed index: it uses `lh` from its input before
the call. Retail stores the original low halfword, then transforms the live
register with `sll 16; sra 12`, which is signed-16 promotion combined with a
16-byte record stride.

| screen | retail result | implication |
|---|---|---|
| callee buckets | no `jal`; no callees | leaf; no call-result or clobber bucket |
| frame / save pressure | no stack frame or saved registers | caller-saved allocation only |
| Stage-0 writers | no symbolic globals; writes only through `a0` at `+0/+0x24/+0x2C/+0x2E/+0x30/+0x32/+0x70` | no global reader census; caller-owned destination only |
| coloring pressure | destination remains `a0`, source remains `a1`, and signed scaled offset remains `a2`; loads use `v0`, with final saved halfword in `v1` | preserve the short formal and do not introduce an explicit retained record pointer |
| `$v0` liveness | each table base/value cycles through `v0`; final sum returns no value | void return; final `v0` is consumed by the return-slot store |
| address retention | source table pointer is reloaded four times because intervening destination stores may alias source state | repeat the natural field expressions; a hoisted pointer could erase retail reloads |
| optimization signal | signed scale fold, scheduled load delays, no frame, no small-data reference | era GCC 2.7.2 `-O2 -G0` |
| loop/back-edge owner | none | straight-line record initialization |

## First and final phrasing

The first natural spelling uses a signed-short formal, a typed 16-byte record
array, and repeated source field expressions:

```c
typedef struct Record16 {
    unsigned short field_00;
    unsigned short field_02;
    unsigned short field_04;
    unsigned short field_06;
    unsigned char pad_08[8];
} Record16;

typedef struct Source {
    unsigned char pad_00[0x18];
    Record16 *records;
} Source;

typedef struct Destination {
    unsigned int field_00;
    unsigned char pad_04[0x20];
    Source *source;
    unsigned char pad_28[4];
    unsigned short field_2C;
    unsigned short field_2E;
    unsigned short field_30;
    unsigned short field_32;
    unsigned char pad_34[0x3C];
    unsigned short field_70;
} Destination;

void func_8003DF50(Destination *dst, Source *source, short index)
{
    dst->field_32 = index;
    dst->field_00 = 0;
    dst->source = source;
    dst->field_70 = source->records[index].field_06;
    dst->field_2C = source->records[index].field_00;
    dst->field_2E = source->records[index].field_02;
    dst->field_30 = source->records[index].field_04 + dst->field_70;
}
```

This matches all 30 content words. GNU assembler rounds the standalone
object's `.text` from `0x78` to `0x80`; both extra words are zero alignment
padding and the build trim guard removes them only after proving that fact.

Reusable lever:

```text
SIGNED-SHORT-16-BYTE-RECORD: a signed-short formal indexed into a 16-byte
typed record naturally emits `sll reg,16; sra reg,12`, preserving an earlier
store of the unscaled halfword. When retail reloads a source-owned table base
between destination stores, retain repeated field expressions instead of
hoisting an explicit record pointer; the possible alias keeps those reloads.
```

The exact invocation is era GCC 2.7.2 `-O2 -G0`, followed by ordinary
maspsx/ASPSX 2.21 and little-endian MIPS-I assembly. No opt-in maspsx gate is
enabled:

```sh
era_compile src/func_8003DF50.c build/src/func_8003DF50.c.o -O2 -G0
```

## Full 30-word object comparison

There are no `.text` relocations; the sole relocation is ordinary `.pdr`
metadata. Every content word is literal:

```text
off   candidate  retail    instruction
00    xxxxxxxx   xxxxxxxx  sh    a2,0x32(a0)
04    xxxxxxxx   xxxxxxxx  sll   a2,a2,16
08    xxxxxxxx   xxxxxxxx  sw    zero,0(a0)
0C    xxxxxxxx   xxxxxxxx  sw    a1,0x24(a0)
10    xxxxxxxx   xxxxxxxx  lw    v0,0x18(a1)
14    xxxxxxxx   xxxxxxxx  sra   a2,a2,12
18    xxxxxxxx   xxxxxxxx  addu  v0,a2,v0
1C    xxxxxxxx   xxxxxxxx  lhu   v0,6(v0)
20    xxxxxxxx   xxxxxxxx  nop
24    xxxxxxxx   xxxxxxxx  sh    v0,0x70(a0)
28    xxxxxxxx   xxxxxxxx  lw    v0,0x18(a1)
2C    xxxxxxxx   xxxxxxxx  nop
30    xxxxxxxx   xxxxxxxx  addu  v0,a2,v0
34    xxxxxxxx   xxxxxxxx  lhu   v0,0(v0)
38    xxxxxxxx   xxxxxxxx  nop
3C    xxxxxxxx   xxxxxxxx  sh    v0,0x2C(a0)
40    xxxxxxxx   xxxxxxxx  lw    v0,0x18(a1)
44    xxxxxxxx   xxxxxxxx  nop
48    xxxxxxxx   xxxxxxxx  addu  v0,a2,v0
4C    xxxxxxxx   xxxxxxxx  lhu   v0,2(v0)
50    xxxxxxxx   xxxxxxxx  nop
54    xxxxxxxx   xxxxxxxx  sh    v0,0x2E(a0)
58    xxxxxxxx   xxxxxxxx  lw    v0,0x18(a1)
5C    xxxxxxxx   xxxxxxxx  lhu   v1,0x70(a0)
60    xxxxxxxx   xxxxxxxx  addu  a2,a2,v0
64    xxxxxxxx   xxxxxxxx  lhu   v0,4(a2)
68    xxxxxxxx   xxxxxxxx  nop
6C    xxxxxxxx   xxxxxxxx  addu  v0,v0,v1
70    xxxxxxxx   xxxxxxxx  jr    ra
74    xxxxxxxx   xxxxxxxx  sh    v0,0x30(a0)
```

Result: `30/30`, zero mismatches.

## Carve geometry

The former active assembly subsegment was `[0x2E034,0x2E7C8)`, size
`0x794`. The target consumes its tail. Sizes come only from file-boundary
arithmetic:

```text
prefix asm:  0x2E750 - 0x2E034 = 0x071C
C leaf:      0x2E7C8 - 0x2E750 = 0x0078
resume asm:  0x2E7C8 - 0x2E7C8 = 0x0000
closure:     0x071C + 0x0078 + 0x0000 = 0x0794
```

The object-alignment size `0x80` is not used in this arithmetic. The trim
guard proves and removes its eight zero tail bytes.

## Full packed-span comparison

The rebuilt executable is compared from the preceding function's return
through the first word of the following function:

```text
file    VRAM       candidate  retail    status
2E748   8003DF48   xxxxxxxx   xxxxxxxx  MATCH
2E74C   8003DF4C   xxxxxxxx   xxxxxxxx  MATCH
2E750   8003DF50   xxxxxxxx   xxxxxxxx  MATCH
2E754   8003DF54   xxxxxxxx   xxxxxxxx  MATCH
2E758   8003DF58   xxxxxxxx   xxxxxxxx  MATCH
2E75C   8003DF5C   xxxxxxxx   xxxxxxxx  MATCH
2E760   8003DF60   xxxxxxxx   xxxxxxxx  MATCH
2E764   8003DF64   xxxxxxxx   xxxxxxxx  MATCH
2E768   8003DF68   xxxxxxxx   xxxxxxxx  MATCH
2E76C   8003DF6C   xxxxxxxx   xxxxxxxx  MATCH
2E770   8003DF70   xxxxxxxx   xxxxxxxx  MATCH
2E774   8003DF74   xxxxxxxx   xxxxxxxx  MATCH
2E778   8003DF78   xxxxxxxx   xxxxxxxx  MATCH
2E77C   8003DF7C   xxxxxxxx   xxxxxxxx  MATCH
2E780   8003DF80   xxxxxxxx   xxxxxxxx  MATCH
2E784   8003DF84   xxxxxxxx   xxxxxxxx  MATCH
2E788   8003DF88   xxxxxxxx   xxxxxxxx  MATCH
2E78C   8003DF8C   xxxxxxxx   xxxxxxxx  MATCH
2E790   8003DF90   xxxxxxxx   xxxxxxxx  MATCH
2E794   8003DF94   xxxxxxxx   xxxxxxxx  MATCH
2E798   8003DF98   xxxxxxxx   xxxxxxxx  MATCH
2E79C   8003DF9C   xxxxxxxx   xxxxxxxx  MATCH
2E7A0   8003DFA0   xxxxxxxx   xxxxxxxx  MATCH
2E7A4   8003DFA4   xxxxxxxx   xxxxxxxx  MATCH
2E7A8   8003DFA8   xxxxxxxx   xxxxxxxx  MATCH
2E7AC   8003DFAC   xxxxxxxx   xxxxxxxx  MATCH
2E7B0   8003DFB0   xxxxxxxx   xxxxxxxx  MATCH
2E7B4   8003DFB4   xxxxxxxx   xxxxxxxx  MATCH
2E7B8   8003DFB8   xxxxxxxx   xxxxxxxx  MATCH
2E7BC   8003DFBC   xxxxxxxx   xxxxxxxx  MATCH
2E7C0   8003DFC0   xxxxxxxx   xxxxxxxx  MATCH
2E7C4   8003DFC4   xxxxxxxx   xxxxxxxx  MATCH
2E7C8   8003DFC8   xxxxxxxx   xxxxxxxx  MATCH
```

`cmp` returns zero for all `0x84` displayed bytes. Candidate and retail slice
SHA-256 are both
`6a29d187eb135e2ebc266e5fa6814090bd6a8f0c6350faaa0f94891241b63e67`.

## Full gates

```text
$ sha1sum build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

$ scripts/build_us.sh  # summary tail
RESULT: EXACT MATCH
Assemble: OK
Compile:  OK
Pad trim: OK
Link:     OK
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH

$ scripts/verify_us.sh  # summary
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 339 leaves

$ grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
339
```

After integration, the active pool is 1,090 spans: Tier 1 8, Tier 2 214,
Tier 3 99, and SKIP/suppressed 769. This first-attempt match leaves
consecutive bounded parks at zero; four leaves have matched in this refreshed
campaign.
