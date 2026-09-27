# ovl_03D2 func_8018F7F0

* Overlay: `ovl_03D2` (PE.IMG overlay, VRAM base 0x8018EFF0)
* Blob offset `0x800`, VRAM `0x8018F7F0`, size `0x168` (90 words)
* Source: `src/overlays/ovl_03D2/func_8018F7F0.c`
* Profile: `era_o2_g0`

## What it does / levers

Title sprite blend: for each row/4-pixel column of the task's image (t->img + 0x14, t->w*3/4 columns, t->h rows) scale source bytes by t->level (>>8) and keep the brighter of that and the destination (max blend); all-zero 4-byte groups are skipped. Levers: hand-unrolled 4x macro with src++ before the saved dst pointer and a '(v >= d) ? v : d' ternary (reproduces retail's extra register copy).

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_8018F7F0.c 0x8018F7F0 0x168 -O2 -G0
linked .text 368 bytes, target 0x168, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=7faee51d4307a4fe597eace0c84ce64b0d5e45b0b4ff45a79c17afaa3be49058 yaml=640100019e00c93eca0c3a0300b8e05f157e8ff5e2935813b29c7a3e8249463a spans=[11 c, 3 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32
```
