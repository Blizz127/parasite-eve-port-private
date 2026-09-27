# func_8010C0FC

Overlay `ovl_039F`. VRAM `0x8010C0FC`, size `0xF0`, file offset `0x404`.
Profile `era_o2_g0_fill_call_delay_slot`: `-O2 -G0` and
`MASPSX_FILL_CALL_DELAY_SLOT=1`. The profile already exists on the EXE
manifest; this overlay only assigns it.

`a0 == 0` stores `0x80000000` through `D_8010DB50`, zeros `D_8010DB24` and
`D_8010DB30`, stores `0x60000000` through a non-volatile view of the same
pointer (so the store can sit in the `jal` delay), then calls
`func_8010C1EC` on `D_8010DA0C` and `D_8010DA90` with length `0x20`.
`a0 == 1` does the same stores and re-reads `D_8010DB30` before the second
`D_8010DB50` store. Any other value calls `func_80071A74(D_8010BCFC)`.

The switch value is pinned to `$a1`. An empty `asm` on that pin at the start
of case 0 keeps the zero stores on `$zero`. Case 1's `0x80000000` is pinned
to `$v0` so it fills the `beq` delay. The second pointer is pinned to `$v1`.

```
TMPDIR=build/tmp MASPSX_FILL_CALL_DELAY_SLOT=1 \
  python3 tools/analysis/era_link_check.py --target ovl_039F \
  src/overlays/ovl_039F/func_8010C0FC.c 0x8010C0FC 0xF0 -O2 -G0
```

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 240
bytes, equal to the declared span. Without `MASPSX_FILL_CALL_DELAY_SLOT` the
leaf is 8 words off: `la` of `D_8010BCFC` stays in front of `jal func_80071A74`
and the delay is `nop`, which also shifts both jumps to the epilogue.
`disc1_preflight.py --target ovl_039F --deep --only func_8010C0FC` passed
before the overlay rebuild (`8 c / 4 asm / 0 rodata`).
