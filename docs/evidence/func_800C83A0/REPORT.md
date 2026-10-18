# exe func_800C83A0 (NON_MATCHING)

SLUS file offset 0xB8BA0, 0x4D0 bytes, VMA 0x800C83A0. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800C83A0.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800C83A0 -- EXE, VRAM 0x800C83A0, file offset 0xB8BA0, 0x4D0 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Model-effect draw with two sub-parts for the effect record a2: sets up the
 * effect environment (func_800C2EAC(3), func_800C3098(0x10), func_800C2FF0(0x20,
 * 0x10), func_800C3238(2)), stores a2->h4 into D_800F34A2, then twice:
 *   - RotMatrix (func_80079754) of an 8-byte angle pair (first pass the 8 bytes
 *     at D_800C2154, second pass the 8 bytes at D_800C215C) -> 3x3 matrix R2,
 *   - copy of the 32-byte matrix a2+0x10 into a local MATRIX M and its
 *     translation replaced by the halfwords a2+8/+0xA/+0xC,
 *   - M.m = M.m * R2 (three GTE mvmva column passes) and M.t = clamp16(TR + R*0)
 *     (mvmva with a zero vector; the translation is the clamped original),
 *   - func_80078CC4(&M, D_800C2164[0..3]) scale, func_800C42A4(D_800F3498, &M, 0).
 * The second pass uses the second angle pair and does not re-zero the vector.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) asm macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800C83A0 --check-gte --uncovered --seeds 96
    e21_leaf: ok func_800C83A0 (exe) 96 seeds, retail coverage 308/308 words
    e21_leaf: uncovered none

## Uncovered words

none (straight-line function: no branches, no loops, no mode switches; every retail word executes on every seed)

## Notes from the decomp agent

The function is straight-line, so one pass covers all words. It runs this sequence twice, once per angle pair. (1) It sets up the effect with func_800C2EAC(3), func_800C3098(0x10), func_800C2FF0(0x20,0x10) and func_800C3238(2). It stores a2->h4 (unsigned short) into D_800F34A2. (2) Each pass calls func_80079754(&angle_pair, R2). The first pass uses the 8 bytes at D_800C2154, the second the 8 bytes at D_800C215C. R2 is a 3x3 short matrix, 18 bytes. (3) Each pass copies the 32-byte MATRIX at a2+0x10 into a local and overwrites its translation words with the signed halfwords at a2+8, +0xA and +0xC. (4) It loads the local matrix as the GTE rotation (ctc2 0-4), then runs three mvmva passes (sf=1, mx=rot, v=IR, cv=none, opcode .word 0x4A49E012). Each pass takes one column of R2 (halfwords at +0, +6, +0xC from R2, +2, +4) and writes the result back into the matching column of the local matrix. This is Psy-Q MulMatrix0 and reuses the gte_rotcol and gte_rttrans macros from func_800C3324.c. (5) It loads the translation into TR (ctc2 5-7) and runs mvmva sf=1, mx=rot, v=V0, cv=TR (.word 0x4A480012) with V0 = 0. The result is the translation, saturated to 16 bits, stored back as three words. The GTE rotation register still holds the pre-multiply matrix. (6) It copies the 16 bytes at D_800C2164 into a stack buffer, then calls func_80078CC4(&M, &buf) and func_800C42A4(D_800F3498, &M, 0). The retail stack vector for the zero vector (words at sp+0x44/+0x48/+0x4C) is written once and reused unchanged by the second pass, so the C declares it once as vz[3]. The retail code never reads stack it did not write: R2 is fully written by func_80079754, and the matrix padding halfword is only copied. A return value is not used (void). The callee prototypes are K&R-style, so arguments are recorded as passed. D_800F3498 is declared unsigned char[] and passed as its address. All three e21 runs used --check-gte, which compares all 64 GTE registers. The directed run adds saturating translation and rotation values. The 400-seed run takes longer than 2 minutes and needs background execution. The deliverable file was the only file edited.

## Independent verification: confirmed

- `e21_leaf.py exe func_800C83A0 --nonmatching B8BA0:4D0 --check-gte --uncovered --seeds 40`
  - e21_leaf: ok func_800C83A0 (exe) 40 seeds, retail coverage 308/308 words / e21_leaf: uncovered none
- `same --seeds 400`
  - e21_leaf: ok func_800C83A0 (exe) 400 seeds, retail coverage 308/308 words / e21_leaf: uncovered none
- `same --seeds 96 --inputs (claimed spec)`
  - e21_leaf: ok func_800C83A0 (exe) 96 seeds, retail coverage 308/308 words / e21_leaf: uncovered none
- `same --seeds 54 --inputs "h(a2+0xC)=0x8000,0x7FFF,-1,0,1,0x1000; h(a2+0x10)=0x8000,0x7FFF,0x1000; h(a2+0x16)=0x8000,0x7FFF,-0x1000"`
  - e21_leaf: ok func_800C83A0 (exe) 54 seeds, retail coverage 308/308 words / e21_leaf: uncovered none
- `same --seeds 36 --inputs "w(a2+0x10)=0x80008000,0x7FFF7FFF,0xFFFF0001; w(a2+0x14)=0x80008000,0x7FFF7FFF; w(a2+0x20)=0x80008000,0x7FFF7FFF; h(a2+0xC)=-0x8000,0x7FFF"`
  - e21_leaf: ok func_800C83A0 (exe) 36 seeds, retail coverage 308/308 words / e21_leaf: uncovered none

None found. Retail asm lines 13-322 contain no conditional branches or loops, only jal calls, so 'straight-line, all words execute' holds. I read the C against the asm for the signed halfword stores to a2+0x24/0x28/0x2C (lh), the lhu column loads, the D_800F34A2 halfword store and the call argument order, and found no discrepancy. Limitation: I did not independently check that the harness compares callee-argument contents, so that rests on e21's design.

