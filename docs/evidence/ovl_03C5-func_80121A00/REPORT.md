# ovl_03C5 func_80121A00

* Overlay: `ovl_03C5` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0xD00`, VRAM `0x80121A00`, size `0x204`
* Source: `src/overlays/ovl_03C5/func_80121A00.c`
* Profile: `era_o2_g0 (default)`

## What it does

Movie teardown. When `D_800B0DBA` is set it clears the stream state words, resets the display, copies both DISPENV/DRAWENV pairs back from `D_801227EC`/`D_80122814`, and moves the movie VRAM areas back (`func_8007512C`). The second move is skipped when `D_800B0CD8` bit 27 is set.

## Levers

First spelling matched. The loop index is `signed char`, as retail's `sll 24`/`sra 24` shows.

## Verification (2026-09-24)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C5 src/overlays/ovl_03C5/func_80121A00.c 0x80121A00 0x204 -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C5
disc1_preflight: PASS
$ scripts/exact_rebuild_overlay.sh ovl_03C5
EXACT_REBUILD_GATE[ovl_03C5]=PASS plan=66c9f23417d3c9afb1bb4bfcb77edc2c007f7520a82a07c8bc95834d7ee23a64 yaml=a7f3fc73341229adb2e746bd18ebd4c09937fd3070ccab50e5edd04e409b5819 spans=[4 c, 3 asm, 0 rodata, 2 bin] sha1_orig=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1_cand=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736
```
