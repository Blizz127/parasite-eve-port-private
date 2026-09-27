# func_8006914C — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x8006914C`, file offset `0x5994C`, size `0x448`; yaml `- [0x5994C, c, func_8006914C]` replaces
  `- [0x5994C, asm]` exactly (next row `[0x59D94, c, func_80069594]`).
- Profile: default `era_o2_g0`. Asset / TIM-texture loader state machine on `D_800B0CD8.state` (sparse switch with
  goto re-dispatch), slot-table init of `D_800942E4` (11 × big) and `D_800942E8` (11 × small), handler sweep through
  `D_800942E0`.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_8006914C
  disc1_preflight: PASS (deep, 2049 c / 213 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_8006914C
  LEAF_STRONG=EXACT func_8006914C span=0x448 vram=0x8006914C compiled=0x450 failures=[none]
```

## Levers

decompB draft: 229 words, all from a 2-word size shift. Both slot-init loop preambles lacked retail's copied address
register (`lui $v0,%hi(D_800942E4)` / `addiu $v0,…` / `addu $a1,$v0,$zero`). Direct global, block copy, barriers,
inline helper, comma for-init were already tried by the earlier lane.
**Closing lever: a loop-body-local pointer to the same global** — keep `pb = &D_800942E4; *pb = …;` before the loop,
and inside the loop `BigSlot **q = &D_800942E4; p = &(*q)[i].s;` (same for `D_800942E8`/`SmallSlot`). loop.c hoists the
invariant `q = &sym` out of the loop as a movable, and cse makes it a copy of the preheader's `pb` register → exactly
retail's `la $v0` + `move $a1,$v0`. First loop only: 226 (1104 bytes); both loops: 0.
