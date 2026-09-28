# Literal reconstruction — AF54 wait/reissue to AF68

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Executable SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
File offset `pc - 0x80010000 + 0x800`. Exclusive window
`0x8006AF54..0x8006AF68` is **5 words** / `0x14`.

```text
0x8006AF44  xxxxxxxx  addiu $s0, $zero, 1        # already in B54D prefix
0x8006AF48  xxxxxxxx  addiu $v0, $zero, -1
0x8006AF4C  xxxxxxxx  beq   $s2, $v0, 0x8006ADD8 # reissue channel 2
0x8006AF50  xxxxxxxx   nop
0x8006AF54  xxxxxxxx  jal   func_8006E7E8        # LIVE POLL — this rung
0x8006AF58  xxxxxxxx   nop
0x8006AF5C  xxxxxxxx  addu  $s2, $v0, $zero
0x8006AF60  xxxxxxxx  bne   $s2, $zero, 0x8006AE08
0x8006AF64  xxxxxxxx   nop
0x8006AF68  xxxxxxxx  addu  $s0, $zero, $zero    # EXCLUSIVE END
```

Canonical locals at first `AF54` visit: `s0 = 1` (`AF44`), `s2 = 1`
(`AE04` after the already-issued channel-2 read). `AF4C` is therefore
not taken on entry.

`AE08` with `s0 != 0` is `bne → AF4C`, the wait head, not the texture
loop. Production therefore does not re-run `func_8006E1C0`.

```text
s2 = func_8006E7E8()
  0   -> AF68 cut
  !=0 -> AE08 -> AF4C
         s2 == -1 -> ADD8 reissue (existing func_8006E6A8 of D_800930EC)
         then poll again
```

`s2` and the poll result are the live `func_8006E7E8` return. They are
not stored as constants.

## Not taken

```text
0x8006AF68  s0 = 0
0x8006AF88  jal func_8006E6A8     # D_800930EE / dest+0x180
0x8006AFA8  jal func_800718D0
0x8006B0AC  jal func_80030894
```
