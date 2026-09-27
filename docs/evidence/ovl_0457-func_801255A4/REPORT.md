# ovl_0457 func_801255A4

* Overlay: `ovl_0457` (PE.IMG, config `configs/USA/overlays/ovl_0457.yaml`)
* Blob offset `0x48A4`, VRAM `0x801255A4`, size `0x88`
* Source: `src/overlays/ovl_0457/func_801255A4.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_801255A4.c 0x801255A4 0x88 -O2 -G0
linked .text 144 bytes, target 0x88, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS ...
```

## Notes

`func_8003E91C` (EXE) is passed as a function-pointer argument; it links as the address-named symbol the asm spells `D_8003E91C`.
