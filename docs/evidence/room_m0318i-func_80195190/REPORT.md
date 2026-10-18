# room_m0318i func_80195190 (NON_MATCHING)

Blob offset 0x61A8, 0x454 bytes, VMA 0x80195190. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80195190.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80195190, blob offset 0x61A8, 0x454 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback taking (mode, a1 = effect record), the cmd 1 / cmd 2
 * sibling of room_m0429i func_801982A4 (minus the GTE matrix set-up).
 * mode 1: per-frame motion; state (+0x10) 0 lasts 16 frames, state 1 moves
 *         the position (+0/+2/+4) by the velocity (+8/+A/+C), damping x/z by
 *         127/128, bouncing y against D_800942EC, and lasts 24 frames;
 *         returns 1 when the frame counter (+0x12) reaches the limit;
 * mode 2: draw. State 0 sets the D_800F3368 block (+0/+0xE/+0x10 = 0x20,
 *         +2 = 2, +4 = 2, +6 = 1) and draws D_80199924 with a sprite whose
 *         u/v offset (buf +4) is advanced by D_800E27EC * 32; state 1 sets
 *         the block (0x10, 1, 2, 0) and draws the record itself.
 * Any other mode / state returns 0. The 8-byte buf is seeded from
 * D_8018F208 (unaligned word-pair copy).
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80195190 --check-gte --uncovered --seeds 64
    e21_leaf: ok func_80195190__16fe76b1_port.c 128 seeds, retail coverage 264/277 words
    e21_leaf: uncovered 80195404 80195408 8019540C 80195410 80195414 80195418 8019541C 80195554 80195558 8019555C 80195560 80195564 80195568

## Uncovered words

8019527C: delay slot `addiu a2,a2,0x7F`, the negative rounding fix-up of SH8*127/128. Not hit in the main run, hit in the supplementary run.
80195404-80195418 and 8019541C: mode 2 state 0, the `D_800F336C == 4 && D_800F3428 != 0` arm (a0==4 compare, D_800F3428 load, `j` and `+8` delay slot). Unreachable in the differential. The function stores D_800F336C = 2 just before calling the mocked func_800CF3AC, and the mock does not write D_800F336C, so the re-read is always 2 (a0 != 4, so the `+4` arm runs). Only the real callee could change it.
80195554-80195568: mode 2 state 1, the same `D_800F336C == 4 && D_800F3428 != 0` arm (`+4`). Unreachable for the same reason: D_800F336C is stored 2 before the call and re-read after it.
All other words are covered, including the mode and state default arms (a0=0,3 and state 2,3 in the main run).

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0318i func_80195190 --nonmatching 61A8:454 --check-gte --uncovered --seeds 64 --inputs "a0=0,1,2,3; h(a1+0x10)=0,1,2,3; h(a1+0x12)=0,5,0x10,0x17; h(a1+2)=0x7000,0x100; h(0x800942EC)=0x500"`
  - e21_leaf: ok func_80195190__16fe76b1_port.c 64 seeds, retail coverage 263/277 words | e21_leaf: uncovered 8019527C 80195404 80195408 8019540C 80195410 80195414 80195418 8019541C 80195554 80195558 8019555C 80195560 80195564 80195568 (identical to the claim)
- `python3 tools/analysis/e21_leaf.py room_m0318i func_80195190 --nonmatching 61A8:454 --check-gte --uncovered --seeds 16 --inputs "a0=1; h(a1+0x10)=1; h(a1+8)=0x8001,0x100; h(a1+0xC)=0x8001,0x100; h(a1+2)=0x7000,0x100"`
  - e21_leaf: ok func_80195190__16fe76b1_port.c 16 seeds, retail coverage 83/277 words; 8019527C is not in the uncovered list (covered)
- `NEW1: ... --seeds 128 --inputs "a0=1; h(a1+0x10)=1; h(a1+8)=0x8000,0x8001,0xFFFF,1,0x7FFF; h(a1+0xA)=0x7FFF,0xFFFF,0x8000; h(a1+2)=0x7000,0x8000; h(a1+0x12)=0x17,0x7FFF; h(0x800942EC)=0x500"`
  - e21_leaf: ok func_80195190__16fe76b1_port.c 128 seeds, retail coverage 83/277 words; 8019527C and the whole mode 1 state 1 body covered
- `NEW2: ... --seeds 80 --inputs "a0=2; h(a1+0x10)=0,1; h(a1+0x12)=0,1,0x7FFF,0x8000,0xFFFF; w(0x800E27EC)=0,0x7FFFFFF,-5; h(0x800E11F8)=0,3; h(0x800E11E8)=0,3"`
  - e21_leaf: ok func_80195190__16fe76b1_port.c 80 seeds, retail coverage 194/277 words; uncovered in the mode 2 range was only 80195310-80195314 (state default arm, not exercised by this spec) plus the claimed 80195404-8019541C and 80195554-80195568

No MISMATCH, notest or skipped in any run. I also compared the retail asm with the C by hand.

- Mode 1 state 0: the 16-frame limit matches.
- Mode 1 state 1: the x and z 127/128 fix-up matches (retail uses signed `lh`, `(v<<7)-v`, then `sra 7`; the C is `SH*127/128`). The y bounce matches: `+1`, then `-y` truncated to a short when `SH(+2) >= D_800942EC` (signed `slt`). The limit of 0x18 matches.
- Mode 2 state 0: the call arguments are in the right order, with `a1` at sp+0x28, `v` at sp+0x30, 0x44 at +0x10, `clut & 0xFFFF` at +0x14, 1 at +0x18, s1 at +0x1C and `&v` at +0x20. The `*5/2` and `/32` arithmetic matches.
- Mode 2 state 1: `(x<<10)/24` matches the `0x2AAAAAAB` multiply, and `0xD8 + D_800F336A*2` matches.
- The writes to D_800F3368, 336A, 336C, 336E, 3370, 3376 and 3378 match in both states.

Two points to note, neither a refutation:
- The new mode 2 run used 80 seeds against a 120-combination product, so it is slightly under-seeded. The retail and port runs it did cover were all ok.
- The uncovered arm (`D_800F336C == 4 && D_800F3428 != 0`, 80195404-8019541C and 80195554-80195568) is consistent with the asm. D_800F336C is stored as 2 immediately before the call to func_800CF3AC and re-read after it, so only the real callee could change it. The C implements the arm the same way: +8 for state 0, +4 for state 1, and +4 as the else arm for state 0. No input to this function reaches the arm while the callee is mocked.

