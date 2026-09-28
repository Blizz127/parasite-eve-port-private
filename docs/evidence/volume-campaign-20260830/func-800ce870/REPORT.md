# `func_800CE870` — matched Tier-1 position selector

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: MATCHED, leaf 340. Exact on the second and final allowed natural-C
phrasing. No pins, inline assembly, file-scope assembly, forged padding, or
toolchain patch was used.

## Function hood and boundaries

The exact span is file `[0xBF070,0xBF0F0)`, VRAM
`[0x800CE870,0x800CE8F0)`: `0x80` bytes / 32 words. It ends with canonical
`jr ra; nop` at file `0xBF0E8/0xBF0EC`.

Twenty-seven distinct instructions call the exact start. Every call encodes
the same direct target (`0C033A1C`, file bytes `1C3A030C`):

```text
800D4CF4  800D4F14  800D76D8  800D77E4  800D7BDC  800D84B0
800D8600  800D8794  800D88E8  800D8C3C  800D8C7C  800D94C8
800D9780  800DA02C  800DAD5C  800DB134  800DBB14  800DC0D4
800DC7A4  800DCAD8  800DCF04  800DD208  800DD7C4  800DDDCC
800DE108  800DF8E4  800DFB00
```

Both boundary sides are real instructions. Preceding `func_800CE78C` ends
immediately with `jr ra; nop` at `0xBF068/0xBF06C`. Following handwritten
`func_800CE8F0` starts immediately at `0xBF0F0` with
`addiu sp,sp,-0x30`.

```text
FUNCTION_HOOD=PROVEN_BY_27_DIRECT_CALLERS_AND_CANONICAL_RETURN
```

This is a genuine function, not padding, data, or a tail fragment.

## Retail semantics and screens

The second argument selects one of two three-component position sources. For
mode 0, the function follows the pointer at object offset `0x238` and copies
words `+0x14/+0x18/+0x1C`, truncating each into the caller's three-halfword
output. For mode 1, it converts three signed 16.16 object fields at
`+0x28/+0x2C/+0x30` to halfwords by taking their signed high halves. Other
mode values leave the output untouched.

The mode-1 instruction offsets are structural evidence: signed `lh` at
`+0x2A/+0x2E/+0x32` reads the high halfword of three aligned 32-bit fields.
They are not naturally modeled as three isolated halfword fields.

| screen | retail result | implication |
|---|---|---|
| callee buckets | no `jal`; no callees | leaf; no call-result or clobber bucket |
| frame / save pressure | no frame or saved registers | caller-saved allocation only |
| Stage-0 writers | no symbolic global; writes only three halfwords through `a2` | no global reader census; caller-owned output only |
| coloring pressure | object/mode/output stay in `a0/a1/a2`; `v0` is the sole value scratch | natural switch and field expressions are sufficient |
| `$v0` liveness | each branch leaves its final loaded component in `v0`, but all callers use the output buffer rather than a return value | void return; do not add an artificial result |
| address retention | mode 0 reloads `obj->source` before every component because intervening output stores may alias the object | preserve repeated source expressions rather than hoisting a pointer |
| optimization signal | two-case switch cascade, scheduled load-delay nops, and `signed int >> 16` folded to `lh` | era GCC 2.7.2 `-O2`; `-G0` is neutral here and matches this code region |
| loop/back-edge owner | none | acyclic selector |

## Two bounded phrasings

Phrasing 1 modeled object offsets `0x2A/0x2E/0x32` as standalone signed-short
fields. It recovered the complete control-flow and memory schedule, but cc1
correctly canonicalized three load/store truncations to `lhu`; retail uses
`lh`:

```text
off   phrasing 1  retail    residual
54    9482002A    8482002A  lhu -> lh
60    9482002E    8482002E  lhu -> lh
6C    94820032    84820032  lhu -> lh
```

The two local `j` words also differ before relocation, as expected; both have
`R_MIPS_26 .text` relocations and resolve exactly in the packed executable.

Phrasing 2 uses the evidence-supported signed 16.16 source fields. GCC folds
each arithmetic shift and truncating store into the retail signed high-half
load. This is the final C:

