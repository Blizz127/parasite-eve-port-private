# room_m0022i func_8018F1CC

* Overlay: `room_m0022i` (PE.IMG room chunk 2, VRAM base 0x8018EFE8)
* Blob offset `0x1E4`, VRAM `0x8018F1CC`, size `0x8` (2 words)
* Source: `src/overlays/room_m0022i/func_8018F1CC.c`
* Profile: `era_o2_g0`

## What it does / levers

Return-0 stub (a behaviour-table entry). The text window's first word; identical in all 30 rooms of this text group.

## Verification (2026-09-22)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target room_m0022i src/overlays/room_m0022i/func_8018F1CC.c 0x8018F1CC 0x8 -O2 -G0
linked .text 16 bytes, target 0x8, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ scripts/exact_rebuild_overlay.sh room_m0022i
EXACT_REBUILD_GATE[room_m0022i]=PASS plan=80412351f8f0aa82d5e007f591e0eae2febfa639b949667bcf00d7b37404c706 yaml=48e54b2123f171899c7dba43ed58c4e99e1d70d23353d56585a364761691ce3d spans=[3 c, 3 asm, 0 rodata, 2 bin] sha1_orig=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1_cand=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3
```
