# room_m0432i func_80196554 (NON_MATCHING)

Blob offset 0x756C, 0xAD8 bytes, VMA 0x80196554. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_80196554.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_80196554, blob offset 0x756C, 0xAD8 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-spawner / callback (a0 = command 0/1/2, a1 = effect record), read from
 * the retail disassembly. Registered with func_800CE560 by itself (cmd 0) and
 * the callback func_801962FC is attached to a second record.
 *   cmd 0: clears +0x18/+0x1A/+0x1C, loads the D_800B0E64 resource 0xC54C4704
 *          into D_80199500, registers the record with func_800CE8F0 /
 *          func_800CE9D4, shifts +0/+2 by (-0x220, -0x80) and registers
 *          func_801962FC with func_800CE560 (its result is the return value).
 *   cmd 1: state (+0x18) 0 waits 16 frames (+0x1A) then goes to state 1; state 1
 *          spawns one debris particle per frame via func_800CE610 for 16 frames
 *          then goes to state 2; state 2 returns 2 (retail leaves v0 = 2).
 *   cmd 2: state 0 / state 1 set the GTE rotation/translation from D_800BCFA4[0],
 *          drive the D_80199500 3D effect (func_800C6xxx) with a scale matrix and
 *          draw sprites through func_800CEE20/func_800D004C/func_800D3BC8
 *          (state 1 only). Every cmd 2 path ends D_800F3372 = 0, D_800F3374 = 4
 *          (0x28 stays only inside the state-1 draw calls) and returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_80196554 --check-gte --uncovered --seeds 200
    e21_leaf: ok func_80196554__346c55cc_port.c 315 seeds, retail coverage 667/694 words
    e21_leaf: uncovered 80196A3C 80196A40 80196A44 80196A48 80196A4C 80196A50 80196A54 80196C1C 80196C20 80196C24 80196C28 80196C2C 80196C30 80196C34 80196E58 80196E5C 80196E60 80196E64 80196E68 80196E6C 80196E70 80196F94 80196F98 80196F9C 80196FA0 80196FA4 80196FA8

## Uncovered words

The 27 uncovered words in the main run are the "D_800F336C == 4 && D_800F3428 != 0" arms. In all four places the C keeps the arm and the oracle never reaches it, because D_800F336C was stored as 3 earlier in the same call and only mocked callees (which write no RAM) run in between.
- 80196A3C..80196A54 (7 words): cmd 2 state 0, `a1 += 7` arm.
- 80196C1C..80196C34 (7 words): cmd 2 state 1, first block, `a1 += 7` arm.
- 80196E58..80196E70 (7 words): cmd 2 state 1, after func_800D004C, `a1 += 8` arm.
- 80196F94..80196FA8 (6 words): cmd 2 state 1, last block, `a1 += 4` arm. The arm is the single `addiu` at 80196FA8; the word range starts at 80196F94 and covers the D_800F3428 test.
80196B7C and 80196B80 (the negative s4*3 rounding fix-up in state 1) are uncovered in the main run only and are executed by the supplementary run above.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0432i func_80196554 --nonmatching 756C:AD8 --check-gte --uncovered --seeds 200 --inputs "a1=0x80120000,0x80130100;a0=0,1,2,0xFFFFFFFF,3;h(a1+0x18)=0,1,2,0xFFFF,3;h(a1+0x1A)=0,15;w(0x800E27EC)=0,1"`
  - e21_leaf: ok func_80196554__346c55cc_port.c 166 seeds, retail coverage 665/694 words; uncovered exactly 80196A3C-54, 80196B7C, 80196B80, 80196C1C-34, 80196E58-70, 80196F94-A8 (29 words listed, matches claim)
- `python3 tools/analysis/e21_leaf.py ... --seeds 8 --inputs "a1=0x80120000;a0=2;h(a1+0x18)=1;h(a1+0x1A)=0,15;w(0x800E27EC)=1;r(0x80077DC4)=0xFFFF0000,0xFFFF8000"`
  - e21_leaf: ok ... 7 seeds, retail coverage 392/694 words; 80196B7C/80196B80 absent from the uncovered list (executed)
- `NEW1: --seeds 16 --inputs "a1=0x80120000;a0=1;h(a1+0x18)=1;h(a1+0x1A)=0x7FFF,0x8000,0xFFFF,1;w(0x800E27EC)=1,3;r(0x80077DC4)=0xFFFF8000,0x7FFFFFFF,0x80000001,0xFFFFFFFF"`
  - NEW1: e21_leaf: ok ... 8 seeds, retail coverage 133/694 words (no MISMATCH)
- `NEW2: --seeds 16 --inputs "a1=0x80120000;a0=2,0x80000000;h(a1+0x18)=0x8000,0xFFFE,2,1;h(a1+0x1A)=0xFFFF,0x8000;w(0x800E27EC)=1,2;r(0x80077CF4)=0xFFFFFFE0,0xFFFFFFFF,31,0x7FFFFFFF"`
  - NEW2: e21_leaf: ok ... 15 seeds, retail coverage 401/694 words (no MISMATCH)

No MISMATCH, notest or skipped in any run. The uncovered arms check out against the asm. At 801969D4-801969D8 the retail code stores the constant 3 into D_800F336C with sh. The following calls (func_80077A64, func_80077AA4 and others) are mocked and write no RAM. The test at 80196A34 (bne $a0,4) therefore never takes the D_800F3428 arm, and the same holds at 80196C1C, 80196E58 and 80196F94. The C in func_80196554.c lines 183, 231 and 254 keeps the `D_800F3368.a6C == 4 && D_800F3428 != 0` test in each arm. Only a real callee that rewrites D_800F336C could reach these arms, and no input spec can produce that. Minor wording nit: the claim says 27 words but the listed ranges add up to 29 words. The 4 ranges are 7+7+7+6 = 27, plus the two words at 80196B7C/80196B80, which the supplementary run covers. That is consistent. The new runs ran fewer seeds than the product of their lists (8 and 15 rather than 32 and 32), but all executed cases passed."


## Note from the main session

The claim gives "692/694 words across the two runs". That is an arithmetic slip. Its own
list leaves 27 words uncovered (7+7+7+6 in the four `D_800F336C == 4` arms), so 667/694 is
right, which is what the re-run above prints. 80196B7C/80196B80 are covered by the
supplementary `r(0x80077DC4)` line.
