# ovl_0700 func_80191E30

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x2E40`, VRAM `0x80191E30`, size `0xCC` (51 words)
* Source: `src/overlays/ovl_0700/func_80191E30.c`
* Profile: `era_o2_g0`

## What it does / levers

Push matrix, install D_8019CC30 + a local 0x300 config in the D_800BCFA4/A8 globals around func_8006DF50, restore, pop. A pointer local for &D_800BCFA4 is load-bearing (keeps it in $s1; frame size).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80191E30.c 0x80191E30 0xCC -O2 -G0
linked .text 208 bytes, target 0xcc, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
