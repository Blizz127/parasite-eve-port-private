# `func_8004F978` — exact callback-registration wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`; matching-C
leaf **402**.

## Function hood first

- File `[0x40178,0x401A0)`, VA `[0x8004F978,0x8004F9A0)`: ten words.
- `0x3A1F4/0x3A1F8` independently materializes the exact start and the branch
  delay at `0x3A200` stores it to callback field `0x30($s0)`.
- It ends at `0x40198/0x4019C` with canonical `jr ra; nop`.
- Real predecessor `func_8004F950` ends at `0x40170/0x40174`; real successor
  `func_8004F9A0` starts at `0x401A0` with a frame prologue.

`FUNCTION_HOOD=PROVEN_BY_INDEPENDENT_EXACT_START_CALLBACK_STORE_AND_CANONICAL_RETURN`.

## Retail body and screens

```text
40178 8004F978 xxxxxxxx  addiu sp,sp,-0x18
4017C 8004F97C xxxxxxxx  sw    ra,0x10(sp)
40180 8004F980 xxxxxxxx  lui   a1,%hi(func_800509E0)
40184 8004F984 xxxxxxxx  addiu a1,a1,%lo(func_800509E0)
40188 8004F988 xxxxxxxx  jal   func_800638D8
4018C 8004F98C xxxxxxxx  nop
40190 8004F990 xxxxxxxx  lw    ra,0x10(sp)
40194 8004F994 xxxxxxxx  addiu sp,sp,0x18
40198 8004F998 xxxxxxxx  jr    ra
4019C 8004F99C xxxxxxxx  nop
```

| screen | result |
|---|---|
| Callee bucket | one direct `func_800638D8`, forwarding `$a0` and callback `func_800509E0` |
| Stage-0 globals | none |
| Coloring pressure | minimal |
| `$v0` liveness | discarded call result; void wrapper |
| Address retention | none |
| `-O` signal | compact era `-O2` frame/materialization |
| Loop/back-edge | none |

No gp access appears, selecting `-G0`.

## C and isolated object

```c
void func_800509E0(void);
void func_800638D8(int slot, void (*callback)(void));

void func_8004F978(int slot) {
    func_800638D8(slot, func_800509E0);
}
```

```text
object: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
relocs: word 3 R_MIPS_HI16 func_800509E0; word 4 R_MIPS_LO16 func_800509E0; word 5 R_MIPS_26 func_800638D8
ROM:    xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
linked: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
BYTE_EXACT=10/10
PHRASINGS_USED=1/2
```

Flags: era GCC 2.7.2 `-O2 -G0`; no pins, asm, fabricated nops, or maspsx
behavior gates.

## Carve and packed span

The leaf consumed the head of active `[0x40178,0x40820)`:

```text
C:      0x401A0 - 0x40178 = 0x028
resume: 0x40820 - 0x401A0 = 0x680
closure: 0x028 + 0x680 = 0x6A8
```

```text
file    retail   candidate  role
40168   xxxxxxxx xxxxxxxx   predecessor restore
4016C   xxxxxxxx xxxxxxxx   predecessor teardown
40170   xxxxxxxx xxxxxxxx   predecessor return
40174   xxxxxxxx xxxxxxxx   predecessor delay
40178   xxxxxxxx xxxxxxxx   leaf 1
4017C   xxxxxxxx xxxxxxxx   leaf 2
40180   xxxxxxxx xxxxxxxx   leaf 3
40184   xxxxxxxx xxxxxxxx   leaf 4
40188   xxxxxxxx xxxxxxxx   leaf 5
4018C   xxxxxxxx xxxxxxxx   leaf 6
40190   xxxxxxxx xxxxxxxx   leaf 7
40194   xxxxxxxx xxxxxxxx   leaf 8
40198   xxxxxxxx xxxxxxxx   leaf 9
4019C   xxxxxxxx xxxxxxxx   leaf 10
401A0   xxxxxxxx xxxxxxxx   successor entry
401A4   xxxxxxxx xxxxxxxx   successor body
401A8   xxxxxxxx xxxxxxxx   successor body
401AC   xxxxxxxx xxxxxxxx   successor body
PACKED_SPAN=EXACT
```

```text
BUILD_RC=0; RESULT: EXACT MATCH
SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
VERIFY_RC=0; Split verification (Phase 4E): OK; compare: EXACT MATCH
matching-C count: 402
```

`MATCHED_C=YES`
`INTEGRATED_EXACT=YES`
