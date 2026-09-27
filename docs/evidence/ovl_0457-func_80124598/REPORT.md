# ovl_0457 func_80124598

* Overlay: `ovl_0457` (PE.IMG, config `configs/USA/overlays/ovl_0457.yaml`)
* Blob offset `0x3898`, VRAM `0x80124598`, size `0x94`
* Source: `src/overlays/ovl_0457/func_80124598.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80124598.c 0x80124598 0x94 -O2 -G0
linked .text 160 bytes, target 0x94, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS ...
```

## Notes

Unsigned-short x/y and unsigned-char cell arguments give retail's `andi` masks; the cursor global is a byte pointer advanced by 0x1C.
