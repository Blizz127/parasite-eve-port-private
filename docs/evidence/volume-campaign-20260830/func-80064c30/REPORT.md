# `func_80064C30` — exact GP-state forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G8`; matching-C
leaf 374 and Tier-2 continuation rung 33.

## Function hood and screens

- File `[0x55430,0x55454)`, VA `[0x80064C30,0x80064C54)`: nine words,
  ending in canonical `jr ra; nop`.
- Exact direct caller `0x80050980` targets the start.
- Preceding already-C `func_80064C20` ends immediately at `0x5542C`;
  following real `func_80064C54` starts immediately at `0x55454`.
- One unresolved callee, `func_8005F354`; the wrapper passes its input in
  `$a0` and GP state in `$a1`. Its call result is dead; there is no loop,
  direct write, address retention, or `$v0` pressure.
- Stage-0 maps `_gp + 0x3F4` to `D_8009D164`; the executable writer is at
  `0x80063974`, and readers are this leaf plus the nearby `64C54/64C80`
  routines. `-G8` is therefore retail-proven.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`.

## C and object

```c
extern int D_8009D164;

void func_8005F354(int value, int state);

void func_80064C30(int value) {
    func_8005F354(value, D_8009D164);
}
```

Flags: era `-O2 -G8`; no maspsx gate.

```text
55430: xxxxxxxx  lw    a1,0x3F4(gp)
55434: xxxxxxxx  addiu sp,sp,-24
55438: xxxxxxxx  sw    ra,16(sp)
5543C: xxxxxxxx  jal   func_8005F354
55440: xxxxxxxx  nop
55444: xxxxxxxx  lw    ra,16(sp)
55448: xxxxxxxx  addiu sp,sp,24
5544C: xxxxxxxx  jr    ra
55450: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_GPREL16 D_8009D164 plus R_MIPS_26 func_8005F354
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x55430,0x55BB8)` = `0x0788`:

```text
leaf:   0x55454 - 0x55430 = 0x0024
resume: 0x55BB8 - 0x55454 = 0x0764
close:  0x0024 + 0x0764 = 0x0788
```

Boundary-derived sizes only; the object's zero alignment is guarded-trimmed.

```text
55420: xxxxxxxx = xxxxxxxx  preceding real leaf
55424: xxxxxxxx = xxxxxxxx
55428: xxxxxxxx = xxxxxxxx  preceding return
5542C: xxxxxxxx = xxxxxxxx  preceding live delay store
55430: xxxxxxxx = xxxxxxxx  leaf 1
55434: xxxxxxxx = xxxxxxxx  leaf 2
55438: xxxxxxxx = xxxxxxxx  leaf 3
5543C: xxxxxxxx = xxxxxxxx  leaf 4
55440: xxxxxxxx = xxxxxxxx  leaf 5
55444: xxxxxxxx = xxxxxxxx  leaf 6
55448: xxxxxxxx = xxxxxxxx  leaf 7
5544C: xxxxxxxx = xxxxxxxx  leaf 8
55450: xxxxxxxx = xxxxxxxx  leaf 9
55454: xxxxxxxx = xxxxxxxx  following real prologue
55458: xxxxxxxx = xxxxxxxx  following word 2
5545C: xxxxxxxx = xxxxxxxx  following call
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 374
```
