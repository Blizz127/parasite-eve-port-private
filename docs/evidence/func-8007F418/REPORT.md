# func_8007F418

VRAM `0x8007F418`, size `0x1F0` (124 words), inside `asm/disc1/6F684.s`.

era `-O2 -G0` with `MASPSX_SYMBOL_LOAD_DEST_TEMP=1` and `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`. Each knob is load-bearing: dest-temp alone is 2 words off, epilogue-fill alone is 21, neither is 23. Both together: `LINK_EXACT`, linked 496 bytes, `nonzero_pad=0`. `leaf_strong_check.py`: `compiled=0x1F0`.

```
TMPDIR=build/tmp \
  MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_8007F418.c 0x8007F418 0x1F0 -O2 -G0
```
