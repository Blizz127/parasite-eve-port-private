# func_80055760 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80055760`, file offset `0x45F60`, size `0x6B4` (429 words), `asm/disc1/45F60.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "profile": "-O2 -G8 + FORCE_ABSOLUTE D_800A1F84,D_800C0E48,D_8009D05C", "levers": ["func_80052E30 body as static inline sel()", "get() range+item lookup inline", "goto outer loops with n = 1; if (D_8009D04C) n = 2; bound (retail recomputes per iteration)"], "note": "outer structure now aligned; residual dominated by one missing callee-saved reg: retail hoists la D_800C0E08 into s2 for the D_800C0E08 < D_800C0E06 (lhu 0/-2(s2)) test in the second pass, cc1 emits constant-address lhu. Draft drafts/func_80055760.c"}
