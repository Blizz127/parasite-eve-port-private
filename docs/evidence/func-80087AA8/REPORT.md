# func_80087AA8 — Akao per-voice slide / LFO-depth tick

- VRAM `0x80087AA8`, ROM `0x782A8`, size `0x4F8` (318 words). Callers: `func_8008DCD0` region (`jal` at `0x8008DCD0`, `0x8008DF2C`).
- Source: `src/func_80087AA8.c`. Compiler: era cc1 **2.7.2** `-O2 -G0` (default profile `era_o2_g0`, no maspsx knob).
  2.8.1 `-mno-split-addresses` on the same source misses by 2 words (epilogue: `addiu $sp` vs `jr`), so 2.7.2 is the one used.

## Verification (private config `build/fanout/tooling2/disc1.yaml`, rows added: `[0x782A8, c, func_80087AA8]`, `[0x787A0, asm]`)

```
python3 tools/analysis/era_link_check.py src/func_80087AA8.c 0x80087AA8 0x4F8 -O2 -G0
  -> linked .text 1280 bytes, target 0x4f8, word mismatches=0, LINK_EXACT
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_80087AA8
  -> disc1_preflight: PASS (deep, 1837 c / 306 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_80087AA8
  -> LEAF_STRONG=EXACT func_80087AA8 span=0x4F8 vram=0x80087AA8 compiled=0x500 failures=[none]
```

## Shape

Twelve countdown-guarded updates on the 0x11C-stride voice record:
- 32-bit volume slide at `+0x44`/`+0x48` (dirty `|= 3` when bits `0xFFE00000` change);
- `+0x5E += +0xD6`;
- `+0x6C`/`+0xD4` (`0x7F00` gate);
- `+0xD8`/`+0xDA` (gate only when `mode & 0x100`);
- `+0x76`/`+0xDC` (`0xFF00` gate);
- plain decrements `+0x8A`/`+0x9E`;
- two one-shot timers `+0xBA`/`+0xBC` that toggle `mask` in `D_8009D2C8->+0x34`/`+0x3C` and call `func_80089960`/`func_80089CF0`;
- the vibrato / tremolo / pan depth ramps (re-evaluating the LFO sample at the current wave position without advancing it);
- the 32-bit pitch slide `+0x34`/`+0x4C` (dirty `|= 0x10`).

## Levers (ladder, `build/fanout/tooling2/v/B*`)

| step | change | 2.7.2 mismatches |
|---|---|---|
| B1 | first draft | 203 |
| B3 | `volatile` on `D_8009D2C4` **and** on `Ctl.f34` — retail keeps the `^= mask` RMW strictly before the `D_8009D2C4 |= 0x10` load (no interleave, load-delay nops) | 32 |
| B4 | `Ctl.f3C` **non**-volatile — its store must fill the `jal func_80089CF0` delay slot | shifts residual to the vib-depth join |
| B6/B7 | depth computed per arm | 155 |
| B8 | per-arm `b = (d * x) >> 7`, single `v->vib_depth = b;` after the join | **0** |

- Why B8 works: `mult`/`mflo`/`srl` stay in the arms until cross-jumping, so sched1 cannot hoist the `+0x8A` load above `mflo`. The result lands in `$v1`, which frees sched2 to put the `+0x8A` load ahead of the depth store. The store then fills the `bnez` slot.
- `+0x46` (the `lh` of the volume word's high half inside the 32-bit `+0x44` slide) is read as `((short *)v)[0x23]`.
- No `asm` barriers, pins or clobbers.
