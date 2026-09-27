# func_8019B1E8

Room `room_m0429i`. VRAM `0x8019B1E8`, size `0x1C`, file offset `0xC200`.
Era `-O2 -G0` (the room default). No new profile.

The first argument is unused. `a1` is stored as a halfword through a pointer
to `D_8019B66C`. `a2` is stored as a halfword to `D_8019B66E`. An empty `asm`
on the pointer keeps that address in `$v0` (`la` / `sh`) while the second
store stays a separate symbol store.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target room_m0429i \
  src/overlays/room_m0429i/func_8019B1E8.c 0x8019B1E8 0x1C -O2 -G0
```

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 32
bytes because `.text` is 16-byte aligned and `0x1C` is 4 bytes short of that
boundary; those bytes are zero. `disc1_preflight.py --target room_m0429i
--deep --only func_8019B1E8` passed before the overlay rebuild
(`56 c / 12 asm / 1 rodata`).
