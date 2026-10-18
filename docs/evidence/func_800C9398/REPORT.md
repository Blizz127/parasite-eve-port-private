# exe func_800C9398 (NON_MATCHING)

SLUS file offset 0xB9B98, 0x4D0 bytes, VMA 0x800C9398. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800C9398.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800C9398 -- EXE, VRAM 0x800C9398, file offset 0xB9B98, 0x4D0 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Model-effect draw with two sub-parts for the effect record a2: sets up the
 * effect environment (func_800C2EAC(3), func_800C3098(0x10), func_800C2FF0(0x20,
 * 0x10), func_800C3238(2)), stores a2->h4 into D_800F34B2, then twice:
 *   - RotMatrix (func_80079754) of an 8-byte angle pair (first pass the 8 bytes
 *     at D_800C2184, second pass the 8 bytes at D_800C218C) -> 3x3 matrix R2,
 *   - copy of the 32-byte matrix a2+0x10 into a local MATRIX M and its
 *     translation replaced by the halfwords a2+8/+0xA/+0xC,
 *   - M.m = M.m * R2 (three GTE mvmva column passes) and M.t = clamp16(TR + R*0)
 *     (mvmva with a zero vector; the translation is the clamped original),
 *   - func_80078CC4(&M, D_800C2194[0..3]) scale, func_800C42A4(D_800F34A8, &M, 0).
 * The second pass uses the second angle pair and does not re-zero the vector.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) asm macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800C9398 --check-gte --uncovered --seeds 54
    e21_leaf: ok func_800C9398 (exe) 54 seeds, retail coverage 308/308 words
    e21_leaf: uncovered none

## Uncovered words

none. The function is straight-line, with no branches or loops, so every word executes on every seed.

## Notes from the decomp agent

func_800C9398 is a near-exact twin of the already-proven src/nonmatching/exe/func_800C83A0.c (same 0x4D0 size, same structure). I copied that file and swapped the constants:

| Item | func_800C83A0 | func_800C9398 |
|---|---|---|
| angle pair 1 | D_800C2154 | D_800C2184 |
| angle pair 2 | D_800C215C | D_800C218C |
| scale vector (16 bytes) | D_800C2164 | D_800C2194 |
| halfword store | D_800F34A2 | D_800F34B2 |
| func_800C42A4 first argument | D_800F3498 | D_800F34A8 |

I compared the asm line by line and did not rewrite any logic.

- The function takes a2 as an effect record. Arguments a0 and a1 are unused.
- It calls func_800C2EAC(3), func_800C3098(0x10), func_800C2FF0(0x20,0x10) and func_800C3238(2), then stores H(a2,4) into D_800F34B2.
- It then runs two passes. Each pass does RotMatrix (func_80079754) of an 8-byte angle pair into a 3x3 matrix R2.
- Each pass copies the 32-byte MATRIX from a2+0x10 into a local M and overrides M.t with the signed halfwords at a2+8, +0xA and +0xC.
- Each pass concatenates R2 into M's columns with the gte_rotcol macro (mvmva 1,0,3,3,0), then runs gte_rttrans with a zero vector (mvmva 1,0,0,0,0) so that M.t = TR.
- Each pass then calls func_80078CC4(&M, scale vector) and func_800C42A4(D_800F34A8, &M, 0).
- The function returns void (it returns the last callee's v0 in retail, which the differential did not flag). The e21 run passed with --check-gte, so all 64 GTE registers matched.
- Retail reads the translation of the R2 matrix (sp+0x44..0x4C). That is the zeroed vector, which the C models as vz[3]. No stale-stack reads showed up in the differential.
- The vector inputs came from random seeds plus the directed values above. Random seeds alone already reached 308/308.
- I touched only the one deliverable. The existing modified or untracked files in git status (e21_diff.py and the room_m0318i and room_m0432i files) were not mine.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800C9398 --nonmatching B9B98:4D0 --check-gte --uncovered --seeds 16`
  - e21_leaf: ok func_800C9398 (exe) 16 seeds, retail coverage 308/308 words | e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py exe func_800C9398 --nonmatching B9B98:4D0 --check-gte --uncovered --seeds 54 --inputs "h(a2+4)=0,1,0xFFFF;h(a2+8)=0,0x1234,0xF000;h(a2+0xA)=0,0x7FFF,0x8001;h(a2+0xC)=0,5,0xFFFF"`
  - e21_leaf: ok func_800C9398 (exe) 54 seeds, retail coverage 308/308 words | e21_leaf: uncovered none
- `NEW1: ... --seeds 81 --inputs "w(a2+0x10)=0,0x7FFF7FFF,0x80008000;w(a2+0x14)=0,0x7FFF8000,0xFFFF0001;w(a2+0x18)=0,0x7FFF7FFF,0x80008000;w(a2+0x1C)=0,0x7FFF8000,0xFFFF0001"`
  - e21_leaf: ok func_800C9398 (exe) 81 seeds, retail coverage 308/308 words | e21_leaf: uncovered none
- `NEW2: ... --seeds 81 --inputs "h(a2+8)=0x8000,0x7FFF,0xFFFF;h(a2+0xA)=0x8000,0x7FFF,1;h(a2+0xC)=0x8000,0x7FFF,0;h(a2+4)=0x8000,0x7FFF,0xFFFF"`
  - e21_leaf: ok func_800C9398 (exe) 81 seeds, retail coverage 308/308 words | e21_leaf: uncovered none
- `NEW3: ... --seeds 81 --inputs "w(a2+0x20)=0,0x7FFF7FFF,0x80008000;w(a2+0x24)=0,0x7FFF8000,0xFFFF0001;w(a2+0x28)=0,0x7FFF7FFF,0x80008000;w(a2+0x2C)=0,0x7FFF8000,0xFFFF0001"`
  - e21_leaf: ok func_800C9398 (exe) 81 seeds, retail coverage 308/308 words | e21_leaf: uncovered none

No MISMATCH, notest, skipped or uncovered words in any run. The asm has only jal calls (func_800C2EAC, 3098, 2FF0, 3238, 79754 x2, 78CC4 x2, 42A4 x2) and one jr $ra, with no conditional branches, so the 308/308 coverage claim holds. I compared the retail asm against the C: lh signed halfword loads for a2+8/A/C, lhu for a2+4, the 32-byte matrix copy, the three mvmva column passes, and the zero vector reused in the second pass. The C matches on all of these. The extra runs covered extreme matrix words (a2+0x10..0x2C) and boundary halfwords (0x8000, 0x7FFF, 0xFFFF). Limits of what this checks: I did not exercise callee-result-dependent paths, because there are none, and I did not independently verify the oracle's stub models for the callees.

