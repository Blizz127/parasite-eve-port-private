# ovl_0457 func_80124704

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x3A04`, VRAM `0x80124704`, size `0xF0` (60 words)
* Source: `src/overlays/ovl_0457/func_80124704.c`
* Profile: `era_o2_g0`

## What it does / levers

Queue the 96x24 credits-title sprite (DR_TPAGE + semi-transparent SPRT, grey level c) from the D_80172CB8 prim cursor into ot[11] of D_80172CB4, advancing the cursor by one 0x1C-byte pair.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80124704.c 0x80124704 0xF0 -O2 -G0
linked .text 240 bytes, target 0xf0, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=ac1617dd10c88d81d69d8a0607763e063cb89d62dd4f18fa5b28a342f2697b9b yaml=d660a5db8a6343962f6e0215afc19bc8e03f718f2733793b2178a6a5487d4ab2 spans=[14 c, 5 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
