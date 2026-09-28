# `func_80017410` — field-VM handler: setter call with a stack result word

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on era `-O2 -G8` (no knob needed); `LINK_EXACT`, 0 word
mismatches.

## Function hood and span

- File span `[0x7C10,0x7C44)` = 13 words. VRAM `[0x80017410,0x80017444)`.
- Field-VM dispatch target `D_800910A0[0x410]`.
- Carved from the former `0x7C10` asm span in `asm/disc1/7C10.s`.

## Semantics (retail bytes)

```text
80017410  xxxxxxxx  addiu sp,sp,-0x28
80017414  xxxxxxxx  li    v0,-1
80017418  xxxxxxxx  sw    ra,0x20(sp)
8001741c  xxxxxxxx  sh    v0,0x10(sp)       ; buf[0] = -1
80017420  xxxxxxxx  lw    v0,0(a0)
80017424  xxxxxxxx  addu  a1,zero,zero
80017428  xxxxxxxx  lh    a0,0(v0)          ; (short)*a0[0]
8001742c  xxxxxxxx  jal   func_800375E0
80017430  xxxxxxxx  addiu a2,sp,0x10       ; (delay) buf
80017434  xxxxxxxx  lw    ra,0x20(sp)
80017438  xxxxxxxx  li    v0,1
8001743c  xxxxxxxx  jr    ra
80017440  xxxxxxxx  addiu sp,sp,0x28       ; (delay)
```

C (`src/func_80017410.c`):

```c
int func_80017410(unsigned short **a0) {
    short buf[8];
    buf[0] = -1;
    func_800375E0((short)*a0[0], 0, buf);
    return 1;
}
```

## Lever: `buf` must be an array

A scalar `short buf` only reserves one stack slot (frame `0x20`, `sh` at
`0x10` but no room for a `sp+0x10` argument window), so cc1 emits
`addiu sp,sp,-0x20` and mismatches retail's `0x28` frame. Declaring
`short buf[8]` produces the retail `0x28` frame with the store at `0x10`
(`era_leaf_match.sh` reports `MISMATCHES=1`; the one diff is the `sp`-adjust
relocation).

## Link-level proof

```text
python3 tools/analysis/era_link_check.py src/func_80017410.c 0x80017410 0x34 -O2 -G8
LINK_EXACT
```

## Registration

- Source `src/func_80017410.c`; YAML `- [0x7C10, c, func_80017410]`.
- Profile `era_o2_g8`.
