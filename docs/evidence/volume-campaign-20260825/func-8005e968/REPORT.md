# `func_8005E968` — exact matching-C packed-value setter

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 302, matched on the first bounded phrasing.

## Function hood and boundaries

Retail span `[0x4F168,0x4F188)`, VRAM `0x8005E968`, eight words. It ends in
`jr ra` plus `nop`. The preceding word at `0x4F164` is the real return delay
slot of `func_8005E914`; the following word at `0x4F188` is `slt v0,a0,a1`,
the real first instruction of `func_8005E988`.

The retail executable contains 16 exact direct `jal 0x8005E968` words
(`0C017A5A`). `FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLERS`.

## Retail screens

| screen | result |
|---|---|
| callee buckets | no `jal`; leaf setter |
| written globals | gp-relative `D_8009D110` and `D_8009D114` at offsets `0x3A0` and `0x3A4` |
| Stage-0 readers | nine functions read one or both values: `func_8005EB64`, `func_8005ED18`, `func_8005EED4`, `func_8005F874`, `func_8006062C`, `func_80061044`, `func_8006153C`, `func_80061C34`, `func_80062090` |
| coloring pressure | low: `$a0` is deliberately mutated after its original value is stored; `$v0` carries the mask |
| `$v0` liveness | mask only; no return value |
| address retention | both stores are gp-relative; no address register is retained |
| optimization signal | `lui`/`ori` mask synthesis and gp-relative stores select era `-O2 -G8` |
| loop/back-edge owner | none |

The hardware/rendering role of the packed fields is not named here; callers
and the `0x7F7F7F` lane mask prove the transformation, not an SDK symbol.

## C and flags

```c
extern int D_8009D110;
extern int D_8009D114;

void func_8005E968(int a0) {
    D_8009D110 = a0;
    D_8009D114 = (a0 >> 1) & 0x7F7F7F;
}
```

Compiled with `era_compile ... -O2 -G8`. `_gp = 0x8009CD70`; the two
`R_MIPS_GPREL16` relocations normalize to `0x3A0` and `0x3A4`. No pin,
inline assembly, or special maspsx switch is used.

## Full single-leaf comparison

| word | retail | candidate after relocation | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lui v0,0x7F` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `ori v0,v0,0x7F7F` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `sw a0,0x3A0(gp)` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `sra a0,a0,1` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `and a0,a0,v0` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `sw a0,0x3A4(gp)` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `nop` |

## Carve and packed-span proof

The prior resume-asm span is `[0x4F0C4,0x4F358)`, size `0x294`:

```text
prefix asm: 0x4F168 - 0x4F0C4 = 0x0A4
C leaf:     0x4F188 - 0x4F168 = 0x020
resume asm: 0x4F358 - 0x4F188 = 0x1D0
closure:    0x0A4 + 0x020 + 0x1D0 = 0x294
```

Full packed span, file `0x4F168..0x4F187`:

```text
retail:    <32 bytes, sha1 34c1db48d99c>
candidate: <32 bytes, sha1 34c1db48d99c>
```

The packed executable SHA-1 is
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. `scripts/verify_us.sh`
passed and reports 302 matching-C leaves.
