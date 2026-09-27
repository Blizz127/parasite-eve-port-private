# func_80068710 — EXACT (lane exeC2, 2026-09-27)

- VRAM `0x80068710`, file offset `0x58F10`, size `0x484` (289 words), `asm/disc1/58F10.s`.
- Source `src/func_80068710.c`; yaml rows `- [0x58F10, c, func_80068710]` + `- [0x59394, asm]`.
- Profile `era_o2_g8` (`-O2 -G8`).

## Verification (private lane config build/lanes/exeC2/)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC2/disc1.yaml --profiles build/lanes/exeC2/disc1_build_profiles.json --deep --only func_80068710
  -> disc1_preflight: PASS (deep, 2081 c / 192 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC2/disc1.yaml --profiles build/lanes/exeC2/disc1_build_profiles.json func_80068710
  -> LEAF_STRONG=EXACT func_80068710 span=0x484 vram=0x80068710 compiled=0x490 failures=[none]
era_link_check.py src/func_80068710.c 0x80068710 0x484 -O2 -G8  -> LINK_EXACT
profile necessity (manual; profile_necessity.py has no --config): -O2 -G0 (default) -> 237 mismatches
```

## Lever: a duplicated return tail that jump2 cross-jumps back into one

The decompB/exeC draft (35 words) was a pure caller-saved permutation: retail best/found/more =
t2/t3/t4, ours t4/t2/t3. `cc1 -dl` shows why: global-alloc priority
`floor_log2(refs)*refs/live_length` gave best 3*15/191 = 0.236, below found (0.30) and more
(0.267). **Fix:** give the wrap/`dir` branch its own copy of the store tail
(`prev = D_8009CDBC; D_8009CDBC = best; ...; return best;`) instead of `goto done`. That adds
refs to `best` and shortens its live range, so `best` now gets allocated first (t2). jump2
cross-jumps the two identical tails after register allocation, so the emitted code is the
single shared tail again: 35 -> 0. Copying the tail into the `found == 1` branch instead also
gives LINK_EXACT; copying it into both gives 23.
