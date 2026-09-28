# func_80071A84 — EXACT (lane exeJ, 2026-09-28)

- VRAM `0x80071A84`, file offset `0x62284`, size `0x884` (545 words). Psy-Q libc `sprintf`-style formatter with the
  jump table `jtbl_80011644`. Source: `src/func_80071A84.c`.
- YAML carve (inside the old `- [0x621E4, asm]` row):
  `- [0x621E4, asm]` / `- [0x62284, c, func_80071A84]` / `- [0x62B08, asm]` / `- [0x62B34, c, func_80072334]`.
- Profile `era_o2_g0_cc1_272cdk_hard_float_three_word_symbol_store_fill_epilogue_delay_slot_dispatch_fold_cdk_split_dispatch`:
  `ERA_CC1_VER=2.7.2-cdk`, flags `-O2 -G0 -mhard-float`, env `MASPSX_THREE_WORD_SYMBOL_STORE=1`,
  `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`, `MASPSX_DISPATCH_FOLD=jtbl_80011644`, `MASPSX_CDK_SPLIT_DISPATCH=1`.
  **This is the first leaf on the gcc-2.7.2-cdk cc1.** `scripts/setup_era.sh` does not fetch it yet (only
  `fetch_gcc 2.7.2` / `fetch_gcc 2.8.1`). A clean checkout needs `fetch_gcc 2.7.2-cdk`, which uses the same old-gcc
  release naming as the local `tools/era/gcc-2.7.2-cdk-psx`.

## New maspsx knob K-CDKJT, landed: `MASPSX_CDK_SPLIT_DISPATCH=1`
Spec and prototype are from lane exeI (`build/lanes/exeI/KNOB_REQUESTS.md`). cdk's split-address switch dispatch
(`beq; lui $t,%hi($Ln)` in the slot, then `addiu $t,$t,%lo($Ln); sll $x,$x,2; addu $x,$x,$t; lw $t,0($x); j $t`) is
rewritten to stock 2.7.2's `sll $t,$x,2` in the slot plus `lw $t,$Ln($t)`. The existing indexed-symbol path and
DISPATCH_FOLD then emit retail's `lui $at / addu $at,$at,$v0 / lw $v0,%lo(jtbl)($at)`.
- `_cdk_split_dispatch` in `tools/era/maspsx/maspsx/__init__.py`. It is called first in `process_lines`, only when
  the env var is `"1"`, so flag-off output is byte-identical by construction.
- Test: `tools/era/maspsx/tests/test_cdk_split_dispatch.py` (5 tests, including gate-off == unset). It is registered
  in `scripts/setup_era.sh` `MASPSX_TRACKED` and in the `.gitignore` negation list.
- Suite: `cd tools/era/maspsx && python3 -m unittest discover -s tests -t .` gives `Ran 551 tests ... OK`.

## Verify (private config build/lanes/exeJ/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeJ/disc1.yaml --profiles build/lanes/exeJ/disc1_build_profiles.json --deep --only func_80071A84
  -> dispatch dedup: stripped 184-byte duplicate table ... ; disc1_preflight: PASS (deep, 2146 c / 147 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeJ/disc1.yaml --profiles build/lanes/exeJ/disc1_build_profiles.json func_80071A84
  -> LEAF_STRONG=EXACT func_80071A84 span=0x884 vram=0x80071A84 compiled=0x890 failures=[none]
```
`profile_necessity.py` has no `--config` option: it reads the main YAML and rejects the private manifest. It was
skipped as the brief allows. The manual ladder below uses `era_link_check.py src/func_80071A84.c 0x80071A84 0x884`:

| profile variant | word mismatches |
|---|---|
| **full** | **0** |
| without CDK_SPLIT_DISPATCH | 378 |
| without DISPATCH_FOLD / THREE_WORD / FILL_EPILOGUE (each) | 2 |
| soft-float (no `-mhard-float`) | 428 |
| stock 2.7.2 instead of cdk | 461 |
| default era_o2_g0 | 494 |

## Source levers (on top of exeI's best draft, 12 → 0)
1. **`register char *digits asm("$7");`**. In the `'X'`/`'x'` cases cdk splits `la` into a `(high)` pseudo and a
   `lo_sum`. `digits` is a global pseudo, so local-alloc gave the high part `$v0`. With `digits` as a hard-register
   variable, the dying high pseudo gets the `$7` suggestion. That gives retail's `lui a3 / j / addiu a3,a3` and
   `lui a3 / addiu a3,a3` (12 → 8).
2. **Template copy through a pinned, opaque pointer**:
   `register struct Spec *tp asm("$5") = &D_80094528; __asm__("" : "=r"(tp) : "0"(tp)); ctx.s = *tp;`.
   The source address becomes a plain register (`$a1`, the high part tied into it) instead of a `lo_sum` memory
   operand. The movstrsi scratches are then `v0/v1/a0`, which is retail's `lui a1 / addiu a1,a1 / lw v0,v1,a0`
   (8 → 6). Without the pin (plain local + barrier) the count is 14. Without the barrier, cse folds `*tp` back
   (8, unchanged).
3. **Three empty `__asm__ volatile("");` in case `'p'`** (exeI's one, plus two). This is the known global-alloc
   priority truncation between the `'-'`/`'+'` flag constants: each empty asm adds 2 to the loop length L, and
   priority is `(int)(120000/L)`. After levers 1–2, one or two asms tie (s6/s7 swapped, 6 words), three give
   retail's s7='-'/s6='+' (0), four swap again. **Fragile**: any insn-count change in this function can flip it.
4. From exeI/exeE4, kept: cdk cc1, `-mhard-float`, the four va_arg barriers, the global `fl` for `'n'`/`h`, the
   struct-ctx `ap` at 0x220, and `signed char *fmt`.

Prior PARK history: `docs/evidence/func-80071A84/PARK.md`.
