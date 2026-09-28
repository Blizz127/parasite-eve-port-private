# `func_800653B8` — exact mailbox queue append

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Leaf 334, matched on the first bounded phrasing after ordinary relocation
normalization.

## Function hood and boundary ownership

Retail span `[0x55BB8,0x55C00)`, VRAM `[0x800653B8,0x80065400)`, is
`0x48` bytes / eighteen words. It ends with canonical `jr ra; nop` at
`0x800653F8/0x800653FC`.

A raw executable scan finds the sole direct caller at `0x80017794`, the
opcode-`0x1C` send path. The preceding real `func_8006536C` owns `jr ra; nop`
at `0x800653B0/0x800653B4`; the following real `func_80065400` begins
immediately with `lbu v0,0x44(gp)`. There is no boundary padding.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLER_AND_CANONICAL_RETURN`.

## Screens and written-state provenance

Existing PE-MBX1/PE-MBX2 evidence independently proves the state model:
`D_800A3180` is a 28-row, 12-byte mailbox queue and byte
`D_8009CDB4` is its append count. `func_8006536C` clears the rows/count;
`func_80065400` drains them. This append intentionally has no full check or
28-row clamp.

| screen | result |
|---|---|
| callee buckets | no `jal`; true leaf |
| written-state Stage 0 | appends one proven 12-byte record to `D_800A3180`, then increments proven gp-relative count `D_8009CDB4`; drain/clear readers are independently documented |
| coloring pressure | `$v0` evolves from `count*12` to retained record pointer; `$v1` evolves from count to absolute table base, then reloads count; `$t0` holds the o32 fifth argument |
| `$v0` liveness | scaled index and record address only; no return value |
| address retention | aggregate subscript retains the computed record address in `$v0` across all five stores |
| optimization signal | early fifth-argument load, aggregate stride synthesis, store scheduling, and gp-relative byte accesses select era GCC 2.7.2 `-O2 -G8` |
| loop/back-edge owner | none |

## Final C and flags

```c
typedef struct {
    unsigned short dest_type;
    unsigned char dest_id;
    unsigned char payload;
    unsigned int extra;
    unsigned int sender;
} MailboxRecord;

extern MailboxRecord D_800A3180[];
extern unsigned char D_8009CDB4;

