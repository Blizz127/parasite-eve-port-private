# room_m0318i func_80192ED4 (NON_MATCHING)

Blob offset 0x3EEC, 0x398 bytes, VMA 0x80192ED4. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80192ED4.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80192ED4, blob offset 0x3EEC, 0x398 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, a0 = mode, a1 = effect record, a2 = parameter block.
 *   mode 0: a1 +8 = func_80071A54() (RNG), a1 +0xC = 0; walk the D_8009D20C
 *           list (stop at the D_8009D254 sentinel / null) for a node whose +0
 *           pointer has a positive +0x10 and whose bytes +0xD/+0xC equal
 *           a2 +0 / +4, then func_800CE870(node, 0, a1); else printf
 *           D_8018F1CC. Returns func_800CE560(D_800F33E0->+8, 8, 0x18,
 *           func_80192DA0).
 *   mode 1: while D_800E27EC < 0x33 spawn a particle in the D_800F33E0->+8
 *           pool: +0/+4 = a1 +0/+4 plus cos/sin(a1 +8) * a1 +0xC / 4096,
 *           +2 = a1 +2, +6 = RNG & 3; a1 +8 advances by 0x8AA + (RNG & 0x1F).
 *           Returns 1 once D_800E27EC >= 0x4A, else 0.
 *   mode 2: D_800F3374 = 8; while D_800E27EC < 0x33 draw a 0x15E/0x1F4/0x14
 *           func_800D0728 ring at a1 +0/+2/+4 (a1 +0xC = sin(frame * 1024 / 50)
 *           / 8) using the D_801994BC colour table; then set the
 *           D_800F3368.. draw-state shorts. Returns 0.
 * a0 outside 0..2 returns 0. Retail leaves no stack read-before-write in this
 * function (the mode 2 frame at sp+0x30..0x47 is written in full before the
 * func_800CF3AC / func_800D0728 calls).
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80192ED4 --check-gte --uncovered --seeds 384
    e21_leaf: ok func_80192ED4__7349384f_port.c 1536 seeds, retail coverage 230/230 words
    e21_leaf: uncovered none

## Uncovered words

None. No word is uncovered once the four runs are combined. Per-run uncovered lists are just the other modes' code. Directed branch arms driven in the final specs:
- Mode 0: list head null; p == D_8009D254 sentinel; *p == 0; +0x10 <= 0 (0 and -1); +0xD byte mismatch; +0xC byte mismatch; key wider than a byte (0x105); loop back-edge taken (second node matches after the first fails); not-found printf path; found path.
- Mode 1: D_800E27EC at 0x1F, 0x32, 0x33, 0x49, 0x4A and -1; func_800CE610 returning 0 and nonzero; both signs of the product for the /4096 rounding fix-up (a1+0xC = 0x1000, -0x1000, 5, -5, 0).
- Mode 2: D_800E27EC at 0, 1, 0x31, 0x32, 0x33, -1 and INT_MAX; D_800E11FA at 0, 3 and 0x7FFF.
- Default arm: negative a0, a0 = 3 and a0 = 5.

## Independent verification: confirmed

- `Re-ran all four claimed e21_leaf.py commands for room_m0318i func_80192ED4 (--nonmatching 3EEC:398 --check-gte --uncovered), with seeds 384, 180, 21 and 5 and the exact claimed --inputs.`
  - e21_leaf: ok func_80192ED4__7349384f_port.c 384 seeds, retail coverage 76/230 words
- `New spec A: e21_leaf.py room_m0318i func_80192ED4 --nonmatching 3EEC:398 --check-gte --uncovered --seeds 72 --inputs "a0=1;a1=0x80130000;w(0x800F33E0)=0x80150000;h(0x80130000)=0,0xFFFF;w(0x8013000C)=0x7FFFFFFF,0x80000000,0xFFFFEFFF,0x1001,1,0xFFFFFFFF;w(0x80130008)=0,0x7FFFFFFF;w(0x800E27EC)=0x32,0x49,0x4A;r(0x800CE610)=0x80140000"`
  - e21_leaf: ok func_80192ED4__7349384f_port.c 180 seeds, retail coverage 78/230 words
- `New spec B: e21_leaf.py room_m0318i func_80192ED4 --nonmatching 3EEC:398 --check-gte --uncovered --seeds 40 --inputs "a0=2;a1=0x80130000;h(0x80130000)=0,0xFFFF;w(0x800E27EC)=0x80000000,0xC0000000,0xFFFFFF00,1,0x32;h(0x800E11FA)=0xFFFF,0x8000;out=800CF3AC:1"`
  - e21_leaf: ok func_80192ED4__7349384f_port.c 21 seeds, retail coverage 107/230 words

No MISMATCH, notest or skipped line in any run. The four claimed runs and my two new specs all printed ok.

Side-by-side read of asm/overlays/room_m0318i/3EEC.s lines 12-258 against the C found no behavioural difference on any path.
- Dispatch: slti a0,2 is signed, so a negative a0 falls to 0 and a0 >= 3 returns 0. The C default arm matches.
- Mode 0: the keys are words but are compared against lbu bytes, which the C reproduces. The unreachable-by-input worry is the sentinel, which retail reloads from D_8009D254 on every iteration while the C loads it once. No call sits inside the loop, so this cannot differ. The found path passes (p, 0, a1) to func_800CE870 and the not-found path passes (D_8018F1CC, key0, key1) to func_80071A74. The a1+0xC = 0 store and the final func_800CE560(D_800F33E0->+8, 8, 0x18, func_80192DA0) call match.
- Mode 1: the product is a 32-bit mflo with round-toward-zero /4096 (addiu 0xFFF if negative, then sra 12), so the C's v / 4096 is equivalent. The +0xC value is read after each call and the final D_800E27EC < 0x4A re-read is signed. Spec A covers the overflow and negative products.
- Mode 2: stack arguments are sp+0x10 = ang, +0x14 and +0x18 = s0, +0x1C = 0, +0x20 = colour, +0x24 = 0x80, +0x28 = 1. The C's f struct layout lines up with these (colour at sp+0x30, position at sp+0x38, angle at sp+0x40). The /50 magic-multiply and the +7 >> 3 rounding match signed division. D_800E11FA is read with lhu, so the index is unsigned, and the D_800F3368..3378 short stores match.

Uncovered-word check: each run's uncovered list is the other modes' code or the other dispatch arms. Mode 0 never reaches the 0x80193004+ words, mode 1 never reaches the 0x80192EF8-0x80192FFC mode 0 words, and the mode 2 words are exercised only in run 3. I constructed no input that reaches a word the claim calls unreachable. The union of the four runs' coverage was not recomputed by me; I only checked the per-run uncovered lists against the asm.

Caveats: the runs were slow because the machine was loaded, and I did not independently recompute the union of coverage across the four runs. The callee mocks (func_80077CF4, func_80077DC4, func_80071A54) are the harness's seeded ones, so trig-dependent results are only as varied as those mocks."

