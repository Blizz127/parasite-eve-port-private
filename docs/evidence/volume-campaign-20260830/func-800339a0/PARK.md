# func_800339A0 — bounded GP load/store scheduling residual

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: `PARKED-GP-LOAD-STORE-SCHEDULING-AND-REGISTER-HOME`.

Disposition: `ACCEPTED-RESIDUAL`. This is evidence disposition only, not a
byte-identity claim. The leaf remains assembly and does not increment the
matching-C count.

## Function hood and boundaries

The exact span is file `[0x241A0,0x24220)`, VRAM
`[0x800339A0,0x80033A20)`: `0x80` bytes / 32 words. It ends with `jr ra` at
`0x80033A18` and `nop` in the delay slot at `0x80033A1C`.

Six distinct raw direct calls target the exact start. Generated duplicate
assembly views were deduplicated by caller PC:

```text
caller PC   file off  target
80016CF0    0074F0    800339A0
80029998    01A198    800339A0
8005C15C    04C95C    800339A0
8005C190    04C990    800339A0
8005C55C    04CD5C    800339A0
8005C654    04CE54    800339A0
```

Each call is the direct `jal` encoding `0C00CE68` (little-endian file bytes
`68 CE 00 0C`). The preceding real `func_800334AC` ends at file
`0x24198/0x2419C` with `jr ra; nop`. The following matched
`func_80033A20` begins immediately at file `0x24220` with a real gp-relative
`lbu`. Both boundary sides are executable.

```text
FUNCTION_HOOD=PROVEN_BY_6_DIRECT_CALLERS_AND_CANONICAL_RETURN
```

## Screens and state provenance

The body is a no-call, no-loop leaf with a 16-byte local aggregate. It copies
the 16-byte constant table at `D_80010E38` to the stack, indexes one four-byte
pair by the low byte of the argument, and publishes the selected pair plus the
original selector to gp state.

| screen | retail result | consequence |
|---|---|---|
| callee buckets | zero `jal` in the body | no callee return/clobber bucket |
| frame | `0x10`, no saved registers | exactly the local table copy; no prologue-save lever |
| coloring pressure | stack base, indexed pair address, two halfword values, and three independent stores | selected-address/result homes are the central pressure |
| `$v0` liveness | `$v0` first holds copied words, then the scaled index, then each selected halfword | retail deliberately reuses `$v0`; candidate gives the pair address to `$v0` instead |
| address retention | retail keeps `sp + ((index & 0xff) << 2)` in `$v1` across both loads and the first store | candidate keeps that address in `$v0` and cannot reproduce the store interleave |
| `-O` signal | full aggregate copy, scheduled load-delay nop, and independent GP stores | era `-O2 -G8` is the justified probe |
| back-edge owner | none | straight-line leaf |

With `_gp = 0x8009CD70`, the written state is:

```text
gp+0x110  D_8009CE80  unsigned byte selector
gp+0x114  D_8009CE84  first selected halfword
gp+0x116  D_8009CE86  second selected halfword
```

A full executable exact-access census finds seven accesses to `gp+0x110`,
nineteen to `gp+0x114`, and twenty-two to `gp+0x116`. This function is the
sole writer in each bucket; the remaining 6/18/21 sites are readers. The
consumer cluster spans `func_800334BC`, `func_80033A20`, and the routines
through `0x80034760`; additional selector readers occur at `0x80034A30`,
`0x80034D90`, `0x80034E28`, and `0x80034EC8`.

The source table bytes are:

```text
dd 00 b1 00  0f 00 b1 00  0f 00 0f 00  dd 00 0f 00
```

or four pairs `{0x00DD,0x00B1}`, `{0x000F,0x00B1}`,
`{0x000F,0x000F}`, and `{0x00DD,0x000F}`. The index mask is preserved
exactly; retail supplies no range guard for values above three. Existing
read-only oracle `pc_port/tools/pe_btl6_339a0_oracle.py` independently
confirms the 32-word body, table contents, and three state writes. No pc-port
file was changed for this matching attempt.

## Retail body — all 32 words

```text
off  word      instruction
00   xxxxxxxx  addiu sp,sp,-0x10
04   xxxxxxxx  lui   a2,0x8001
08   xxxxxxxx  addiu a2,a2,0x0E38
0C   xxxxxxxx  lwl   v0,3(a2)
10   xxxxxxxx  lwr   v0,0(a2)
14   xxxxxxxx  lwl   v1,7(a2)
18   xxxxxxxx  lwr   v1,4(a2)
1C   xxxxxxxx  lwl   a1,11(a2)
20   xxxxxxxx  lwr   a1,8(a2)
24   xxxxxxxx  swl   v0,3(sp)
28   xxxxxxxx  swr   v0,0(sp)
2C   xxxxxxxx  swl   v1,7(sp)
30   xxxxxxxx  swr   v1,4(sp)
34   xxxxxxxx  swl   a1,11(sp)
38   xxxxxxxx  swr   a1,8(sp)
3C   xxxxxxxx  lwl   v0,15(a2)
40   xxxxxxxx  lwr   v0,12(a2)
44   xxxxxxxx  nop
48   xxxxxxxx  swl   v0,15(sp)
4C   xxxxxxxx  swr   v0,12(sp)
50   xxxxxxxx  andi  v0,a0,0xff
54   xxxxxxxx  sll   v0,v0,2
58   xxxxxxxx  addu  v1,sp,v0
5C   xxxxxxxx  lhu   v0,0(v1)
60   xxxxxxxx  nop
64   xxxxxxxx  sh    v0,0x114(gp)
68   xxxxxxxx  lhu   v0,2(v1)
6C   xxxxxxxx  sb    a0,0x110(gp)
70   xxxxxxxx  sh    v0,0x116(gp)
74   xxxxxxxx  addiu sp,sp,0x10
78   xxxxxxxx  jr    ra
7C   xxxxxxxx  nop
```

