# `func_8004F2E4` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf **400**.

## Function hood first

- File `[0x3FAE4,0x3FB0C)`, VA `[0x8004F2E4,0x8004F30C)`: `0x28`, ten words.
- Files `0x3FA84/0x3FA88` materialize its exact start; the `func_80062CB8`
  call at `0x3FA8C` stores it to callback field `0x30($a0)` in its delay
  slot at `0x3FA90`.
- Canonical return is `jr ra` at `0x3FB04`, with `nop` at `0x3FB08`.
- Preceding real `func_8004F23C` ends at `0x3FADC/0x3FAE0`; following real
  `func_8004F30C` starts at `0x3FB0C` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

```text
3FA84 8004F284 3C028005  lui   v0,%hi(func_8004F2E4)
3FA88 8004F288 2442F2E4  addiu v0,v0,%lo(func_8004F2E4)
3FA8C 8004F28C 0C018B2E  jal   func_80062CB8
3FA90 8004F290 AC820030  sw    v0,0x30(a0)
```

## Retail body and screens

```text
3FAE4 8004F2E4 xxxxxxxx  addiu sp,sp,-0x18
3FAE8 8004F2E8 xxxxxxxx  sw    ra,0x10(sp)
3FAEC 8004F2EC xxxxxxxx  lui   a1,%hi(func_80050728)
3FAF0 8004F2F0 xxxxxxxx  addiu a1,a1,%lo(func_80050728)
3FAF4 8004F2F4 xxxxxxxx  jal   func_800638D8
3FAF8 8004F2F8 xxxxxxxx  nop
3FAFC 8004F2FC xxxxxxxx  lw    ra,0x10(sp)
3FB00 8004F300 xxxxxxxx  addiu sp,sp,0x18
3FB04 8004F304 xxxxxxxx  jr    ra
3FB08 8004F308 xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct callee, `func_800638D8`; forwards slot `$a0` and callback `func_80050728` in `$a1` |
| Stage-0 globals | none in the wrapper |
| Coloring pressure | minimal; unchanged `$a0`, symbolic callback in `$a1` |
| `$v0` liveness | callee result discarded; void wrapper |
| Address retention | none |
| `-O` signal | compact frame and callback materialization select era `-O2` |
| Loop/back-edge | none |

No gp-relative access appears, selecting `-G0`.

## Minimal C and single-leaf comparison

```c
void func_80050728(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004F2E4(int slot) {
    func_800638D8(slot, func_80050728);
}
```

```text
xxxxxxxx <func_8004F2E4>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  lui   a1,0            R_MIPS_HI16 func_80050728
   c: xxxxxxxx  addiu a1,a1,0        R_MIPS_LO16 func_80050728
  10: xxxxxxxx  jal   0              R_MIPS_26 func_800638D8
  14: xxxxxxxx  nop
  18: xxxxxxxx  lw    ra,16(sp)
  1c: xxxxxxxx  addiu sp,sp,24
  20: xxxxxxxx  jr    ra
  24: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=R_MIPS_HI16/R_MIPS_LO16 func_80050728; R_MIPS_26 func_800638D8
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Compile flags: era GCC 2.7.2 `-O2 -G0`; no pins, inline/file-scope assembly,
fabricated nops, or maspsx gates.

## Carve geometry

The prior active asm span was `[0x3F758,0x3FC48)` = `0x4F0`:

```text
asm prefix: 0x3FAE4 - 0x3F758 = 0x38C
C leaf:     0x3FB0C - 0x3FAE4 = 0x028
asm resume: 0x3FC48 - 0x3FB0C = 0x13C
closure:    0x38C + 0x028 + 0x13C = 0x4F0
```

All sizes derive from retail boundaries, not aligned object sizes.

## Packed span and gates

```text
file    VA        retail   candidate  role
3FAD4 8004F2D4  xxxxxxxx xxxxxxxx   preceding restore
3FAD8 8004F2D8  xxxxxxxx xxxxxxxx   preceding teardown
3FADC 8004F2DC  xxxxxxxx xxxxxxxx   preceding return
3FAE0 8004F2E0  xxxxxxxx xxxxxxxx   preceding delay
3FAE4 8004F2E4  xxxxxxxx xxxxxxxx   leaf 1
3FAE8 8004F2E8  xxxxxxxx xxxxxxxx   leaf 2
3FAEC 8004F2EC  xxxxxxxx xxxxxxxx   leaf 3
3FAF0 8004F2F0  xxxxxxxx xxxxxxxx   leaf 4
3FAF4 8004F2F4  xxxxxxxx xxxxxxxx   leaf 5
3FAF8 8004F2F8  xxxxxxxx xxxxxxxx   leaf 6
3FAFC 8004F2FC  xxxxxxxx xxxxxxxx   leaf 7
3FB00 8004F300  xxxxxxxx xxxxxxxx   leaf 8
3FB04 8004F304  xxxxxxxx xxxxxxxx   leaf 9
3FB08 8004F308  xxxxxxxx xxxxxxxx   leaf 10
3FB0C 8004F30C  xxxxxxxx xxxxxxxx   following real entry
3FB10 8004F310  xxxxxxxx xxxxxxxx   following body
3FB14 8004F314  xxxxxxxx xxxxxxxx   following body
3FB18 8004F318  xxxxxxxx xxxxxxxx   following body
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0
RESULT: EXACT MATCH
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe

VERIFY_RC=0
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 400 leaves

grep matching-C count: 400
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
