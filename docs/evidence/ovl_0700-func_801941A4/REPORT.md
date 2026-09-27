# ovl_0700 func_801941A4

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x51B4`, VRAM `0x801941A4`, size `0x158` (86 words)
* Source: `src/overlays/ovl_0700/func_801941A4.c`
* Profile: `era_o2_g0`

## What it does / levers

Full-screen semi-transparent grey POLY_G4 (colour c) plus a DR_TPAGE (tpage 0x40), both addPrim'd into ot[0] of the current D_8019C9C0 buffer, advancing primptr.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_801941A4.c 0x801941A4 0x158 -O2 -G0
linked .text 352 bytes, target 0x158, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
