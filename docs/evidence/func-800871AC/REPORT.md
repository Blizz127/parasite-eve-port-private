# func_800871AC — EXACT (lane exeD4, 2026-09-27)

VRAM `0x800871AC`, file `0x779AC`, size `0x268` (`asm/disc1/779AC.s`). Sound-bank upload. If the
`D_8009D270 & 1` "header pending" flag is set, it parses the bank header (size, first/last tone,
tone count), clamps the first chunk, picks the SPU mode (0x8000 / 0x38000 from `D_8009D2C8`
state), uploads the chunk, and copies the tone table into `D_800B2900`. Otherwise it continues the
upload by one clamped chunk.

- Profile: `era_o2_g0_cc1_281_unfill_epilogue_delay_slot` (existing): 2.8.1 `-O2 -G0 -mno-split-addresses`,
  `MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1`.
- YAML: `- [0x779AC, c, func_800871AC]`. The next row is `[0x77C14, c, func_80087414]`.

## Verification (private config build/lanes/exeD4/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD4/disc1.yaml --profiles build/lanes/exeD4/disc1_build_profiles.json --deep --only func_800871AC
  disc1_preflight: PASS (deep, 2089 c / 189 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD4/disc1.yaml --profiles build/lanes/exeD4/disc1_build_profiles.json func_800871AC
  LEAF_STRONG=EXACT func_800871AC span=0x268 vram=0x800871AC compiled=0x270 failures=[none]
era_link_check (ERA_CC1_VER=2.8.1 MASPSX_UNFILL_EPILOGUE_DELAY_SLOT=1, -O2 -G0 -mno-split-addresses):
  linked .text 624 bytes, target 0x268, word mismatches=0
```
profile_necessity.py has no `--config` (it reads configs/USA/disc1.yaml), so it was skipped per
the brief. The lead's gate covers it.

## Load-bearing levers (exeD best was 13 words)
1. The mode default `a1 = 0x8000` goes in the fallback path (just before `D_8009CDE8 &= ~0x100`),
   not before `if (n < 0x31)`. Reorg then steals it into both the `n<49` and `p->4 == 0` branch
   slots, which gives retail's second, redundant `ori $s2,$0,0x8000` at 0x800872A8. 13 → 12.
2. The chunk clamp is `if (a1 >= size) chunk = size; else chunk = a1;`, testing `a1` rather than the
   copy, so combine keeps retail's `addu $s1,$s2,$0` copy. The `chunk = a1; if (chunk >= size)`
   forms coalesce the copy (121–122 words); the `?:` form gives 12. 12 → 8.
3. `unsigned int t = a1 - 0x40; asm("" : "=r"(t) : "0"(t)); a1 = t - chunk;` stops fold from
   reassociating the expression to `a1 - (chunk + 64)`. 8 → 6.
4. The tone-table copy counter is `chunk` itself, which gives retail's `$s1` counter. The loop is a
   `goto` loop, so flow gives it loop-depth weight 1, and the continuation path uses a separate
   `c`. Together these put chunk's global-alloc priority (13 refs / 56 = 0.70) between a0 (1.03)
   and a1 (0.62), so a0/chunk/a1 get $s0/$s1/$s2 as in retail. A `do/while` counter over-weights
   chunk (1.29, or 1.14 with `c`) and swaps $s0/$s1 (31 words). The goto loop without `c` gives
   1.12, also 31 words. 6 → 0.
