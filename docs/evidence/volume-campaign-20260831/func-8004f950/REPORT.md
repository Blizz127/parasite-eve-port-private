# `func_8004F950` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **401**.

## Function hood first

- File `[0x40150,0x40178)`, VA `[0x8004F950,0x8004F978)`: `0x28`, ten words.
- Among multiple independent references, `0x3E33C/0x3E340` materializes the
  exact start and `0x3E344` stores it to callback field `0x30($s1)`.
- Canonical return is `jr ra` at `0x40170`, with `nop` at `0x40174`.
- Preceding real `func_8004F910` ends at `0x40148/0x4014C`; following real
  `func_8004F978` starts at `0x40178` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_INDEPENDENT_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
3E33C 8004DB3C 3C028005  lui   v0,%hi(func_8004F950)
3E340 8004DB40 2442F950  addiu v0,v0,%lo(func_8004F950)
3E344 8004DB44 AE220030  sw    v0,0x30(s1)
```

## Retail body and screens

```text
40150 8004F950 xxxxxxxx  addiu sp,sp,-0x18
40154 8004F954 xxxxxxxx  sw    ra,0x10(sp)
40158 8004F958 xxxxxxxx  lui   a1,%hi(func_800509A8)
4015C 8004F95C xxxxxxxx  addiu a1,a1,%lo(func_800509A8)
40160 8004F960 xxxxxxxx  jal   func_800638D8
40164 8004F964 xxxxxxxx  nop
40168 8004F968 xxxxxxxx  lw    ra,0x10(sp)
4016C 8004F96C xxxxxxxx  addiu sp,sp,0x18
40170 8004F970 xxxxxxxx  jr    ra
40174 8004F974 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, `func_800638D8`; forwards slot `$a0` and callback `func_800509A8` in `$a1` |
| Stage-0 globals | none in the wrapper |
| Coloring pressure | minimal; unchanged `$a0`, symbolic callback in `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact frame and callback materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_800509A8(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004F950(int slot) {
    func_800638D8(slot, func_800509A8);
}
```

```text
xxxxxxxx <func_8004F950>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_800509A8
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_800509A8
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_800509A8; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The prior active asm span was `[0x40038,0x40820)` = `0x7E8`:

```text
asm prefix: 0x40150 - 0x40038 = 0x118
C leaf:     0x40178 - 0x40150 = 0x028
asm resume: 0x40820 - 0x40178 = 0x6A8
closure:    0x118 + 0x028 + 0x6A8 = 0x7E8
```

All sizes derive from retail boundaries, not aligned object sizes.

## Packed span and gates

```text
file    VA        retail   candidate  role
40140 8004F940  xxxxxxxx xxxxxxxx   preceding restore
40144 8004F944  xxxxxxxx xxxxxxxx   preceding teardown
40148 8004F948  xxxxxxxx xxxxxxxx   preceding return
4014C 8004F94C  xxxxxxxx xxxxxxxx   preceding delay
40150 8004F950  xxxxxxxx xxxxxxxx   leaf 1
40154 8004F954  xxxxxxxx xxxxxxxx   leaf 2
40158 8004F958  xxxxxxxx xxxxxxxx   leaf 3
4015C 8004F95C  xxxxxxxx xxxxxxxx   leaf 4
40160 8004F960  xxxxxxxx xxxxxxxx   leaf 5
40164 8004F964  xxxxxxxx xxxxxxxx   leaf 6
40168 8004F968  xxxxxxxx xxxxxxxx   leaf 7
4016C 8004F96C  xxxxxxxx xxxxxxxx   leaf 8
40170 8004F970  xxxxxxxx xxxxxxxx   leaf 9
40174 8004F974  xxxxxxxx xxxxxxxx   leaf 10
40178 8004F978  xxxxxxxx xxxxxxxx   following real entry
4017C 8004F97C  xxxxxxxx xxxxxxxx   following body
40180 8004F980  xxxxxxxx xxxxxxxx   following body
40184 8004F984  xxxxxxxx xxxxxxxx   following body
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0
RESULT: EXACT MATCH
Compare: EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
VERIFY_RC=0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 401 leaves
grep matching-C count: 401
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
