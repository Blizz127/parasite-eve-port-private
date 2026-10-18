# exe func_800CAE0C (NON_MATCHING)

SLUS file offset 0xBB60C, 0x944 bytes, VMA 0x800CAE0C. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800CAE0C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800CAE0C -- EXE, VRAM 0x800CAE0C, file offset 0xBB60C, 0x944 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Four-pass sibling of func_800C9398 (same effect environment and per-pass body)
 * for the effect record a2: func_800C2EAC(3), func_800C3098(0x10),
 * func_800C2FF0(0x20, 0x10), func_800C3238(2), then four passes, pass k:
 *   - D_800F34D0 / D_800F34D2 set (pass 1 -10 / (short)a2.h4 >> 1; pass 3 -0x32 /
 *     a2.h4; passes 2 and 4 keep the previous values),
 *   - RotMatrix (func_80079754) of the k-th 8-byte angle pair of the 32-byte
 *     table D_800C21D4 (stack copy) -> 3x3 matrix R2,
 *   - copy of the 32-byte matrix a2+0x10 into a local MATRIX M with its
 *     translation replaced by the halfwords a2+8/+0xA/+0xC,
 *   - M.m = M.m * R2 (three GTE mvmva column passes) and M.t = TR + R*V0 with
 *     the stack vector V0 = {0, 0, -0x14} (passes 1-2) / {0, -10, -0x190} (3-4),
 *   - func_80078CC4(&M, copy of the 16 bytes at D_800C21F4),
 *     func_800C42A4(D_800F34C8, &M, 0).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) asm macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800CAE0C --check-gte --uncovered --seeds 240
    e21_leaf: ok func_800CAE0C (exe) 240 seeds, retail coverage 593/593 words
    e21_leaf: uncovered none

## Uncovered words

none. The function is straight-line, with no branches or loops, so every word executes on every seed.

## Notes from the decomp agent

func_800CAE0C is a four-pass sibling of the existing src/nonmatching/exe/func_800C9398.c. I reused that file's GTE macros: gte_SetRotMatrix, gte_SetTransMatrix, gte_rotcol (mvmva 0x4A49E012) and gte_rttrans (mvmva 0x4A480012). The signature is (a0, a1, a2), and only a2, the effect record, is used. Structure:

- Setup: func_800C2EAC(3), func_800C3098(0x10), func_800C2FF0(0x20,0x10), func_800C3238(2).
- Stack copy: the 32 bytes at D_800C21D4 are copied to the stack (four 8-byte angle pairs), at sp+0x50.
- Pass k, for k = 1..4: func_80079754(angle_k, R2) fills R2. The 32-byte matrix at a2+0x10 is copied into a local MATRIX M, and M.t is replaced by the halfwords a2+8, a2+0xA and a2+0xC. M.m is multiplied by R2 with three mvmva column passes. gte_rttrans then computes M.t = TR + R*V0, where V0 is the stack vector at sp+0x44. func_80078CC4(&M, copy of the 16 bytes at D_800C21F4) runs next, and func_800C42A4(D_800F34C8, &M, 0) last.
- Per-pass state: V0 = {0, 0, -0x14} for passes 1 and 2, and {0, -10, -0x190} for passes 3 and 4. Pass 1 stores D_800F34D0 = -10 and D_800F34D2 = (short)a2.h4 >> 1 (retail sll 16 / sra 17). Pass 3 stores D_800F34D0 = -0x32 and D_800F34D2 = a2.h4. Passes 2 and 4 keep the previous values.

Observations:
- No retail stack read-before-write mechanism was found. All stack inputs to GTE and callees are written in the function first (matrices, V0, scale vectors). Nothing needed to be reported or hidden.
- D_800F34D2 is overwritten in pass 3, and every callee is mocked. The differential therefore cannot see the pass-1 `>> 1` signedness: swapping to an unsigned shift still passed. The C follows the retail instruction pair (sll 16 / sra 17), so it is correct but not covered by the differential.
- No `out=` options were needed. The default OUT_EXTENTS entries cover func_80079754 arg 1 (18 bytes) and func_80078CC4 arg 0 (20 bytes).
- Only the one deliverable file was created. Scratch is under /dev/shm/pe-native-34f506ce/work/exe_func_800CAE0C, including orig.c, which is a copy of the deliverable.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800CAE0C --nonmatching BB60C:944 --check-gte --uncovered --seeds 240 --inputs "h(a2+4)=0,1,0x8001,0xFFFF,0x7FFF; h(a2+8)=0,0x7FFF,0x8000,0x100; h(a2+0xA)=0,-1,0x7FFF; w(a2+0x1C)=0x7FFF0000,0,0x12345678,0x80000000"`
  - e21_leaf: ok func_800CAE0C (exe) 240 seeds, retail coverage 593/593 words | e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py exe func_800CAE0C --nonmatching BB60C:944 --check-gte --uncovered --seeds 200 --inputs "h(a2+0xC)=0,-1,0x7FFF,0x8000,0x1234; h(a2+8)=-1,0x8000,0x7FFF,1,0x4000; h(a2+0xA)=0x8000,1,0xFFFF,0x7FFE,0x100; h(a2+4)=0x8000,0xFFFE,2,3,0xFFFF; w(a2+0x10)=0,0x7FFF7FFF"`
  - e21_leaf: ok func_800CAE0C (exe) 200 seeds, retail coverage 593/593 words | e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py exe func_800CAE0C --nonmatching BB60C:944 --check-gte --uncovered --seeds 243 --inputs "w(a2+0x10)=0x80008000,0x0000FFFF,0x7FFF8001; w(a2+0x14)=0,0x80000000,0xFFFFFFFF; w(a2+0x18)=0x10001000,0xF000F000,1; w(a2+0x20)=0,0x7FFFFFFF,0x80000001; w(a2+0x24)=0,0x40004000,0xC000C000"`
  - e21_leaf: ok func_800CAE0C (exe) 243 seeds, retail coverage 593/593 words | e21_leaf: uncovered none

No MISMATCH, notest, skipped or uncovered word in any run. The function is straight-line, so the 593/593 coverage and the empty uncovered list are consistent with the asm (calls and GTE ops only, no branches). I read only part of the asm (lines 66-140 of BB60C.s) and compared it with the C. The call arguments, the D_800F34D0/D_800F34D2 stores (-10, lhu>>17 then -0x32, lhu), the 32-byte matrix copy, the lh translation overrides at a2+8/+0xA/+0xC and the vz init (0,0,-0x14) match the C. I did not read the later passes' asm line by line. Weaknesses: the second spec used 200 seeds against a 1250-combination product, so it is a random sample and not exhaustive. The mutation sanity check in the claim was not re-run, since I may not edit the C file. I did not find any inputs that expose a difference.

