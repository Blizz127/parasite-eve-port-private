# `func_8005F594` — exact GP-state forwarding twin

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G8`; matching-C
leaf 375 and Tier-2 continuation rung 34.

## Function hood and screens

- File `[0x4FD94,0x4FDB8)`, VA `[0x8005F594,0x8005F5B8)`: nine words,
  ending in canonical `jr ra; nop`.
- Seven direct callers target the exact start: `0x80044E48`, `0x8004BCD0`,
  `0x8004C53C`, `0x8004C55C`, `0x8004CE0C`, `0x8004CFF4`, and
  `0x8004D008`.
- Preceding real `func_8005F354` ends immediately at `0x4FD8C/0x4FD90`;
  following real `func_8005F5B8` starts immediately at `0x4FDB8`.
- One unresolved callee, `func_8005F354`; input remains in `$a0`, GP state
  is loaded into `$a1`, and the call result is dead. No loop or address
  retention exists.
- Stage-0 maps `_gp + 0x3C8` to `D_8009D138`. The writer is at
  `0x800630F4`; another reader exists at `0x80062B90`. This proves `-G8`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`.

## C and object

```c
extern int D_8009D138;

void func_8005F354(int value, int state);

void func_8005F594(int value) {
    func_8005F354(value, D_8009D138);
}
```

Flags: era `-O2 -G8`; no maspsx gate.

```text
4FD94: xxxxxxxx  lw    a1,0x3C8(gp)
4FD98: xxxxxxxx  addiu sp,sp,-24
4FD9C: xxxxxxxx  sw    ra,16(sp)
4FDA0: xxxxxxxx  jal   func_8005F354
4FDA4: xxxxxxxx  nop
4FDA8: xxxxxxxx  lw    ra,16(sp)
4FDAC: xxxxxxxx  addiu sp,sp,24
4FDB0: xxxxxxxx  jr    ra
4FDB4: xxxxxxxx  nop

object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
ROM/C:  xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_GPREL16 D_8009D138 plus R_MIPS_26 func_8005F354
BYTE_EXACT=9/9
```

## Carve and packed gate

Former asm `[0x4F6D4,0x51CA0)` = `0x25CC`:

```text
prefix: 0x4FD94 - 0x4F6D4 = 0x06C0
leaf:   0x4FDB8 - 0x4FD94 = 0x0024
resume: 0x51CA0 - 0x4FDB8 = 0x1EE8
close:  0x06C0 + 0x0024 + 0x1EE8 = 0x25CC
```

Only zero standalone-object alignment is guarded-trimmed.

```text
4FD84: xxxxxxxx = xxxxxxxx  preceding restore
4FD88: xxxxxxxx = xxxxxxxx  preceding teardown
4FD8C: xxxxxxxx = xxxxxxxx  preceding return
4FD90: xxxxxxxx = xxxxxxxx  preceding delay
4FD94: xxxxxxxx = xxxxxxxx  leaf 1
4FD98: xxxxxxxx = xxxxxxxx  leaf 2
4FD9C: xxxxxxxx = xxxxxxxx  leaf 3
4FDA0: xxxxxxxx = xxxxxxxx  leaf 4
4FDA4: xxxxxxxx = xxxxxxxx  leaf 5
4FDA8: xxxxxxxx = xxxxxxxx  leaf 6
4FDAC: xxxxxxxx = xxxxxxxx  leaf 7
4FDB0: xxxxxxxx = xxxxxxxx  leaf 8
4FDB4: xxxxxxxx = xxxxxxxx  leaf 9
4FDB8: xxxxxxxx = xxxxxxxx  following real prologue
4FDBC: xxxxxxxx = xxxxxxxx  following word 2
4FDC0: xxxxxxxx = xxxxxxxx  following word 3
PACKED_SPAN=EXACT
```

```text
retail/candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
matching-C count: 375
```
