# ovl_03C9 func_801F0BA0

* Overlay: `ovl_03C9` (PE.IMG overlay, VRAM base 0x801ED7F8)
* Blob offset `0x33A8`, VRAM `0x801F0BA0`, size `0x190` (100 words)
* Source: `src/overlays/ovl_03C9/func_801F0BA0.c`
* Profile: `era_o2_g0`

## What it does / levers

Effect handler (mode, obj): mode 1 counts step to 16 once wait is 0 (returns 1 when done); mode 2 draws the step-scaled sprite through func_800CEE20 with a rcos-style size (func_80077CF4 + 0x800), a func_800CF3AC-built colour buffer, and the CLUT row from D_800E1204[D_800F336C] (+4 for area 4 when D_800F3428). Levers: switch with break (not return 0) keeps the branchy compare; 'o->step * 32' (not << 5) keeps retail's lh; int CLUT row avoids an andi 0xFFFF.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C9 src/overlays/ovl_03C9/func_801F0BA0.c 0x801F0BA0 0x190 -O2 -G0
linked .text 400 bytes, target 0x190, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh ovl_03C9
EXACT_REBUILD_GATE[ovl_03C9]=PASS plan=e5994b0efd644bd2a65465ee9e4f86cf84bb98f718ec51f3c99cd80c9963b6b5 yaml=e7d30d7aecc534414cd398642f74e9d74858adda02dcce046bd9702a7add9218 spans=[8 c, 2 asm, 0 rodata, 2 bin] sha1_orig=1d0e972cf9446dc700c82a8597292ced45958ef4 sha1_cand=1d0e972cf9446dc700c82a8597292ced45958ef4 sha1=1d0e972cf9446dc700c82a8597292ced45958ef4
```
