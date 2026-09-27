# ovl_03D2 func_80192F98

* Overlay: `ovl_03D2` (PE.IMG, config `configs/USA/overlays/ovl_03D2.yaml`)
* Blob offset `0x3FA8`, VRAM `0x80192F98`, size `0x50`
* Source: `src/overlays/ovl_03D2/func_80192F98.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_80192F98.c 0x80192F98 0x50 -O2 -G0
linked .text 80 bytes, target 0x50, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03D2
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS ...
```

## Notes

Same named-id lever as `func_801931BC` (`int id = 7;`); with the literal the `beqz` moves ahead of the `sw $zero,0x20` (2 words).
