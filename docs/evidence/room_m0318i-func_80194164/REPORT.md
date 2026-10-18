# room_m0318i func_80194164 (NON_MATCHING)

Blob offset 0x517C, 0x384 bytes, VMA 0x80194164. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80194164.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80194164, blob offset 0x517C, 0x384 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback taking (mode, a1 = base position vector).
 * mode 0: init/attach (func_800CE870, arm the +0xD flag object, start the two
 *         D_800B0E64 sounds, register func_80193FC4 via func_800CE560);
 * mode 1: per-frame (randomised position jitter around a1, frame-0x27/0x28
 *         hooks, returns 1 once D_800E27EC >= 0x35);
 * mode 2: set the D_800F3368 draw block from the D_800E2850 table.
 * Any other mode returns 0.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80194164 --check-gte --uncovered --seeds 32
    e21_leaf: ok func_80194164__067e18b6_port.c 79 seeds, retail coverage 225/225 words
    e21_leaf: uncovered none

## Uncovered words

none in the union. Per-run uncovered words are the other modes' arms by design: run A (mode 0) leaves the mode 1, mode 2 and dispatch words uncovered; run B (mode 1) leaves the dispatch/mode 0 words (80194180-801942A8) and the mode 2 block (80194460-801944CC); run C leaves the mode 0/1 bodies. The default/negative-mode arms (80194194/98, 8019419C-801941AC) are covered by run C (a0=-1,3,0x100), and the mode 2 block (80194460-801944CC) by run C (a0=2).

## Independent verification: confirmed

- `e21_leaf.py room_m0318i func_80194164 --nonmatching 517C:384 --check-gte --uncovered --seeds 16 --inputs 'a0=0;CH;w(0x80100200)=0x01000000;w(0x80100300)=1,2,0;w(0x8010040C)=0x100,0;w(0x800B0E64)=0x80100700,0' (claim run A, re-run exactly)`
  - A: e21_leaf: ok func_80194164__067e18b6_port.c 10 seeds, retail coverage 81/225 words
- `same, --seeds 32 --inputs 'a0=1;CH;w(0x800E27EC)=0x10,0x27,0x28,0x35;w(0x80100200)=0x01000000,0x02000000;w(0x8010040C)=0x100,0' (claim run B)`
  - B: e21_leaf: ok func_80194164__067e18b6_port.c 26 seeds, retail coverage 122/225 words
- `same, --seeds 4 --inputs 'a0=-1,3,2,0x100' (claim run C)`
  - C: e21_leaf: ok func_80194164__067e18b6_port.c 4 seeds, retail coverage 53/225 words (covers 80194164-801941AC and 80194460-801944E4, matching the claim)
- `NEW1: --seeds 54 --inputs 'a0=1;a1=0x80100800;CH;w(0x800E27EC)=0x14,0x15,0x26,0x27,0x28,0x34;w(0x80100200)=0x01000000,0x3F000000,0x7F000000,0xFFFFFFFF,0x02000000,0;w(0x8010040C)=0x100,0,0xFF;w(0x800F33E0)=0x80100900'`
  - NEW1: e21_leaf: ok ... 52 seeds, retail coverage 120/225 words
- `NEW2 (mode 0, null chain links): --seeds 48 --inputs 'a0=0;CH minus 0x80100008;w(0x80100008)=0x80100100,0;w(0x80100100)=0x80100200,0;w(0x80100300)=1,2,0xFF;w(0x8010040C)=0,0x7F;w(0x800B0E64)=0x80100700,0'`
  - NEW2: e21_leaf: ok ... 33 seeds, retail coverage 63/225 words
- `NEW3 (mode 1, a1 extreme halfwords, frame boundaries incl. -1/0x36): --seeds 24 --inputs 'a0=1;a1=0x80100800;CH;w(0x80100008)=0x80100100;w(0x80100800)=0xFFFFFFFF,0x00018000,0;w(0x800E27EC)=0x13,0x14,0x29,0x35,0x36,-1;w(0x80100200)=0x01000000;w(0x8010040C)=0x100'`
  - NEW3: e21_leaf: ok ... 14 seeds, retail coverage 70/225 words
- `NEW4 (mode 2, table index edges): --seeds 8 --inputs 'a0=2;w(0x800E11EA)=0,1,0xFFFF,0x8000'`
  - NEW4: e21_leaf: ok ... 8 seeds, retail coverage 47/225 words

No refutation found. I read the retail asm (517C.s) against the C line by line and found no discrepancy. Arguments and order for func_800CE870 (D_8009D254, 0, a1), func_8006DF50 (stack arg 0x7F, 0x5F1/0x5F2) and func_800CE560 (return value passed through) all match. The a0 compare is signed, so negative modes return 0. The 0x27/0x28 reload of D_800E27EC, the 0x3F000000/0x01000000 mask, the 0xC0FFFFFF/0x19000000 and 0x80000000 RMW, the 0x1FF mask with -0x100 and the lhu/sh widths all match. The mode 2 sh stores and the D_800E2850 table index (lhu, sll 1) also match. Coverage of the claim's union checks out against the printed lists: A covers 801941B0-801942A8 and the epilogue, B covers 801942AC-8019445C, C covers 80194164-801941AC and 80194460-801944E4. Minor caveats. My runs covered only the same arms. They did not add a 225/225 run, because no single run exceeds about 183 words. Seed counts were capped by the harness below the product of the value lists (52 vs 54, 33 vs 48, 14 vs 24), so some value combinations were not run. func_80020D50 and func_80020DD0 are called with no arguments in both versions, so any $a0 dependence in those callees depends on the oracle mock. The e21 harness only compares what it models.

