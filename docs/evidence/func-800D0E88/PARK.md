# func_800D0E88 — PARKED (1 word)

- VRAM 0x800D0E88, file offset 0xC1688, size 0x4FC; on the Day 1 route (fan-in 24). Still `asm` in `configs/USA/disc1.yaml`.
- Best draft: `build/lanes/m348/drafts/func_800D0E88.c` (lane-local; inline-GTE macros + era `-O2 -G0 + MASPSX_EXPAND_DIV=1`).
- Check command: `MASPSX_EXPAND_DIV=1 python3 tools/analysis/era_link_check.py <draft> 0x800D0E88 0x4FC 0x4FC`

## Residual

One word at the OT pointer: retail `addu $s0,$v0,$v1` (shift, base), ours `addu $s0,$v1,$v0` (base, shift). With the shift-first spelling `(otz << 2) + (int)ordering[bank]` the order matches, but cc1 then schedules the `sll` before the ordering load and the `&D_800B0E38` materialisation (11 words). The same spelling is exact in func_800D0728 and func_800D004C, where the base comes from a register add rather than the `lui $at` macro load.

## Levers tried

- The struct `RenderBufferPrefix` for D_800B0E38 (the mode packet via `t0+0x20`), and a `cam` local for D_800BCFA4 (285 → 11 / 1).
- The OT spellings tried: `ptr + otz`, `otz + ptr`, `&ptr[otz]`, `ptr + (otz << 2)` as char, `(int)ptr + (otz << 2)`, `(otz * 4) + ptr`, and the unsigned casts. Each is either 1 (base first) or 11 (shift scheduled early).
- A named `z` temp for `otz - bias`, `long`/`unsigned` otz, `(otz -= bias)` in the condition, and `if (otz < 0 || otz >= 0x1000)`: no change.
- A two-statement `ot = base; ot += otz` (90), and re-evaluating the OT expression inside each addPrim (93).
