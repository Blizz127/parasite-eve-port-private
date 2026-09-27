# ovl_0457 func_801247F4

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x3AF4`, VRAM `0x801247F4`, size `0xA8` (42 words)
* Source: `src/overlays/ovl_0457/func_801247F4.c`
* Profile: `era_o2_g0`

## What it does / levers

Frame flip: DrawSync(0), VSync(2), SetDispMask(1), then draw/put the D_8009CDDC buffer's env pair (D_800BCE80 / D_800BCDC8 rows) and toggle D_8009CDDC.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_801247F4.c 0x801247F4 0xA8 -O2 -G0
linked .text 176 bytes, target 0xa8, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=92e5ad98ea1258e25fc8ce6d467d41c6cbc8d326902ccca0ba97e601eeaeef22 yaml=e423552e71bb5d27621862f9ff8831917b8fc4710bdf0b702d9dcf7728767d72 spans=[8 c, 7 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
