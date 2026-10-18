# room_m0318i func_80198268 (NON_MATCHING)

Blob offset 0x9280, 0x690 bytes, VMA 0x80198268. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80198268.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80198268, blob offset 0x9280, 0x690 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Sibling of func_801944E8 in the same room (and of the m0429i/m0269i
 * particle callbacks), read from the retail disassembly (7E64.s).
 * a0 = command, a1 = effect object.
 * cmd 1: state 0 integrates the velocity (+8/+A/+C) into the position
 * (+0/+2/+4) with 50/51 damping of vx/vz and a vy gravity step that is
 * negated once the object falls below D_800942EC (16 frames); state 1 adds
 * 0x32 to +0xE for 24 frames; state 2 drifts y down by 1 for 24 frames.
 * cmd 2: state 0 draws the sprite on a sin-sized glow at {x, y, -(f<<5)}
 * (D_800F3368 block 0x20, +2 = 2) with a colour curve D_80199890; state 1
 * places the glow at velocity-vector * sin(f*64) / 4096 (func_800CF844) and
 * draws it 3/2 sin-sized with size 4 and a 0x80 tint; state 2 draws a cos
 * sized sprite (block 0x40, +2 = 4) with a 1/32 cos tint and sets
 * D_800F3374 = 4 afterwards.
 * Return: cmd 1 returns 1 when the timer ends, otherwise 0.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80198268 --check-gte --uncovered --seeds 300
    e21_leaf: ok func_80198268__d68e9481_port.c 600 seeds, retail coverage 400/420 words
    e21_leaf: uncovered 80198550 80198554 80198558 8019855C 80198560 80198564 8019870C 80198710 80198714 80198718 8019871C 80198720 80198724 80198864 80198868 8019886C 80198870 80198874 80198878 8019887C

## Uncovered words

Remaining 20 words, all provably impossible, in three groups.
- 80198550-80198564 (6 words: lui, lw D_800F3428, beqz, nop, addiu +4): cmd 2 state 0 stores D_800F336C = 2 immediately before the `k == 4` test, and the mocked func_800CF3AC cannot change it, so `bne v1,4` is always taken.
- 8019870C-80198724 (7 words: the `D_800F3428 != 0` / `+7` arm): cmd 2 state 1 stores D_800F336C = 3 before the test, so `k == 4` is never true.
- 80198864-8019887C (7 words: the `+6` arm): cmd 2 state 2 also stores D_800F336C = 3 before the test, so the arm is never reached.
The 10 default-arm words 801982C0/C4 (a0 not 1/2), 801982E8/EC/FC and 80198300 (cmd 1, state not 0/1/2) and 80198478/7C/88/8C (cmd 2, state not 0/1/2) are covered by the supplementary run.

## Independent verification: confirmed

- `e21_leaf.py room_m0318i func_80198268 --nonmatching 9280:690 --check-gte --uncovered --seeds 300 --inputs "a0=1,2; h(a1+0x10)=0,1,2; h(a1+0x12)=0,3,9,0x17,0x30"`
  - e21_leaf: ok func_80198268__d68e9481_port.c 300 seeds, retail coverage 390/420 words; uncovered list identical to claim (30 words)
- `e21_leaf.py ... --seeds 24 --inputs "a0=0,1,2,3; h(a1+0x10)=3,0xFFFF"`
  - e21_leaf: ok ... 24 seeds, retail coverage 60/420 words; uncovered list excludes 801982C0/C4/E8/EC/FC/300/80198478/7C/88/8C, so the 10 default-arm words are covered
- `NEW s2: --seeds 120 --inputs "a0=1,2; h(a1+0x10)=0,1,2; h(a1+0x12)=0x8000,0xFFFF,0xFFFE,1,0x7FFF,0xFFFD,0x7FFE,2,0x20,0x40"`
  - e21_leaf: ok ... 120 seeds, retail coverage 390/420 words (negative/sign-bit/odd values of +0x12 for the /2 and *3/2 rounding; no mismatch)
- `NEW s3: --seeds 96 --inputs "a0=1,2; h(a1+0x10)=0,1,2,4; h(a1+0x12)=0,1,0xFFFF,0x7FFF,0x8000,0x100"`
  - e21_leaf: ok ... 96 seeds, retail coverage 394/420 words (state 4 also covers 801982FC, 80198300, 80198488, 8019848C); remaining uncovered only the 16 claimed-impossible words plus 801982C0/C4/E8/EC/80198478/7C which are default arms covered by the supplementary run. No MISMATCH, notest or skipped.

No mismatches. The 20 uncovered words check out against the asm. At 80198504 the D_800F336C store is the constant $a1=2 (set at 801984D4), and the 801986B0 and 80198810 stores are the constant 3. Each is followed by func_800CF3AC, then the reload of D_800F336C and the compare with 4. The 4 is reachable only if the callee writes D_800F336C, and the callee is mocked. Caveat: the real func_800CF3AC is not in this room's asm, so I could not confirm that retail never writes D_800F336C. The unreachability is therefore relative to the harness mock. Under the mock it is sound.