```c
typedef struct PositionSource {
    unsigned char pad_00[0x14];
    int field_14;
    int field_18;
    int field_1C;
} PositionSource;

typedef struct PositionObject {
    unsigned char pad_00[0x28];
    int field_28;
    int field_2C;
    int field_30;
    unsigned char pad_34[0x204];
    PositionSource *source;
} PositionObject;

void func_800CE870(PositionObject *obj, int mode, short *out)
{
    switch (mode) {
    case 0:
        out[0] = obj->source->field_14;
        out[1] = obj->source->field_18;
        out[2] = obj->source->field_1C;
        break;
    case 1:
        out[0] = obj->field_28 >> 16;
        out[1] = obj->field_2C >> 16;
        out[2] = obj->field_30 >> 16;
        break;
    }
}
```

Reusable lever:

```text
SIGNED-16.16-HIGH-HALF: when retail reads signed halfwords at +2 inside a
sequence of aligned four-byte fields, model the source as signed 32-bit fixed
point and spell the conversion as `field >> 16`. Era cc1 can fold the
arithmetic shift plus halfword destination into a single signed `lh` from the
high-half address. Modeling isolated short fields can instead canonicalize to
`lhu` because only the low 16 stored bits remain observable.
```

The exact invocation is era GCC 2.7.2 `-O2 -G0`, followed by ordinary
maspsx/ASPSX 2.21 and little-endian MIPS-I assembly. No opt-in maspsx gate is
enabled:

```sh
era_compile src/func_800CE870.c build/src/func_800CE870.c.o -O2 -G0
```

## Full 32-word object comparison

The standalone object has exactly two `.rel.text` entries:

```text
Offset     Type       symbol
00000010   R_MIPS_26  .text
0000004c   R_MIPS_26  .text
```

Normalizing those two local jumps from object-local `0800001E` to their
linked word `08033A3A` gives 32/32 exact words:

```text
off  object/raw  normalized  retail    instruction
00   xxxxxxxx    xxxxxxxx    xxxxxxxx  beqz a1,+5
04   xxxxxxxx    xxxxxxxx    xxxxxxxx  addiu v0,zero,1
08   xxxxxxxx    xxxxxxxx    xxxxxxxx  beq a1,v0,+0x12
0C   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
10   xxxxxxxx    xxxxxxxx    xxxxxxxx  j 800CE8E8
14   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
18   xxxxxxxx    xxxxxxxx    xxxxxxxx  lw v0,0x238(a0)
1C   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
20   xxxxxxxx    xxxxxxxx    xxxxxxxx  lw v0,0x14(v0)
24   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
28   xxxxxxxx    xxxxxxxx    xxxxxxxx  sh v0,0(a2)
2C   xxxxxxxx    xxxxxxxx    xxxxxxxx  lw v0,0x238(a0)
30   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
34   xxxxxxxx    xxxxxxxx    xxxxxxxx  lw v0,0x18(v0)
38   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
3C   xxxxxxxx    xxxxxxxx    xxxxxxxx  sh v0,2(a2)
40   xxxxxxxx    xxxxxxxx    xxxxxxxx  lw v0,0x238(a0)
44   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
48   xxxxxxxx    xxxxxxxx    xxxxxxxx  lw v0,0x1C(v0)
4C   xxxxxxxx    xxxxxxxx    xxxxxxxx  j 800CE8E8
50   xxxxxxxx    xxxxxxxx    xxxxxxxx  sh v0,4(a2)
54   xxxxxxxx    xxxxxxxx    xxxxxxxx  lh v0,0x2A(a0)
58   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
5C   xxxxxxxx    xxxxxxxx    xxxxxxxx  sh v0,0(a2)
60   xxxxxxxx    xxxxxxxx    xxxxxxxx  lh v0,0x2E(a0)
64   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
68   xxxxxxxx    xxxxxxxx    xxxxxxxx  sh v0,2(a2)
6C   xxxxxxxx    xxxxxxxx    xxxxxxxx  lh v0,0x32(a0)
70   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
74   xxxxxxxx    xxxxxxxx    xxxxxxxx  sh v0,4(a2)
78   xxxxxxxx    xxxxxxxx    xxxxxxxx  jr ra
7C   xxxxxxxx    xxxxxxxx    xxxxxxxx  nop
```

