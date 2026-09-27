# ovl_0457 func_801258EC

* Overlay: `ovl_0457` (PE.IMG, config `configs/USA/overlays/ovl_0457.yaml`)
* Blob offset `0x4BEC`, VRAM `0x801258EC`, size `0x94`
* Source: `src/overlays/ovl_0457/func_801258EC.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_801258EC.c 0x801258EC 0x94 -O2 -G0
linked .text 160 bytes, target 0x94, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS ...
```

## Notes

Levers: a pointer local `int *p = &D_80172CE8` holds the position in `$s1` across both calls; `D_80172CF0` and `D_80172CF4` must both be `volatile` — that orders the mirror store before the counter and yields retail's trailing re-read of `D_80172CF4`.
