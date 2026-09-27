# ovl_0700 func_80190254

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x1264`, VRAM `0x80190254`, size `0x25C` (151 words)
* Source: `src/overlays/ovl_0700/func_80190254.c`
* Profile: `era_o2_g0_expand_div`

## What it does / levers

func_8018FFF4 side test extended with a distance gate: a side that fails the sign test still passes when |coord| / D_8019CBC8 (or D_8019CC00) is below (short)(n * 8 + 30). Levers: `a, b` declaration order + `b = 0; a = 0;` init order, `$5` pins for n/lim (keeps n in $a1, lim reuses it), a `$2` temp for n * 8 (lets reorg duplicate `sll v0,a1,3` into a delay slot) and for the second (short)lim; MASPSX_EXPAND_DIV for the div break guards.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu MASPSX_EXPAND_DIV=1 python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80190254.c 0x80190254 0x25C -O2 -G0
linked .text 608 bytes, target 0x25c, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
