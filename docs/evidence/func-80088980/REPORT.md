# func_80088980 — EXACT (lane exeE2, 2026-09-28)

AKAO voice LFO / volume / pan / pitch update.

- VRAM `0x80088980`, file offset `0x79180`, size `0x4E4` (ends on its `jr $ra`; next row
  `[0x79664, c, func_80088E64]`).
- Profile: default `era_o2_g0` (cc1 **2.7.2** `-O2 -G0`) — no knobs. (2.8.1 lacks the retail
  frame: 282 mismatches, per the earlier exeE PARK.)
- YAML: `- [0x79180, c, func_80088980]` (replaces `- [0x79180, asm]`).
- Source: `src/func_80088980.c` (copy at `build/lanes/exeE2/drafts/func_80088980.exact.c`).

## Verification (private config build/lanes/exeE2/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeE2/disc1.yaml \
  --profiles build/lanes/exeE2/disc1_build_profiles.json --deep --only func_80088980
  -> disc1_preflight: PASS (deep, 2102 c / 178 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeE2/disc1.yaml \
  --profiles build/lanes/exeE2/disc1_build_profiles.json func_80088980
  -> LEAF_STRONG=EXACT func_80088980 span=0x4E4 vram=0x80088980 compiled=0x4F0 failures=[none]
ERA_CC1_VER=2.7.2 tools/analysis/era_link_check.py src/func_80088980.c 0x80088980 0x4E4 -O2 -G0
  -> word mismatches=0, nonzero_pad=0
```
`profile_necessity.py` has no `--config` option (reads the main yaml); the leaf uses the
default profile, so it needs no assignment.

## Levers (mismatch ladder from the exeE PARK best of 10)
The 10-word residual was `pan` (`$v1` in retail) vs the `D_8009D2C0` switch value
(`$a0` in retail) swapped. Two causes, both needed:
1. **`pan = 0x80` in the else arm** of `if (!(U32(0x2C) & 0x2000000)) {...} else pan = 0x80;`
   instead of before the `if`. Before the if, the `0x80` set precedes the `lui 0x200` mask
   temp, which local-alloc puts in `$v1`, so `pan` conflicts with hard reg 3 (greg dump
   `;; 78 conflicts: ... 2 3 ...`). In the else arm the conflict disappears (alone: still 10,
   because the switch value now outranks `pan` for `$v1`).
2. **One empty `asm("" : : "r"(pan));` before the `switch`** raises `pan`'s global-alloc
   priority above the 3-ref switch pseudo, so `pan` takes `$v1` and the switch value `$a0`.
   10 -> **0** (1, 2, 4 or 8 such uses all give 0; without lever 1, 1–16 uses stay at 10).
Carried over from exeE: `register int s asm("$4")` sample pins in the three LFO blocks and the
`$3`/`$2` pins in block 2.
