# Literal reconstruction — `func_800718D0`

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Executable SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
File offset `pc - 0x80010000 + 0x800`. 29 words,
`0x800718D0..0x80071944` exclusive.

```text
0x800718D0  xxxxxxxx  addiu $sp, $sp, -32
0x800718D4  xxxxxxxx  sw    $ra, 24($sp)
0x800718D8  xxxxxxxx  sw    $s1, 20($sp)
0x800718DC  xxxxxxxx  sw    $s0, 16($sp)
0x800718E0  xxxxxxxx  lw    $v0, 4($a0)          # TIM flag
0x800718E4  xxxxxxxx  nop
0x800718E8  xxxxxxxx  andi  $v0, $v0, 8
0x800718EC  xxxxxxxx  beq   $v0, $zero, 0x80071904
0x800718F0  xxxxxxxx  addu  $s0, $zero, $zero    # delay: s0=0
0x800718F4  xxxxxxxx  lw    $v0, 8($a0)          # CLUT bnum
0x800718F8  xxxxxxxx  addiu $s0, $a0, 8
0x800718FC  xxxxxxxx  j     0x80071908
0x80071900  xxxxxxxx  addu  $v0, $s0, $v0        # image = clut + bnum
0x80071904  xxxxxxxx  addiu $v0, $a0, 8          # no-CLUT image
0x80071908  xxxxxxxx  addiu $a0, $v0, 4          # image RECT
0x8007190C  xxxxxxxx  addiu $s1, $v0, 12         # image pixels
0x80071910  xxxxxxxx  jal   func_8007506C        # IMAGE first
0x80071914  xxxxxxxx  addu  $a1, $s1, $zero
0x80071918  xxxxxxxx  beq   $s0, $zero, 0x80071928
0x8007191C  xxxxxxxx  addiu $a0, $s0, 4          # CLUT RECT
0x80071920  xxxxxxxx  jal   func_8007506C        # CLUT second
0x80071924  xxxxxxxx  addiu $a1, $s0, 12
0x80071928  xxxxxxxx  addu  $v0, $s1, $zero
0x8007192C  xxxxxxxx  lw    $ra, 24($sp)
0x80071930  xxxxxxxx  lw    $s1, 20($sp)
0x80071934  xxxxxxxx  lw    $s0, 16($sp)
0x80071938  xxxxxxxx  addiu $sp, $sp, 32
0x8007193C  xxxxxxxx  jr    $ra
0x80071940  xxxxxxxx  nop
```

Sole callee: `func_8007506C` twice (`0x0C01D41B`). No MMIO, no
`func_80030894`.

## Pack sites (in `func_8006AD40`, after `jal 718D0`)

```text
0x8006AFB0  a1 = 0
0x8006AFB4  loop:
              pack D_80091648+a1 {A,B,C,D} -> dests +8/+0xA
0x8006AFF8  sh packed1, 0x1650($at)     # tpage  D_80091650 when a1=0
0x8006B02C  sh packed2, 0x1652($at)     # CLUT   D_80091652 when a1=0
0x8006B030  a1 += 0x10
0x8006B034  sltiu a1, 0x20
0x8006B038  bne -> AFB4
```

`a1=0` writes record 0 (`0x0025` / `0x3F14`). `a1=0x10` writes
record 1 (`0x0026` / `0x3F15`). Same GetTPage/GetClut formula as
B54D records 2/3.
