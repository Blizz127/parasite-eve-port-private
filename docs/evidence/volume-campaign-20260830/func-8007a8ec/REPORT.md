# `func_8007A8EC` — exact slot-3 forwarding twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf 380 and Tier-2 continuation rung 39.

## Function hood and screens

- File `[0x6B0EC,0x6B110)`, VA `[0x8007A8EC,0x8007A910)`: nine words,
  ending in canonical `jr ra; nop`.
- Exact direct caller `0x8007A2E0` targets the start.
- Preceding real `func_8007A8CC` ends at `0x6B0E4/0x6B0E8`; following real
  `func_8007A910` starts immediately at `0x6B110`.
- One unresolved callee, `func_80073CF4`; no globals, loop, address retention,
  or result liveness. Input moves to `$a1`, fixed slot 3 fills the call delay.
- The body is word-identical to independently accepted `func_800824F0`, but
  function hood and compilation were checked independently. Established era
  `-O2 -G0` call scheduling needs no maspsx gate.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALL`.

## C and object

```c
void func_80073CF4(int slot, unsigned int value);

void func_8007A8EC(unsigned int value) {
    func_80073CF4(3, value);
}
```

```text
6B0EC: xxxxxxxx  addiu sp,sp,-24
6B0F0: xxxxxxxx  sw    ra,16(sp)
6B0F4: xxxxxxxx  addu  a1,a0,zero
6B0F8: xxxxxxxx  jal   func_80073CF4
6B0FC: xxxxxxxx  addiu a0,zero,3
6B100: xxxxxxxx  lw    ra,16(sp)
6B104: xxxxxxxx  addiu sp,sp,24
6B108: xxxxxxxx  jr    ra
6B10C: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_26 func_80073CF4
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x6B0AC,0x6C930)` = `0x1884`:

```text
prefix: 0x6B0EC - 0x6B0AC = 0x0040
leaf:   0x6B110 - 0x6B0EC = 0x0024
resume: 0x6C930 - 0x6B110 = 0x1820
close:  0x0040 + 0x0024 + 0x1820 = 0x1884
```

```text
6B0DC: xxxxxxxx = xxxxxxxx  preceding epilogue
6B0E0: xxxxxxxx = xxxxxxxx
6B0E4: xxxxxxxx = xxxxxxxx
6B0E8: xxxxxxxx = xxxxxxxx  live return delay
6B0EC: xxxxxxxx = xxxxxxxx  leaf 1
6B0F0: xxxxxxxx = xxxxxxxx  leaf 2
6B0F4: xxxxxxxx = xxxxxxxx  leaf 3
6B0F8: xxxxxxxx = xxxxxxxx  leaf 4
6B0FC: xxxxxxxx = xxxxxxxx  leaf 5
6B100: xxxxxxxx = xxxxxxxx  leaf 6
6B104: xxxxxxxx = xxxxxxxx  leaf 7
6B108: xxxxxxxx = xxxxxxxx  leaf 8
6B10C: xxxxxxxx = xxxxxxxx  leaf 9
6B110: xxxxxxxx = xxxxxxxx  following real prologue
6B114: xxxxxxxx = xxxxxxxx
6B118: xxxxxxxx = xxxxxxxx
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/build_us.sh: exit 0; Compare: EXACT SHA-1 MATCH
matching-C count: 380
```
