# ovl_0700 func_80192740

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x3750`, VRAM `0x80192740`, size `0xC0` (48 words)
* Source: `src/overlays/ovl_0700/func_80192740.c`
* Profile: `era_o2_g0`

## What it does / levers

Link the double-buffered 0x24-byte prim into ot[0xFFF] (PSY-Q addPrim via P_TAG bitfields) when D_8019C1F0 == 1, then step D_8019C02C through func_80193B5C. The != ternary order is load-bearing (3 words).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80192740.c 0x80192740 0xC0 -O2 -G0
linked .text 192 bytes, target 0xc0, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
