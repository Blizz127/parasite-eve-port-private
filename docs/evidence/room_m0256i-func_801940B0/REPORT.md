# room_m0256i func_801940B0 (NON_MATCHING)

Blob offset 0x50C8, 0x1270 bytes, VMA 0x801940B0. The span is `asm` in `configs/USA/overlays/room_m0256i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0256i/func_801940B0.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0256i — func_801940B0, blob offset 0x50C8, 0x1270 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Particle effect callback, read from the retail disassembly. Mode (a0) 1 steps
 * the object at a1 (state at a1+0x14, frame counter a1+0x16): state 0 seeds the
 * position from the pool object (func_800CE8F0/9D4) and spawns a child, state 1
 * is the main flight (grow/scale curve, respawn children every 3rd frame, a
 * sound/flag trigger when func_800C6B90 fires), states 2..5 are timed bounces
 * and fades. Mode 2 draws it (state a1+0x14 - 1): 0 = glow pair around the
 * object, 1 = two-polygon spin, 2 = scaled sprite (func_80078CC4), 3..4 = glow
 * quads with the D_800F3368 draw block set up per state.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0256i func_801940B0 --check-gte --uncovered --seeds 480
    e21_leaf: ok func_801940B0__e0c81fde_port.c 4320 seeds, retail coverage 1173/1180 words
    e21_leaf: uncovered 801943E0 80194ED0 80194ED4 80194ED8 80194EDC 80194EE0 80194EE4

## Uncovered words

801943E0 (1 word) and 80194ED0, 80194ED4, 80194ED8, 80194EDC, 80194EE0, 80194EE4 (6 words).
- 801943E0: the `addiu v0,v0,0xF` fix-up after `mult v0,v0` in mode 1 state 1 (the `x*x/16` step). x is a signed 16-bit value, so x*x is never negative and the `bgez` always branches.
- 80194ED0-80194EE4: mode 2 state 3 tests `D_800F336C == 4` (then `D_800F3428`, then `a2 += 4`). Retail stores `D_800F336C = 3` just before `func_80077A64` and only reads it back afterwards. The arm is reachable only if that callee writes `D_800F336C`, and the real GetTPage-style callee does not.
- Every other word is covered by at least one run. 80194394 (the negative `/4096` fix-up) is covered only by R8, which uses `r(0x80077CF4)` and `w(0x80195EF0)` overrides to force a negative product.

## Independent verification: confirmed

- `R5 re-run exactly (a0=2 draw-mode spec, --seeds 260, --check-gte --uncovered)`
  - R5: e21_leaf: ok func_801940B0__e0c81fde_port.c 260 seeds, retail coverage 603/1180 words (matches claim; uncovered list includes 80194ED0..80194EE4)
- `R8 re-run exactly (a0=1 state 1, r(0x80077CF4)/w(0x80195EF0) overrides, --seeds 24)`
  - R8: e21_leaf: ok func_801940B0__e0c81fde_port.c 24 seeds, retail coverage 232/1180 words (matches claim)
- `N1: e21_leaf room_m0256i func_801940B0 --nonmatching 50C8:1270 --check-gte --uncovered --seeds 40 --inputs "a0=1; a1=0x80180800; h(a1+0x14)=2; h(a1+0x16)=0xF,0xFFFF,0x7FFF; h(a1+2)=0x8000,0xFFFF,0x500,0x4FF; h(0x800942EC)=0x500,0xFB00; h(a1+8)=0x8000,0x7FFF; h(a1+0xC)=0x8001,1; h(a1+0xA)=0x7FFF,0x8000; <PTR>"`
  - N1: e21_leaf: ok func_801940B0__e0c81fde_port.c 40 seeds, retail coverage 118/1180 words
- `N2: same base command --seeds 80 --inputs "a0=2; a1=0x80180800; h(a1+0x14)=4,5; h(a1+0x16)=0xFFFF,0,1,0xC,0xD,0x7FFF,0x8000,0x8001,0x1F; w(0x800F3428)=1,0; h(0x800F336C)=0,2,4; w(0x800E27EC)=-1,3; <PTR>"`
  - N2: e21_leaf: ok func_801940B0__e0c81fde_port.c 80 seeds, retail coverage 283/1180 words
- `N3: same base command --seeds 60 --inputs "a0=1; a1=0x80180800; h(a1+0x14)=1; h(a1+0xE)=0; h(a1+0x10)=0x19,0x31,0x32,0xFFE6,0x7FFF; h(a1+0x12)=0x7FF,0x800,0x7C0,0xFFFF; r(0x800C6B90)=0,1; h(a1+2)=0x8000,0x7FFF,0x2FF; h(0x800942EC)=0x500,0xFB00; w(0x800E27EC)=-3,0x50,0x51; r(0x800CE610)=0x80180A00,0; <PTR>"`
  - N3: e21_leaf: ok func_801940B0__e0c81fde_port.c 14 seeds, retail coverage 391/1180 words (the harness ran 14 seeds although --seeds 60 was requested; no MISMATCH, notest or skipped printed)

No refutation found. I re-ran only R5 and R8 of the nine claimed runs. I did not re-run R1b, R2b, R3, R4a, R4b, R6 or R7, so their quoted results and the 1173/1180 union are taken from the claim, not re-checked. The two re-runs matched the claimed coverage counts, and all three new specs printed ok with no MISMATCH, notest or skipped.\n\nThe two uncovered-word justifications hold against the asm.\n- 801943E0 is the `addiu v0,v0,0xF` after `lh v0,0x10(s4); mult v0,v0; mflo v0; bgez v0`. A signed 16-bit value squared is at most 2^30 and never negative, so the branch always skips it. It is genuinely unreachable.\n- 80194ED0..80194EE4 is the `D_800F336C==4` arm in mode 2 state 3. In the asm, `sh 3,D_800F336C` at 80194E74 comes before `jal func_80077A64` at 80194E8C, and `lhu D_800F336C` at 80194E9C comes after it. src/func_80077A64.c is a pure bit-pack of its arguments and writes no globals, so the value read back is always 3 and the `bne` always takes the branch. The arm is dead for the retail callee.\n\nMinor caveat. R8's override-driven coverage of the negative `/4096` fix-up at 80194394 relies on `r(0x80077CF4)` and `w(0x80195EF0)` overrides to force a negative product. That is a legitimate way to reach it, since the callee result is an input, but it is not reachable from sine-table values alone. I did not check that.\n\nThe new inputs (N1 to N3) covered mode 1 state 2 with sign and limit edges on a2, a8, a10, a12 and a16, and mode 1 state 1 with a10 and a12 boundaries and a negative or boundary D_800E27EC. They also covered mode 2 states 3 and 4 with a16 of -1, 0, 1, 0xC, 0xD, 0x7FFF and 0x8000, D_800F336C in {0,2,4}, and D_800F3428 in {0,1}. All matched the retail asm."]

