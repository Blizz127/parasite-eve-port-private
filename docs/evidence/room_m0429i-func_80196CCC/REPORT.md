# room_m0429i func_80196CCC (NON_MATCHING)

Blob offset 0x7CE4, 0x620 bytes, VMA 0x80196CCC. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_80196CCC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_80196CCC, blob offset 0x7CE4, 0x620 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, modes 0/1/2 on a record a1, read from the retail
 * disassembly. Mode 0 zeroes +0, sets +2 = 0x10, plays sound 0x5E1/0x5E2 and
 * arms func_80196748 (0x10, 0x32). Mode 1 counts +0 up; on odd ticks it spawns
 * one state-2 spark with three random coordinates, every 4th tick (while +2 >
 * 0) a state-0 spark with a +-16 velocity (and decrements +2), and on tick 1 a
 * burst of 26 state-1 sparks; done at tick 0x30. Mode 2 places the D_800F3368
 * draw block, reads a world position through func_800CE8F0 into D_8019B670,
 * draws three func_800CEE20 sprites and two func_800D0728 rings (size by
 * tick*128/48), and finishes by re-seeding the draw block.
 * inline GTE/asm: none (the Psy-Q SetRot/SetTrans macros of the twins are
 * not used by this callback).
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_80196CCC --check-gte --uncovered --seeds 120
    e21_leaf: ok func_80196CCC__0c265747_port.c 206 seeds, retail coverage 380/392 words
    e21_leaf: uncovered 801970BC 801970C0 801970C4 801970C8 801970CC 801970D0 80197168 8019716C 80197170 80197174 80197178 8019717C

## Uncovered words

Main-run uncovered words: 80196D58, 80196D5C, 80196D6C and 80196D70 are the mode-dispatch default arms (a0 neither 0, 1 nor 2). The supplementary run with a0=0xFFFFFFFF,3,5 covers them. 801970BC-801970D0 and 80197168-8019717C are the 'D_800F336C == 4 && D_800F3428 != 0 then +4' CLUT adjustments in mode 2. They are impossible: the function stores D_800F336C = 3 (at 80196FA0) just before reading it, and nothing between that store and the reads can change it, because the callee mocks do not write it. The C keeps the same branch.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196CCC --nonmatching 7CE4:620 --check-gte --uncovered --seeds 120 --inputs "a0=0,1,2; h(a1+0)=0,1,3,0x2E,0x2F; h(a1+2)=0,5,0xFFFF; h(a1+0x10)=0,1"`
  - e21_leaf: ok func_80196CCC__0c265747_port.c 86 seeds, retail coverage 376/392 words; e21_leaf: uncovered 80196D58 80196D5C 80196D6C 80196D70 801970BC 801970C0 801970C4 801970C8 801970CC 801970D0 80197168 8019716C 80197170 80197174 80197178 8019717C (reproduced exactly)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196CCC --nonmatching 7CE4:620 --check-gte --uncovered --seeds 6 --inputs "a0=0xFFFFFFFF,3,5"`
  - e21_leaf: ok func_80196CCC__0c265747_port.c 6 seeds, retail coverage 52/392 words; uncovered list starts at 80196D74, so 80196D58/5C/6C/70 are covered
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196CCC --nonmatching 7CE4:620 --check-gte --uncovered --seeds 120 --inputs "a0=1; h(a1+0)=0,1,2,3,4,5,0x2E,0x2F,0x30,0xFFFF; h(a1+2)=0xFFFF,0,1,2,0x7FFF,0x8000"`
  - new run A (a0=1 sweep with signed edge values for h(a1+0) and h(a1+2)): e21_leaf: ok func_80196CCC__0c265747_port.c 86 seeds, retail coverage 138/392 words (no MISMATCH, notest or skipped)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80196CCC --nonmatching 7CE4:620 --check-gte --uncovered --seeds 40 --inputs "a0=0,2,1; h(a1+0)=0xFFFF,0x7FFF,0x8000,0x30; h(a1+2)=0x8000,0x7FFF,0xFFFF"`
  - new run B (a0=0,2,1 with 0x7FFF/0x8000/0xFFFF edges): e21_leaf: ok func_80196CCC__0c265747_port.c 26 seeds, retail coverage 336/392 words (no MISMATCH, notest or skipped)
- `grep/sed inspection of asm/overlays/room_m0429i/3FB4.s lines 4105-4512 (branch targets and the D_800F336C store/read sites)`
  - Asm check: the only branch into the mode-2 block is beq $a3,$v0 -> .L80196F4C. That block sets $v0=3 at 80196F64 and stores it to D_800F336C at 80196FA0 (sh $v0). The next reads are at 80197094 (lhu $v1) and 80197124 (lhu $a2). No label or branch lets mode 2 skip the store.

No MISMATCH, notest or skipped result in any run. The claimed commands reproduce exactly, and my two new edge-value runs both pass. The four default-arm words (80196D58/5C/6C/70) are covered by the supplementary a0=0xFFFFFFFF,3,5 run. The uncovered words 801970BC-801970D0 and 80197168-8019717C are only reachable if D_800F336C == 4 at the read, and the function stores 3 into it first. The 80197094 and 80197124 reads follow calls to func_800D004C, func_800CE8F0, func_80077CF4, func_800CF3AC and func_800CEE20. The e21 mocks of these do not write D_800F336C, but I did not find the real callee bodies, so I could not confirm the real ones never write it. One caveat: the reasoning that the branches are unreachable rests on the mocks, not on the real callee bodies. I found no input to the function itself that reaches those words. Run A's coverage of 138/392 is expected, since it only exercises mode 1."]

