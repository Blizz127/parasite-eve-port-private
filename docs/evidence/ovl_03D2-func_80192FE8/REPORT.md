# ovl_03D2 func_80192FE8

* Overlay: `ovl_03D2` (PE.IMG, config `configs/USA/overlays/ovl_03D2.yaml`)
* Blob offset `0x3FF8`, VRAM `0x80192FE8`, size `0x9C`
* Source: `src/overlays/ovl_03D2/func_80192FE8.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_80192FE8.c 0x80192FE8 0x9C -O2 -G0
linked .text 160 bytes, target 0x9c, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03D2
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS ...
```

## Notes

Levers: the abs-style ternary must test `a >= 0` first; the f1C value is computed into the local and stored only after f24 and the f14 callback pointer are loaded (retail schedules the `sw` after both loads); `c += (c != 0)` gives the `sltu`/`addu` operand order.
