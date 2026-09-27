# ovl_0700 func_80191740

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x2750`, VRAM `0x80191740`, size `0x14` (5 words)
* Source: `src/overlays/ovl_0700/func_80191740.c`
* Profile: `era_o2_g0`

## What it does / levers

Retail: addiu $v0,$zero,0xC8 / lui $at,%hi(D_8019CC50) / sh $v0,%lo(D_8019CC50)($at) / jr $ra / nop  A 16-bit store of the constant 200 into the overlay's own data word at 0x8019CC50 (blob offset 0xDC60, inside the CFD4.bin data tail); the symbol is address-named and binds to that address at link time like the EXE lane's D_ externs. Era profile: default era_o2_g0 (overlays use no $gp data).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80191740.c 0x80191740 0x14 -O2 -G0
linked .text 32 bytes, target 0x14, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
