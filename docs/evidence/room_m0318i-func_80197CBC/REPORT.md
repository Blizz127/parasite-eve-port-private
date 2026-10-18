# room_m0318i func_80197CBC (NON_MATCHING)

Blob offset 0x8CD4, 0x5AC bytes, VMA 0x80197CBC. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80197CBC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80197CBC, blob offset 0x8CD4, 0x5AC bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, a0 = mode, a1 = effect record (frame counter at +0xA).
 *   mode 0: clear a1 +8/+0xA/+0xC/+0xE, func_800D3F64(0x5EB,
 *           func_800D3FD8()), func_800D3F64(0x5EC, 0x80), then
 *           return func_800CE560(D_800F33E0->+8, 0x14, 0x28, func_80197618).
 *   mode 1: a1 +0xA++, func_800CE870(D_800F32D0->+8, 0, a1); position copy
 *           sp+0x30 = a1 +0/+2/+4; angle = (frame << 10) / 141:
 *           a1 +0xC = cos/32, a1 +0xE = sin; while frame < 0x86 spawn up to
 *           three particles in the D_800F33E0->+8 pool (every frame a
 *           +-0x20 burst; odd frames a +-0x100 / +-16 spray, state 0; every
 *           fourth frame a +-0x100 ring, state 2); D_8019993C.. = a1 +0..+4.
 *           Returns 1 from frame 0x8D on.
 *   mode 2: load the D_800BCFA4 rotation/translation into the GTE; while
 *           frame < 0x8E draw func_800D004C (0x3E8 ring, +0xC / 0.75 on odd
 *           D_800E27EC) and func_800D0728 (0x578 / 0x708 ring) around the
 *           position; then D_800F3372 = 0, D_800F3374 = 4. Returns 0.
 * a0 outside 0..2 returns 0. The buf seed (lwl/lwr of D_8018F200) is passed
 * by address to the ring draws.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80197CBC --check-gte --uncovered --seeds 1000
    e21_leaf: ok func_80197CBC__539e1884_port.c 824 seeds, retail coverage 363/363 words
    e21_leaf: uncovered none

## Uncovered words

none

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0318i func_80197CBC --nonmatching 8CD4:5AC --check-gte --uncovered --seeds 1000 --inputs "a0=0,1,2,3,-1;h(a1+0xA)=0,1,2,3,0x84,0x85,0x8B,0x8C,0x8D,0x8E;w(0x800E27EC)=0,1"`
  - e21_leaf: ok func_80197CBC__539e1884_port.c 824 seeds, retail coverage 363/363 words | e21_leaf: uncovered none (claim reproduced exactly)
- `python3 tools/analysis/e21_leaf.py room_m0318i func_80197CBC --nonmatching 8CD4:5AC --check-gte --uncovered --seeds 400 --inputs "a0=1,2;h(a1+0xA)=0xFFFF,0xFFFE,0x8000,0x7FFF,0x85,0x86,0x8C,0x8D,0x8E,0x8F,5,6,7,8,11,12;h(a1+0xC)=0,1,0xFFFF,0x8000,3;w(0x800E27EC)=0,1,2,3,-1"`
  - e21_leaf: ok func_80197CBC__539e1884_port.c 268 seeds, retail coverage 335/363 words | uncovered 80197D0C..80197D84 (mode-0 arm, excluded by my a0=1,2 list)
- `python3 tools/analysis/e21_leaf.py room_m0318i func_80197CBC --nonmatching 8CD4:5AC --check-gte --uncovered --seeds 600 --inputs "a0=1,2;h(a1+0xA)=0x28,0x29,0x2A,0x2B,0x7C,0x7D,0x7E,0x7F,0x84,0x8A;h(a1+0xE)=0,1,0xFFFF,0x8000,0x7FFF;w(0x800E27EC)=0,1;h(a1+0xC)=0xFFFD,7"`
  - e21_leaf: ok func_80197CBC__539e1884_port.c 363 seeds, retail coverage 333/363 words | uncovered same mode-0 range plus 801980E8, 801980EC (frame>=0x8E mode-2 tail, my max frame was 0x8A)

No MISMATCH, notest or skipped in any run. The claimed run reproduced exactly with 363/363 words covered and no uncovered words. My two extra runs targeted negative and large frame counters, odd and negative values of D_800E27EC, signed and negative +0xC/+0xE (c*3/4 rounding), the particle-spawn frame boundaries and the odd and every-fourth-frame branches. Both printed ok. The uncovered words in them are the mode-0 arm and the frame>=0x8E mode-2 tail. I left those out of my input lists, and the claim's own full run covers them, so they are reachable and covered. I did not separately check the justification text for the uncovered words, since the claim lists none.

