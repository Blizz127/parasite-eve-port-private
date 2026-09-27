# func_80199218 and func_80199234

Room `room_m0318i`. Both are 0x1C, era `-O2 -G0` (the room default). No new profile.

`func_80199218` at `0x80199218` stores `a2` through a pointer to `D_801998F0` and `a1` into `D_801998F4`. `func_80199234` at `0x80199234` is the same shape for `D_8019990C` and `D_80199910`. An empty `asm` on the pointer keeps the first address in `$v0` (`la` / `sw`) while the second store stays a separate symbol store. Without the barrier both stores are plain symbol stores and the leaf is 6 words short of the `la`.

`era_link_check.py --target room_m0318i` prints `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 32 bytes because `.text` is 16-byte aligned and `0x1C` is 4 bytes short of that boundary; those bytes are zero. `disc1_preflight.py --target room_m0318i --deep --only` passed for both before the overlay rebuild.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target room_m0318i \
  src/overlays/room_m0318i/func_80199218.c 0x80199218 0x1C -O2 -G0
```
