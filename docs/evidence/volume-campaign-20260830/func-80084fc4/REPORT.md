# `func_80084FC4` — exact root-counter checkpoint

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

Outcome: **MATCHED** on natural C phrasing 1 under era `-O2 -G0` with the
established `MASPSX_FILL_STORE_DELAY_SLOT=1` gate. Integrated as matching-C
leaf 349 and Tier-2 probe rung 8.

## Function hood and retail span

- File `[0x757C4,0x757E4)`, VRAM `[0x80084FC4,0x80084FE4)`: `0x20`
  bytes, eight words.
- Canonical return: `jr ra` at `0x80084FDC`, with the final global store as
  its live delay slot at `0x80084FE0`.
- Seven unique exact direct callers exist at `0x80082E4C`, `0x80083068`,
  `0x80083328`, `0x80083464`, `0x8008424C`, `0x800843CC`, and
  `0x80084444`.
- The preceding real `func_80084F8C` returns at file `0x757B0/0x757B4`;
  three explicit zero alignment words occupy `0x757B8..0x757C0`.
- The following real `func_80084FE4` begins immediately at file `0x757E4`.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLS`. The preceding padding remains asm and
is not part of this body.

## Retail body

```text
757C4 80084FC4 801F023C  lui  v0,0x1F80
757C8 80084FC8 xxxxxxxx  ori  v0,v0,0x1120
757CC 80084FCC xxxxxxxx  lhu  v0,0(v0)
757D0 80084FD0 xxxxxxxx  lui  at,%hi(D_800BD02C)
757D4 80084FD4 xxxxxxxx  sw   a0,%lo(D_800BD02C)(at)
757D8 80084FD8 xxxxxxxx  lui  at,%hi(D_800A76D0)
757DC 80084FDC xxxxxxxx  jr   ra
757E0 80084FE0 xxxxxxxx  sw   v0,%lo(D_800A76D0)(at)
```

## Screens

| Screen | Result |
|---|---|
| Callee buckets | no calls or frame |
| Stage-0 globals | `D_800A76D0` is the sampled-counter baseline read by `func_80084FE4` and later elapsed checks; `D_800BD02C` is the caller-supplied limit read beside that baseline; another proven setup path writes both together |
| Coloring pressure | sampled halfword naturally stays in `$v0`; limit stays in `$a0` |
| `$v0` liveness | `$v0` owns both the MMIO address and loaded sample, then remains live to the return-delay store |
| Address retention | no symbolic address survives an access; two transient `$at` materializations |
| MMIO | volatile 16-bit read at `0x1F801120`, the PSX root-counter 2 count register; following code also reads its mode at `0x1F801128` to account for wrap |
| `-O` signal | frame-free volatile read and ordered stores are era `-O2`; fixed address is materialized as `lui/ori` |
| Loop/back-edge | none |
| Relocations | two normalized `HI16/LO16` global pairs |

## Minimal C and flags

```c
extern unsigned int D_800BD02C;
extern unsigned int D_800A76D0;

void func_80084FC4(unsigned int limit) {
    unsigned short count = *(volatile unsigned short *)0x1F801120;

    D_800BD02C = limit;
    D_800A76D0 = count;
}
```

Compile: era cc1, `-O2 -G0`, plus the existing default-off store-delay gate.
Without the gate cc1 emits the same first seven operations followed by the
last store, `jr ra`, and `nop` (nine content words). The gate moves only that
independent final store into the return delay slot; no toolchain code changed.

## Single-leaf object and ROM comparison

```text
xxxxxxxx <func_80084FC4>:
   0: xxxxxxxx  lui v0,0x1f80
   4: xxxxxxxx  ori v0,v0,0x1120
   8: xxxxxxxx  lhu v0,0(v0)
   c: xxxxxxxx  lui at,0              R_MIPS_HI16 D_800BD02C
  10: xxxxxxxx  sw  a0,0(at)          R_MIPS_LO16 D_800BD02C
  14: xxxxxxxx  lui at,0              R_MIPS_HI16 D_800A76D0
  18: xxxxxxxx  jr  ra
  1c: xxxxxxxx  sw  v0,0(at)          R_MIPS_LO16 D_800A76D0

ROM: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
C:   xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
RELOCS_NORMALIZED=two HI16/LO16 pairs
BYTE_EXACT=8/8
```

## Carve geometry

The former active span was `[0x74FB0,0x75884)` = `0x08D4`:

```text
asm prefix: 0x757C4 - 0x74FB0 = 0x0814
C leaf:     0x757E4 - 0x757C4 = 0x0020
asm resume: 0x75884 - 0x757E4 = 0x00A0
closure:    0x0814 + 0x0020 + 0x00A0 = 0x08D4
```

## Packed span and gates

```text
757B4: xxxxxxxx = xxxxxxxx  preceding return delay
757B8: xxxxxxxx = xxxxxxxx  alignment
757BC: xxxxxxxx = xxxxxxxx  alignment
757C0: xxxxxxxx = xxxxxxxx  alignment
757C4: xxxxxxxx = xxxxxxxx  leaf 1
757C8: xxxxxxxx = xxxxxxxx  leaf 2
757CC: xxxxxxxx = xxxxxxxx  leaf 3
757D0: xxxxxxxx = xxxxxxxx  leaf 4
757D4: xxxxxxxx = xxxxxxxx  leaf 5
757D8: xxxxxxxx = xxxxxxxx  leaf 6
757DC: xxxxxxxx = xxxxxxxx  leaf 7
757E0: xxxxxxxx = xxxxxxxx  leaf 8 / return delay
757E4: xxxxxxxx = xxxxxxxx  following function entry
757E8: xxxxxxxx = xxxxxxxx  following function word 2
757EC: xxxxxxxx = xxxxxxxx  following function word 3
757F0: xxxxxxxx = xxxxxxxx  following function word 4
PACKED_SPAN=EXACT
```

```text
Compare:  EXACT SHA-1 MATCH
candidate SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
scripts/verify_us.sh: exit 0; Split verification OK; compare EXACT MATCH
grep -cE ',[[:space:]]*c,' configs/USA/disc1.yaml: 349
```
