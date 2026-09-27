# ovl_0700 func_8018FFF4

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x1004`, VRAM `0x8018FFF4`, size `0x130` (76 words)
* Source: `src/overlays/ovl_0700/func_8018FFF4.c`
* Profile: `era_o2_g0`

## What it does / levers

Side test of an int XYZ point against two rows of the D_8019CBB0 matrix (+D_8019CB48/4C); returns 1 when both signs agree with D_8019CC04 / D_8019CC0C. Declaring 'int b, a' (not 'a, b') is load-bearing: it decides the $a1/$a2 homes (4 words).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_8018FFF4.c 0x8018FFF4 0x130 -O2 -G0
linked .text 304 bytes, target 0x130, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
