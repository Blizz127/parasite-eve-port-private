# func_800762BC — bounded texture-window layout residual

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Status: `PARKED-TEXTURE-WINDOW-CONTROL-FLOW-LOAD-SCHEDULE-AND-COLORING`.

Disposition: `ACCEPTED-RESIDUAL`. This is evidence disposition only, not a
byte-identity claim. The leaf remains assembly and does not increment the
matching-C count.

This was the third consecutive bounded park after `func_800339A0` and
`func_80080C48`; campaign hard stop H5 therefore fired. No eligible Tier-1
candidate was skipped to avoid the stop.

## Function hood and boundaries

The exact span is file `[0x66ABC,0x66B3C)`, VRAM
`[0x800762BC,0x8007633C)`: `0x80` bytes / 32 words. It ends with `jr ra` at
`0x80076334` and the live `addiu sp,sp,0x10` frame teardown in the delay slot
at `0x80076338`.

Four distinct raw direct calls target the exact start:

```text
caller PC   file off  target
80075B64    066364    800762BC
80075CC8    0664C8    800762BC
80075D78    066578    800762BC
80075F70    066770    800762BC
```

Each call is the direct `jal` word `0C01D8AF` (little-endian file bytes
`AF D8 01 0C`). The preceding proven `func_800762A0` ends immediately at file
`0x66AB4/0x66AB8` with `jr ra` and a live packed-command OR in its delay slot.
The following matched pointer getter `func_8007633C` starts immediately at
file `0x66B3C` with a real `lui`. Both boundary sides are executable.

```text
FUNCTION_HOOD=PROVEN_BY_4_DIRECT_CALLERS_AND_CANONICAL_RETURN
```

## Semantics and screens

The input is a four-signed-halfword GPU rectangle (`x,y,w,h`). A null input
returns zero. Otherwise the function builds a GP0 E2 texture-window command:

```text
x = ((rect->x & 0xff) >> 3)
y = ((rect->y & 0xff) >> 3)
w = ((-rect->w & 0xff) >> 3)
h = ((-rect->h & 0xff) >> 3)
return 0xe2000000 | (y << 15) | (x << 10) | (h << 5) | w
```

The identity is independently supported by the local native implementation
in `pc_port/platform/pe_libgpu.c`, the four surrounding libGPU callers, and a
PsyQ-compatible `_get_tw` header expression available in the local Xenogears
research tree. That header uses the equivalent `~(w - 1)` / `~(h - 1)` forms.
No external SDK source was imported and no pc-port file was changed.

| screen | retail result | consequence |
|---|---|---|
| callee buckets | zero `jal` in body | no callee result/clobber bucket |
| frame | unconditional `0x10`, no saved registers | four word-sized component homes are structural evidence |
| Stage-0 globals | no symbolic read or write | no global census required |
| coloring pressure | pointer plus X/Y/W/H components and E2 accumulator | retail assigns X/W to `a1/a2`, Y/result to `v0`, H to `v1` |
| `$v0` liveness | Y component becomes the packed result and survives to return | do not introduce a separate return accumulator |
| address retention | input remains in `$a0` through the final H load, then `$a0` becomes E2/H-shift scratch | load order determines when the argument register can be reused |
| `-O` signal | folded `~(field-1)` to `negu`, masks/shifts, packed OR schedule, and live return-slot frame teardown | era GCC 2.7.2 `-O2 -G0` is the justified probe |
| back-edge owner | none | one null arm and a straight-line body |

The four `sw` instructions at `sp+0/+4/+8/+C` are never reloaded. They are
nevertheless retail instructions and cannot be discarded as accidental
padding or forged back with inline assembly.

## Retail body — all 32 words

```text
off  word      instruction
00   xxxxxxxx  bnez  a0,0x10
04   xxxxxxxx  addiu sp,sp,-0x10
08   xxxxxxxx  j     0x80076334
0C   xxxxxxxx  addu  v0,zero,zero
10   xxxxxxxx  lbu   a1,0(a0)
14   xxxxxxxx  nop
18   xxxxxxxx  srl   a1,a1,3
1C   xxxxxxxx  sw    a1,0(sp)
20   xxxxxxxx  lh    a2,4(a0)
24   xxxxxxxx  nop
28   xxxxxxxx  negu  a2,a2
2C   xxxxxxxx  andi  a2,a2,0xff
30   xxxxxxxx  sra   a2,a2,3
34   xxxxxxxx  sw    a2,8(sp)
38   xxxxxxxx  lbu   v0,2(a0)
3C   xxxxxxxx  sll   a1,a1,10
40   xxxxxxxx  srl   v0,v0,3
44   xxxxxxxx  sw    v0,4(sp)
48   xxxxxxxx  sll   v0,v0,15
4C   xxxxxxxx  lh    v1,6(a0)
50   xxxxxxxx  lui   a0,0xe200
54   xxxxxxxx  or    a1,a1,a0
58   xxxxxxxx  or    v0,v0,a1
5C   xxxxxxxx  negu  v1,v1
60   xxxxxxxx  andi  v1,v1,0xff
64   xxxxxxxx  sra   v1,v1,3
68   xxxxxxxx  sll   a0,v1,5
6C   xxxxxxxx  or    v0,v0,a0
70   xxxxxxxx  or    v0,v0,a2
74   xxxxxxxx  sw    v1,12(sp)
78   xxxxxxxx  jr    ra
7C   xxxxxxxx  addiu sp,sp,0x10
```

## Two bounded C phrasings

Both attempts use era GCC 2.7.2 `-O2 -G0`, maspsx ASPSX 2.21 with
`--dont-expand-li`, and GNU MIPS-I `as`. No pin, inline asm, volatile ordering
fiction, mismatch-hiding macro, or forged nop was used.

