# room_m0318i func_80196E4C (NON_MATCHING)

Blob offset 0x7E64, 0x7CC bytes, VMA 0x80196E4C. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80196E4C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i -- func_80196E4C, blob offset 0x7E64, 0x7CC bytes. NON_MATCHING
 * (behaviour C, not a byte match).
 * e21 status (in-tree harness, unmodified): MISMATCH at seed 0 on call
 * func_800D1384 a3 (calls 2 and 4, the c2 colour). Cause: retail spills &c2 to
 * sp+0x70 (`sw $t1,0x70($sp)`, retail 80196F9C, `lw $a3,0x70($sp)` at 80197090
 * / 80197154) which is 8 bytes past c2, so the harness's default 16-byte
 * stack-argument window at c2 includes a dead retail stack address (0x801FFEC8)
 * that no native frame (arena 0x801FE000-0x801FF000) can reproduce. func_800D1384
 * reads only 3 bytes through col0/col1 (pc_port/game/boot/func_800D1384_port.c
 * lines 74/76), so the compare needs STACK_EXTENTS[(0x800D1384,3)] = 3 and
 * [(0x800D1384,4)] = 3 in tools/analysis/e21_diff.py (not editable here).
 * Diagnostic only (a build with the dead slot set to the constant 0x801FFEC8,
 * NOT this file): ok, 129 seeds, retail coverage 492/499 words; the seven
 * uncovered words 80197370-80197388 are the D_800F336C == 4 arm, dead because
 * mode 2 stores 3 to D_800F336C (sh at 80197310) and only func_800CE8F0 runs
 * before the lhu at 80197350. The F layout below mirrors retail's frame.
 * Effect-object callback (a0 = mode, a1 = effect record, a2 = int *busy).
 * Mode 0 sets the record header (+0 = 6, +2 = 0x13, +4 = +6 = 0), then draws
 * four beam segments between actor joints 8 and 0x15 (func_800CE8F0 for the
 * points, func_800D1384 for the gradient segments at record +8/+0xA8/+0x58/
 * +0x118, the offsets of joints 0x15 mirrored on x) and returns the result of
 * func_800CE560 (registers the 0x10-entry task func_80196C48).
 * Mode 1 counts frames at +6; returns 2 while *a2 is nonzero, else spawns an
 * effect entry (func_800CE610) at the joint (+0 or +2 picks the joint by frame
 * parity) and returns 0. Mode 2 loads the camera matrix into the GTE, sets
 * the 0x800E27EC-dependent size, draws the glow sprite via func_800CEE20, then
 * redraws the four segments and the D_800F3368 sprite block.
 * Any other mode returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80196E4C --check-gte --uncovered --seeds 80
    e21_leaf: ok func_80196E4C__c8096190_port.c 260 seeds, retail coverage 492/499 words
    e21_leaf: uncovered 80197370 80197374 80197378 8019737C 80197380 80197384 80197388

## Uncovered words

Diagnostic run only: 80197370, 80197374, 80197378, 8019737C, 80197380, 80197384, 80197388. This is the D_800F336C == 4 && D_800F3428 != 0 arm of mode 2 (retail bne at 80197368, lw D_800F3428 at 80197374). It is dead. Mode 2 stores 3 to D_800F336C (sh at 80197310 / 8019730C-80197310). The only call before the lhu at 80197350 is func_800CE8F0, which writes through its stack output pointer (a3 = sp+0x28), and e21 treats that as pure output. So bne a0,4 is always taken. The branch stays in the C for fidelity. Nothing else was uncovered in the diagnostic run. The roughly 480 uncovered words in the in-tree output (80196FB4-80196FCC, 8019719C-801975E0) are only the seed-0 abort.

## Independent verification: refuted

- `cd /home/blizz/dev/Parasite-Eve-Decompilation/build/wt-native-20261017 && python3 tools/analysis/e21_leaf.py room_m0318i func_80196E4C --nonmatching 7E64:7CC --check-gte --uncovered --seeds 80 --inputs "a0=0,1,2,3,-1;h(a1+6)=0,1;w(a2+0)=0,1;w(0x800E27EC)=0,1"`
  - e21_leaf: MISMATCH func_80196E4C__c8096190_port.c seed 0: call 800D1384 a3 stack arg @801FFEC8:800064002c1b1c80c8fe1f80e8c11d8000000000000000000000000000000000 vs S:801FEBF0:8000640002000000F0EB1F8064B11D80D4101480C4BB1580780C1980FF000000 (retail-written wwww....wwww....................); followed by an 'uncovered' line listing ~480 words (80196FB4-80196FCC, 8019719C-801975E0) because the run aborted at seed 0
