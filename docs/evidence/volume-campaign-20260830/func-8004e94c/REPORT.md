# `func_8004E94C` — exact GP countdown forwarding wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G8`; matching-C
leaf 376 and Tier-2 continuation rung 35.

## Function hood and screens

- File `[0x3F14C,0x3F170)`, VA `[0x8004E94C,0x8004E970)`: nine words,
  ending in canonical `jr ra; nop`.
- Exact direct caller `0x8004976C` targets the start.
- Preceding real `func_8004E704` ends immediately at `0x3F144/0x3F148`;
  following already-C `func_8004E970` starts immediately at `0x3F170`.
- One unresolved callee, `func_8004E704`; the wrapper has no written global,
  loop, address retention, or coloring pressure. Its call result is dead, so
  there is no `$v0` liveness constraint.
- Stage-0 maps `_gp + 0x19C` to `D_8009CF0C`. Retail writers occur at
  `0x8004E994` and in accepted initializer `func_8004F808` at `0x8004F81C`;
  accepted getter `func_8004E970` is an additional reader. The GP-relative
  load is direct evidence for `-G8`.
- The only arithmetic is the source-level countdown `- 1`; cc1 places its
  `addiu a0,a0,-1` in the call delay slot. There is no optimization-level
  ambiguity beyond the established era `-O2` call-wrapper fingerprint.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`.

## C and object

```c
extern int D_8009CF0C;

void func_8004E704(int value);

void func_8004E94C(void) {
    func_8004E704(D_8009CF0C - 1);
}
```

Flags: era `-O2 -G8`; no maspsx gate.

```text
3F14C: xxxxxxxx  lw    a0,0x19C(gp)
3F150: xxxxxxxx  addiu sp,sp,-24
3F154: xxxxxxxx  sw    ra,16(sp)
3F158: xxxxxxxx  jal   func_8004E704
3F15C: xxxxxxxx  addiu a0,a0,-1
3F160: xxxxxxxx  lw    ra,16(sp)
3F164: xxxxxxxx  addiu sp,sp,24
3F168: xxxxxxxx  jr    ra
3F16C: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_GPREL16 D_8009CF0C plus R_MIPS_26 func_8004E704
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x3E4A4,0x3F170)` = `0x0CCC`:

```text
prefix: 0x3F14C - 0x3E4A4 = 0x0CA8
leaf:   0x3F170 - 0x3F14C = 0x0024
close:  0x0CA8 + 0x0024 = 0x0CCC
```

Boundary-derived sizes only; the leaf closes directly against existing C
`func_8004E970`.

```text
3F13C: xxxxxxxx = xxxxxxxx  preceding epilogue
3F140: xxxxxxxx = xxxxxxxx
3F144: xxxxxxxx = xxxxxxxx  preceding return
3F148: xxxxxxxx = xxxxxxxx  preceding delay
3F14C: xxxxxxxx = xxxxxxxx  leaf 1
3F150: xxxxxxxx = xxxxxxxx  leaf 2
3F154: xxxxxxxx = xxxxxxxx  leaf 3
3F158: xxxxxxxx = xxxxxxxx  leaf 4
3F15C: xxxxxxxx = xxxxxxxx  leaf 5
3F160: xxxxxxxx = xxxxxxxx  leaf 6
3F164: xxxxxxxx = xxxxxxxx  leaf 7
3F168: xxxxxxxx = xxxxxxxx  leaf 8
3F16C: xxxxxxxx = xxxxxxxx  leaf 9
3F170: xxxxxxxx = xxxxxxxx  following getter
3F174: xxxxxxxx = xxxxxxxx
3F178: xxxxxxxx = xxxxxxxx
3F17C: xxxxxxxx = xxxxxxxx  next real prologue
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/build_us.sh: exit 0; Compare: EXACT SHA-1 MATCH
matching-C count: 376
```
