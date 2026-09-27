# ovl_0700 func_80195D3C

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x6D4C`, VRAM `0x80195D3C`, size `0x110` (68 words)
* Source: `src/overlays/ovl_0700/func_80195D3C.c`
* Profile: `era_o2_g0`

## What it does / levers

While the D_8019C040 counter is >= 2: step it, advance D_8019C041, and sample two path points (func_8018F55C) into D_8019C810 / D_8019C330.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80195D3C.c 0x80195D3C 0x110 -O2 -G0
linked .text 272 bytes, target 0x110, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
