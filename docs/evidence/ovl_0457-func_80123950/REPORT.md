# ovl_0457 func_80123950

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x2C50`, VRAM `0x80123950`, size `0x1CC` (115 words)
* Source: `src/overlays/ovl_0457/func_80123950.c`
* Profile: `era_o2_g0`

## What it does / levers

Credits text renderer: walk the D_80125C88 line table (cursor D_80172CB0) from scroll y = D_80172CA4 >> 8, 0x12 px per line, drawing visible lines (y in [-7, 0xE0]) glyph by glyph through D_80172CD0->draw; '\\' centres the rest of the line, TAB advances 14 spaces, ' ' just advances; 0xFF ends. Returns 1 when no line was visible; advances the scroll by D_80172CA8. Levers: a switch (GCC's decision tree gives retail's == 0x20 / slti 0x21 / 9 / 0x5C order) with case '\\' first, unsigned char c, and p[1] indexing in the width loop.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80123950.c 0x80123950 0x1CC -O2 -G0
linked .text 464 bytes, target 0x1cc, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=bad90d75793b08b338441e79d57bb56b8f076d9396432d8053a09caf9a531c93 yaml=5708fec9437def1204a81faea63021cbf3bc5385066eaf6593d245d46b17c330 spans=[19 c, 7 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