void func_800653B8(unsigned int payload, unsigned int dest_id,
                   unsigned int dest_type, unsigned int sender,
                   unsigned int extra) {
    MailboxRecord *record = &D_800A3180[D_8009CDB4];

    record->payload = payload;
    record->dest_id = dest_id;
    record->sender = sender;
    record->dest_type = dest_type;
    record->extra = extra;
    D_8009CDB4++;
}
```

Compiler: era GCC 2.7.2-psx plus maspsx 2.21, `-O2 -G8`. No pins,
inline assembly, or special maspsx switch is used. The 12-byte aggregate is
the source lever that produces retail's `count*12` DAG and one retained
record pointer.

With `_gp = 0x8009CD70`:

```text
D_8009CDB4 - _gp = 0x44
D_800A3180          = HI16 0x800A / LO16 0x3180
```

## Full eighteen-word comparison

| word | retail | candidate normalized | instruction |
|---:|---:|---:|---|
| 0 | `xxxxxxxx` | `xxxxxxxx` | `lbu v1,0x44(gp)` |
| 1 | `xxxxxxxx` | `xxxxxxxx` | `lw t0,0x10(sp)` |
| 2 | `xxxxxxxx` | `xxxxxxxx` | `sll v0,v1,1` |
| 3 | `xxxxxxxx` | `xxxxxxxx` | `addu v0,v0,v1` |
| 4 | `xxxxxxxx` | `xxxxxxxx` | `sll v0,v0,2` |
| 5 | `xxxxxxxx` | `xxxxxxxx` | `lui v1,0x800A` |
| 6 | `xxxxxxxx` | `xxxxxxxx` | `addiu v1,v1,0x3180` |
| 7 | `xxxxxxxx` | `xxxxxxxx` | `addu v0,v0,v1` |
| 8 | `xxxxxxxx` | `xxxxxxxx` | `sb a0,3(v0)` |
| 9 | `xxxxxxxx` | `xxxxxxxx` | `sb a1,2(v0)` |
| 10 | `xxxxxxxx` | `xxxxxxxx` | `lbu v1,0x44(gp)` |
| 11 | `xxxxxxxx` | `xxxxxxxx` | `sw a3,8(v0)` |
| 12 | `xxxxxxxx` | `xxxxxxxx` | `sh a2,0(v0)` |
| 13 | `xxxxxxxx` | `xxxxxxxx` | `sw t0,4(v0)` |
| 14 | `xxxxxxxx` | `xxxxxxxx` | `addiu v1,v1,1` |
| 15 | `xxxxxxxx` | `xxxxxxxx` | `sb v1,0x44(gp)` |
| 16 | `xxxxxxxx` | `xxxxxxxx` | `jr ra` |
| 17 | `xxxxxxxx` | `xxxxxxxx` | `nop` |

Single-leaf object before relocation:

```text
00000000 <func_800653B8>:
   0: 93830000  lbu    v1,0(gp)
      0: R_MIPS_GPREL16 D_8009CDB4
   4: 8fa80010  lw     t0,16(sp)
   8: 00031040  sll    v0,v1,1
   c: 00431021  addu   v0,v0,v1
  10: 00021080  sll    v0,v0,2
  14: 3c030000  lui    v1,0
      14: R_MIPS_HI16 D_800A3180
  18: 24630000  addiu  v1,v1,0
      18: R_MIPS_LO16 D_800A3180
  1c: 00431021  addu   v0,v0,v1
  20: a0440003  sb     a0,3(v0)
  24: a0450002  sb     a1,2(v0)
  28: 93830000  lbu    v1,0(gp)
      28: R_MIPS_GPREL16 D_8009CDB4
  2c: ac470008  sw     a3,8(v0)
  30: a4460000  sh     a2,0(v0)
  34: ac480004  sw     t0,4(v0)
  38: 24630001  addiu  v1,v1,1
  3c: a3830000  sb     v1,0(gp)
      3c: R_MIPS_GPREL16 D_8009CDB4
  40: 03e00008  jr     ra
  44: 00000000  nop
```

The trim guard verified the GNU-as alignment bytes beyond the `0x48` body
were all zero before removing them.

## Carve geometry and packed-span proof

The prior asm span was `[0x55430,0x5ADBC)`, size `0x598C`:

```text
prefix asm:  0x55BB8 - 0x55430 = 0x0788
C leaf:      0x55C00 - 0x55BB8 = 0x0048
resume asm:  0x5ADBC - 0x55C00 = 0x51BC
closure:     0x0788 + 0x0048 + 0x51BC = 0x598C
```

Retail and packed candidate are identical through both real boundaries:

```text
retail:
00055ba8: 0c00a524 440080a3 0800e003 00000000
00055bb8: 44008393 1000a88f 40100300 21104300
00055bc8: 80100200 0a80033c 80316324 21104300
00055bd8: 030044a0 020045a0 44008393 080047ac
00055be8: 000046a4 040048ac 01006324 440083a3
00055bf8: 0800e003 00000000 44008293 d8ffbd27

candidate:
00055ba8: 0c00a524 440080a3 0800e003 00000000
00055bb8: 44008393 1000a88f 40100300 21104300
00055bc8: 80100200 0a80033c 80316324 21104300
00055bd8: 030044a0 020045a0 44008393 080047ac
00055be8: 000046a4 040048ac 01006324 440083a3
00055bf8: 0800e003 00000000 44008293 d8ffbd27
```

The packed target bytes equal the relocated single-leaf object:

```text
<32 bytes, sha1 181345b35c12>
<32 bytes, sha1 a37ad50ec945>
0800e00300000000
```

## Gates

```text
Pack:     OK (build/disc1.candidate.exe, size 0x1EE800)
Compare:  EXACT SHA-1 MATCH
452fb033f2eaa4b18aa20a5bca60b8125af3a37b  build/disc1.candidate.exe
Split verification (Phase 4E): OK.
candidate: build/disc1.candidate.exe SHA-1 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
compare: EXACT MATCH to original
C conversion: Phase 5HD-12850 — 334 leaves
334
```

Result: `MATCHED=18/18` after ordinary relocation normalization, first
phrasing; consecutive parks reset from one to zero.
