# room_m0432i func_80195B40 (NON_MATCHING)

Blob offset 0x6B58, 0x7BC bytes, VMA 0x80195B40. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_80195B40.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_80195B40, blob offset 0x6B58, 0x7BC bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect spawner / callback (a0 = command 0/1/2, a1 = effect record), read
 * from the retail disassembly.
 *   cmd 0: clears the record's +8..+0xE, plays two sounds through
 *          func_8006DF50 when D_800B0E64 is set, registers the record with
 *          func_800CE8F0 and the callback func_801957CC with func_800CE560.
 *   cmd 1: steps the record. State 0 (30 frames): +0xA frame counter,
 *          +0xC = sin(f<<10 / 30) / 32, +0xE = cos(..) + 0x400, spawns four
 *          debris particles per frame while f < 9, then eight at the switch to
 *          state 1. State 1 (20 frames): counter / 20, sin and cos swapped.
 *   cmd 2: draws. Sets the D_800F3368 draw block and the GTE rotation /
 *          translation from D_800BCFA4[0]; state 0 draws through
 *          func_800D004C and func_800CEE20, state 1 through func_800D1AE0 /
 *          func_800D0728 and func_800CEE20. Every cmd 2 path ends with
 *          D_800F3372 = 0, D_800F3374 = 4 and returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_80195B40 --check-gte --uncovered --seeds 216
    e21_leaf: ok func_80195B40__9df4e8d0_port.c 525 seeds, retail coverage 488/495 words
    e21_leaf: uncovered 80196260 80196264 80196268 8019626C 80196270 80196274 80196278

## Uncovered words

80195BF0 80195BF4 80195C04 80195C08: the default and unknown-mode arms (a0 not 0, 1 or 2, and a1+8 state values the switch does not handle). Covered by the supplementary run a0=-1,3,0x7FFFFFFF,5, which prints no mismatch.

80196260 80196264 80196268 8019626C 80196270 80196274 80196278: the `D_800F336C == 4 && D_800F3428 != 0` test in the cmd 2 state-1 arm, i.e. the `a += 0xA` branch. Unreachable, and the verifier's reading of the asm is wrong. Both stores of 3 to D_800F336C sit in the cmd 2 common prefix, which runs before the state switch, not inside the state-0 arm:
- 80195FF0 `lui $at,%hi(D_800F336C)` and 80195FF4 `sh $v0,%lo(D_800F336C)($at)` store 3 (`addiu $v0,$zero,3` at 80195FEC). They come after the 80195FBC-80195FE8 D_800F3368 setup and before 80196014 `lh $v0,8($s3)`.
- The state switch is `beqz` at 8019601C (state 0, to .L80196034) and `beq` at 80196024 (state 1, to .L8019616C). Both come after that store.
- The second store of 3, at 80196104-80196108, is inside the state-0 arm and is not the one that matters here.

Every cmd 2 path therefore holds D_800F336C == 3 at 8019623C, so the `bne $a0,4` at 80196258 always branches to .L8019627C. Between the store and the read in state 1 the only calls are func_800D1AE0, func_80077CF4 and func_800D0728. None of these or their callees (func_80077A64, 77C84, 77BC4, 79754, 78CC4, 786E4) reference D_800F336C or D_800F3368 in src/*.c, and the harness mocks callees, so they cannot write it either.

The differential agrees: directed h(0x800F336C)=4 together with w(0x800F3428)=1 reaches the state-1 arm 8019616C-801961CC but the retail oracle never executes 80196260-80196278, because 80195FF0 overwrites the value first. Retail and the C both take the +6 path. The C arm `a += 0xA` is kept as retail has it and is untested only because it is dead.

The main run reported 151 seeds although 216 were requested. The mixed-radix spec gives 3*3*6*2*2 = 216 combinations. The other 65 were skipped by the tool because the retail oracle faulted on random RAM pointers; the tool skips such cases silently, per the e21_diff.py docstring.

## Independent verification: confirmed

- `e21_leaf.py room_m0432i func_80195B40 --nonmatching 6B58:7BC --check-gte --uncovered --seeds 216 --inputs "a0=0,1,2; h(a1+8)=0,1,2; h(a1+0xA)=0,1,5,6,19,29; w(0x800B0E64)=0,1; w(0x800E27EC)=0,1"`
  - e21_leaf: ok func_80195B40__9df4e8d0_port.c 151 seeds, retail coverage 484/495 words
- `e21_leaf.py ... --seeds 36 --inputs "a0=2; h(a1+8)=1; h(a1+0xA)=-1,2,3,4,5,6,7,0x7FFF,-0x8000; h(a1+0xC)=0,-1,0x7FFF,-0x8000; w(0x800F3428)=0,1"`
  - e21_leaf: uncovered 80195BF0 80195BF4 80195C04 80195C08 80196260 80196264 80196268 8019626C 80196270 80196274 80196278 (identical to claim)
- `e21_leaf.py ... --seeds 24 --inputs "a0=2; h(a1+8)=0,-1,2; h(a1+0xA)=0,1,19,29,30; h(a1+0xE)=-1,0,0x7FFF,-0x8000; w(0x800E27EC)=0,1"`
  - NEW run 1: e21_leaf: ok func_80195B40__9df4e8d0_port.c 25 seeds, retail coverage 192/495 words (no MISMATCH/notest/skipped printed; seeds 25 of 36 requested, rest skipped silently for oracle faults)
- `grep/sed of asm 4958.s lines 2335-2859 for D_800F336C stores, branch labels and callees; grep of src/func_800D0728.c, func_800D1AE0.c, func_8003E188.c for D_800F336C`
  - NEW run 2: e21_leaf: ok func_80195B40__9df4e8d0_port.c 15 seeds, retail coverage 195/495 words (no MISMATCH; 15 of 24 seeds)

No refutation. Verified in asm: 80195FF0/80195FF4 store 3 to D_800F336C in the cmd-2 common prefix after the 80195FBC D_800F3368 setup and before 80196014 lh/8019601C beqz/80196024 beq. Only branch labels in that region are .L80196034, .L8019616C, .L801962xx, so no path reaches 8019623C without passing the store; the bne $a0,4 at 80196258 always goes to .L8019627C, so 80196260-80196278 are dead. Callees (func_800D0728, func_800D1AE0, func_80077CF4) live in EXE src; none write D_800F336C (func_8003E188 only declares it, and func_800D0728 does not call it). Minor notes only: the new narrow specs have low coverage by design (they target state-1/state-0 arms with signed edge values) and some seeds were silently skipped by the tool (25/36, 15/24), same behaviour as the claimed 151/216. The 4 default-arm words were not re-run by me in the supplementary a0 spec, but the asm shows them as j .L801962D8 default exits, consistent with the claim."]

