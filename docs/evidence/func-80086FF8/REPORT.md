# `func_80086FF8` / `func_80087024` — D_800CCD80 command then `func_8008CBA8`

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** as a twin pair on the first natural C phrasing under
era `-O2 -G0` (YAML default). Matching-C leaves 562 and 563.

## Function hood

| symbol | file | VRAM | size | next |
|---|---|---|---|---|
| `func_80086FF8` | `[0x777F8,0x77824)` | `[0x80086FF8,0x80087024)` | 11 words | `func_80087024` |
| `func_80087024` | `[0x77824,0x77850)` | `[0x80087024,0x80087050)` | 11 words | `func_80087050` (`bne a0,zero`) |

Both end in canonical `jr ra; nop`. Preceding `0x777F4` is the delay-slot
nop of the previous function. Direct `jal` encodings:

- `func_80086FF8` (`0C021BFE`): 8 sites — `8006A274 8006A5CC 8006D2A0 8006D4A4 8006D84C 8006DA18 8006E890 8006F0A0`
- `func_80087024` (`0C021C09`): 7 sites — `800164CC 8003F354 8003F6D4 8006A27C 8006A5D4 8006B274 8006D65C`

## Retail body (86FF8; 87024 is identical except `li v0,0xF1`)

```text
80086ff8  xxxxxxxx  addiu sp,sp,-24
80086ffc  xxxxxxxx  li    v0,0xF0
80087000  xxxxxxxx  sw    ra,16(sp)
80087004  xxxxxxxx  lui   at,0x800C
80087008  xxxxxxxx  sw    v0,-0x3280(at)  # D_800CCD80
8008700c  xxxxxxxx  jal   func_8008CBA8
80087010  xxxxxxxx  nop
80087014  xxxxxxxx  lw    ra,16(sp)
80087018  xxxxxxxx  addiu sp,sp,24
8008701c  xxxxxxxx  jr    ra
80087020  xxxxxxxx  nop
```

Semantics: `D_800CCD80 = 0xF0` or `0xF1`, then `func_8008CBA8()`. No
return value consumed.

## Single-leaf objects

```text
AS=mipsel-linux-gnu-as OBJDUMP=mipsel-linux-gnu-objdump \
  OBJCOPY=mipsel-linux-gnu-objcopy \
  tools/analysis/era_leaf_match.sh src/func_80086FF8.c 0x80086FF8 0x2C -O2 -G0
# same for src/func_80087024.c 0x80087024 0x2C
```

Instruction words match retail except link-time placeholders:
`lui/sw` HI16/LO16 for `D_800CCD80` (`3c010000/ac220000` vs
`3c01800c/ac22cd80`) and `jal` `0c000000` vs `0c0232ea`. GNU as pad is
one trailing `nop`.

YAML mid-`765E8` carve: prefix `0x1210`, two C leaves `0x2C` each, resume
`[0x77850, asm]` to existing `func_800870E0` at `0x778E0`.
