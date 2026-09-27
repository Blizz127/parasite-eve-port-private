# ovl_03C9 func_801F1B60

* Overlay: `ovl_03C9` (PE.IMG, config `configs/USA/overlays/ovl_03C9.yaml`)
* Blob offset `0x4368`, VRAM `0x801F1B60`, size `0x38`
* Source: `src/overlays/ovl_03C9/func_801F1B60.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C9 src/overlays/ovl_03C9/func_801F1B60.c 0x801F1B60 0x38 -O2 -G0
linked .text 64 bytes, target 0x38, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C9
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03C9
EXACT_REBUILD_GATE[ovl_03C9]=PASS ...
```

## Notes

Straightforward nested if; no lever needed.
