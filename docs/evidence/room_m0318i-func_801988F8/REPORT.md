# room_m0318i func_801988F8 (NON_MATCHING)

Blob offset 0x9910, 0x920 bytes, VMA 0x801988F8. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_801988F8.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_801988F8, blob offset 0x9910, 0x920 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect main for the sprite/model burst of this room, read from the retail
 * disassembly (7E64.s). a0 = mode, a1 = effect record (+0/+2/+4 position,
 * +8/+A/+C counters, written here).
 * mode 0: clear a1 +8/+A/+C; if D_800B0E64 is set, play sound 0x5ED (volume
 *         func_800D3FD8()) and, if it is still set, 0x5EE (func_8006DF50);
 *         load the model 0xC5509704 into D_80199944 (func_8006E498 +
 *         func_800C6D5C); when D_800E2368[0xD] is set flip the state byte at
 *         D_800F32D0->+8->+0->+0x18 from 1 to 2; run func_800CE9D4 and start
 *         the 0x14-slot pool with the particle callback func_80198268
 *         (returns func_800CE560's result).
 * mode 1: a1 +A is the frame (a1 +C = sin((f << 10) / 24) / 32); func_800CE870
 *         fills the stack vector with the target position. For f < 0x19 it
 *         spawns three pool particles at that position: a scattered spark
 *         (+0..+6 jittered by 6-bit randoms, a1 position at +8..+C, type 1),
 *         on odd frames a wide one (9-bit jitter and %32 velocities, type 0)
 *         and a small one (5-bit jitter, type 2). Then, if the position is
 *         within 0x19A of the player, D_800E2368[0xD] is set and the hit
 *         object word has the 0x01000000 state, it flags the D_8009D254
 *         record (+0x4C |= 0x4000) and moves the hit word to state 0x19
 *         with the top bit set. Returns 2 once f >= 0x18, else 0.
 * mode 2: GTE rotation/translation from D_800BCFA4[0]; f >= 0x19 only resets
 *         D_800F3372/D_800F3374 (4). Otherwise it draws the 0x384 x 0x320
 *         ring (func_800D004C) with the D_80199890 colour curve and a model
 *         at the target position + a1 offset with a sin/cos-sized scale
 *         (func_80078CC4) and frame a1 +C (x 3/4 on odd frames), tpage and
 *         CLUT from the D_800E2850 / D_800E1204 tables. Returns 0.
 * The 4th halfword of the stack angle vector (sp+0x46) is never written
 * (the vector is passed to func_80079754).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_801988F8 --check-gte --uncovered --seeds 448
    e21_leaf: ok func_801988F8__52031b08_port.c 1792 seeds, retail coverage 578/584 words
    e21_leaf: uncovered 8019911C 80199120 80199124 80199128 8019912C 80199130

## Uncovered words

8019911C-80199130 (6 words: lui/lw D_800F3428, nop, beqz, nop, and the addiu $a2,$a2,4 in the CLUT-index fix-up). This is the "D_800F336C == 4 && D_800F3428" arm. Retail stores 3 to D_800F336C immediately before and reads it back after only the func_80077A64 call, which the harness mocks, so no seed can reach it. The C re-reads D_800F336C after that call, so the arm is still faithful if the real callee ever changed it.

## Independent verification: confirmed

- `Claim run 1: e21_leaf.py room_m0318i func_801988F8 --nonmatching 9910:920 --check-gte --uncovered --seeds 448 --inputs <mode 1 spec, exactly as claimed>`
  - e21_leaf: ok func_801988F8__52031b08_port.c 448 seeds, retail coverage 272/584 words
- `Claim run 2: same tool, --seeds 120, mode 2 spec as claimed`
  - e21_leaf: ok func_801988F8__52031b08_port.c 120 seeds, retail coverage 244/584 words
- `Claim run 3: same tool, --seeds 32, mode 0 spec as claimed`
  - e21_leaf: ok func_801988F8__52031b08_port.c 32 seeds, retail coverage 107/584 words
- `Claim run 4: same tool, --seeds 3, inputs a0=-1,3,0x7FFFFFFF; a1=0x80180800`
  - e21_leaf: ok func_801988F8__52031b08_port.c 3 seeds, retail coverage 35/584 words
- `New run A: --seeds 192 --inputs "a0=2; a1=0x80180800; h(a1+0)=0,0x8000; h(a1+2)=0,0xF800; h(a1+0xA)=0,0xFFFF,0x18; h(a1+0xC)=0x8001,0x7FFF,1,0xFFFF; w(0x800E27EC)=0,1; h(0x800E11EA)=0,1; w(0x800BCFA4)=0x80180C00; w(0x800F32D0)=0x80180100; w(0x80180108)=0x80180200; out=800CE870:2:6"`
  - A: e21_leaf: ok func_801988F8__52031b08_port.c 192 seeds, retail coverage 244/584 words (negative/odd a1+0xC for the 3/4 scale, a1+0xA = 0, -1 and 0x18, a1 angle halfwords at sign limits, D_800E11EA 0/1)
- `New run B: --seeds 192 --inputs "a0=2; a1=0x80180800; h(a1+0xA)=5; h(a1+0xC)=0x8001; h(0x800E11EA)=0,1; h(0x800E2850)=0,0x8000; h(0x800E2852)=1,0xFFFF; h(0x800E120A)=0,0x1234; r(0x80077A64)=0,0xFFFF0000,-1; r(0x80077AA4)=0,0x12345678; r(0x80077CF4)=0x1234,-0x1235; w(0x800E27EC)=1; w(0x800BCFA4)=0x80180C00; w(0x800F32D0)=0x80180100; w(0x80180108)=0x80180200; out=800CE870:2:6"`
  - B: e21_leaf: ok func_801988F8__52031b08_port.c 192 seeds, retail coverage 244/584 words (varied table entries, callee returns for func_80077A64/80077AA4/80077CF4 including negatives and 0xFFFF0000)

I found no mismatch, notest or skipped in any of the six runs. All four claimed runs reproduced the claimed ok lines and coverage counts (272, 244, 107 and 35 of 584 words). Both new mode-2 runs printed ok at 244/584.

The coverage line for each new run is the same as the claimed mode-2 run. I read that as the same arms being hit, but I did not compare the full uncovered lists beyond what the log shows. The log lines were truncated at 200-260 characters, so I could not see whether 8019911C-80199130 was in the new runs' uncovered lists. I checked the justification against the asm instead.

The dead arm at 8019911C-80199130 holds up under the mocks. In asm/overlays/room_m0318i/7E64.s (lines 2335-2352), retail loads D_800F336C into $a1 at 801990EC, after the jal to func_80077A64. It then branches on $a1 != 4 at 80199114. The only earlier writes to D_800F336C in this function are the constant 3, written before the call. A mocked func_80077A64 cannot change it, so the D_800F3428 arm is unreachable by any seed. The C re-reads D_800F336C after the call, so it stays faithful if the real callee ever writes 4.

I could not check that retail never reaches this arm when the real func_80077A64 runs. The harness only covers the mocked callee.

The run that printed the 448-seed result was started in a work dir shared with the original worker. It already held run1.log and run23.log, but I did not read them. The ok lines come from my own runs.

