# func_800D5EB4 — EXACT (lane exeD5, 2026-09-27)

VRAM `0x800D5EB4`, file `0xC66B4`, size `0x3E8` (`asm/disc1/C66B4.s`). FieldEng effect template
(switch on a0: case 0 init, case 1 particle update, case 2 buf/vec fill + func_800CEE20). Supersedes
`PARK.md` (3-word residual, lanes exeD and exeD3).

- Source: `src/func_800D5EB4.c` (copy: `build/lanes/exeD5/drafts/func_800D5EB4.final.c`)
- yaml: `- [0xC66B4, c, func_800D5EB4]` (next row `[0xC6A9C, c, func_800D629C]` already exists)
- Profile: default `era_o2_g0` (2.7.2 `-O2 -G0`), no assignment needed.

## Verification (private config build/lanes/exeD5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD5/disc1.yaml --profiles build/lanes/exeD5/disc1_build_profiles.json --deep --only func_800D5EB4
  -> disc1_preflight: PASS (deep, 2103 c / 177 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD5/disc1.yaml --profiles build/lanes/exeD5/disc1_build_profiles.json func_800D5EB4
  -> LEAF_STRONG=EXACT func_800D5EB4 span=0x3E8 vram=0x800D5EB4 compiled=0x3F0 failures=[none]
```

## Lever: a single-set variable for the case-2 `0x2400`
The old draft reused `s2v` for both the case-1 random scale `(rand & 7) + 0x10` and the case-2 constant `0x2400`.
With two sets the pseudo has `reg_n_sets == 2`. sched1's register-pressure rule (`birthing_insn_p`, which counts
only single-set pseudos that are live) then no longer treats the `li 0x2400` as a birth, and the join block comes
out as `li 0x2400; li -512; sra s0,…`. reorg then steals `li $s2` into the bgez slot and moves `sra` into the bne slot.
With a fresh `s2b = 0x2400` (set once), sched1 gives retail's `li -512; sra; …; li 0x2400`. reorg then puts `li -512`
into the bgez slot and `li $s2,0x2400` into the bne slot.
Ladder: shared `s2v` 3 words. Separate variable 0. Renaming the case-1 use instead (s2v kept for 0x2400,
case 1 uses another var) 28. The 16 statement placements tried earlier (exeD3) were all 3, because a statement move
cannot change n_sets.
