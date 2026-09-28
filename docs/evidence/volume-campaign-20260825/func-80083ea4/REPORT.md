# `func_80083EA4` — exact matching-C initializer twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 308, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x746A4,0x746C4)`, VRAM `0x80083EA4`, eight words. It ends in
`jr ra` with a real byte store in the delay slot. The preceding word at
`0x746A0` is the real return delay slot of existing C function
`func_80083E84`; the following word at `0x746C4` is the real first instruction
of existing C function `func_80083EC4`.

An exact direct caller occurs at `0x800838F8`.
`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLER`.

## Retail screens

| screen | result |
|---|---|
| callee buckets | no `jal`; leaf initializer |
| global access / Stage 0 | none |
| coloring pressure | low: `$v0` is reused for tag, interior pointer, and state value |
| `$v0` liveness | `0x46`, then `a0+0x24`, then `1`; no return value |
| address retention | only the argument-relative interior pointer stored at `a0+0x2C` |
| optimization signal | exact structural twin of `func_80083E50`, `func_80083E84`, and `func_80083EC4`; era `-O2 -G0` |
| loop/back-edge owner | none |

## C and flags

```c
void func_80083EA4(unsigned char *a0, unsigned char a1) {
    a0[0x36] = 0x46;
    *(unsigned char **)(a0 + 0x2C) = a0 + 0x24;
    a0[0x24] = a1;
    a0[0x35] = 1;
}
```

Compiled with `era_compile ... -O2 -G0`. There are no relocations, pins,
inline assembly, or special maspsx switches.

## Full single-leaf comparison

| word | retail | candidate | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,zero,0x46` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `sb v0,0x36(a0)` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,a0,0x24` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sw v0,0x2C(a0)` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `addiu v0,zero,1` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `sb a1,0x24(a0)` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `sb v0,0x35(a0)` |

## Carve and packed-span proof

The prior asm span is exactly `[0x746A4,0x746C4)`, size `0x20`:

```text
C leaf:  0x746C4 - 0x746A4 = 0x20
closure: 0x20 = 0x20
```

Both neighbors were already C, so this removes the asm unit with no prefix or
resume span. Full packed span, file `0x746A4..0x746C3`:

```text
retail:    <32 bytes, sha1 ef6de6cd26ac>
candidate: <32 bytes, sha1 ef6de6cd26ac>
```

The packed executable SHA-1 is
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. `scripts/verify_us.sh` exits
zero and reports 308 matching-C leaves.
