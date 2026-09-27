# ovl_03C5 func_80121004

* Overlay: `ovl_03C5` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x304`, VRAM `0x80121004`, size `0x26C`
* Source: `src/overlays/ovl_03C5/func_80121004.c`
* Profile: `ovl_03C5_three_word_symbol_store (local: -O2 -G0 + MASPSX_THREE_WORD_SYMBOL_STORE=1)`

## What it does

Movie display setup for buffer `buf`: SetDefDispEnv/SetDefDrawEnv at 480 (24-bit mode, display and clip width x2/3) or 320 wide, then the draw-env flags.

## Levers

`signed char` parameters (plain `char` is unsigned for this cc1, which gives `andi 0xFF`; 153 → 0). The indexed `D_800BCE91[i]`-style stores need the three-word symbol store.

## Verification (2026-09-24)

```
$ MASPSX_THREE_WORD_SYMBOL_STORE=1 LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C5 src/overlays/ovl_03C5/func_80121004.c 0x80121004 0x26C -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C5
disc1_preflight: PASS
$ scripts/exact_rebuild_overlay.sh ovl_03C5
EXACT_REBUILD_GATE[ovl_03C5]=PASS plan=66c9f23417d3c9afb1bb4bfcb77edc2c007f7520a82a07c8bc95834d7ee23a64 yaml=a7f3fc73341229adb2e746bd18ebd4c09937fd3070ccab50e5edd04e409b5819 spans=[4 c, 3 asm, 0 rodata, 2 bin] sha1_orig=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1_cand=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736
```
