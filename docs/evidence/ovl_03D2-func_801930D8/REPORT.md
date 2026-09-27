# ovl_03D2 func_801930D8

* Overlay: `ovl_03D2` (PE.IMG, config `configs/USA/overlays/ovl_03D2.yaml`)
* Blob offset `0x40E8`, VRAM `0x801930D8`, size `0x94`
* Source: `src/overlays/ovl_03D2/func_801930D8.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_801930D8.c 0x801930D8 0x94 -O2 -G0
linked .text 160 bytes, target 0x94, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03D2
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS ...
```

## Notes

Levers: (1) the increment is a separate `inc` local that defaults to 0 and is set to `a < 0x54` only when `a != 0`; the ternary form reorders the `sw`. (2) a separate accumulator `a` for f20 (`a += inc; t->f20 = a;`) keeps it in `$v0` while f1C gets `$v1`. (3) the final clamp is `r = 0x18; if (d >= 0x18) r = d;` (retail's default-in-delay-slot shape; `if (d < 0x18) d = 0x18` misses 4 words).
