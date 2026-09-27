# func_80070D6C — MATCHED (25 words, LINK_EXACT)

VRAM `0x80070D6C`, span `0x64`. era `-O1 -G0`.

Walks the embedded table at `0x80070E04` / `0x80070E08` with registers
pinned to the retail homes (`$t0`, `$t7`, `$t8`, `$t1`–`$t6`, `$v0`,
`$zero`). Empty register barriers keep the address temps. Verified
`LINK_EXACT` at `-O1 -G0`.
