# ovl_03D2 func_80193200

* Overlay: `ovl_03D2` (PE.IMG, config `configs/USA/overlays/ovl_03D2.yaml`)
* Blob offset `0x4210`, VRAM `0x80193200`, size `0x54`
* Source: `src/overlays/ovl_03D2/func_80193200.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_80193200.c 0x80193200 0x54 -O2 -G0
linked .text 96 bytes, target 0x54, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03D2
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS ...
```

## Notes

Compound `-=` then two separate field tests; first try.
