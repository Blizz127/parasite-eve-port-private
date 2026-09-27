# ovl_0700 func_80191C94

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x2CA4`, VRAM `0x80191C94`, size `0x154` (85 words)
* Source: `src/overlays/ovl_0700/func_80191C94.c`
* Profile: `era_o2_g0`

## What it does / levers

Load PE.IMG ranges 16 (-> D_801D0260) and 18 or 17 (-> D_8019CE10, by D_8019C1F0) with retry-on-error polls. A base pointer local for D_800B0CD8 (LBA word at +0x100) keeps it in $s2.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80191C94.c 0x80191C94 0x154 -O2 -G0
linked .text 352 bytes, target 0x154, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
