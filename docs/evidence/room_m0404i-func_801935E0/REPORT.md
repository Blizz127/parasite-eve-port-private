# room_m0404i func_801935E0 (NON_MATCHING)

Blob offset 0x45F8, 0x48C bytes, VMA 0x801935E0. The span is `asm` in `configs/USA/overlays/room_m0404i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0404i/func_801935E0.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0404i — func_801935E0, blob offset 0x45F8, 0x48C bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback a0 = mode (1 step, 2 draw), a1 = object, state at
 * +0x10, frame at +0x12.
 * mode 1 state 0: ballistic step (vx/vz decay 255/256, vy gravity +1 with a
 * bounce off the floor D_800942EC), position z/x advanced by the sine/cosine
 * of frame << 9 / 512; ends at frame 0x40. State 1: same with 127/128 decay,
 * gravity +2 and a plain bounce, ends at frame 0x20.
 * mode 2 state 0: SetRot/SetTrans from D_800BCFA4[0] and two func_800D2104
 * draws (object position, then position with y + 8). State 1: sets the
 * D_800F3368 draw block, picks a colour from the D_80193F68 curve and draws
 * the sprite through func_800CEE20.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0404i func_801935E0 --check-gte --uncovered --seeds 512
    e21_leaf: ok func_801935E0__37010045_port.c 983 seeds, retail coverage 291/291 words
    e21_leaf: uncovered none

## Uncovered words

No word is left uncovered by the union of the two runs. The main run alone missed these 7 words, and the supplementary run executed all of them:
- 80193654, 80193658: the `mode != 1 && mode != 2` default arm (`j` to the epilogue). Reached with a0=0.
- 80193674, 80193678: the mode 1 arm with state not 0 or 1. Reached with a0=1 and state 2.
- 801938A4, 801938A8: the mode 2 arm with state not 0 or 1. Reached with a0=2 and state 2.
- 80193A10: the `addiu 3` fix-up for negative `D_800F336A * (f / 4)`, where f is the frame `lh 0x12`. Reached with a negative frame (-5, -6) in mode 2 state 1.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0404i func_801935E0 --nonmatching 45F8:48C --check-gte --uncovered --seeds 512 --inputs "a0=1,2; h(a1+0x10)=0,1; h(a1+0x12)=0x3E,0x3F,0x1E,0x1F; h(a1+0xA)=5,-20; h(0x800942EC)=0x7FFF,-0x8000; h(0x800F336C)=4,0; w(0x800F3428)=1,0; h(a1+8)=0x100,-0x100; out=800CF3AC:1:3"`
  - e21_leaf: ok func_801935E0__37010045_port.c 471 seeds, retail coverage 284/291 words | e21_leaf: uncovered 80193654 80193658 80193674 80193678 801938A4 801938A8 80193A10 (exactly the claimed 7)
- `python3 tools/analysis/e21_leaf.py room_m0404i func_801935E0 --nonmatching 45F8:48C --check-gte --uncovered --seeds 12 --inputs "a0=0,1,2; h(a1+0x10)=1,2; h(a1+0x12)=-5,-6; out=800CF3AC:1:3"`
  - e21_leaf: ok func_801935E0__37010045_port.c 12 seeds, retail coverage 159/291 words; its uncovered list does not contain any of the 7 words (starts at 8019367C, and 80193A10 is absent), so the union is 291/291
- `NEW1: ... --seeds 256 --inputs "a0=1; h(a1+0x10)=0,1; h(a1+0x12)=-1,0x3F,0x1F,0x7FFF; h(a1+0xA)=0x7FFF,-0x8000,-1,0; h(0x800942EC)=0x7FFF,-0x8000; h(a1+8)=-1,1; h(a1+0xC)=-0x8000,3"`
  - NEW1: e21_leaf: ok ... 256 seeds, retail coverage 171/291 words (no MISMATCH, no notest, no skipped)
- `NEW2: ... --seeds 128 --inputs "a0=2; h(a1+0x10)=1; h(a1+0x12)=-7,-4,-1,0,3,0x7FFF,-0x8000,5; h(0x800F336C)=0,4,5,0x7FFF; w(0x800F3428)=0,1,-1; out=800CF3AC:1:3" (all with --nonmatching 45F8:48C --check-gte --uncovered)`
  - NEW2: e21_leaf: ok ... 128 seeds, retail coverage 107/291 words (no MISMATCH, no notest, no skipped)

No refutation found. Both claimed runs reproduce exactly. The two new edge-value runs cover mode 1 (state 0/1, 16-bit wrap of vy via 0x7FFF/-0x8000, negative vx/vz, /256 and /128 rounding, bounce/no-bounce at both floor limits) and mode 2 state 1 (negative, zero, 0x7FFF and -0x8000 frames, the /4 and mult rounding, D_800F336C 0/4/5/0x7FFF, D_800F3428 0/1/-1). All print ok. Reading the asm against the C, I found no divergence in load signedness, the sra/addiu rounding fix-ups, call argument order, the D_800F3368 stores, or the return value (0 on every path except mode 1 returning 1 once the frame reaches 0x40 or 0x20). The 7 uncovered words are reachable and were executed by the supplementary run, so the claim's justification holds. Caveat: the runs only prove equivalence over the sampled inputs, and the callee behaviour comes from the harness stubs, so callee results that change branches are only as well explored as the harness allows. Runs are slow, at several minutes each."

