# ovl_03C9 func_801F1B10

* Overlay: `ovl_03C9` (PE.IMG, config `configs/USA/overlays/ovl_03C9.yaml`)
* Blob offset `0x4318`, VRAM `0x801F1B10`, size `0x10`
* Source: `src/overlays/ovl_03C9/func_801F1B10.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C9 src/overlays/ovl_03C9/func_801F1B10.c 0x801F1B10 0x10 -O2 -G0
linked .text 16 bytes, target 0x10, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C9
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03C9
EXACT_REBUILD_GATE[ovl_03C9]=PASS ...
```

## Notes

`return &D_801F1F28;` Six byte-identical accessors sit back to back at 0x801F1B00..0x801F1B50 (table slots of the handler module).
