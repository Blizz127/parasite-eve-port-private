# ovl_0457 func_80125A40

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x4D40`, VRAM `0x80125A40`, size `0xB0` (44 words)
* Source: `src/overlays/ovl_0457/func_80125A40.c`
* Profile: `era_o2_g0`

## What it does / levers

Every 32nd VSync: stop (func_800870F0) when the D_80172CF0 counter passes D_80172CEC, fade out (func_801258EC) when it trails D_80172CE8-300, else schedule func_80125AF0. Pointer-to-volatile keeps the counter base in $a0 with two loads; '*p > CEC' operand order is load-bearing.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80125A40.c 0x80125A40 0xB0 -O2 -G0
linked .text 176 bytes, target 0xb0, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=61a6c3f99ef176fbd1ca962bed33a59aefff46d9d67c39e3f7e31226e72a09ce yaml=2654458b63c91567c140e6022b744f874a5ecc0d552e12f161d28fdfbce9b762 spans=[11 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