- `python3 tools/analysis/e21_leaf.py room_m0318i func_80196E4C --nonmatching 7E64:7CC --check-gte --uncovered --seeds 32 --inputs "a0=0,2,-1,0x7fffffff;h(a1+6)=0,1,-1,0x7fff;w(a2+0)=0,1"`
  - e21_leaf: MISMATCH func_80196E4C__c8096190_port.c seed 0: call 800D1384 a3 stack arg (identical text to run 1); uncovered line again lists the same ~480 words
- `grep -n 'STACK_EXTENTS' tools/analysis/e21_diff.py`
  - STACK_EXTENTS defined at e21_diff.py:962 (no (0x800D1384,3) entry); default extent 16 bytes is used at line 1271

The in-tree harness prints MISMATCH at seed 0 on both the claimed command and a new spec. The failing item is the 4th argument (a3) of the call to 800D1384. The stack-extent comparison reads 16 bytes at 801FFEC8 (C) against 801FEBF0 (retail), and the contents differ. The claim says this is a harness artifact fixed by a patched out-of-tree copy of e21_diff.py. That copy is not in the tree, so the in-tree result is what the rule scores, and it is a MISMATCH. I did not verify the 'ok 64 seeds, 492/499' result from the patched copy. I did not verify the claim that 80197370-80197388 is unreachable. I did not inspect the stack-pointer difference between the C and retail frames. That difference could point to a real divergence, such as a different local buffer layout or argument pointer, or it could be only a frame-address difference. The claim leaves this open, and the patch hides it by narrowing the compared extent to 3 bytes. Because every in-tree run aborts at seed 0, no coverage figure is available from the unpatched harness. The uncovered line lists about 480 words, which is not the 7 words the claim reports.


## Main-session verification (the workflow's verifier refuted on a harness artifact)

The decomp agent's in-tree runs stopped at seed 0 with `call 800D1384 a3 stack
arg` MISMATCH. The workflow's verifier scored that as refuted, and the repair
agent could not change the harness. The mismatching bytes were outside what
the callee reads. Retail func_800D1384 reads each colour pointer only as
three bytes:
- col0 = a3 -> t2, lbu 0/1/2(t2) at 800D1538..800D1580;
- col1 = sp+0x90 -> t4, lbu 0/1/2(t4) at 800D15BC..800D1604.

The harness's default 16-byte window behind c2 took in retail's spill of &c2
at sp+0x70, a guest stack address no native frame can hold. Commit
55fe00601 sets `(0x800D1384, 3): 3` and `(0x800D1384, 4): 3` in
STACK_EXTENTS. After that, the claim's own command:

    e21_leaf.py room_m0318i func_80196E4C --nonmatching 7E64:7CC --check-gte --uncovered --seeds 80 --inputs "a0=0,1,2,3,-1;h(a1+6)=0,1;w(a2+0)=0,1;w(0x800E27EC)=0,1"
    e21_leaf: ok func_80196E4C__c8096190_port.c 64 seeds, retail coverage 492/499 words
    e21_leaf: uncovered 80197370 80197374 80197378 8019737C 80197380 80197384 80197388

Three edge-value specs of the main session's own, now in nonmatching.yaml:

    --seeds 48 --inputs "a0=1;h(a1+6)=0,1,2,0x7FFF,0xFFFF;w(a2+0)=0,1,-1"
    e21_leaf: ok ... 38 seeds, retail coverage 141/499 words
    --seeds 48 --inputs "a0=2;w(0x800E27EC)=0,1,0x10,0x7FFFFFFF,-1;h(0x800F336C)=4,0;w(0x800F3428)=1,0"
    e21_leaf: ok ... 36 seeds, retail coverage 329/499 words
    --seeds 48 --inputs "a0=0,3,-1,0x7FFFFFFF;h(a1+0)=0,6;w(a2+0)=0"
    e21_leaf: ok ... 45 seeds, retail coverage 225/499 words

The 7 uncovered words are the `D_800F336C == 4 && D_800F3428 != 0` arm.
Mode 2 stores 3 to D_800F336C (sh at 80197310), and only func_800CE8F0 runs
before the lhu at 80197350.
