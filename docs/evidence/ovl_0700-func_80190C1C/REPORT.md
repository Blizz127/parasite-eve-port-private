# ovl_0700 func_80190C1C

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x1C2C`, VRAM `0x80190C1C`, size `0xEC` (59 words)
* Source: `src/overlays/ovl_0700/func_80190C1C.c`
* Profile: `era_o2_g0`

## What it does / levers

Allocate a pool record in the 7-argument layout (shorts 0..6, words 8/0x38/0x3C, f36 = func_8019959C(a6, a1)) and insert under parent.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80190C1C.c 0x80190C1C 0xEC -O2 -G0
linked .text 240 bytes, target 0xec, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
