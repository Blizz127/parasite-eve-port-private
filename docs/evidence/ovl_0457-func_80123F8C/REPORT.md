# ovl_0457 func_80123F8C

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x328C`, VRAM `0x80123F8C`, size `0xA8` (42 words)
* Source: `src/overlays/ovl_0457/func_80123F8C.c`
* Profile: `era_o2_g0`

## What it does / levers

Point the two stream windows D_80172CB4/B8 into D_80011610 by buffer parity D_8009CDDC and the PE.IMG range width (D_80093168 - D_80093166) sectors, then ClearOTag-style func_800752AC(D_80172CB4, 0x10). Both globals volatile (retail reloads CB4 after both stores); $2/$3/$4/$5 pins reproduce retail's in-place adds.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80123F8C.c 0x80123F8C 0xA8 -O2 -G0
linked .text 176 bytes, target 0xa8, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=61a6c3f99ef176fbd1ca962bed33a59aefff46d9d67c39e3f7e31226e72a09ce yaml=2654458b63c91567c140e6022b744f874a5ecc0d552e12f161d28fdfbce9b762 spans=[11 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
