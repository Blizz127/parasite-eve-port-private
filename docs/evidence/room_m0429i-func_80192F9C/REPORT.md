# room_m0429i func_80192F9C (NON_MATCHING)

Blob offset 0x3FB4, 0x2500 bytes, VMA 0x80192F9C. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_80192F9C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_80192F9C, blob offset 0x3FB4, 0x2500 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback with three modes on a record a1 (frame at +0x1A, state at
 * +0x18; +0..+4 position, +8/+0xA, +0x10 sub-vectors):
 *   mode 0  binds five effect handles (func_8006E498 -> D_8019B680..B690 and
 *           func_800C6D5C), clears the state/frame, copies the record's three
 *           vectors with func_800CE870/func_800CE9D4 and registers
 *           func_80192E08 with func_800CE560 (its result is the return value).
 *   mode 1  steps the state machine (states 0..4; +0x1A counts frames, the
 *           state advances at 16/16/16/32 frames, state 4 returns 1 at 24).
 *           State 0 plays two sounds at frame 2, state 3 spawns a particle
 *           (func_800CE610 record, randomised +0/+2/+4) every frame, raises
 *           the D_8009D254 flag when the func_800C6B90 phase tests agree and
 *           sets D_8019B668 = 0x80; state 4 spawns the same particle and sets
 *           D_8019B668 = sin(f * 1024 / 24) / 32.
 *   mode 2  draws: states 0..2 are func_800CEE20 glows plus (1, 2) a
 *           func_800D004C / func_800D0728 pair (state 2 also a 16-spoke
 *           func_800D0E88 fan); states 3 and 4 are five rotated and scaled
 *           handle glows (the `glow` helper below: func_80079754 builds the
 *           rotation, func_80078CC4 scales it, func_800C6EF8/6FA0/71E4/6F4C
 *           render through the handle) plus func_800D004C / func_800D0728
 *           draws. Every path ends by resetting the D_800F3368 draw block.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_80192F9C --check-gte --uncovered --seeds 168
    e21_leaf: ok func_80192F9C__5fc90d58_port.c 998 seeds, retail coverage 2282/2368 words
    e21_leaf: uncovered 80193308 8019330C 80193310 80193314 80193488 80193498 8019349C 801934A0 801934C8 801934D8 801934DC 801934E0 801935CC 801935DC 801935E0 801935E4 8019360C 8019361C 80193620 80193624 80193AA0 80193AA4 80193AA8 80193AAC 80193AB0 80193AB4 80193DF4 80193DF8 80193DFC 80193E00 80193E04 80193E08 80194288 8019428C 80194290 80194294 80194298 8019429C 8019446C 80194470 80194474 80194478 8019447C 80194480 8019467C 80194680 80194684 80194688 8019468C 80194690 801949D4 801949D8 801949DC 801949E0 801949E4 801949E8 80194BA4 80194BA8 80194BAC 80194BB0 80194BB4 80194BB8 80194D5C 80194D60 80194D64 80194D68 80194D6C 80194D70 80194F64 80194F68 80194F6C 80194F70 80194F74 80194F78 80195160 80195164 80195168 8019516C 80195170 80195174 80195330 80195334 80195338 8019533C 80195340 80195344

## Uncovered words

66 words = 11 copies x 6 words of the dead arm `if (D_800F336C == 4 && D_800F3428) a2 += 4`: 80193AA0-80193AB4, 80193DF4-80193E08, 80194288-8019429C, 8019446C-80194480, 8019467C-80194690, 801949D4-801949E8, 80194BA4-80194BB8, 80194D5C-80194D70, 80194F64-80194F78, 80195160-80195174, 80195330-80195344. D_800F336C is stored as 3 immediately before each test and no call in between writes RAM, so the arm is provably unreachable (the `bne` that skips it is covered).
16 words = break guards of the signed `div` by $s1 = 0x200 in the mode 1 particle spawn (states 3 and 4): 80193488, 80193498-801934A0, 801934C8, 801934D8-801934E0, 801935CC, 801935DC-801935E4, 8019360C, 8019361C-80193624. The divisor is a constant, so the div-by-zero (break 7) and 0x80000000 / -1 overflow (break 6) arms are never executed.
4 words = 80193308-80193314: the third func_800C6B90 call (phase 0x6A4) in mode 1 state 3. It runs only when the first call returns nonzero and the second returns 0. The oracle mock returns one value per callee per seed (mock_ret and r() are per target), so that combination cannot be produced. This is a harness limit, not dead code; the C encodes the same A && (B || !C) chain, and the A=B=1 and A=0 arms are covered.

