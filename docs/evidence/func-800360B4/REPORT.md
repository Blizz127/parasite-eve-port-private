# func_800360B4

- VRAM: `0x800360B4`, size `0x140`
- Profile: `era_o2_g8`
- Source: `src/func_800360B4.c`

## Verification (lane-2 private config, $S = session scratchpad)

```
python3 tools/build/disc1_preflight.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json --deep --only func_800360B4
disc1_preflight: PASS (deep, 1664 c / 341 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config $S/lanes/lane2/disc1.yaml --profiles $S/lanes/lane2/disc1_build_profiles.json func_800360B4
LEAF_STRONG=EXACT func_800360B4 span=0x140 vram=0x800360B4 compiled=0x140 failures=[none]
```

## Load-bearing spelling levers

`-O2 -G8` (list heads/counters gp-relative); Actor struct (+4 next, +8 prev, +0x98 flags, +0x18C parent); flags old value kept in `f` for the `& 0x10` test; the free-list push is written in BOTH unlink branches (retail duplicates it); head branch order `n = a->next; t = D_8009D2AC; D_8009D2AC = a; D_8009D20C = n; n->prev = 0; a->next = t;` and the else branch uses a SEPARATE temp `u` (sharing `t` misallocates). Found by constrained permutation sweep.

Actor reaper: frees every active actor flagged 0x10 (or whose parent is), unlinks it and pushes it on the free list.
