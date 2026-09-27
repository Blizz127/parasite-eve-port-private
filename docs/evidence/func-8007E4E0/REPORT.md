# func_8007E4E0 — MATCHED (13 words, LINK_EXACT)

VRAM `0x8007E4E0`, span `0x34`. era `-O1 -G0`.

Copies words from `func_8007E334 + 0x10` through `func_8007E3B4` to
`0xDF80`. `-O1` keeps the load-delay `nop`. Pointers pinned to `$v0`,
`$t2`, `$t1`, and the loaded word to `$v1`. Verified `LINK_EXACT`.
