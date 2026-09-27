# ovl_0457 func_8012562C

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x492C`, VRAM `0x8012562C`, size `0x118` (70 words)
* Source: `src/overlays/ovl_0457/func_8012562C.c`
* Profile: `era_o2_g0`

## What it does / levers

XA stream start: CdInit, wait for a ready drive (retry search on 0/-1), CdSearchFile(D_80125B48), record start/end sector (D_80172CE8/CEC), reset the stream counters and point the ring D_80172D00 into D_80011610. Loop written with goto (keeps li s0,1 per retry); D_80172CE8 through a pointer-to-volatile; CEC/CF0/CF4/CF8/CFA volatile, D00 not (load-bearing store order).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_8012562C.c 0x8012562C 0x118 -O2 -G0
linked .text 288 bytes, target 0x118, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=ac1617dd10c88d81d69d8a0607763e063cb89d62dd4f18fa5b28a342f2697b9b yaml=d660a5db8a6343962f6e0215afc19bc8e03f718f2733793b2178a6a5487d4ab2 spans=[14 c, 5 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
