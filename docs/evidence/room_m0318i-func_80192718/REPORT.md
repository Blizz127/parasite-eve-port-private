# room_m0318i func_80192718 (NON_MATCHING)

Blob offset 0x3730, 0x364 bytes, VMA 0x80192718. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80192718.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80192718, blob offset 0x3730, 0x364 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Particle callback registered by func_80192A7C, a0 = mode, a1 = particle.
 *   mode 1: ++a1[4]; a1[8] = 700 * D_800E27EC / 36 + D_801998FA - 0x2BC;
 *           a1[6]/a1[0xA] = D_801998F8/FC + sin/cos(a1[2]) * a1[0xC] / 4096;
 *           a1[2] += 0x80; every 8th RNG tick spawns a trail point (+0/2/4 =
 *           a1[6/8/0xA]) from func_800CE610(D_80199900); returns
 *           D_800E27EC >= 0x24.
 *   mode 2: three-state sprite (a1[0]: 0 grow 12 frames, 1 hold 12 frames,
 *           2 fall with a1[0xC] += a1[4] << 2); draws it through func_800CEE20
 *           at a1[6/8/0xA] with CLUT from D_800E1204[D_800F336C]; returns 0.
 * inline GTE/asm: none used here.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80192718 --check-gte --uncovered --seeds 200
    e21_leaf: ok func_80192718__86a83ea4_port.c 192 seeds, retail coverage 215/217 words
    e21_leaf: uncovered 80192764 80192768

    python3 tools/analysis/e21_leaf.py room_m0318i func_80192718 --nonmatching 3730:364 --check-gte --uncovered --seeds 8 --inputs "a0=0,3,-1,5"
    e21_leaf: ok func_80192718__86a83ea4_port.c 8 seeds, retail coverage 28/217 words
    e21_leaf: uncovered 8019276C 80192770 80192774 80192778 8019277C 80192780 80192784 80192788 8019278C 80192790 80192794 80192798 8019279C 801927A0 801927A4 801927A8 801927AC 801927B0 801927B4 801927B8 801927BC 801927C0 801927C4 801927C8 801927CC 801927D0 801927D4 801927D8 801927DC 801927E0 801927E4 801927E8 801927EC 801927F0 801927F4 801927F8 801927FC 80192800 80192804 80192808 8019280C 80192810 80192814 80192818 8019281C 80192820 80192824 80192828 8019282C 80192830 80192834 80192838 8019283C 80192840 80192844 80192848 8019284C 80192850 80192854 80192858 8019285C 80192860 80192864 80192868 8019286C 80192870 80192874 80192878 8019287C 80192880 80192884 80192888 8019288C 80192890 80192894 80192898 8019289C 801928A0 801928A4 801928A8 801928AC 801928B0 801928B4 801928B8 801928BC 801928C0 801928C4 801928C8 801928CC 801928D0 801928D4 801928D8 801928DC 801928E0 801928E4 801928E8 801928EC 801928F0 801928F4 801928F8 801928FC 80192900 80192904 80192908 8019290C 80192910 80192914 80192918 8019291C 80192920 80192924 80192928 8019292C 80192930 80192934 80192938 8019293C 80192940 80192944 80192948 8019294C 80192950 80192954 80192958 8019295C 80192960 80192964 80192968 8019296C 80192970 80192974 80192978 8019297C 80192980 80192984 80192988 8019298C 80192990 80192994 80192998 8019299C 801929A0 801929A4 801929A8 801929AC 801929B0 801929B4 801929B8 801929BC 801929C0 801929C4 801929C8 801929CC 801929D0 801929D4 801929D8 801929DC 801929E0 801929E4 801929E8 801929EC 801929F0 801929F4 801929F8 801929FC 80192A00 80192A04 80192A08 80192A0C 80192A10 80192A14 80192A18 80192A1C 80192A20 80192A24 80192A28 80192A2C 80192A30 80192A34 80192A38 80192A3C 80192A40 80192A44 80192A48 80192A4C 80192A50 80192A54 80192A58 80192A5C

