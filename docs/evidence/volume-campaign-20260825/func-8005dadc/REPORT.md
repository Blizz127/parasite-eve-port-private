# `func_8005DADC` — exact matching-C retained-address index helper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 304, matched on the second bounded phrasing.

## Function hood and boundaries

Retail span `[0x4E2DC,0x4E2FC)`, VRAM `0x8005DADC`, eight words. It ends in
`jr ra` with the final indexed add in the delay slot. The preceding word at
`0x4E2D8` is the real return delay slot of `func_8005DAB4`; the following
word at `0x4E2FC` is the real `lui v0,%hi(D_800A802C)` start of
`func_8005DAFC`.

Five exact direct callers occur at `0x80030910`, `0x80031394`, `0x8005EB78`,
`0x8005ED30`, and `0x80060648`.
`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLERS`.

## Retail screens

| screen | result |
|---|---|
| callee buckets | no `jal`; leaf address helper |
| global access | reads the word at `D_800A8030`; writes no global |
| coloring pressure | `$v1` must retain the symbol address while `$v0` carries the loaded offset/result |
| `$v0` liveness | loaded offset, then accumulated return address |
| address retention | central lever: one `D_800A8030` address is used for both the load and the adjusted base |
| optimization signal | absolute HI/LO address and delay-slot final add select era `-O2 -G0` |
| loop/back-edge owner | none |

## Bounded phrasings

Attempt 1 expressed the load and base as independent symbol expressions:

```c
unsigned char *base = (unsigned char *)&D_800A8030 - 8;
return base + D_800A8030 + (a0 * 8);
```

cc1 materialized the symbol twice (`lui/lw` through `$v0`, then a second
`lui/addiu` through `$v1`). Retail materializes it once in `$v1`, loads
through that pointer, and then adjusts the retained pointer.

Attempt 2 made the shared address explicit and matched:

```c
extern unsigned int D_800A8030;

unsigned char *func_8005DADC(int a0) {
    unsigned int *address = &D_800A8030;
    unsigned int offset = *address;
    unsigned char *base = (unsigned char *)address - 8;

    return base + offset + (a0 * 8);
}
```

Compiled with `era_compile ... -O2 -G0`. No pin, inline assembly, or special
maspsx switch is used.

## Full single-leaf comparison

The object's HI16/LO16 relocation pair for `D_800A8030` normalizes to the
retail address.

| word | retail | candidate after relocation | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lui v1,%hi(D_800A8030)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `addiu v1,v1,%lo(D_800A8030)` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `lw v0,0(v1)` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `addiu v1,v1,-8` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `sll a0,a0,3` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `addu v0,v0,v1` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `addu v0,v0,a0` |

## Carve and packed-span proof

The prior asm span is `[0x4CC98,0x4E914)`, size `0x1C7C`:

```text
prefix asm: 0x4E2DC - 0x4CC98 = 0x1644
C leaf:     0x4E2FC - 0x4E2DC = 0x0020
resume asm: 0x4E914 - 0x4E2FC = 0x0618
closure:    0x1644 + 0x0020 + 0x0618 = 0x1C7C
```

Full packed span, file `0x4E2DC..0x4E2FB`:

```text
retail:    <32 bytes, sha1 51d13836c9bd>
candidate: <32 bytes, sha1 51d13836c9bd>
```

The packed executable SHA-1 is
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. `scripts/verify_us.sh`
passed and reports 304 matching-C leaves.
