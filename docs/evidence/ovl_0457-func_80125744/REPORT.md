# ovl_0457 func_80125744

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x4A44`, VRAM `0x80125744`, size `0x150` (84 words)
* Source: `src/overlays/ovl_0457/func_80125744.c`
* Profile: `era_o2_g0`

## What it does / levers

XA playback setup: wait for drive ready, CdControl(0xE setmode 0xC8) and CdControl(0xD setfilter {1,0}) with CdSync-style polling (retry on 0 / error), install the stream callback func_80125980 and VSync hook func_80125A40, start the fade (func_801258EC(200, 0x1B)), wait ready, return 0. Written with goto retries like func_8012562C.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80125744.c 0x80125744 0x150 -O2 -G0
linked .text 336 bytes, target 0x150, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=ae119139ff3f4c37bdc63d469c59abd9a85691ca2b885adbdae430fb40eb9fbb yaml=b1d4fb922de32387b74730a18a567ea374ae103908fb5f78c7024179b25f1132 spans=[17 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
