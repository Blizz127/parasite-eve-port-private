# ovl_0700 func_80190998

* Overlay: `ovl_0700` (PE.IMG map-exit overlay, VRAM base 0x8018EFF0)
* Blob offset `0x19A8`, VRAM `0x80190998`, size `0x154` (85 words)
* Source: `src/overlays/ovl_0700/func_80190998.c`
* Profile: `era_o2_g0`

## What it does / levers

Initialise the record pool (199 records + root D_8019C340), the D_8019C830 free list, the three depth sentinels (D_8019CC58 <-> D_801EA5E8) and the D_801E4A88 link table. Sentinel store order (prev link first) is load-bearing (4 words).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80190998.c 0x80190998 0x154 -O2 -G0
linked .text 352 bytes, target 0x154, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```
