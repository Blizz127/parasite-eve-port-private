# ovl_0457 func_80125AF0

* Overlay: `ovl_0457` (PE.IMG, config `configs/USA/overlays/ovl_0457.yaml`)
* Blob offset `0x4DF0`, VRAM `0x80125AF0`, size `0x58`
* Source: `src/overlays/ovl_0457/func_80125AF0.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80125AF0.c 0x80125AF0 0x58 -O2 -G0
linked .text 96 bytes, target 0x58, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS ...
```

## Notes

`D_80172CFC` must be `volatile`: retail reloads it for the test and again for the copy (without volatile cc1 reuses `$v0` and the object is 2 words shorter).