Phrasing 1 reproduces the locally attested `_get_tw` ternary expression
directly:

```c
typedef struct Rect {
    short x;
    short y;
    short w;
    short h;
} Rect;

unsigned int func_800762BC(Rect *rect)
{
    return rect
        ? 0xE2000000u
            | (((rect->y & 0xFF) >> 3) << 15)
            | (((rect->x & 0xFF) >> 3) << 10)
            | (((~(rect->h - 1) & 0xFF) >> 3) << 5)
            | ((~(rect->w - 1) & 0xFF) >> 3)
        : 0;
}
```

cc1 reassociates that expression, needs no stack component homes, and emits
25 content words plus three alignment zeros (`0x70` bytes total). Its complete
object text is:

```text
00802821 10A00014 3C03E200 90A40002 90A20000 000420C2 000423C0 000210C2
00021280 00431025 00822025 84A30006 84A20004 00031823 00031880 306303E0
00832025 00021023 304200FF 000210C3 08000017 00821025 00001021 03E00008
00000000 00000000 00000000 00000000
```

The object-local jump at offset `0x50` has `R_MIPS_26 .text`; normalizing it
does not address the absent frame, different null layout, or shorter body.

Phrasing 2 models the exact 16-byte frame geometry as a natural four-element
component array:

```c
unsigned int func_800762BC(Rect *rect)
{
    unsigned int component[4];

    if (rect == 0) {
        return 0;
    }

    component[0] = (rect->x & 0xFF) >> 3;
    component[1] = (rect->y & 0xFF) >> 3;
    component[2] = (~(rect->w - 1) & 0xFF) >> 3;
    component[3] = (~(rect->h - 1) & 0xFF) >> 3;

    return 0xE2000000u
        | (component[1] << 15)
        | (component[0] << 10)
        | (component[3] << 5)
        | component[2];
}
```

It emits exactly 32 words and all four word stores at the correct stack
offsets:

```text
off  candidate  retail     result
00   xxxxxxxx   xxxxxxxx   beqz null layout != bnez body layout
04   xxxxxxxx   xxxxxxxx   = frame allocation
08   xxxxxxxx   xxxxxxxx   body fall-through != null-arm jump
0C   xxxxxxxx   xxxxxxxx   load nop != zero result
10   xxxxxxxx   xxxxxxxx   shifted body
14   xxxxxxxx   xxxxxxxx   shifted body / X colored a2 != a1
18   xxxxxxxx   xxxxxxxx   Y load occurs before W
1C   xxxxxxxx   xxxxxxxx   different schedule
20   xxxxxxxx   xxxxxxxx   different schedule
24   xxxxxxxx   xxxxxxxx   different schedule
28   xxxxxxxx   xxxxxxxx   W colored a1 != a2
2C   xxxxxxxx   xxxxxxxx   different schedule
30   xxxxxxxx   xxxxxxxx   different schedule
34   xxxxxxxx   xxxxxxxx   different schedule
38   xxxxxxxx   xxxxxxxx   different schedule
3C   xxxxxxxx   xxxxxxxx   different schedule
40   xxxxxxxx   xxxxxxxx   H load shifted
44   xxxxxxxx   xxxxxxxx   E2 materialization shifted
48   xxxxxxxx   xxxxxxxx   different schedule
4C   xxxxxxxx   xxxxxxxx   different schedule
50   xxxxxxxx   xxxxxxxx   different schedule
54   xxxxxxxx   xxxxxxxx   different schedule
58   xxxxxxxx   xxxxxxxx   different schedule
5C   xxxxxxxx   xxxxxxxx   different schedule
60   xxxxxxxx   xxxxxxxx   different schedule
64   xxxxxxxx   xxxxxxxx   different schedule
68   xxxxxxxx   xxxxxxxx   linked body-exit jump vs fall-through
6C   xxxxxxxx   xxxxxxxx   H home occurs in jump slot too early
70   xxxxxxxx   xxxxxxxx   null result vs final W OR
74   xxxxxxxx   xxxxxxxx   early teardown vs final H home
78   xxxxxxxx   xxxxxxxx   = return
7C   xxxxxxxx   xxxxxxxx   nop vs live teardown slot
```

The candidate's raw object jump is `0800001D` with `R_MIPS_26 .text`; linked
at this function it becomes `0801D8CC`, targeting `0x80076330`. Retail's
`0801D8CD` is the distinct null-arm jump to the common epilogue at
`0x80076334`. Thus the jump difference is control-flow layout, not relocation
noise. Positional equality after normalization is 2/32 (`0x04`, `0x78`).

The residual has three coupled compiler decisions: null arm placement,
independent X/Y/W/H load scheduling, and the resulting component register
colors. The different CFG also prevents retail's frame teardown from filling
the return delay slot. A third source perturbation would violate the bounded
attempt rule.

```text
RESIDUAL_MECHANISM=TEXTURE_WINDOW_CONTROL_FLOW_LOAD_SCHEDULE_AND_COLORING
FUTURE_UNBLOCKER=a compiler block-layout/allocation/scheduler lever that emits
  the bnez-into-body null shape, orders X/W before Y/H with a1/a2 homes, and
  leaves the common epilogue as jr ra with frame teardown in its delay slot
```

The final candidate is preserved in labeled stash
`park func_800762BC texture-window layout residual`; recover the untracked
source from that stash's third parent. Both attempt objects remain under
`build/attempts/func_800762BC/` locally. There is no YAML, build, or verifier
integration. The exact matching-C count remains 340.

```text
HARD_STOP=H5_THREE_CONSECUTIVE_PARKS
PARK_SEQUENCE=func_800339A0,func_80080C48,func_800762BC
TIER1_SKIPPED_TO_AVOID_STOP=NO
```
