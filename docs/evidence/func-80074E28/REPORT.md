# func_80074E28 — GPU debug RECT report (0x80074E28, 0x11C)

Status: **matched** (tooling2 lane, 2026-09-23), `LINK_EXACT` + `LEAF_STRONG=EXACT`.

- YAML: `- [0x65628, c, func_80074E28]` (replaces the `[0x65628, asm]` row; the
  next row 0x65744 is unchanged).
- Profile: existing `era_o2_g0_cc1_281_fill_call_delay_slot`
  (`ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses` + patch 8).
- Source: `src/func_80074E28.c` (lane-4 draft + one lever).

## Closing lever (cc1-side, not a maspsx knob)

The parked residual (9 words) was the final 5-argument `D_80095748` call:
retail loads x/y/w into `$a1..$a3` and `h` into `$v1` **last**, and the
`sw $v1,0x10($sp)` lands in the `jalr` slot. cc1 loaded `h` first into `$2`
and stored it early; that store could not be moved by the assembler (a later
`lw $2,D_80095748` redefines `$2`). Declaring
`register int h asm("$3"); ... h = r->h;` after the x/y/w locals makes cc1
load it last into `$3` and fill the `jalr` slot itself (9 -> 0).

| variant | mismatches |
|---|---|
| lane-4 draft (patch 8) | 9 |
| int/short temps, fn-pointer local (5 spellings) | 9 |
| `h` pinned to `$3` (patch 8) | **0** |
| `h` pinned, no patch 8 | 26 |
| 2.7.2 -O2 -G0 (build-facts hint) | 58 |

## Verification (private config build/fanout/tooling2/)

```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_80074E28
  -> disc1_preflight: PASS (deep, 1836 c / 303 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_80074E28
  -> LEAF_STRONG=EXACT func_80074E28 span=0x11C vram=0x80074E28 compiled=0x120 failures=[none]
```
