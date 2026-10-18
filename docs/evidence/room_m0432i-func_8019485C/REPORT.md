# room_m0432i func_8019485C (NON_MATCHING)

Blob offset 0x5874, 0x704 bytes, VMA 0x8019485C. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_8019485C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_8019485C, blob offset 0x5874, 0x704 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback, cmd 1 (integrate: x/y/z += velocity, velocity decays
 * by 127/128, state 0 also bounces y against D_800942EC and adds a sin/cos
 * wobble; frame limits 0x20 / 0x10 / 8 for states 0 / 1 / 2) and cmd 2 (draw
 * the sprite through func_800CEE20 with a per-state D_800F3368 block).
 * Read from the retail disassembly.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_8019485C --check-gte --uncovered --seeds 200
    e21_leaf: ok func_8019485C__d42bf0c5_port.c 400 seeds, retail coverage 435/449 words
    e21_leaf: uncovered 80194C70 80194C74 80194C78 80194C7C 80194C80 80194C84 80194C88 80194E04 80194E08 80194E0C 80194E10 80194E14 80194E18 80194E1C

## Uncovered words

Main run left 20 words uncovered. The supplementary run closes six of them: 801948B0/801948B4 (mode not 1 or 2 -> return 0), 801948D8/801948DC (cmd 1, state outside 0..2 -> return 0), 80194B8C/80194B90 (cmd 2, state < 0 or > 2 -> return 0).
The remaining 14 are unreachable. 80194C70-80194C88 (cmd 2 state 0) is the "D_800F336C == 4 && D_800F3428 != 0 -> clut index +8" arm, and 80194E04-80194E1C (cmd 2 state 1) is the same arm with +6. Both are dead because the function stores D_800F336C = 3 a few instructions earlier and the only call in between (func_800CF3AC) is mocked and writes no globals. The `bne $a0,4` therefore always branches to the +4 / +2 arm. The C keeps the arms for fidelity.

## Independent verification: confirmed

- `e21_leaf.py room_m0432i func_8019485C --nonmatching 5874:704 --check-gte --uncovered --seeds 200 --inputs "a0=1,2;h(a1+0x10)=0,1,2,3;h(a1+0x12)=0,5,0x1f,0x20,7,8,0xf,0x10,-3,0x7ff0,0x1e;h(0x800F336C)=4,0;w(0x800F3428)=1,0"`
  - e21_leaf: ok func_8019485C__d42bf0c5_port.c 200 seeds, retail coverage 429/449 words; uncovered 801948B0/B4 801948D8/DC 80194B8C/B90 80194C70-C88 80194E04-E1C (identical to the claim)
- `e21_leaf.py room_m0432i func_8019485C --nonmatching 5874:704 --check-gte --uncovered --seeds 24 --inputs "a0=0,1,2,3;h(a1+0x10)=-1,3,4"`
  - e21_leaf: ok ... 24 seeds, retail coverage 60/449 words; its uncovered list starts at 801948F4, so the six default-arm words (801948B0/B4, 801948D8/DC, 80194B8C/B90) are covered as claimed
- `NEW1: ... --seeds 60 --inputs "a0=1,2;h(a1+0x10)=0,1,2;h(a1+0x12)=-1,0,1,0x7fff,-0x8000,0x21;h(0x800F336C)=3,4,5"`
  - NEW1: e21_leaf: ok ... 60 seeds, retail coverage 425/449 words; uncovered 801948B0 B4 D8 DC EC F0 80194B8C B90 B9C BA0 plus 80194C70-C88 and 80194E04-E1C (the extra uncovered words are only the default arms that the first command's other input lists cover)
- `NEW2: ... --seeds 40 --inputs "a0=-1,0,1,2,3,0x10001,0x7fffffff;h(a1+0x10)=0x8000,0x7fff,-2,0xffff,1,2;h(a1+0x12)=0x1e,0x1f,0x20,0x21,16,15"`
  - NEW2: e21_leaf: ok ... 40 seeds, retail coverage 288/449 words (a0 values outside 1/2 and out-of-range states, so much of the function is skipped by design); no MISMATCH, notest or skipped

No MISMATCH, notest or skipped in any of the four runs. The 14 words claimed unreachable (80194C70-C88 and 80194E04-E1C) do not turn out to be reachable. The asm stores 3 to D_800F336C at 80194C04 (state 0) and at 80194D6C (state 1) with `sh $v0` (v0=3). The only call in between is func_800CF3AC, and its source src/func_800CF3AC.c shows it writes only through its own header and out pointers, so D_800F336C stays 3 and `bne $a0,4` always branches. NEW1 set h(0x800F336C)=3,4,5 as an input and those words stayed uncovered, which fits the claim. One caveat: the argument is oracle-based. It assumes func_800CF3AC's `out` pointer (a1=$s3) never aliases D_800F336C, and `out` looks like a stack address. I could not construct an input that reaches those words."]

