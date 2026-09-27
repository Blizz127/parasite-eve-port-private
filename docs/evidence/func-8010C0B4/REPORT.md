# func_8010C0B4 and func_8010C0D8

Overlay `ovl_039F`. Both are `0x24`, era `-O2 -G0` (the overlay default). No new profile.

`func_8010C0B4` at `0x8010C0B4` (file `0x3BC`) calls `func_80073CF4(0, a0)`.
`func_8010C0D8` at `0x8010C0D8` (file `0x3E0`) calls `func_80073CF4(1, a0)`.
The constant is in the `jal` delay. The frame is `0x18` with `$ra` at `0x10($sp)`, and `addiu $sp,$sp,0x18` sits before `jr $ra` with a `nop` delay.

```
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target ovl_039F \
  src/overlays/ovl_039F/func_8010C0B4.c 0x8010C0B4 0x24 -O2 -G0
TMPDIR=build/tmp python3 tools/analysis/era_link_check.py --target ovl_039F \
  src/overlays/ovl_039F/func_8010C0D8.c 0x8010C0D8 0x24 -O2 -G0
```

Both print `LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is 48 bytes because `.text` is 16-byte aligned and `0x24` is 12 bytes short of that boundary; those bytes are zero. `disc1_preflight.py --target ovl_039F --deep --only func_8010C0B4 --only func_8010C0D8` passed before the overlay rebuild (`5 c / 5 asm / 0 rodata`).
