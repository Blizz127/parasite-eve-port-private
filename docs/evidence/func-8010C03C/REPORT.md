# func_8010C03C and func_8010C078

Overlay `ovl_039F`. Both are `0x3C`, era `-O2 -G0` (the overlay default). No new profile.

`func_8010C03C` at `0x8010C03C` (file `0x344`) calls `func_8010C308()` when `a0` is zero and otherwise returns bit 29 of `func_8010C430()`. `func_8010C078` at `0x8010C078` (file `0x380`) is the same shape with `func_8010C39C()` and bit 24. The nonzero arm is written first, so retail's `bnez` keeps the zero call as the fall-through. The shift is an `unsigned int` `>>`, which is `srl`, then `& 1`.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target ovl_039F \
  src/overlays/ovl_039F/func_8010C03C.c 0x8010C03C 0x3C -O2 -G0
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target ovl_039F \
  src/overlays/ovl_039F/func_8010C078.c 0x8010C078 0x3C -O2 -G0
```

Both print `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 64 bytes because `.text` is 16-byte aligned and `0x3C` is 4 bytes short of that boundary; those bytes are zero. Writing the zero arm first is 11 words off (`beqz` and the arms swapped). `disc1_preflight.py --target ovl_039F --deep --only func_8010C03C --only func_8010C078` passed before the overlay rebuild (`7 c / 4 asm / 0 rodata`).
