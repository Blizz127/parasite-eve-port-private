# func_800740D0 — EXACT (lane exeD6, 2026-09-28)

VRAM `0x800740D0`, file `0x648D0`, size `0x148` (82 words). libetc interrupt-callback setter over `D_800945E4`
{u16 active; void *cb[11]; u16 mask @0x30}, with I_MASK through the volatile pointer `D_80095674`.
Profile `era_o2_g0_no_schedule_insns_cc1_281` (existing; cc1 2.8.1 `-O2 -G0 -mno-split-addresses -fno-schedule-insns`).
YAML `- [0x648D0, c, func_800740D0]` (replaces `- [0x648D0, asm]`; next row `[0x64A18, c, func_80074218]`).
Source `src/func_800740D0.c` (= `build/lanes/exeD6/drafts/func_800740D0.final.c`). The link is 0x150 with a zero gas pad.

## Verification (private config build/lanes/exeD6/, refreshed from main)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD6/disc1.yaml --profiles build/lanes/exeD6/disc1_build_profiles.json --deep --only func_800740D0
  disc1_preflight: PASS (deep, 2130 c / 155 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config ... func_800740D0
  LEAF_STRONG=EXACT func_800740D0 span=0x148 vram=0x800740D0 compiled=0x150 failures=[none]
```
Profile necessity (manual): 2.7.2 `-O2 -G0` 78; 2.8.1 `-O2 -G0` 65; 2.8.1 `-mno-split-addresses` with scheduling on 81;
the assigned profile gives LINK_EXACT.

## Levers (from the exeD round-2 draft, 12 aligned lines)
1. The volatile I_MASK read goes to a local: `unsigned int r = *D_80095674; *D_80095674 = 0; m = r & 0xFFFF;`. This gives
   retail's separate `andi $19,$3,0xffff` in the `beq` slot.
2. `__asm__("" : "=r"(cb) : "0"(cb));` at the top of the clear arm. Without it, cse substitutes the known-zero `cb`
   register (`sw $18`) for the stored 0. Retail has `sw $0`. 12 → 11.
3. The clear-arm mask is a `$2`-pinned local, `register unsigned int nb asm("$2") = ~(1 << idx);`, applied to both `m`
   and the table mask. This gives retail's `$2` mask / `$3` halfword. 11 → 4.
4. **Unpin `m` and `slot`.** The draft's `$19` pin on m made the volatile read land directly in `$19`, and the `$4` pin
   on slot made the `sll` target `$4`. Without them, the allocator reproduces retail's `$3` temp and `sll $2`. 4 → 0.
   Unpinning only m gives 2, and unpinning only slot gives 7.
