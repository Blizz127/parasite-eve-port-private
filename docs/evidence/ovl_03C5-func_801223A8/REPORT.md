# ovl_03C5 func_801223A8

* Overlay: `ovl_03C5` (PE.IMG, config `configs/USA/overlays/ovl_03C5.yaml`)
* Blob offset `0x16A8`, VRAM `0x801223A8`, size `0x4C`
* Source: `src/overlays/ovl_03C5/func_801223A8.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C5 src/overlays/ovl_03C5/func_801223A8.c 0x801223A8 0x4C -O2 -G0
linked .text 80 bytes, target 0x4c, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C5
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03C5
EXACT_REBUILD_GATE[ovl_03C5]=PASS ...
```

## Notes

`signed char` parameter gives the bare `sll 24` truth test (`char`/unsigned emits `andi 0xFF`). The shared store must be reached by a `goto` from the a0 != 0 arm: the if/else, early-return and ternary forms all invert the first inner branch (2 words).
