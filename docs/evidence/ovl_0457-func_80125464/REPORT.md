# ovl_0457 func_80125464

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x4764`, VRAM `0x80125464`, size `0x140` (80 words)
* Source: `src/overlays/ovl_0457/func_80125464.c`
* Profile: `era_o2_g0`

## What it does / levers

Additive glow over the credits image: for rows 0x6E..0x85, each 16-bit mask word selects per-pixel intensity nibbles (D_80172C9C table, & 0x1F << 3) added to RGB24 pixels with 0xFF clamps. Levers: load r/g/b into locals before the add, 'r, b, g' declaration order, $14/$13 pins for the row/line bases.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80125464.c 0x80125464 0x140 -O2 -G0
linked .text 320 bytes, target 0x140, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=ac1617dd10c88d81d69d8a0607763e063cb89d62dd4f18fa5b28a342f2697b9b yaml=d660a5db8a6343962f6e0215afc19bc8e03f718f2733793b2178a6a5487d4ab2 spans=[14 c, 5 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
