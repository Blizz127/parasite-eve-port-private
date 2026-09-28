# `func_80050088` — exact callback-unregister wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0`, with no
maspsx behavior gate. Integrated as matching-C leaf 359 and Tier-2
continuation rung 18.

## Function hood and retail span

- File `[0x40888,0x408A8)`, VRAM `[0x80050088,0x800500A8)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x800500A0`, followed by `nop` in the delay
  slot.
- There is no direct `jal` caller. The exact start is constructed at
  `0x80048E4C–0x80048E50` and stored at `0x80048E54` into callback slot
  `+0x30` of the record returned by `func_8006322C`.
- Preceding real `func_80050060` ends at file `0x40880/0x40884`; following
  real `func_800500A8` begins immediately at file `0x408A8`.

`FUNCTION_HOOD=PROVEN_BY_EXACT_START_CALLBACK_REGISTRATION`; the generated
label is supported by executable pointer provenance and is not inferred from
its name.

## Retail body

```text
40888 80050088 xxxxxxxx  addiu sp,sp,-0x18
4088C 8005008C xxxxxxxx  sw    ra,0x10(sp)
40890 80050090 xxxxxxxx  jal   func_800638D8
40894 80050094 xxxxxxxx  addu  a1,zero,zero
40898 80050098 xxxxxxxx  lw    ra,0x10(sp)
4089C 8005009C xxxxxxxx  addiu sp,sp,0x18
408A0 800500A0 xxxxxxxx  jr    ra
408A4 800500A4 xxxxxxxx  nop
```

## Screens

| Screen | Result |
|---|---|
| Callee bucket | one unresolved retail/asm callee, `func_800638D8`; sibling `func_80050060` passes a real callback as argument 2, while this leaf passes null |
| Stage-0 globals | none accessed directly by this wrapper; callback-list and rendering state effects belong to the callee |
| Coloring pressure | none beyond preserving `$ra`; `$a0` flows through and null `$a1` fills the call delay slot |
| `$v0` liveness | dead after the call, matching the void source contract |
| Address retention | none |
| `-O` signal | canonical 24-byte frame, useful null call-delay slot, and post-call restore/teardown sequence are era `-O2` |
| Loop/back-edge | none in the wrapper |
| Relocations | one normalized `R_MIPS_26` call relocation |

## Minimal C and flags

```c
void func_800638D8(void *object, void *callback);

void func_80050088(void *object) {
    func_800638D8(object, 0);
}
```

Compile: era cc1, `-O2 -G0`; no maspsx gate.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80050088>:
   0: xxxxxxxx  addiu sp,sp,-24
   4: xxxxxxxx  sw    ra,16(sp)
   8: xxxxxxxx  jal   0                 R_MIPS_26 func_800638D8
   c: xxxxxxxx  move  a1,zero
  10: xxxxxxxx  lw    ra,16(sp)
  14: xxxxxxxx  addiu sp,sp,24
  18: xxxxxxxx  jr    ra
  1c: xxxxxxxx  nop

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=one R_MIPS_26 call
BYTE_EXACT=8/8
```

## Carve geometry

The former active span was `[0x40838,0x41518)` = `0x0CE0`:

```text
asm prefix: 0x40888 - 0x40838 = 0x0050
C leaf:     0x408A8 - 0x40888 = 0x0020
asm resume: 0x41518 - 0x408A8 = 0x0C70
closure:    0x0050 + 0x0020 + 0x0C70 = 0x0CE0
```

These are boundary-derived text sizes; no aligned object size was used for
the carve.

## Packed span and gates

All sixteen words below were read from both the rebuilt packed executable and
retail. Each pair is equal:

```text
40878: xxxxxxxx = xxxxxxxx  preceding restore ra
4087C: xxxxxxxx = xxxxxxxx  preceding teardown
40880: xxxxxxxx = xxxxxxxx  preceding return
40884: xxxxxxxx = xxxxxxxx  preceding delay
40888: xxxxxxxx = xxxxxxxx  leaf 1
4088C: xxxxxxxx = xxxxxxxx  leaf 2
40890: xxxxxxxx = xxxxxxxx  leaf 3
40894: xxxxxxxx = xxxxxxxx  leaf 4 / call delay
40898: xxxxxxxx = xxxxxxxx  leaf 5
4089C: xxxxxxxx = xxxxxxxx  leaf 6
408A0: xxxxxxxx = xxxxxxxx  leaf 7
408A4: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
408A8: xxxxxxxx = xxxxxxxx  following function entry
408AC: xxxxxxxx = xxxxxxxx  following word 2
408B0: xxxxxxxx = xxxxxxxx  following word 3
408B4: xxxxxxxx = xxxxxxxx  following word 4
PACKED_SPAN=EXACT
```

```text
build/extracted/disc1/SLUS_006.62:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
build/disc1.candidate.exe:
452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 359
```
