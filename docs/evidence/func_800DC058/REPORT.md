# exe func_800DC058 (NON_MATCHING)

SLUS file offset 0xCC858, 0x564 bytes, VMA 0x800DC058. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800DC058.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800DC058 — EXE 0x800DC058, SLUS offset 0xCC858, 0x564 bytes.
 * NON_MATCHING (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback  int f(int mode, unsigned char *rec):
 *   mode 0: rec[0x10] = func_80071A54(); rec[8] = rec[0xC] = 0;
 *           func_800CE870(D_8009D254, 1, rec); hides both sprites (800C6D5C x2).
 *   mode 1: ++rec[0xC]; done (1) once the clock D_800E27EC reaches 0x40.
 *   mode 2: rec[0x10] += 0x10; rec[8] is a phase (0: ramp 0..0x18 frames,
 *           1: ramp 0..0xC frames, else full) giving the glow size (a0 / 4)
 *           and the second-sprite lead s4; draws the colour-curve sprite
 *           D_800F3418 at rec's vector and, when s4 != 0, a white twin D_800F342C.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800DC058 --check-gte --uncovered --seeds 256
    e21_leaf: ok func_800DC058 (exe) 256 seeds, retail coverage 331/345 words
    e21_leaf: uncovered 800DC338 800DC33C 800DC340 800DC344 800DC348 800DC34C 800DC350 800DC510 800DC514 800DC518 800DC51C 800DC520 800DC524 800DC528

## Uncovered words

800DC338-800DC350 (7 words, first draw) and 800DC510-800DC528 (7 words, second draw): the "D_800F336C == 4 && D_800F3428 != 0" arm that adds 0xA to the palette index. It is unreachable. D_800F336C is stored 1 (sh at 800DC270) earlier in the same call, and only the mocked callees func_800CEDA8 and func_80077A64 run between that store and the read. The directed spec still toggles D_800F3428, but the compare against 4 can never hold. The a0==4 comparison and the +6 else arm are covered.

## Notes from the decomp agent

The function is int func_800DC058(int mode, unsigned char *rec).

Mode 0: rec+0x10 = func_80071A54(); rec+8 = rec+0xC = 0; func_800CE870(D_8009D254, 1, rec); func_800C6D5C(D_800F3418, 0, 0) and func_800C6D5C(D_800F342C, 0, 0); returns 0.

Mode 1: ++rec[0xC]; returns 0 while the signed int D_800E27EC is below 0x40, else 1.

Mode 2: rec+0x10 += 0x10. The phase word at rec+8 drives two values. Phase 0 sets a0 = (rec[0xC] << 12) / 24 and, at rec[0xC] >= 0x18, sets phase 1 and clears the counter. Phase 1 sets s4 = (((rec[0xC] / 2) + 1) << 12) / 6 and a0 = 0x1000. At rec[0xC] >= 0xC it clears the counter and sets phase 2, using the literal mode value that retail stores from a2. Any other phase gives a0 = 0x1000 and s4 = 0.
- The constant divisions /24 and /6 come from the 0x2AAAAAAB multiplies.
- The scale vector on the stack is {0x44C, a0 / 4, 0x44C}.
- First draw: func_800CF3AC(D_800E1E24, col, D_800E27EC), RotMatrix, ScaleMatrix, translation from rec shorts 0/2/4, SetRot/SetTransMatrix from *D_800BCFA4, func_80077A64(0,3,0,0), a CLUT/tpage word, then the func_800C6EC0/6ED8/6EF8/7098/71E4 draw calls on D_800F3418. A second matrix with size 0x4B0 and Y offset -0x3E8 is drawn next, ending in func_800C6F4C.
- When s4 != 0, a white twin on D_800F342C is drawn with scale 0x4B0, Y rotation rec[0x10] + s4, func_80077A64(0,1,0,0) and colour 0xFF,0xFF,0xFF.

Details a reviewer might trip over:
- func_800CE870 takes rec as its third argument. Retail leaves a2 = s2 there, and the host prototype has arity 3.
- func_800CF3AC is called as (D_800E1E24, col, D_800E27EC), with the colour buffer second.
- D_800BCFA4 is a MATRIX * (lw from the symbol), not an array of pointers.
- The CLUT/tpage word is (D_800E2850[D_800E11F6] | return of func_80077A64) & 0xFFFF, using v0 straight from the call.

The differential never showed retail reading stack it did not write.

The first --seeds 64 run with no directed inputs covered only 27/345 words. The 256-seed directed run is the main proof.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800DC058 --nonmatching CC858:564 --check-gte --uncovered --seeds 256 --inputs "a0=0,1,2,3,-1;w(a1+8)=0,1,2;w(a1+0xC)=-2,-1,5,0xB,0xC,0x17,0x18;w(0x800E27EC)=0x3F,0x40"`
  - e21_leaf: ok func_800DC058 (exe) 256 seeds, retail coverage 331/345 words | e21_leaf: uncovered 800DC338..800DC350 and 800DC510..800DC528 (14 words, identical to claim)
- `python3 tools/analysis/e21_leaf.py exe func_800DC058 --nonmatching CC858:564 --check-gte --uncovered --seeds 224 --inputs "a0=0,1,2,3,4,0x7FFFFFFF,0x80000000;w(a1+8)=0,1,2,3;w(a1+0xC)=-0x8000,-1,0,0xB,0xC,0x17,0x18,0x7FFF;w(0x800F3428)=0,1"`
  - e21_leaf: ok func_800DC058 (exe) 224 seeds, retail coverage 329/345 words | e21_leaf: uncovered 800DC128 800DC12C (this spec's a0 values never reach them) plus the same 14 words 800DC338-800DC350, 800DC510-800DC528
- `python3 tools/analysis/e21_leaf.py exe func_800DC058 --nonmatching CC858:564 --check-gte --uncovered --seeds 90 --inputs "a0=-0x8000,-2,-5,0xFFFF,0x10000;w(a1+8)=0,1,2;w(a1+0x10)=-0x8000,0,0x7FFF,0x8000,-1,5"`
  - e21_leaf: ok func_800DC058 (exe) 90 seeds, retail coverage 32/345 words | uncovered 800DC0B4 onward (this spec never leaves the early exit via the default a1+0xC; no MISMATCH, notest or skipped printed)

No MISMATCH, notest or skipped in any run. The unreachability claim holds against the asm. D_800F336C is stored 1 at 800DC270 (sh). Between that store and the read at 800DC310 (lhu), the only calls are func_800CEDA8 and func_80077A64. The same holds for the second draw: 800DC4E4 follows only func_80077A64 (800DC4C0), plus the first-draw callees (func_80077AA4, func_800C6EC0/ED8/EF8, func_800C7098, func_800C71E4, func_800C6F4C, func_80079754, func_80078CC4). The bne against 4 at 800DC330 and 800DC508 therefore never falls through.\n\nThe second draw block is only reached by falling through from the first. The only branches into the 800DC4xx region are beqz $s4 to 800DC594 (an exit) and the sequential flow. No entry skips the 800DC270 store.\n\nI checked the retail callees. func_800CEDA8 is C in src/func_800CEDA8.c and has no D_800F336C write. All other asm writers of D_800F336C are in other functions (C8B88.s, C7D1C.s, C5A90.s, C6D14.s, CE8A8.s), at VMAs 800D5xxx to 800DE6xx. None of them is a callee here. I did not check func_80077A64, func_80077AA4, func_80079754, func_80078CC4 or func_80071A54, so the claim rests on those not writing D_800F336C. I saw no sign they do.\n\nThe C at func_800DC058.c lines 121-130 and 175-176 mirrors the asm: store 1, mocked calls, reload, compare against 4, then +0xA or +6. This is a sound argument for the 14 uncovered words, not a 100% coverage proof.\n\nMinor: my second spec did not drive past the early exit, so it adds little. The first two runs cover signed and extreme a0 and a1+0xC values and toggle D_800F3428 with no divergence."]

