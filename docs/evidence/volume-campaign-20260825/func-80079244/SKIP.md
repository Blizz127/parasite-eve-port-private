# `func_80079244` — handwritten libGTE RTPS wrapper

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

`DISPOSITION=SKIP-SDK-LIBRARY-COP2`. No C attempt, integration, or matching
count change.

## Function hood

Retail span `[0x69A44,0x69A70)`, VRAM `0x80079244`, is eleven words and ends
in canonical `jr ra` with a useful arithmetic delay slot. Six unique direct
calls target the exact start:

```text
0x80068544  0x80068574  0x800685A8
0x800685DC  0x8006E04C  0x800C3D4C
```

The preceding real `func_80079228` returns before the alignment nop at
`0x80079240`; the following handwritten `func_80079274` begins after the
alignment nop at `0x80079270`. Those nops are outside the exact called span
and remain asm padding.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLERS`.

## Retail body and classification

```text
80079244: xxxxxxxx  lwc2   $0,0(a0)
80079248: xxxxxxxx  lwc2   $1,4(a0)
8007924c: xxxxxxxx  nop
80079250: xxxxxxxx  rtps
80079254: xxxxxxxx  swc2   $14,0(a1)
80079258: xxxxxxxx  swc2   $8,0(a2)
8007925c: xxxxxxxx  cfc2   v1,$31
80079260: xxxxxxxx  mfc2   v0,$19
80079264: xxxxxxxx  sw     v1,0(a3)
80079268: xxxxxxxx  jr     ra
8007926c: xxxxxxxx  sra    v0,v0,2
```

Existing projection evidence independently identifies this as the GTE `RTPS`
wrapper returning `SZ3 >> 2`. Its semantic body depends on COP2 data loads,
the `RTPS` command, COP2 result/control reads, and COP2 stores. Ordinary
sanctioned C has no spelling for those architectural side effects, and inline
assembly is forbidden.

This is the same established Psy-Q/libGTE redirect policy as the enumerated
handwritten COP2 family, not an optimizer park. It moves from Tier 1 to SKIP
without consuming a phrasing. Matching C remains 318; consecutive parks stay
at 1.
