# room_m0022i func_801924C4

* Overlay: `room_m0022i` (PE.IMG room chunk 2, VRAM base 0x8018EFE8)
* Blob offset `0x34DC`, VRAM `0x801924C4`, size `0x1C` (7 words)
* Source: `src/overlays/room_m0022i/func_801924C4.c`
* Profile: `era_o2_g0`

## What it does / levers

Set the object's state byte to 4 and clear the word its +0x10 link points at (if any); returns 0.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target room_m0022i src/overlays/room_m0022i/func_801924C4.c 0x801924C4 0x1C -O2 -G0
linked .text 32 bytes, target 0x1c, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh room_m0022i
EXACT_REBUILD_GATE[room_m0022i]=PASS plan=80412351f8f0aa82d5e007f591e0eae2febfa639b949667bcf00d7b37404c706 yaml=48e54b2123f171899c7dba43ed58c4e99e1d70d23353d56585a364761691ce3d spans=[3 c, 3 asm, 0 rodata, 2 bin] sha1_orig=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1_cand=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3
```
