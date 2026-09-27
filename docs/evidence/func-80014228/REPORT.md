# func_80014228

- VRAM `0x80014228`, file offset `0x4A28`, size `0x188` (98 words)
- Profile: `era_o2_g0_no_rerun_cse_after_loop`; maspsx env: none; flags -O2 -G0 -fno-rerun-cse-after-loop (new profile)
- Source: `src/func_80014228.c`

## Verification (lane-1 private config, session 2026-09-22)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json --deep --only func_80014228
disc1_preflight: PASS (deep, ...)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane1/disc1.yaml --profiles $S/lanes/lane1/disc1_build_profiles.json func_80014228
LEAF_STRONG=EXACT func_80014228 span=0x188 vram=0x80014228
```

## Load-bearing levers

**Flag lever, not a spelling:** retail re-reads `*arg0->f4` in the list-walk preheader (loop.c hoist) instead of reusing the first read. With default `-O2`, the post-loop CSE rerun merges the two (70-word miss); `-fno-rerun-cse-after-loop` (or equally `-fno-cse-skip-blocks`) reproduces retail exactly. Four source spellings (nested if, inverted if, else-for, local key) did not avoid the merge. The walk must be a real `for` loop with `break` (a goto loop is not hoisted). Operand order `*arg0->f4 == p->f0C` gives retail's `bne v1,v0`. The integrator should confirm the flag is acceptable (e.g. with profile_necessity.py).
