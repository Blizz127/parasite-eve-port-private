# room_m0318i func_801955E4 (NON_MATCHING)

Blob offset 0x65FC, 0x320 bytes, VMA 0x801955E4. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_801955E4.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_801955E4, blob offset 0x65FC, 0x320 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, a0 = mode, a1 = effect record, a2 = parameter block.
 *   mode 0: clear a1 +8/+0xA, seed the sprite pool (func_800D3F64(0x5EA, ...)),
 *           register func_80195190 with the pool at D_800F33E0->+8; returns
 *           that call's result (v0 is not cleared on this path).
 *   mode 1: advance frame a1 +0xA, call func_800CE870, move a1 +2 by a ramp
 *           of a2 (+8 * frame / +4), spawn trail particles every 4th frame
 *           (while frame <= a2 +4), returns 1 once frame >= a2 +4 + 8.
 *   mode 2: load the rotation/translation matrix D_800BCFA4[0] into the GTE
 *           and latch the a1 position into D_80199924/26/28.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_801955E4 --check-gte --uncovered --seeds 120
    e21_leaf: ok func_801955E4__02b76f64_port.c 97 seeds, retail coverage 196/200 words
    e21_leaf: uncovered 801956E0 801956F0 801956F4 801956F8

## Uncovered words

801956E0 (break 7): divide-by-zero trap when a2+4 == 0; retail traps, host C would fault, and the duration word is never zero in the callers. 801956F0/801956F4/801956F8 (bne $a0,0x80000000 then break 6): signed-division overflow check, only reachable when divisor == -1 and a2+8 * frame == 0x80000000; a2+4 is a positive duration, so it is provably impossible in practice. All other words (all modes 0/1/2, the default return, both signed-modulo arms, the s0==0 skips) are covered.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0318i func_801955E4 --nonmatching 65FC:320 --check-gte --uncovered --seeds 120 --inputs "a0=0,1,2,3,-1;h(a1+0xA)=2,3,27,40;w(a2+0)=0,1;w(a2+4)=20;w(a2+8)=-7,300;w(a2+0xC)=5"`
  - e21_leaf: ok func_801955E4__02b76f64_port.c 97 seeds, retail coverage 196/200 words | e21_leaf: uncovered 801956E0 801956F0 801956F4 801956F8 (matches the claim exactly)
- `python3 tools/analysis/e21_leaf.py room_m0318i func_801955E4 --nonmatching 65FC:320 --check-gte --uncovered --seeds 96 --inputs "a0=1;h(a1+0xA)=-5,0,3,7;w(a2+0)=0,1;w(a2+4)=-3,1,20;w(a2+8)=-7,300;w(a2+0xC)=0,5"`
  - e21_leaf: ok func_801955E4__02b76f64_port.c 48 seeds, retail coverage 132/200 words (mode-1 only; negative/positive duration, negative divisor, negative/positive product, s0 zero and nonzero, frame 0/3/7 incl. every-4th-frame particle path) | uncovered includes 801956E0/F0/F4/F8
- `python3 tools/analysis/e21_leaf.py room_m0318i func_801955E4 --nonmatching 65FC:320 --check-gte --uncovered --seeds 72 --inputs "a0=1;h(a1+0xA)=-1,2,0x7FFF;w(a2+0)=-1,0;w(a2+4)=7,100;w(a2+8)=-0x80000000,0x7FFFFFFF,77;w(a2+0xC)=0,-0x8000"`
  - e21_leaf: ok func_801955E4__02b76f64_port.c 26 seeds, retail coverage 130/200 words (INT_MIN/INT_MAX a2+8, frame 0x7FFF, s0 = -1) | uncovered includes 801956E0/F0/F4/F8
- `python3 tools/analysis/e21_leaf.py room_m0318i func_801955E4 --nonmatching 65FC:320 --check-gte --uncovered --seeds 30 --inputs "a0=-2147483648,2147483647,4,-3,0x10000;h(a1+0xA)=3;w(a2+0)=0;w(a2+4)=20;w(a2+8)=5;w(a2+0xC)=5"`
  - e21_leaf: ok func_801955E4__02b76f64_port.c 30 seeds, retail coverage 28/200 words (default arm for a0 = INT_MIN, INT_MAX, 4, -3, 0x10000) | no MISMATCH

No MISMATCH, notest or skipped in any run. I read the asm and C side by side and found no divergence. Loads: lhu is used for the frame increment and the position copies, and lh for the compares. The signed `% 32` matches the bgez/addiu 0x1F/sra/sll/subu sequence. The a1+0xA store, the `slt` of a2+4 against the frame (C `W(a2,4) >= n`), and the unconditional second func_800CE610 call all match. The func_80071A54 call order is correct: the fourth call is discarded with its delay-slot store of +8 kept, and the fifth feeds +0xC. Call arguments match: func_800CE870(W(D_800F32D0,8), 0, a1), func_800D3F64(0x5EA, func_800D3FD8()), and func_800CE560(W(D_800F33E0,8), 0x14, 0x20, func_80195190). Mode-0's return value is func_800CE560's result, as claimed. Mode 2 stores the same GTE control regs and globals.\n\nCaveat on the uncovered words. 801956E0 (break 7, a2+4 == 0) and 801956F0/F4/F8 (break 6) were all uncovered in every run, as claimed. Both are retail traps. The break 6 path is reachable by a constructible input, for example a2+4 = -1 with a2+8 * frame == 0x80000000 (a2+8 = 0x40000000, frame 2). The claim's 'provably impossible' rests only on a2+4 being a positive duration in the callers, which is an assumption that nobody has checked against the callers. In both cases retail traps and the host C would fault (a SIGFPE on x86 for INT_MIN / -1). So the gap is the same kind as the break 7 one and not a behavioural mismatch. It is a documented non-covered trap path, not a refutation.\n\nOperational note. My first, oversized run (about 8700 combinations at 1152 seeds) was too slow and I killed it, so it produced no result. Only the four runs listed above completed. They used --seeds >= the product of the lists, and the tool deduplicated them to fewer effective seeds. The new specs did not independently reach the default arm for -1 / mode 2 together with mode 1. That came from the original spec (a0=0..3,-1), which covers mode 0, 1, 2 and the default."]

