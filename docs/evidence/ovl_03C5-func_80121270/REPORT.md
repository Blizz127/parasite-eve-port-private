# ovl_03C5 func_80121270

* Overlay: `ovl_03C5` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x570`, VRAM `0x80121270`, size `0x264`
* Source: `src/overlays/ovl_03C5/func_80121270.c`
* Profile: `era_o2_g0` (default)

## What it does

MDEC frame fetch. Checks the drive (`func_8007F72C`/`func_8007F7A8`, exit on a disc mismatch), then
polls `func_8007C484` up to 2000 times for the next stream frame. Near the movie end it fades the
XA volume (`D_800B0DBE * v / 14`) and flags the end (`D_801223F5`). On a frame-size change it
clears the display area and records the new size. It publishes width (x1.5 in 24-bit mode) and
height into `out`, and returns the frame address.

## Levers (mismatch ladder 127 → 47 → 36 → 35 → 22 → 4 → 2 → 0)

* `RECT r` declared in the size-change block (its stack slot sits above `addr`/`hdr`).
* `func_8007F7A8` returns `int` (no `andi 0xFFFF`), and the divisor is 14 (magic `0x92492493` with `sra 3`).
* `r.h = 480` in both branches, which gives retail's shared `sh v0,0x1E`.
* Chained stores `out[13] = out[17] = …` (the right-hand field is stored first).
* `volatile short *p = &D_80122418[1]` for the double read.
* A separate `t = frameCount + 16`, then `v = t - last; v = 14 - v`.

## Verification (2026-09-24)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_03C5 src/overlays/ovl_03C5/func_80121270.c 0x80121270 0x264 -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_03C5
disc1_preflight: PASS (deep, 5 c / 3 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_03C5
(pending: gates are held by the lead; queued in build/fanout/overlays2/gate_queue.txt)
```