## Uncovered words

80192764 and 80192768 (the `j .L80192A60` plus its delay-slot nop for a mode that is neither 1 nor 2) are uncovered in the main runs because their a0 values are only 1,2. They are covered by the a0=0,3,-1,5 supplementary run. No other words are uncovered.

## Independent verification: confirmed

- `e21_leaf.py room_m0318i func_80192718 --nonmatching 3730:364 --check-gte --uncovered --seeds 48 --inputs "a0=1,2;h(a1+0)=0,1,2;h(a1+4)=0,0xC;h(0x800F336C)=4,0;w(0x800F3428)=1,0"`
  - e21_leaf: ok func_80192718__86a83ea4_port.c 45 seeds, retail coverage 215/217 words; uncovered 80192764 80192768
- `e21_leaf.py ... --seeds 200 --inputs "a0=1,2;h(a1+0)=0,1,2;h(a1+4)=0,0xC,0xB,0xFFF5;h(0x800F336C)=4,0;w(0x800F3428)=1,0;w(0x800E27EC)=0x23,0x24,0xFFFFFFF0,0x1000"`
  - e21_leaf: ok ... 192 seeds, retail coverage 215/217 words; uncovered 80192764 80192768
- `e21_leaf.py ... --seeds 8 --inputs "a0=0,3,-1,5"`
  - e21_leaf: ok ... 8 seeds, retail coverage 28/217 words; uncovered list begins at 8019276C, so 80192764/80192768 are covered
- `NEW: e21_leaf.py ... --seeds 300 --inputs "a0=2;h(a1+0)=0,1,2,3,0xFFFF;h(a1+4)=0,1,0xB,0xC,0xD,0x7FFF,0x8000,0xFFFF;h(a1+0xC)=0,0xFFFF,0x7FFF;h(0x800F336C)=4,0,3,0x8000"`
  - NEW mode-2 run: ok ... 300 seeds, coverage 132/217; uncovered 80192764..801928B4 only (the mode-1 body plus the neither-mode jump, none of which mode 2 touches); every mode-2 word (801928B8..80192A5C) was executed
- `NEW: e21_leaf.py ... --seeds 400 --inputs "a0=1;h(a1+2)=0,0x7FFF,0x8000,0xFFFF;h(a1+0xC)=0,0xFFFF,0x8000,0x7FFF,1;h(a1+4)=0,0xFFFF;w(0x800E27EC)=0,1,35,36,0x23,0xFFFFFFFF,0x80000000,0x7FFFFFFF"`
  - NEW mode-1 run: ok ... 386 seeds, coverage 107/217; uncovered 8019275C..80192768 and 801928B8..80192A5C (mode-2 and the default dispatch, none of which mode 1 reaches); all mode-1 words executed

No MISMATCH, notest or skipped output in any run. The uncovered words 80192764 (j .L80192A60) and 80192768 (nop) are reachable only when a0 is neither 1 nor 2. The a0=0,3,-1,5 run covers them, so the claim's justification holds. I read the asm against the C by hand and found no divergence. The /36 multiply-high (0x38E38E39 then sra 3) and /12 multiply-high (0x2AAAAAAB then sra 1) match C division by 36 and 12. The sra-12 rounding with +0xFFF and the sra-6 rounding with +0x3F match C division by 4096 and by 64. The mode-2 states 0/1/2, the unsigned 0xC compare and the CLUT +9 or +5 branch (w==4 and D_800F3428) are all in the C, and so are the stack argument order and the 0x24 return threshold. The edge-value runs (a1[4] up to 0xFFFF, a1[0xC] up to 0xFFFF, D_800E27EC at 0x80000000, 0x7FFFFFFF and -1, D_800F336C at 0x8000) all passed. None of the runs swept D_800F3428 or the func_800CE610 null-return branch. These are the only residual gaps, and both are exercised in the claimed runs."]

