# ovl_0457 func_80123E08

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x3108`, VRAM `0x80123E08`, size `0x184` (97 words)
* Source: `src/overlays/ovl_0457/func_80123E08.c`
* Profile: `era_o2_g0`

## What it does / levers

Credits init: display/graph reset, scroll state (D_80172CA4 = 0xE000, D_80172CA8 = -0x100), clear all of VRAM, parse the embedded TIM D_8012682C (pixel/CLUT pointers, TPage and CLUT ids), start the libpress decoder (func_8010BE3C/func_8010BD4C) on the stream buffer, enable display.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80123E08.c 0x80123E08 0x184 -O2 -G0
linked .text 400 bytes, target 0x184, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=430d9333800bb5169e195b37b9a495daf0b22ae99cc8b6b89e477d149125337e yaml=755cd582163a662ed4eb448370791af06d069d47998f7d42e1cc75c79a10aab1 spans=[18 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
