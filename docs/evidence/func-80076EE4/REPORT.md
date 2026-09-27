# func_80076EE4

VRAM `0x80076EE4`, size `0x260` (152 words), inside `asm/disc1/67434.s`.

Returns 1 when `*D_80095860` has bit `0x01000000` set. Otherwise it calls `func_80073E10(0)`, stores that into `D_80095880`, and while the GPU queue indices differ and bit `0x01000000` stays clear it advances `D_80095878`, spinning until `*D_80095854` has bit `0x04000000`, then calls the stride-96 handler in `D_800BD030`. After the drain it calls `func_80073E10(D_80095880)` and, when the indices match and the busy bit is clear, invokes the word at `D_80095754+4` after clearing `D_80095754`. The return value is `(D_80095874 - D_80095878) & 0x3F`. The `addiu $sp,0x20` sits in the `jr` delay.

cc1 2.8.1, `-O2 -G0 -mno-split-addresses`, `MASPSX_FILL_BRANCH_MACRO_SPLIT=1`, `MASPSX_SYMBOL_LOAD_DEST_TEMP=1`. Dropping the split knob is 135 words; dropping dest-temp is 9 words; dropping `-mno-split-addresses` is 144 words. `MASPSX_FILL_EPILOGUE_DELAY_SLOT` is not required. `LINK_EXACT`, linked 608 bytes equals `0x260`, `nonzero_pad=0`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x260 compiled=0x260`.

```
TMPDIR=build/tmp ERA_CC1_VER=2.8.1 \
  MASPSX_FILL_BRANCH_MACRO_SPLIT=1 MASPSX_SYMBOL_LOAD_DEST_TEMP=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_80076EE4.c 0x80076EE4 0x260 -O2 -G0 -mno-split-addresses
```
