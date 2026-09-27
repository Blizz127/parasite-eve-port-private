# ovl_0457 func_80125094

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x4394`, VRAM `0x80125094`, size `0x3B0`
* Source: `src/overlays/ovl_0457/func_80125094.c`
* Profile: `ovl_0457_dispatch_801235BC (local)`

## What it does

Credits page step. It blits the current 480x224 page (`D_80125B84[D_80172CBC]`), handles pad left/right page selection, then advances the XA/stream state machine `D_80172CCC` (states 0..5; the 8-entry table's 6 and 7 are no-ops).

## Levers

* `(char *)D_80011610 + (off + K)` keeps retail's add-offset-then-base association (32 → 0).
* `case 6: case 7: break;` gives the 8-entry bound (`sltiu 8`).
* `RECT` fields in x, y, w, h order (44 → 32).

## Verification (2026-09-24)

```
$ MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_801235BC LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80125094.c 0x80125094 0x3B0 -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=642d987efd66c8a0cb71eb6ee6e68bdcd47ef9fb1e7d81b6b3d4bc7c556c4187 yaml=7a3cf291011ce810539a0e9bd9d0d1d1c8f325426d46b8db1ea7187b08c6203e spans=[24 c, 3 asm, 1 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