## Two bounded C phrasings

Both attempts used the era compiler with `-O2 -G8`, maspsx ASPSX 2.21 with
the repository's standard flags, and GNU MIPS-I `as`. The aggregate and field
types follow the observed copy and halfword accesses:

```c
typedef struct Pair {
    unsigned short first;
    unsigned short second;
} Pair;

typedef struct PairTable {
    Pair pairs[4];
} PairTable;

extern const PairTable D_80010E38;
extern unsigned char D_8009CE80;
extern unsigned short D_8009CE84;
extern unsigned short D_8009CE86;

void func_800339A0(unsigned int index)
{
    PairTable table = D_80010E38;
    unsigned int selected = index & 0xFF;

    D_8009CE84 = table.pairs[selected].first;
    D_8009CE80 = index;
    D_8009CE86 = table.pairs[selected].second;
}
```

Attempt 1 put the selector store first (`CE80`, `CE84`, `CE86`). Attempt 2,
shown above, follows retail source order for the first two stores
(`CE84`, `CE80`, `CE86`). cc1 emitted byte-identical output for both, proving
that the residual is not controlled by natural assignment order. No pin,
inline asm, volatile ordering fiction, mismatch-hiding macro, or forged nop
was used.

## Complete candidate object and normalized comparison

Both attempts emit 31 content words (`0x7C`) followed by one zero alignment
word. Relocations are shown normalized to linked values below:

```text
off  candidate  retail     result
00   xxxxxxxx   xxxxxxxx   =
04   xxxxxxxx   xxxxxxxx   =  HI16 D_80010E38
08   xxxxxxxx   xxxxxxxx   =  LO16 D_80010E38
0C   xxxxxxxx   xxxxxxxx   =
10   xxxxxxxx   xxxxxxxx   =
14   xxxxxxxx   xxxxxxxx   =
18   xxxxxxxx   xxxxxxxx   =
1C   xxxxxxxx   xxxxxxxx   =
20   xxxxxxxx   xxxxxxxx   =
24   xxxxxxxx   xxxxxxxx   =
28   xxxxxxxx   xxxxxxxx   =
2C   xxxxxxxx   xxxxxxxx   =
30   xxxxxxxx   xxxxxxxx   =
34   xxxxxxxx   xxxxxxxx   =
38   xxxxxxxx   xxxxxxxx   =
3C   xxxxxxxx   xxxxxxxx   =
40   xxxxxxxx   xxxxxxxx   =
44   xxxxxxxx   xxxxxxxx   =
48   xxxxxxxx   xxxxxxxx   =
4C   xxxxxxxx   xxxxxxxx   =
50   xxxxxxxx   xxxxxxxx   =
54   xxxxxxxx   xxxxxxxx   =
58   xxxxxxxx   xxxxxxxx   address home v0 != v1
5C   xxxxxxxx   xxxxxxxx   first result v1 != v0
60   xxxxxxxx   xxxxxxxx   second load hoisted; retail load-delay nop
64   xxxxxxxx   xxxxxxxx   selector store != first-half store
68   xxxxxxxx   xxxxxxxx   first-half store != deferred second load
6C   xxxxxxxx   xxxxxxxx   second-half store != selector store
70   xxxxxxxx   xxxxxxxx   epilogue starts one word early
74   xxxxxxxx   xxxxxxxx   shifted
78   xxxxxxxx   xxxxxxxx   shifted
7C   xxxxxxxx   xxxxxxxx   alignment zero vs retail return-slot nop
```

In the relocatable object, the two table words are `3C060000/24C60000` with
`HI16/LO16 D_80010E38`; the three GP stores carry `GPREL16` relocations and
link to offsets `0x110/0x114/0x116`. Those ordinary relocations normalize to
the retail words above and are not residuals.

The first mismatch is therefore offset `0x58`. Retail retains the selected
pair address in `$v1`, loads the first halfword into `$v0`, leaves the required
load-delay nop, stores that result, then performs the second load before the
selector and second-half stores. cc1 instead gives the address to `$v0`, the
first result to `$v1`, hoists the independent second load, and emits all three
GP stores consecutively. Source-order reversal did not alter any decision.

```text
RESIDUAL_MECHANISM=GP_LOAD_STORE_SCHEDULING_AND_REGISTER_HOME
FUTURE_UNBLOCKER=a compiler allocation/scheduler lever that retains the
  selected-pair address in v1, keeps the first result in v0, and declines to
  hoist the second lhu across the first gp-relative halfword store
```

The final candidate is preserved in labeled stash
`park func_800339A0 gp-load-store scheduling residual`; recover the untracked
source from that stash's third parent. There is no YAML, build, or verifier
integration. The exact matching-C count remains 340.
