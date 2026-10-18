# room_m0429i func_80196748 (NON_MATCHING)

Blob offset 0x7760, 0x584 bytes, VMA 0x80196748. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_80196748.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_80196748, blob offset 0x7760, 0x584 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, modes 1/2 on a record a1, read from the retail disassembly.
 * Mode 1 steps the record by state (+8): 0 moves it by (+0xC,+0xE,+0x18) for
 * an open-ended 0, 1 and 2 count +0xA up to 0x30 / 0x10 and return 1 when done.
 * Mode 2 loads the GTE rotation/translation from D_800BCFA4[0] and draws:
 * state 0 a glow (func_800D0E88) sized func_80077CF4(min(f,16) << 6) / 32;
 * state 1 a sprite at a rotated offset (func_800CFB7C, func_800D2370) timed
 * by f*1024/48; state 2 sets the D_800F3368 draw block and draws func_800CEE20.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_80196748 --check-gte --uncovered --seeds 400
    e21_leaf: ok func_80196748__434ad31d_port.c 721 seeds, retail coverage 347/353 words
    e21_leaf: uncovered 80196C40 80196C44 80196C48 80196C4C 80196C50 80196C54

## Uncovered words

80196808 and 8019680C: the mode 1 default arm (state at a1+8 is negative and not 0 or 1, so `j end`). Covered by the supplementary run with h(a1+8)=-1. 80196950 and 80196954: the mode 2 equivalent (negative state, `j end`). Covered by the same supplementary run. 80196C40-80196C54 (lui/lw D_800F3428, nop, beqz, nop, addiu a1,4): provably unreachable. In mode 2 state 2 retail stores 3 to D_800F336C (sh), then reloads it and tests it against 4 before adding 4 to the clut index. The reloaded value is always 3, so the `bne v1,4` always jumps over this block. The state 1 copy of the same test (80196B14-B28) is covered. I kept the dead test in the C, which matches retail.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196748 --nonmatching 7760:584 --check-gte --uncovered --seeds 400 --inputs "a0=1,2,0; h(a1+8)=0,1,2,3; h(a1+0xA)=0x2E,0x2F,0xE,0xF,0x10,0x11,3,4; h(0x800F336C)=4,0; w(0x800F3428)=1,0"`
  - e21_leaf: ok func_80196748__434ad31d_port.c 373 seeds, retail coverage 343/353 words | e21_leaf: uncovered 80196808 8019680C 80196950 80196954 80196C40 80196C44 80196C48 80196C4C 80196C50 80196C54
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196748 --nonmatching 7760:584 --check-gte --uncovered --seeds 4 --inputs "a0=1,2; h(a1+8)=-1,0x7fff"`
  - e21_leaf: ok func_80196748__434ad31d_port.c 3 seeds, retail coverage 93/353 words (uncovered list does not include 80196808, 8019680C, 80196950, 80196954, so the negative-state default arms are covered and match)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196748 --nonmatching 7760:584 --check-gte --uncovered --seeds 72 --inputs "a0=1,2,0; h(a1+8)=0,1,2,-1; h(a1+0xA)=0x2F,0x30,-1,0x8000,0xF,0x10"`
  - e21_leaf: ok func_80196748__434ad31d_port.c 70 seeds, retail coverage 337/353 words (uncovered 8019681C 80196820 80196960 80196964 80196B10-80196B24 80196C40-80196C54; only inputs this spec did not drive)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196748 --nonmatching 7760:584 --check-gte --uncovered --seeds 72 --inputs "a0=1,2; h(a1+8)=0,1,2; h(a1+0xE)=0,-1,0x7fff,0x8000; h(a1+0xC)=0,-1,0x8000"`
  - e21_leaf: ok func_80196748__434ad31d_port.c 62 seeds, retail coverage 331/353 words (uncovered only words my narrow spec did not drive, plus the same C40-C54 block)

No MISMATCH, notest or skipped in any of the four runs. The main run reproduced exactly as claimed, 373 seeds with 343/353 coverage and the same 10 uncovered words. The supplementary run (a0=1,2; h(a1+8)=-1,0x7fff) did not list 80196808, 8019680C, 80196950 or 80196954 as uncovered, so the claim that it covers them holds.

Asm check of 80196C40-80196C54. 80196BF8/80196BFC store 3 to D_800F336C with sh. 80196C18/80196C1C reload it with lhu into $v1, and 80196C50 is `bne $v1,4`. Nothing between the store and the reload can change the value (no call, no other store to that address). So the bne is always taken and the lui/lw D_800F3428, beqz and addiu a1,4 block is dead. I constructed no input that reaches it. The state-1 copy of the same test (80196B10-80196B24) is reachable: my first new spec (a0=1,2,0) left it uncovered, but it is covered in the claimed main run, which drives h(0x800F336C)=4,0 and w(0x800F3428)=1,0.

My two new specs targeted these signed and halfword edges: lh versus lhu at a1+0xA with 0x2F, 0x30, 0x8000, -1, 0xF and 0x10 (the slti 0x30 and slti 0x10 boundaries); a negative state at a1+8; and a0=0. The second spec varied the halfword fields at a1+0xE and a1+0xC with 0, -1, 0x7fff and 0x8000. Both matched with zero mismatches. I found no behavior difference in the lh and lhu widths or signedness, the compares, or the arms.

Coverage came to 347 of 353 words across the two claimed runs, with only the 6 dead words at C40-C54 outside it, which matches the claim."

