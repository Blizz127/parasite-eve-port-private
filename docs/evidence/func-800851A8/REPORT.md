# func_800851A8 — MATCHED (58 words, LINK_EXACT)

VRAM `0x800851A8`, span `0xE8`. era `-O2 -G0`.

SPU upload. Callee-saved pins `$s0`–`$s3` plus `$v0`/`$v1`/`$a1`, with empty
register barriers on the pointer walk. Verified `LINK_EXACT` (240-byte
object, zero pad). Carve waits until the in-flight disc-1 gate finishes.
