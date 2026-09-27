# ovl_0700 func_80191EFC

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x2F0C`, VRAM `0x80191EFC`, size `0xEC` (59 words)
* Source: `src/overlays/ovl_0700/func_80191EFC.c`
* Profile: `era_o2_g0`

## What it does / levers

Project pos[0..2] through D_8019CC30 (func_8006DFA8) and hand the screen coords to func_800868F0/func_80086A28. Same &D_800BCFA4 pointer-local lever as func_80191E30.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80191EFC.c 0x80191EFC 0xEC -O2 -G0
linked .text 240 bytes, target 0xec, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
