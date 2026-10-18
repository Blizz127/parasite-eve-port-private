# room_m0429i func_8019549C (NON_MATCHING)

Blob offset 0x64B4, 0xC28 bytes, VMA 0x8019549C. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_8019549C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_8019549C, blob offset 0x64B4, 0xC28 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback (registered by func_801960C4 through func_800CE560;
 * same family as func_801982A4 / room_m0269i func_8019070C).  Seeds sp+0x30
 * from the 8 text bytes at func_8018F210 and sp+0x48 from D_8018F218.
 * state at a1+0x1C, frame counter at a1+0x1E, position at a1+0/2/4,
 * velocity at a1+8/0xA/0xC.
 * cmd 1 state 0: spawn, seeds position from func_800CE8F0 / func_800CE9D4 and
 *   func_800CFE94 (height test), random jitter into a1+8/0xA/0xC.
 * cmd 1 state 1: integrate (func_800CFAA8/CFD50/CFB7C), clamp speed a1+0x18
 *   and a1+0x1A, move, floor test D_800942EC; state 2: timed life (0x18 ticks)
 *   with the same gate-flag block; state 3: 12 ticks.
 * cmd 2 state 1: three func_800CEE20 sprites + func_800D004C ring, flash
 *   func_800D3BC8 pair on odd a1+6, func_800D0E88 trail; state 2: grow/shrink
 *   glow (func_80077DC4 envelope) with a func_800CEE20 pair.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_8019549C --check-gte --uncovered --seeds 320
    e21_leaf: ok func_8019549C__f976b874_port.c 605 seeds, retail coverage 777/778 words
    e21_leaf: uncovered 80195748

## Uncovered words

80195748 (1 word, `addiu $v0,$v0,0xF` in the signed /16 rounding fix-up before `sll 12; sra 16`): taken only when `lh*lh` is negative. The square is `mult` of a sign-extended 16-bit value with itself, so it is at most 2^30 and never negative; the branch is provably impossible. The C omits the fix-up and computes `(int)((unsigned)sq << 12) >> 16`. Words that each single run leaves uncovered are covered by the other run: M1 misses the a0!=1 dispatch (80195508-18), state-4 default (80195558-5C) and all of mode 2 (801959CC-801960A0); M2 misses the negative-state return (8019553C-40, state 0xFFFF) and the mode-1 gate-flag block and state 2/3 arms (80195810-78, 80195928-90, 801959A0-B8, 801959C4-C8).

## Independent verification: confirmed

- `e21_leaf.py room_m0429i func_8019549C --nonmatching 64B4:C28 --check-gte --uncovered --seeds 320 --inputs <M1 spec, verbatim from claim>`
  - M1: e21_leaf: ok func_8019549C__f976b874_port.c 320 seeds, retail coverage 332/778 words
- `same with --seeds 288 --inputs <M2 spec, verbatim from claim>`
  - M2: e21_leaf: ok func_8019549C__f976b874_port.c 255 seeds, retail coverage 712/778 words
- `NEW1: --seeds 400 --inputs 'a0=1; h(a1+0x1C)=1,2; h(a1+0x1E)=0xA,0x16; h(a1+0x18)=0x8000,0x7FFF,0xFFFF,0,1,0xF,0x10,0xFFF0,0x8001; h(a1+0x1A)=0x8000,0x7FFF,0xFFFF,0,1,0x7FF; r(0x800C6B90)=1,0'`
  - NEW1 (squares of 0x8000/0x7FFF/0xFFFF/0/1/0xF/0x10/0xFFF0/0x8001 incl. negative lh values, rounding edges): e21_leaf: ok func_8019549C__f976b874_port.c 300 seeds, retail coverage 221/778 words
- `NEW2: --seeds 400 --inputs 'a0=2,3,1,0,5,0xFFFFFFFF; h(a1+0x1C)=0,1,2,3,4,5,0x8000,0xFFFF; h(a1+0x1E)=0,9,0xA,0xB,0x15,0x16,0x17,0x8000; h(a1+6)=0,1,0xFFFF; h(0x800F336C)=0,4,5; w(0x800F3428)=1,0; r(0x80077DC4)=-0x8000,-1,0,1,0x7FFF'`
  - NEW2 (a0 values 0..5/-1, state 0..5/0x8000/0xFFFF, extreme r(0x80077DC4) values): e21_leaf: ok func_8019549C__f976b874_port.c 384 seeds, retail coverage 680/778 words

I found no refutation. The asm at 8019572C-8019574C reads `lh $v0,0x18($s1)`, runs `mult $v0,$v0` and `mflo`, then `bgez $v0` with `addiu $a0,$s1,8` in the delay slot. The `addiu $v0,$v0,0xF` at 80195748 is the fall-through, taken only when the product is negative. A square of a sign-extended 16-bit value is at most 0x40000000 (from -32768) and never negative, so 80195748 is unreachable and the claim's justification holds. NEW1 fed h(a1+0x18)=0x8000, 0xFFFF and 0xFFF0 (negative lh values). It compared equal and left 80195748 uncovered, consistent with that. Caveats, none of which refute the claim. Coverage of 777/778 is a hand-computed union of two runs, not one run. M2 dropped 33 of 288 seeds because the retail oracle faulted on them. NEW1 requested 400 seeds but ran 300 and NEW2 ran 384, so the harness trimmed the requested seed counts. Neither run printed a skipped line. The run directed at the largest block in the function, the mode-2 block at 801959CC-801960A0, was the claim's M2 run. My NEW2 widened its inputs and also compared equal, with 680/778 coverage.

