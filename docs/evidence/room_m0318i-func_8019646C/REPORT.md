# room_m0318i func_8019646C (NON_MATCHING)

Blob offset 0x7484, 0x3B4 bytes, VMA 0x8019646C. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_8019646C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_8019646C, blob offset 0x7484, 0x3B4 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback: a0 = command (1 update, 2 draw), a1 = effect record
 * (+0/+2/+4 position, +6, +8/+0xA/+0xC vector, +0x10 state, +0x12 frame).
 * cmd 1 state 0 waits 32 frames; state 1 integrates +8/+A/+C += +0/+2/+4 for
 * 40 frames. cmd 2 state 0 draws with func_800D0728 (colour from D_80199798,
 * scale sin(f << 5)); state 1 draws the sprite with func_800D2370, size
 * sin(f << 5) * 0x76C >> 12 (f <= 0x20), height cos(f*1024/40) / 64 + 0x20,
 * CLUT from D_800E1204[D_800F336C] (+4 when D_800F336C == 4 and D_800F3428).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_8019646C --check-gte --uncovered --seeds 300
    e21_leaf: ok func_8019646C__c8a7499b_port.c 581 seeds, retail coverage 237/237 words
    e21_leaf: uncovered none

## Uncovered words

The main directed run leaves 6 words uncovered: 801964C8 and 801964CC (the `j .L801967F4` and its delay-slot nop, taken when a0 is neither 1 nor 2), 801964E8 and 801964EC (the same pair when a0=1 and a1+0x10 is neither 0 nor 1), and 80196584 and 80196588 (the same pair when a0=2 and a1+0x10 is neither 0 nor 1). All six are default arms that return 0. The supplementary run (a0=0,1,2,3; state 2, 0xFFFF, 5) executes them and prints ok. No word is left uncovered in the union.

## Independent verification: confirmed

- `R1 (claim cmd 1): e21_leaf.py room_m0318i func_8019646C --nonmatching 7484:3B4 --check-gte --uncovered --seeds 300 --inputs "a0=1,2;h(a1+0x10)=0,1;h(a1+0x12)=0,1,2,0x1F,0x20,0x21,0x27,0x28,3;h(0x800F336C)=4,0;w(0x800F3428)=1,0"`
  - R1: e21_leaf: ok func_8019646C__c8a7499b_port.c 281 seeds, retail coverage 231/237 words | e21_leaf: uncovered 801964C8 801964CC 801964E8 801964EC 80196584 80196588
- `R2 (claim cmd 2): ... --seeds 60 --inputs "a0=0,1,2,3;h(a1+0x10)=2,0xFFFF,5;h(a1+0x12)=1,0x7FFF,0x8000,0xFFFF,0x20"`
  - R2: e21_leaf: ok func_8019646C__c8a7499b_port.c 60 seeds, retail coverage 52/237 words | uncovered list begins at 801964F0 and does not contain 801964C8/CC, 801964E8/EC, 80196584/88, so the union with R1 is 237/237
- `N1: ... --seeds 1000 --inputs "a0=2;h(a1+0x10)=1;h(a1+0x12)=0,1,2,3,0x1F,0x20,0x21,0x22,0x7FFF,0x8000,0xFFFF,0xFFFE,0xFFFD,0xFFE0,0x100,0x3FF,0x400,0x401;h(0x800F336C)=0,1,2,3,4,5,6,7;w(0x800F3428)=0,1,0xFFFFFFFF"`
  - N1 (cmd2 state1, edge frame values, all D_800F336C, D_800F3428 0/1/-1): ok 762 seeds, retail coverage 168/237 words
- `N2: ... --seeds 400 --inputs "a0=1;h(a1+0x10)=0,1;h(a1+0x12)=0x1E,0x1F,0x20,0x26,0x27,0x28,0x7FFF,0x8000,0xFFFF;h(a1+0)=0,1,0x7FFF,0x8000,0xFFFF;h(a1+2)=0,0xFFFF,0x8000;h(a1+4)=0,0xFFFF,0x7FFF;h(a1+8)=0,0xFFFF;h(a1+0xA)=0,0xFFFF;h(a1+0xC)=0,0x8000"`
  - N2 (cmd1 states 0/1, frame and vector edge values incl. 16-bit wrap): ok 400 seeds, retail coverage 70/237 words
- `N3: ... --seeds 300 --inputs "a0=2;h(a1+0x10)=0;h(a1+0x12)=0,1,0x1F,0x20,0x7FFF,0x8000,0xFFFF,0xFFFF"`
  - N3 (cmd2 state0, frame edges incl. 0x7FFF/0x8000/0xFFFF): ok 300 seeds, retail coverage 64/237 words
- `N4: ... --seeds 100 --inputs "a0=0x10001,0xFFFFFFFF,0x80000001;h(a1+0x10)=0,1"`
  - N4 (a0 = 0x10001, 0xFFFFFFFF, 0x80000001, i.e. full 32-bit compare against 1 and 2): ok 100 seeds, retail coverage 36/237 words

No MISMATCH, notest or skipped output in any run. R1 reproduced the claim exactly, including the same six uncovered words. R2 covers those six words, so the claim's union of 237/237 holds. The six uncovered words are the default-arm `j .L801967F4` plus delay-slot nop pairs. They are reachable only with a0 not 1 or 2 (801964C8/CC), or with state not 0 or 1 under a0=1 (801964E8/EC) or a0=2 (80196584/88). The asm shows each pair returns 0, and the C returns 0 on those paths too. The claim's justification is correct, and the words are reachable and were executed by R2.

I read the asm and the C side by side and found no divergence.
- The lh/lhu loads, the signed slti compares against 0x20/0x28/0x21, and the `sll 10` then divide-by-40 all match the C.
- The 0x66666667/0x55555556 multiply-high sequences correspond to /40 and *2/3 with round-toward-zero.
- The /32 and /64 bgez adjustments and the 0x76C*sin>>12 (+0xFFF for negatives) match.
- The call order is 77DC4, 77CF4, 77DC4 in both versions.
- Stack-passed arguments and the 0x48 buffer copy match.
- The `+4` CLUT adjustment is gated on D_800F336C==4 and D_800F3428!=0, and N1 exercised all eight D_800F336C values and D_800F3428 of 0, 1 and 0xFFFFFFFF.

Two caveats, neither a refutation. N2 and N1 had seed counts below the product of their input lists, so they were sampled, not exhaustive (N1 reported 762 seeds). The claim's two commands are written joined by a pipe. Run separately they behave as described.

