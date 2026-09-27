# func_80058FEC — PARKED (lane exeC, 2026-09-27)

- VRAM `0x80058FEC`, file offset `0x497EC`, size `0x548` (338 words), `asm/disc1/497EC.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompA`: {"status": "parked", "profile": "-O2 -G8 + FORCE_ABSOLUTE D_800C0E20,D_800C0E22", "levers": ["r kept live via goto out (s4 exact)", "static inline swap(p,q) gives retail a1/a0 xor-swap", "shared done: func_80055760 tail after first swap", "item() lookup inline shared with func_80058AA8"], "note": "265-word residual (mostly address shift): retail slot()/range-lookup result is copied v0->a1->a3 (inline return-value copies); int/short/ternary/macro/early-return spellings give either no copies or lhu+sll/sra. Draft drafts/func_80058FEC.c"}
