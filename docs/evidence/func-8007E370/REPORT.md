# func_8007E370 — MATCHED (17 words, LINK_EXACT)

VRAM `0x8007E370`, span `0x44`. era `-O2 -G0`.

A file-scope `register volatile unsigned int *hw asm("$3")` is the incoming
`$v1` hardware base. The busy bit is `hw[0x41D]`, then a computed goto
through `*(unsigned int *)0xDFFC`. cc1 warns that `$v1` is call-clobbered;
this leaf makes no call. Verified `LINK_EXACT`.
