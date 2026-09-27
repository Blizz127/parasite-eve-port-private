# ovl_03D2 func_80192C9C

* Overlay: `ovl_03D2` (PE.IMG, config `configs/USA/overlays/ovl_03D2.yaml`)
* Blob offset `0x3CAC`, VRAM `0x80192C9C`, size `0x4C`
* Source: `src/overlays/ovl_03D2/func_80192C9C.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_80192C9C.c 0x80192C9C 0x4C -O2 -G0
linked .text 80 bytes, target 0x4c, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03D2
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS ...
```

## Notes

Byte-for-byte twin of ovl_03C5 `func_801223A8` (target `D_801D0DC0`); same signed-char + goto levers.
