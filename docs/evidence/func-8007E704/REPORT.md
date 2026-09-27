# func_8007E704

VRAM `0x8007E704`, size `0x1F0` (124 words), `asm/disc1/6EF04.s`.

Walks the 24-byte `D_800A3540` records from `D_800A3600` for `D_800A3608` steps, calls `func_8007EB88` when the record id changes, records nonzero `D_800A3550` callbacks on an 8-slot stack list, clears the three cursor words and the eight records, then `jalr`s the collected callbacks.

era `-O2 -G0` with `MASPSX_SYMBOL_LOAD_DEST_TEMP=1` and `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1`. Dest-temp alone is 2 words off; epilogue-fill alone is 6; neither is 8. Both together: `LINK_EXACT`, linked 496 bytes, `nonzero_pad=0`. `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x1F0 compiled=0x1F0`.

```
TMPDIR=build/tmp \
  MASPSX_SYMBOL_LOAD_DEST_TEMP=1 MASPSX_FILL_EPILOGUE_DELAY_SLOT=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_8007E704.c 0x8007E704 0x1F0 -O2 -G0
```
