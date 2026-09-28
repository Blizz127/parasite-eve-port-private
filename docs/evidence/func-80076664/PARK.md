# func_80076664 — PARKED at 15 words (lane exeE, 2026-09-27)

VRAM `0x80076664`, file `0x66E64`, size `0x23C` (143 words). libgpu-style GPU image
upload (clamp rect w/h to `D_80095750/52`, `(w*h+1)/2` words, GP0 `0x01000000` +
`0xA0000000/0xB0000000` header through `*D_80095850`, poll `*D_80095854 & 0x04000000`
with `func_80077404` timeout, PIO copy of the remainder, DMA for `n` 16-word blocks).
Best draft: `build/lanes/exeE/drafts/func_80076664.c`.

Compiler: `ERA_CC1_VER=2.8.1 -O2 -G0 -mno-split-addresses` (2.7.2 inflates the frame by
16 bytes per short `?:` clamp — `vars=32` — which retail does not have; 2.8.1 reproduces
retail's `lh`+`lhu` double reads of the clamp).

## Ladder (era_link_check mismatched words)
| step | words |
|---|---|
| first structural draft, 2.8.1 | 106 |
| `words`/`size`/`n` split: `size = words >> 4; ...; n = size; size = words - (n << 4)` | 77 |
| barrier on the poll mask so the loop constant is rematerialized (`lui s3,0x400`) | 74/67 |
| pins `register Rect *r asm("$17")`, `register unsigned int *p asm("$18")` | 53 |
| copy-loop `-1` via barrier-ed `k` (retail `li a0,-1` remat) | 52 |
| data/status pointers **non-volatile** (volatile store blocked reorg's `bne` slot fill) | 26 |
| `unsigned int *d = D_80095850; c = 0xA0000000; if (full) c = 0xB0000000; *d = c;` | 21 |
| `asm("" : "=r"(m), "=r"(st) : "0"(0x4000000), "1"(st))` on the first poll test | 18 |
| `c = 0xA0000000; asm("" : "=r"(c) : "0"(c));` placed before `*D_80095854 = 0x4000000` (reorg then steals it into both poll-branch slots) | **15** |

## Residual (15 words)
1. Prologue pair order (4 words): cc1 emits `sw s2 / move s2,a1` before `sw s1 / move s1,a0`;
   retail saves the s1/a0 pair first. Invariant under pin order, init order, unpinned
   params. maspsx patch 26 (`MASPSX_SWAP_S2_S1_SAVE_PAIRS`) is the opposite direction.
2. Size block local-alloc (6–8 words): retail biased value in `$v1`, sign temp `$v0`,
   `n` copied through `$v1` (`move v1,s0 / sll v0,v1,4 / subu s0,a0,v0 / ... / move s4,v1`
   with the last move in the status-pointer load-delay slot). The `{ int t = size; size = words - (t << 4); n = t; }`
   form reproduces the `$v1` copy exactly but schedules `move s4,v1` one slot early
   (+1 word → 95 shifted).
Tried: `-fno-schedule-insns` (132), `-fschedule-insns2` (no change), `-O3`, `-fno-cse-*`,
`-fno-rerun-cse-after-loop`, `-fno-expensive-optimizations`, `size/16`+`%16`, `words & 15`.
