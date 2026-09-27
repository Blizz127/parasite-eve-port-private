# ovl_0457 func_801235DC

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x28DC`, VRAM `0x801235DC`, size `0x374`
* Source: `src/overlays/ovl_0457/func_801235DC.c`
* Profile: `ovl_0457_dispatch_80122FA4 (local)`

## What it does

Credits main loop. It picks the credit op table (`D_80125B68` or `D_80125B4C` by `D_8009D1A0` bit 16), starts the stream, then loops update/fade/draw. The fade runs as a 5-state machine (`switch` on `jtbl_80122FA4`) until `D_80172CE0` is set, then calls the end hook and clears bit 16.

## Levers

First spelling matched. The 5-entry table is in the `[0x22A4, rodata]` pool; its trailing `.align 3` zero word is excluded from the pool count.

## Verification (2026-09-24)

```
$ MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80122FA4 LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_801235DC.c 0x801235DC 0x374 -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=642d987efd66c8a0cb71eb6ee6e68bdcd47ef9fb1e7d81b6b3d4bc7c556c4187 yaml=7a3cf291011ce810539a0e9bd9d0d1d1c8f325426d46b8db1ea7187b08c6203e spans=[24 c, 3 asm, 1 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
