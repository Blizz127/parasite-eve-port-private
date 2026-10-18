# exe func_800C689C (NON_MATCHING)

SLUS file offset 0xB709C, 0x284 bytes, VMA 0x800C689C. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800C689C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800C689C -- EXE, VRAM 0x800C689C, file offset 0xB709C, 0x284 bytes.
 * Three GTE OP (cross product) calls on scratchpad (0x1F800000) edge/delta
 * vectors built from a0 (six halfwords at +0,+4,+8,+0xC,+0x10,+0x14) and three
 * halfwords of *D_8009D254 (+0x2A,+0x2E,+0x32). Returns nonzero when the three
 * cross-product Z words (0x1F800064/74/84) are not all equal in the
 * (x^y)|(y^z) > 0 sense.
 * NON_MATCHING behaviour C. inline GTE/asm: Psy-Q-shaped COP2 asm macro.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800C689C --check-gte --uncovered --seeds 256
    e21_leaf: ok func_800C689C (exe) 256 seeds, retail coverage 161/161 words
    e21_leaf: uncovered none

## Uncovered words

none. The function is straight-line, so random seeds already reach every word. A mutation check showed that random seeds alone do not exercise the return value. With random seeds, replacing `> 0` with `!= 0` was not caught, and neither was changing the first xor term to `s[0x1C]`. The directed run caught both, and also caught changes to the other xor terms (`s[0x1D]^s[0x20]` and `s[0x1D]^s[0x1D]`). Use the directed spec as the recorded input spec.

## Notes from the decomp agent

What the function does: a0 points at 3 vertices as shorts (halfwords at +0, +2, +4, +6, +8, +0xA, +0xC, +0xE, +0x10, +0x14 are read; x at 0/8/0x10, z at 4/0xC/0x14 in byte offsets). It reads the pointer D_8009D254 and uses halfwords +0x2A and +0x32 of it as a point (x, z). A +0x2E halfword is loaded and stored to the stack but never used, so the C omits it.

The code builds edge vectors and point-minus-vertex vectors in the scratchpad at 0x1F800000. It stores 0x1F800000 into the global D_800E2844 and re-reads it, which the C reproduces. It runs three GTE OP (cross product) commands, `.word 0x4B70000C`, with the rotation diagonal taken from scratchpad words and the vector words loaded into IR1..IR3. The MAC results go to 0x1F800060, 0x1F800070 and 0x1F800080.

The return value is `((s[0x19]^s[0x1D]) | (s[0x1D]^s[0x21])) > 0`, a signed compare. These three words are the MAC2 results, so it tests whether the three Y cross-product components are equal.

The C uses a macro `gte_op_rowvec(r0, v0, o0)` with GTE inline asm, plus raw scratchpad pointers cast from 0x1F800000. It declares `extern short *D_8009D254;` and `extern int *D_800E2844;`. I declared D_800E2844 as `int *` because retail stores a pointer there and reloads it.

I created the directory src/nonmatching/exe/ because it did not exist. It is untracked; I made no other edits.

There is no retail read of unwritten stack, and no uncovered branches.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800C689C --nonmatching B709C:284 --check-gte --uncovered --seeds 64`
  - e21_leaf: ok func_800C689C (exe) 64 seeds, retail coverage 161/161 words / e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py exe func_800C689C --nonmatching B709C:284 --check-gte --uncovered --seeds 256 --inputs "h(a0+0)=0,1;h(a0+4)=0,3;h(a0+8)=0,5;h(a0+0xC)=0,7;h(a0+0x10)=0,2;h(a0+0x14)=0,9;h(a0+2)=0,4;h(a0+6)=0,1"`
  - e21_leaf: ok func_800C689C (exe) 256 seeds, retail coverage 161/161 words / e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py exe func_800C689C --nonmatching B709C:284 --check-gte --uncovered --seeds 64 --inputs "h(a0+0)=-32768,32767;h(a0+4)=-32768,32767;h(a0+8)=-32768,32767;h(a0+0xC)=-32768,32767;h(a0+0x10)=-32768,32767;h(a0+0x14)=-32768,32767"`
  - e21_leaf: ok func_800C689C (exe) 64 seeds, retail coverage 161/161 words / e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py exe func_800C689C --nonmatching B709C:284 --check-gte --uncovered --seeds 243 --inputs "h(a0+0)=-1,0,1;h(a0+4)=-1,0,1;h(a0+8)=-1,0,1;h(a0+0xC)=-1,0,1;h(a0+0x10)=-1,0,1"`
  - e21_leaf: ok func_800C689C (exe) 243 seeds, retail coverage 161/161 words / e21_leaf: uncovered none

No refutation found. Two new specs ran with no MISMATCH, notest or skipped lines, and no uncovered words. One is the +/-32768/32767 limits on all six a0 halfwords (64 seeds). The other is -1/0/1 on five of them (243 seeds, +0x14 left random). I also checked the C against the asm by hand.

Both runs used the default random values for the *D_8009D254 halfwords (+0x2A, +0x2E, +0x32). The spec syntax can set a0 fields but I did not direct the pointer target.

Hand-check findings, none of which is a defect. All scratchpad offsets match: the three OP calls write 0x60, 0x70 and 0x80, and the return reads 0x64, 0x74 and 0x84. The a0 deltas (a0[4]-a0[0] and the rest) use the right halfword offsets, and `lh` sign extension matches `short`. The zero stores (0x04, 0x14, 0x24, 0x34, 0x44, 0x54) are present. D_800E2844 is set to 0x1F800000. The return is a signed `slt 0,x`, which matches `(a|b) > 0`.

Retail also stores the 0x2A and 0x32 halfwords to sp+0 and sp+4 and reloads them. The C reads them directly, which gives the same values and is not observable. Retail's store of the 0x2E halfword to sp+2 is dead.

The run times were unusually long, probably because of machine contention from other workers' jobs. This does not affect the results."

