# func_80199250

Room `room_m0318i`. VRAM `0x80199250`, size `0x44`, file offset `0xA268`.
Era `-O2 -G0` (the room default). No new profile.

`if (a0 == 1)` stores `a1` into `D_80199914`. The other arm stores `a1`, `a2`,
and `a3` into `D_80199918`, `D_8019991C`, and `D_80199920`. Both arms return
`&D_80199914`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target room_m0318i \
  src/overlays/room_m0318i/func_80199250.c 0x80199250 0x44 -O2 -G0
```

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 80
bytes because `.text` is 16-byte aligned and `0x44` is 12 bytes short of that
boundary; those bytes are zero. `disc1_preflight.py --target room_m0318i
--deep --only func_80199250` passed before the overlay rebuild
(`50 c / 10 asm / 1 rodata`).