## Carve geometry

The former active assembly subsegment was `[0xBEC9C,0xC5050)`, size
`0x63B4`. Sizes come only from file-boundary arithmetic:

```text
prefix asm:  0xBF070 - 0xBEC9C = 0x03D4
C leaf:      0xBF0F0 - 0xBF070 = 0x0080
resume asm:  0xC5050 - 0xBF0F0 = 0x5F60
closure:     0x03D4 + 0x0080 + 0x5F60 = 0x63B4
```

The leaf is itself 16-byte aligned and its object `.text` is exactly `0x80`;
no aligned object size was used to derive any boundary.

## Full packed-span comparison

The final rebuilt and retail executables were independently disassembled as
raw little-endian MIPS over all 32 words at `[0xBF070,0xBF0F0)`. Both produce
this complete sequence:

```text
800CE870 xxxxxxxx  beqz a1,800CE888
800CE874 xxxxxxxx  li v0,1
800CE878 xxxxxxxx  beq a1,v0,800CE8C4
800CE87C xxxxxxxx  nop
800CE880 xxxxxxxx  j 800CE8E8
800CE884 xxxxxxxx  nop
800CE888 xxxxxxxx  lw v0,0x238(a0)
800CE88C xxxxxxxx  nop
800CE890 xxxxxxxx  lw v0,0x14(v0)
800CE894 xxxxxxxx  nop
800CE898 xxxxxxxx  sh v0,0(a2)
800CE89C xxxxxxxx  lw v0,0x238(a0)
800CE8A0 xxxxxxxx  nop
800CE8A4 xxxxxxxx  lw v0,0x18(v0)
800CE8A8 xxxxxxxx  nop
800CE8AC xxxxxxxx  sh v0,2(a2)
800CE8B0 xxxxxxxx  lw v0,0x238(a0)
800CE8B4 xxxxxxxx  nop
800CE8B8 xxxxxxxx  lw v0,0x1C(v0)
800CE8BC xxxxxxxx  j 800CE8E8
800CE8C0 xxxxxxxx  sh v0,4(a2)
800CE8C4 xxxxxxxx  lh v0,0x2A(a0)
800CE8C8 xxxxxxxx  nop
800CE8CC xxxxxxxx  sh v0,0(a2)
800CE8D0 xxxxxxxx  lh v0,0x2E(a0)
800CE8D4 xxxxxxxx  nop
800CE8D8 xxxxxxxx  sh v0,2(a2)
800CE8DC xxxxxxxx  lh v0,0x32(a0)
800CE8E0 xxxxxxxx  nop
800CE8E4 xxxxxxxx  sh v0,4(a2)
800CE8E8 xxxxxxxx  jr ra
800CE8EC xxxxxxxx  nop
```

Result: packed candidate and ROM are 32/32 exact, including both linked local
jump words.

## Full-build and verifier gates

Raw full-build tail:

```text
volume file 0xBF070 (CE870): cand=<128 bytes, sha1 44a08ef8e1c8> orig=<128 bytes, sha1 44a08ef8e1c8>
RESULT: EXACT MATCH

=== Summary ===
Assemble: OK
Compile:  OK
Link:     OK
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
```

Raw SHA gate:

```text
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/extracted/disc1/SLUS_006.62
```

Raw verifier tail and authoritative YAML count:

```text
=== Summary ===
Split verification (Phase 4E): OK.
Rebuild status (scripts/build_us.sh):
  scripts/build_us.sh: present
  candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
  compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 340 leaves (+ script VM step; era -O2 -G8 + EXPAND_DIV + THREE_WORD + DISPATCH_FOLD)
  source: src/func_80012850.c

$ grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml
340
```

```text
MATCHING_C_COUNT=340
ACCEPTED_RESIDUAL_COUNT=20
```
