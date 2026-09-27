# ovl_0457 func_80124AA4

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x3DA4`, VRAM `0x80124AA4`, size `0x160` (88 words)
* Source: `src/overlays/ovl_0457/func_80124AA4.c`
* Profile: `era_o2_g0`

## What it does / levers

Per-frame image flip: LoadImage the current D_80125B88 frame into the back half, advance the D_80172CC4 frame counter when state 6 (-> state 0, or 7 after 31 frames), then DrawSync/VSync/PutDispEnv/PutDrawEnv on buffer D_8009CDDC and toggle it.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80124AA4.c 0x80124AA4 0x160 -O2 -G0
linked .text 352 bytes, target 0x160, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=ae119139ff3f4c37bdc63d469c59abd9a85691ca2b885adbdae430fb40eb9fbb yaml=b1d4fb922de32387b74730a18a567ea374ae103908fb5f78c7024179b25f1132 spans=[17 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
