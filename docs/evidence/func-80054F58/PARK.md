# func_80054F58 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80054F58`, file offset `0x45758`, size `0x44C` (275 words), `asm/disc1/45758.s`.

## This lane (exeC)

- 119 -> 117 (masked 0xE0 value reusing the pinned $4 'a'); inline find()/find_m() search helpers 160 (size 1072). Residual: third-loop copies (retail moves b a0->a1 and puts the q[0x14] bound in a0) + 1<<(x-8) tail homes. Draft unchanged from decompA.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "profile": "era_o2_g8", "levers": "static inline rec() = func_8005332C body with goto-done for the three table cases and an early 'return 0' for out-of-range (retail's s1=0 slot form); first-loop compare value pinned asm($4)", "note": "119 words: inline item-record lookups and first attribute loop exact; remaining are register-allocation choices in the 0xE0-class loop (retail copies the masked value a0->a1 and reuses a0 for the bound) and the 1<<(x-8) bitmask tail (s2/s3 homes shift when the tail is restructured as a single return r). Draft drafts/func_80054F58.c"}
