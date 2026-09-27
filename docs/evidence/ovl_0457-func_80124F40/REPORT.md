# ovl_0457 func_80124F40

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x4240`, VRAM `0x80124F40`, size `0x154` (85 words)
* Source: `src/overlays/ovl_0457/func_80124F40.c`
* Profile: `era_o2_g0`

## What it does / levers

Set up display/draw env pair i (D_800BCE80 / D_800BCDC8): 320x224 display at y 0 or 224, RGB24 on, screen y 8 / h 224; 480x224 draw env at the other half, clip width scaled by 2/3, dtd/isbg cleared (includes retail's disp.w self-assignment).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80124F40.c 0x80124F40 0x154 -O2 -G0
linked .text 352 bytes, target 0x154, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=ae119139ff3f4c37bdc63d469c59abd9a85691ca2b885adbdae430fb40eb9fbb yaml=b1d4fb922de32387b74730a18a567ea374ae103908fb5f78c7024179b25f1132 spans=[17 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
