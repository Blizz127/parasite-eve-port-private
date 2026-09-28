# func_800768A0 — PARKED at 17 words (lane exeE, 2026-09-27)

VRAM `0x800768A0`, file `0x670A0`, size `0x280`. Read-back twin of func_80076664
(GP0 `0xC0000000`, second poll on `0x08000000`, PIO copy `*p++ = *D_80095850`, DMA
`0x01000200`). Best draft: `build/lanes/exeE/drafts/func_800768A0.c`, 2.8.1
`-O2 -G0 -mno-split-addresses`, same levers as func_80076664 (see its PARK.md).
Residual: the same two classes as func_80076664 (prologue s1/s2 pair order 4 words,
size-block `$v0/$v1` local-alloc ~8 words) plus the second poll's mask `lui` scheduled
before the status load (retail: `lw v0,22612(v0) / nop / lw v0,0(v0) / lui v1,0x800`).
Barrier variants for the second poll tried: tie order swapped, split barriers, st-only barrier
+ separate loop constant (17–19).
