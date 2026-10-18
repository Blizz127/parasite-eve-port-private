# room_m0318i func_80195D20 (NON_MATCHING)

Blob offset 0x6D38, 0x74C bytes, VMA 0x80195D20. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80195D20.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80195D20, blob offset 0x6D38, 0x74C bytes. NON_MATCHING
 * behaviour C written from the retail asm (2026-10-18; not a byte match).
 * Effect task, a0 = mode, a1 = parameter block (x y z halfwords at +0/+2/+4).
 *   mode 0: func_800CE870(D_8009D254, 0, a1, buf), then registers the
 *           func_80195904 particle callback with func_800CE560 (returned).
 *   mode 1: per-frame step on the D_800E27EC frame counter. Returns 1 when
 *           the D_800E2368 record's +0x16 halfword is zero. Frame 2 plays two
 *           sound effects (0x5E7 / 0x5E8) through func_8006DF50 when the
 *           D_800B0E64 voice exists. Frame 3 spawns three particles (kind 1,
 *           velocity from three rand()%50-25), frames < 5 spawn two more
 *           (kind 0, spread rand()&0x7F, velocity rand()%190-95) through the
 *           func_800CE610 pool at D_800F33E0[2]. Returns 2 from frame 8 on.
 *   mode 2: draw. Sets the D_800F3368 block, loads the camera GTE
 *           matrix (D_800BCFA4[0]), sets the screen flash (func_800D1AE0 on
 *           frames 1 and 2), and for frame <= 32 draws three sprites
 *           (func_800CEE20 / func_800D004C / func_800D0728) sized by the sin
 *           of frame << 5. Always ends by resetting the draw block; returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80195D20 --check-gte --uncovered --seeds 400
    e21_leaf: ok func_80195D20__14ab3e33_port.c 645 seeds, retail coverage 461/467 words
    e21_leaf: uncovered 8019628C 80196290 80196294 80196298 8019629C 801962A0

## Uncovered words

8019628C 80196290 80196294 80196298 8019629C 801962A0: the 'if (D_800F336C == 4 && D_800F3428 != 0) a1 += 4' arm in mode 2. Retail stores 3 to D_800F336C a few instructions earlier (80195 1E4 sh v0,%lo(D_800F336C)) and reads it back with no intervening writer other than mocked callees, so the ==4 test can never be true: provably impossible branch (the C keeps the test for fidelity). All other words, including the mode>2 default arm (80195D98/9C), negative mode (80195D84/88), the negative-value rounding fix-ups, and both null/non-null func_800CE610 spawn arms, are covered across the main and supplementary runs.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0318i func_80195D20 --nonmatching 6D38:74C --check-gte --uncovered --seeds 400 --inputs "a0=1,2,0,-1,3; a1=0x80110000; w(0x800E27EC)=0,1,2,3,4,5,8,0x20,0x21,-1; w(0x800B0E64)=1,0; r(0x800CE610)=0x80120000"`
  - Claimed cmd 1: e21_leaf: ok func_80195D20__14ab3e33_port.c 340 seeds, retail coverage 461/467 words | e21_leaf: uncovered 8019628C 80196290 80196294 80196298 8019629C 801962A0 (matches the claim; no MISMATCH/notest/skipped)
- `python3 tools/analysis/e21_leaf.py ... --seeds 400 --inputs "a0=0,1,2,3; a1=0x80110000; w(0x800E27EC)=0,1,2,3,4,8,0x21,0x20,5,7,-1; w(0x800B0E64)=1,0; h(0x800F336C)=4,0; w(0x800F3428)=1,0" (started, did not finish before I stopped; not relied on)`
  - NEW 1: e21_leaf: ok ... 353 seeds, retail coverage 365/467 words (uncovered only because my spec pruned mode/pool arms: 80195E14-80195F94 etc.; no MISMATCH)
- `NEW 1: ... --seeds 400 --inputs "a0=0,1,2,3,-5,0x7fff; a1=0,0x80110000; w(0x800E27EC)=0,1,0x1f,0x20,0x21,0x22,-1,-0x21; w(0x800B0E64)=1,0; r(0x800CE610)=0,0x80120000"`
  - NEW 2: e21_leaf: ok ... 228 seeds, retail coverage 412/467 words (uncovered 80195D98.., 80195E14.., 80196210.., 8019628C-801962A0; no MISMATCH)

No MISMATCH, notest or skipped in any completed run. The claimed 461/467 coverage reproduced. One caveat on the uncovered words 8019628C-801962A0. In the asm, 801961E4 stores 3 to D_800F336C, and 80196268 reads it back with lhu. Between them the only calls are func_800D1AE0 (80196228, on one arm only) and func_80077DC4 (8019624C). func_80077DC4 is an EXE sign-folded table-lookup wrapper, so it is unlikely to write the variable. func_800D1AE0 is an external resident-overlay function (0x800D1AE0). Its body is not in asm/, so I could not check whether it writes D_800F336C. Other overlays do write that halfword, for example ovl_03C9. So 'provably impossible' holds only if func_800D1AE0 never writes D_800F336C. That is not proven from the retail bytes. It is a minor fidelity risk: the C keeps the test, so behaviour is preserved either way, and the harness's mocked callees cannot exercise it. My own specs ran only 353 and 228 seeds, with restricted lists for a0, w(0x800E27EC) and h(0x800F336C), so they did not re-cover every arm. They found no divergence in the words they did cover."

