# ovl_03D2 func_801931BC

* Overlay: `ovl_03D2` (PE.IMG, config `configs/USA/overlays/ovl_03D2.yaml`)
* Blob offset `0x41CC`, VRAM `0x801931BC`, size `0x44`
* Source: `src/overlays/ovl_03D2/func_801931BC.c`, profile `era_o2_g0` (default)

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03D2 src/overlays/ovl_03D2/func_801931BC.c 0x801931BC 0x44 -O2 -G0
linked .text 80 bytes, target 0x44, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03D2
disc1_preflight: PASS (deep, ...)
$ scripts/exact_rebuild_overlay.sh ovl_03D2
EXACT_REBUILD_GATE[ovl_03D2]=PASS ...
```

## Notes

Lever: the task id compared in the list walk must be a named local (`int id = 2;`). With the literal, cc1 leaves `li $a0,2` after the `beqz` and the assembler emits `beqz`/`nop`/`li`; with the local, cc1 hoists it into the delay slot as retail does (4 words).
