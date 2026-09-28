# func_800735C4 — PARKED (lane exeD, 2026-09-27)

VRAM `0x800735C4`, file `0x63dc4`, size `0x314`, `asm/disc1/63DC4.s`.

- Best: ~100 diff lines (123 mismatches) under 2.7.2 and 2.8.1 both tried.
- Draft: `build/lanes/exeD/drafts/func_800735C4.c`.
- Residual / levers tried: __muldf3 (Psy-Q soft-float, compiled C: the neighbours __floatsidf/__fixdfsi and the S64 helpers are matched C on era_o2_g0). The structure is reconstructed exactly: sign in $fp, exponent sum e spilled at sp+0x50, the 21-bit split multiply via func_800738D8, round-by-0x400/0x200, overflow via func_800739C4(0x22,0xD). Residual: parameter/register allocation. Retail copies only `a` to t0/t1 and uses b in a2/a3; the local stack slots are acc 0x18, rnd 0x20, ma 0x28, mb 0x30, t 0x38, p 0x40, r 0x48, and e is spilled (the frame is 0x80). Tried double params (108/100 lines), S64 params (homed to stack, worse) and 4 scalar words (113/106). Draft build/lanes/exeD/drafts/func_800735C4.c.
- Measured with `tools/analysis/era_link_check.py` through `build/lanes/exeD/t.sh`; the diff-lines counts come from an aligned objdump diff.


## exeD round 2 (2026-09-27)

- Best: 68 diff lines (2.7.2) / 66 (2.8.1) under 2.7.2 -O2 -G0.
- Draft: `build/lanes/exeD/drafts/func_800735C4.c` (if present).
- __muldf3 is NOT libgcc fp-bit.c: fp-bit's __muldf3 is unpack_d x2 / _fpmul_parts / pack_d over a long-long fraction struct. Retail instead calls the Psy-Q 64-bit helpers directly: func_80073294 (logical shift, x5), func_800738D8 (32x32->64 multiply, x3), func_80072DF4 (64-bit add, x4), and func_800739C4 (matherr, x1). S64 results come back through a hidden pointer, with no pack/unpack calls. It is ordinary compiled C (no handwritten ops). Neighbours __floatsidf/__fixdfsi and the helpers are matched C on era_o2_g0. Maspsx patch 11's ROM notes list this function's `j`-slot `sw $zero,N($sp)` fills as volatile stack locals. Levers found: `volatile int e` (the exponent sum spilled at sp+0x50, which gives retail's 0x80 frame); locals declared acc, rnd, ma, mb, t, p then r (stack slots 0x18..0x48); the rounding block written as three separate `func_80073294` calls (retail cross-jumps them to one jal). Left: `b` copied to t2/t3 where retail uses a2/a3 directly (mask allocated to $a2), the &t/&p s4/s5 swap, retail's 0x80000000 constant kept in $s0 for an `and` test, and t3 use at the e reload.
