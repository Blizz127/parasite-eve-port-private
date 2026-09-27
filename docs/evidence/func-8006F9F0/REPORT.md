# func_8006F9F0 — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x8006F9F0`, file offset `0x601F0`, size `0x228`; yaml `- [0x601F0, c, func_8006F9F0]` replaces
  `- [0x601F0, asm]` exactly (next row `[0x60418, c, func_8006FC18]`).
- Profile: default `era_o2_g0`. Record-state dispatcher (see the PARK for the semantics); the state-5 reset block is
  the same inline shape as the tail of the matched `func_8006FC18` (fresh `q` recomputed from `idx`).

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_8006F9F0
  disc1_preflight: PASS (deep, 2050 c / 212 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_8006F9F0
  LEAF_STRONG=EXACT func_8006F9F0 span=0x228 vram=0x8006F9F0 compiled=0x230 failures=[none]
```

## Levers (ladder, fresh draft built from func_8006F8EC / func_8006FC18)

1. Positive `if (A || B) { body } return 0;` → 106 (the PARK's duplicated `li v0,4` in the first slot).
2. **Both range tests jump to one join label**: `if ((unsigned)(p[0]-1) < 2) goto body; if ((unsigned)(p[0]-4) < 2)
   goto body; return 0; body:` → 105 (layout now retail's: fall-through is the shared `return 0`).
3. **Zero-code barrier `asm("" : "=r"(p) : "0"(p));` at `body:`**: retail re-reads `p[0]` (`lbu $v1,0($s0)`) in the
   body; the barrier stops cse reusing the first load, and because the body now starts with a load, reorg fills the
   first test's slot with the second test's `addiu $v0,$v1,-4` (retail) instead of stealing `li $v0,4` → 9.
   (`st = *(volatile unsigned char *)p` gives the reload but adds an `andi 0xFF`: 93.)
4. **Handler call as `if (fn == 0) r = -1; else { r = fn(p); p[4]++; } return r;`**: keeps the `-1` arm in front of the
   call (retail `bnez` / `j ret` / `li $v0,-1`); the `if (…) return -1;` form lets jump optimization move the `-1`
   block to the end → 0.
