# ovl_0700 func_8019BD78

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0xCD88`, VRAM `0x8019BD78`, size `0x1D8` (118 words)
* Source: `src/overlays/ovl_0700/func_8019BD78.c`
* Profile: `era_o2_g0`

## What it does / levers

Graphics init: geometry/fog/back colour, the two-buffer DB at D_8019C1F8 (ClearOTag, draw/disp envs, isbg/rgb cleared), clear both 320x240 halves of VRAM, enable display, D_8019C9C0 = &db[0].

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_8019BD78.c 0x8019BD78 0x1D8 -O2 -G0
linked .text 480 bytes, target 0x1d8, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
