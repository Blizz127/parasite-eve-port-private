# room_m0432i func_8019408C (NON_MATCHING)

Blob offset 0x50A4, 0x7D0 bytes, VMA 0x8019408C. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_8019408C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_8019408C, blob offset 0x50A4, 0x7D0 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback (a0 = command, a1 = object):
 *  cmd 0  init: clear +8/+A/+C, register the 0x5DB sound/effect id, create
 *         the func_80193940 callback task (returns that call's result);
 *  cmd 1  update: state 0 (16 frames, sound + lift at +C), state 1 (24 frames,
 *         spawns spark objects with random velocity), state 2 (24 frames);
 *  cmd 2  draw: sets the D_800F3368 draw block and, when +C is non-zero,
 *         draws two glow quads (func_800D004C / func_800D0728) with a stack
 *         colour block chosen by D_800E27EC bit 0.
 * Read from the retail disassembly.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_8019408C --check-gte --uncovered --seeds 400
    e21_leaf: ok func_8019408C__f88be1b4_port.c 780 seeds, retail coverage 500/500 words
    e21_leaf: uncovered none

## Uncovered words

Main run leaves only 0x8019464C (addiu v0,0x2, the fall-through of cmd1/state1 into state:=2 when the frame counter reaches >=0x20). It is covered by the supplementary run with h(a1+0xA)=0x1f,0x7fff,0x20 (that run reports ok and 0x8019464C is not in its uncovered list). The first 400-seed run, which has no pointer-chain setup, left the default/negative mode and state arms (0x80194108-0x80194120, 0x8019420C-0x80194224) and the pointer-chain blocks (0x801941BC/C0, 0x801945CC-0x80194634) uncovered; the main run's a0=3,-1 and h(a1+8)=3,-1 and the w()/h() chain setup cover them.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0432i func_8019408C --nonmatching 50A4:7D0 --check-gte --uncovered --seeds 400 --inputs "a0=0,1,2;h(a1+8)=0,1,2;h(a1+0xA)=0,1,0x17,0x18,0x1f,6;h(a1+0xC)=0,5"`
  - e21_leaf: ok func_8019408C__f88be1b4_port.c 247 seeds, retail coverage 463/500 words (uncovered default arms and pointer-chain blocks, as the claim states)
- `claimed run 2 (a0=0,1,3,-1,1,2; ... pointer-chain w()/h() setup ...), --seeds 400`
  - e21_leaf: ok func_8019408C__f88be1b4_port.c 345 seeds, retail coverage 499/500 words; e21_leaf: uncovered 8019464C
- `claimed run 3 (a0=1; h(a1+8)=1; h(a1+0xA)=0x1f,0x7fff,0x20; chain setup), --seeds 60`
  - e21_leaf: ok func_8019408C__f88be1b4_port.c 24 seeds, retail coverage 218/500 words; 0x8019464C is absent from its uncovered list (verified by grep; it appears only in run 2's line)
- `NEW 1: --seeds 300 --inputs "a0=1; h(a1+8)=1; h(a1+0xA)=0x7fff,0x8000,0xffff,0x17,0x18,0x19,0x1a,0x1f,0x20,0x21,0xc,0x6; <pointer chain w()/h() setup>; h(a1+0xC)=0,0xffff"`
  - e21_leaf: ok func_8019408C__f88be1b4_port.c 133 seeds, retail coverage 289/500 words
- `NEW 2: --seeds 300 --inputs "a0=1,2; h(a1+8)=0,2; h(a1+0xA)=0x7fff,0x8000,0xffff,0xf,0x10,0x17,0x18,0x19; h(a1+0xC)=0,1,0x7fff,0x8000,0xffff; w(0x800E27EC)=0,1; <pointer chain setup>"`
  - e21_leaf: ok func_8019408C__f88be1b4_port.c 255 seeds, retail coverage 193/500 words

No MISMATCH, notest or skipped in any of the five runs. The run 1, 2 and 3 coverage figures and the uncovered lists are exactly as claimed. The uncovered-word check holds: asm/overlays/room_m0432i/4958.s line 896 puts 0x8019464C in the delay-slot-following fall-through of 'slti v0,v0,0x20; bnez' at 0x80194644. It is reachable only when h(a1+0xA) >= 0x20 in cmd1/state1, and the supplementary run does reach it with 0x20 and 0x7fff. My two new runs deliberately probed signedness and wrap, with 0xA values 0x7fff/0x8000/0xffff and 0xC values 0x8000/0xffff, plus every mode arm and D_800E27EC bit 0. They agree with retail on all seeds. Their low coverage is expected, because the seeds are the product of the lists and I did not re-add the full claimed spec; the claim's coverage rests on the claimed runs. Caveat: the claimed run 3 coverage is only 218/500 words by itself, so the 500/500 figure is a union of runs, as the claim says. Retail 'ok' results depend on the e21 differential harness only, since this is NON_MATCHING behaviour C."

