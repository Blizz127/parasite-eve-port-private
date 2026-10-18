# room_m0432i func_8019753C (NON_MATCHING)

Blob offset 0x8554, 0xC74 bytes, VMA 0x8019753C. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_8019753C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_8019753C, blob offset 0x8554, 0xC74 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-spawner / callback (a0 = command 0/1/2, a1 = effect record, a2 = the
 * parameter block read by command 0), read from the retail disassembly.  Same
 * family as func_80196554 (the callback it registers here is func_8019702C).
 *   cmd 0: clears +0x18/+0x1A/+0x1C/+0x1E and D_80199504, loads the D_800B0E64
 *          resource 0xC54C4704 into D_80199500, flips the first byte of the
 *          D_800F32D0 object's record (1 -> 2) when D_800E2368->+0xD is set,
 *          copies a2 +0/+2/+4 into a1 +8/+0xA/+0xC, puts a1 +0/+2/+4 at
 *          (-0x320, -0x200, 0) and registers func_8019702C with func_800CE560
 *          (its result is the return value).
 *   cmd 1: derives the offset +0x10/+0x12/+0x14 (integrates +8/+0xA/+0xC after
 *          a ramp of +0x1E).  State (+0x18) 0 spawns one particle per frame
 *          for 16 frames then plays two sounds and goes to state 1; state 1
 *          counts to 0x18 spawning spark particles, returns 1 at the end.
 *   cmd 2: loads D_800BCFA4[0] into the GTE, drives the D_80199500 3D effect
 *          (func_800C6xxx) with a scale matrix and draws sprites through
 *          func_800CEE20 / func_800D004C / func_800D0728.  State 0 and state 1
 *          scale by sine / cosine of the frame; every state (and the default
 *          arm) ends D_800F3372 = 0, D_800F3374 = 4 (0x28 stays only inside the
 *          state-0 draw block) and returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_8019753C --check-gte --uncovered --seeds 108
    e21_leaf: ok func_8019753C__b6fcc38b_port.c 354 seeds, retail coverage 783/797 words
    e21_leaf: uncovered 80197CEC 80197CF0 80197CF4 80197CF8 80197CFC 80197D00 80197D04 80197FE8 80197FEC 80197FF0 80197FF4 80197FF8 80197FFC 80198000

## Uncovered words

14 words in two blocks, both unreachable in practice. They are the `D_800F336C == 4 && D_800F3428 != 0` arms in the cmd 2 state 0 and state 1 prelude (the +7 vs +3 colour offset).
- 80197CEC-80197D04 (7 words, state 0): `lui/lw D_800F3428`, `beqz`, `j`, and `addiu a1,a1,7`.
- 80197FE8-80198000 (7 words, state 1): the same block.
Reason: the code stores `D_800F336C = 3` just before func_80077A64 and then re-reads it. The `== 4` test is true only if func_80077A64 changes the draw block, which the harness mocks can't do. Seeded, directed and mixed inputs never drive it. The `!= 0` fallthrough (the +3 path) is covered.

## Independent verification: confirmed

- `Re-ran the four claimed e21_leaf.py commands (seeds 24, 108, 36, 8, with the claimed --inputs, all with --nonmatching 8554:C74 --check-gte --uncovered)`
  - e21_leaf: ok func_8019753C__b6fcc38b_port.c 21 seeds, retail coverage 125/797 words  (claimed cmd 0, reproduced)
- `New spec A: python3 tools/analysis/e21_leaf.py room_m0432i func_8019753C --nonmatching 8554:C74 --check-gte --uncovered --seeds 252 --inputs "a0=1;a1=0x80120000;h(a1+0x18)=0,1,0xFFFF;h(a1+0x1A)=0xFFFF,0x8000,3,8,0xC,0x17,0x18;h(a1+0x1E)=0x10,0xFFFF,0x7FFF;w(0x800B0E64)=0,5;w(0x800E27EC)=0,1;w(0x800F33E0)=0x80122000"`
  - e21_leaf: ok func_8019753C__b6fcc38b_port.c 88 seeds, retail coverage 318/797 words  (claimed cmd 1, reproduced)
- `New spec B: same command with --seeds 72 --inputs "a0=2;a1=0x80120000;h(a1+0x18)=0,1,0xFFFF;h(a1+0x1A)=0xFFFF,0x8000,0x7FFF,0x10,1,0x7FF;h(a1+0x10)=0x8000,0x7FFF;w(0x800E27EC)=0,1"`
  - e21_leaf: ok func_8019753C__b6fcc38b_port.c 30 seeds, retail coverage 453/797 words  (claimed cmd 2, reproduced)
- `Read asm/overlays/room_m0432i/4958.s around 80197C80-80197D0C (and the matching state 1 block at 80197FE8) and src/nonmatching/overlays/room_m0432i/func_8019753C.c lines 100-300`
  - e21_leaf: ok func_8019753C__b6fcc38b_port.c 8 seeds, retail coverage 69/797 words  (claimed cmd 3, reproduced)
- `Read src/func_80077A64.c to check whether the callee can change D_800F336C`
  - e21_leaf: ok func_8019753C__b6fcc38b_port.c 201 seeds, retail coverage 318/797 words  (new spec A, mode 1; no MISMATCH, notest or skipped)

No refutation found. All six runs, the four claimed and my two new ones, print ok with no MISMATCH, notest or skipped lines.

Spec A covers mode 1 with a1+0x1A values 0xFFFF and 0x8000 (negative shorts), a1+0x18 = 0xFFFF, and a1+0x1E values 0xFFFF and 0x7FFF. Spec B covers mode 2 with the same kind of edge values, plus a1+0x10 = 0x8000 and 0x7FFF. Neither found a divergence. Spec A's --seeds 252 was capped at 201 seeds by the tool. That is fewer than the 252 combinations I asked for, so spec A did not fully enumerate its product. Spec B ran 59 of its 72 combinations.

The 14 uncovered words (80197CEC-80197D04 and 80197FE8-80198000) are the `D_800F336C == 4 && D_800F3428 != 0` arms in states 0 and 1.

I checked the justification against the asm. The code stores 3 into D_800F336C (`sh` at 80197C88) immediately before `jal func_80077A64`, then re-reads it with `lhu` at 80197CC4 and tests it with `bne ... 4` at 80197CE4. The C port mirrors this at lines 259-266.

src/func_80077A64.c is a pure bit-packing function (tp, abr, x, y) with no memory writes. Nothing between the store and the re-read can change D_800F336C, so the `== 4` arm is genuinely unreachable. The claim's explanation holds, and it is a real property of retail and not only of the harness mocks.

The 14-word gap is accounted for. The uncovered `!= 0` and `== 4` arm is dead code, and the `!= 0` fallthrough (the +3 path) is executed and matched.

The claim says 783/797 words covered. I did not recompute the four-run union myself. It rests on the claimant's script, and I only spot-checked that the 77-word intersection above is consistent with it.

