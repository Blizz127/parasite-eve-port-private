# `func_80087864` — volatile-store scheduling family screen

> Retail instruction words and byte strings in this file are elided (`xxxxxxxx`, `<N bytes …>`): no game data is committed. Read them from your own disc (`tools/extract/disc_cache.py`, EXE `SLUS_006.62`).

`PARKED-VOLATILE-STORE-SCHEDULING-FAMILY`. No C attempt was spent because
the complete body reproduces the just-bounded `func_8008783C` mechanism.

## Function hood

Retail span `[0x78064,0x7808C)`, VRAM `0x80087864`, ten words. It ends in
the canonical `jr ra` with `sh v0,0(a0)` in the delay slot. The exact direct
caller is at `0x80087A80`. The preceding `func_8008783C` ends at
`0x7805C/0x78060`, and the next real function begins at `0x7808C`; there is
no boundary padding or ownership ambiguity.

`FUNCTION_HOOD=PROVEN_BY_DIRECT_CALLER`.

## Retail body and family proof

```text
xxxxxxxx  lui   v0,0x1F80
xxxxxxxx  ori   v0,v0,0x1C08
xxxxxxxx  sll   a0,a0,4
xxxxxxxx  addu  a0,a0,v0
xxxxxxxx  lhu   v0,0(a0)
xxxxxxxx  nop
xxxxxxxx  andi  v0,v0,0xFFF0
xxxxxxxx  or    v0,v0,a1
xxxxxxxx  jr    ra
xxxxxxxx  sh    v0,0(a0)
```

This updates the same per-voice SPU register at
`0x1F801C08 + voice*0x10`, replacing bits 0..3 rather than bits 4..7. It has
the same retained `$a0` MMIO pointer and the same direct volatile halfword
store in the return slot as `func_8008783C`.

The two bounded `8783C` phrasings—void volatile RMW and explicit returned
value—both emitted `sh; jr; nop` after otherwise exact address/data flow.
The extra `a1 << 4` in `8783C` is before the mask/merge and does not own the
return-slot decision. Removing it here cannot supply a different delay-slot
lever. The existing maspsx store-fill rule is limited to absolute symbolic
`sw` macros and does not rewrite direct `sh 0(a0)`.

No source, YAML, build, or verifier change was made. Matching-C remains 311,
and this no-attempt family screen does not increment the consecutive-park
counter.
