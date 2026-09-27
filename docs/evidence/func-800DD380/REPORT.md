# func_800DD380 — matching C leaf

- VRAM `0x800DD380`, ROM offset `0XCDB80`, span `0x3EC`
- Source: `src/func_800DD380.c`; YAML row `[0XCDB80, c, func_800DD380]`
- Build profile: `era_o2_g0`

## Levers
FieldEng callback (a1[4] state machine: switch (a1[4]) { case 0: func_800783E4 lerp into pos; case 1: respawn with rand() % 400 jitter }, then the shared w=D_800E1204[idx] (+4) / func_800CEE20 tail). Levers: (1) array indexing a1[i] everywhere EXCEPT the case-1 respawn block, which needs byte-pointer casts `*(short *)((char *)a1 + n)` so the D_800E2234/36/38 loads stay below the preceding a1 stores (all-array: 119 words; all-cast: 193); (2) jitter spelled `x += func_80071A54() % 400 - 200;`; (3) `a1[5] = 0` written AFTER the a1[2] jitter (sched lifts it above that store, as retail); (4) the `>= 0xC` test value read into a fresh local u (declared last) BEFORE the `blk[0] = s0v | (s0v << 8) | ((r & 0xF) << 16)` store, so the load schedules above the stack store and slti reuses $a0 (reusing t costs 2 words, w 3).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_800DD380
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_800DD380
  -> LEAF_STRONG=EXACT func_800DD380 span=0x3EC failures=[none]
```

