# func_800562A4 — matching C leaf (lane exeC4, 2026-09-27)

- VRAM `0x800562A4`, file offset `0x46AA4`, size `0x880` (544 words); source `src/func_800562A4.c`
- YAML row `- [0x46AA4, c, func_800562A4]` (replaces `[0x46AA4, asm]`; next row `[0x47324, c, func_80056B24]`)
- Profile `era_o2_g8` (`-O2 -G8`), cc1 2.7.2. The exeC3 draft used
  `era_o2_g8_force_descriptor_absolute`; the force knob is not load-bearing here (plain `-O2 -G8`
  is LINK_EXACT and LEAF_STRONG=EXACT), so the minimal profile is recorded.
- Role: party/equipment group bitmap builder: two passes (normal / alternate descriptor set via
  `sel()`), per-entry group compare into the `D_8009D058` bit words, popcount into `n`.

## Verification (lane private config build/lanes/exeC4/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC4/disc1.yaml --profiles build/lanes/exeC4/disc1_build_profiles.json --deep --only func_800562A4
  -> disc1_preflight: PASS (deep, 2093 c / 185 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_800562A4
  -> LEAF_STRONG=EXACT func_800562A4 span=0x880 vram=0x800562A4 compiled=0x880 failures=[none]
python3 tools/analysis/era_link_check.py src/func_800562A4.c 0x800562A4 0x880 -O2 -G8   -> word mismatches=0 / LINK_EXACT
python3 tools/analysis/era_link_check.py src/func_800562A4.c 0x800562A4 0x880 -O2 -G0   -> 527 mismatches (default profile misses)
```

## Levers on top of the exeC3 draft (22 raw → 0; all global-alloc priority levers)
Diagnosis via `cc1 -dl` ("Register N used R times across L insns"; priority ≈ floor_log2(R)·R/L).
1. **Empty asm use `asm("" : : "r"(sh));` at the join of the first-style loops** (after the
   `if/else` that builds `v`, before `*w = v`): adds refs to `sh` so it outranks `w` → retail's
   `sh`=a2 / `w`=a3 (22 → 12). Placed after the store it does nothing; placed at `sh`'s definition
   it pins the `andi` early.
2. **Separate `one` / `one2` pseudos for the two passes** plus asm uses of each in its pass's
   first-style loop (`asm("" : : "r"(sh), "r"(one));` / `(..., "r"(one2))`): the extra refs keep
   `one` ahead of `t` (s3/s4) and `one2` ahead of `n`, so `one2` reuses the dead `idx` home s2 and
   `n` lands in s5 (12 → 9 → 5 regs swapped).
3. **`asm("" : : "r"(was));` immediately before `sel(!was)`**: `was` outranks `n` → `was`=s3 (reuses
   pass-1 `one`), `n`=s5 (9 → 0). Before `sel(was)` it blocks the `beqz` delay-slot fill of
   `n += ...`; after `was = ...` it perturbs scheduling.
