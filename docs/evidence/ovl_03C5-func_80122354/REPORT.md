# ovl_03C5 func_80122354

* Overlay: `ovl_03C5` (PE.IMG, config `configs/USA/overlays/ovl_03C5.yaml`)
* Blob offset `0x1654`, VRAM `0x80122354`, size `0x54`
* Source: `src/overlays/ovl_03C5/func_80122354.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C5 src/overlays/ovl_03C5/func_80122354.c 0x80122354 0x54 -O2 -G0
linked .text 96 bytes, target 0x54, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C5
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03C5
EXACT_REBUILD_GATE[ovl_03C5]=PASS ...
```

## Notes

Plain `D_800B0DBA--` spells the base twice (`lui $at` store); a pointer local `unsigned char *p = &D_800B0DBA` reproduces retail's shared `$v1` base (4 words otherwise). `func_8010C0D8` lives in ovl_039F and binds by address.
