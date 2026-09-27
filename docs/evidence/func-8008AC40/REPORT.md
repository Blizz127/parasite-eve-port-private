# func_8008AC40

VRAM `0x8008AC40`, size `0x254` (149 words), `asm/disc1/7B440.s`.

Copies `D_8009D2C8` into `D_800B8968` (0x68 bytes) and `D_800B8AC0` into `D_800B6B80` (0x1AA0 bytes), then walks 24 bit slots of the word at `D_8009D2C8+4`. A set bit adds `arg0 - D_800B8994` through the `0x11C`-stride record and ORs `0x1FF93` into `+0x9C`. A clear bit stores 2/4 and `&D_8009B8F4`.

era `-O2 -G0` (default profile, no assignment). `-O1 -G0` is 143 words off. `LINK_EXACT`, linked 608 bytes, `nonzero_pad=0` (three trailing zero words). `leaf_strong_check.py`: `LEAF_STRONG=EXACT span=0x254 compiled=0x260`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py \
  src/func_8008AC40.c 0x8008AC40 0x254 -O2 -G0
```
