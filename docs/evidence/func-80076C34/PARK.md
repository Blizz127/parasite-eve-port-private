# func_80076C34 — PARKED (lane exeD, 2026-09-27)

VRAM `0x80076C34`, file `0x67434`, size `0x2b0`, `asm/disc1/67434.s`.

- Best: 38 words (archived); 13 diff lines on 2.8.1 -fno-schedule-insns + FILL_CALL+LOAD_DELAY_NOP_BEFORE_LABEL+DEST_TEMP under 2.8.1.
- Draft: `build/lanes/exeD/drafts/func_80076C34.c`.
- Residual / levers tried: Residual (1) from the archived park, a 3-word indexed symbol store in a `j label` slot, needs a maspsx knob (KNOB_REQUESTS.md K2). The volatile GpuQ tail store/reload order stays as the archived note says.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.
