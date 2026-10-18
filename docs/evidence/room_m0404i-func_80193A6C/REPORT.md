# room_m0404i func_80193A6C (NON_MATCHING)

Blob offset 0x4A84, 0x3B8 bytes, VMA 0x80193A6C. The span is `asm` in `configs/USA/overlays/room_m0404i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0404i/func_80193A6C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0404i — func_80193A6C, blob offset 0x4A84, 0x3B8 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Three-command room callback (mode in a0, record pointer in a1):
 *   mode 0: set up the D_800F32D0 / D_800F33E0 handles (func_800CE870,
 *           func_800CE8F0 with the 8-byte stack seed D_8018F21C, then
 *           func_800D3F64 0x5C0/0x5C1, then returns func_800CE560 with
 *           func_801935E0; v0 is left holding that call's result).
 *   mode 1: while D_800E27EC < 5 spawn 6 particles (random x%50-25 /
 *           -(x%39) / x%50-25 velocities, state 1), then 2 more
 *           (x%80-40 / -(x%80) / x%80-40, state 0); returns 2 once
 *           D_800E27EC >= 2, else 0.
 *   mode 2: set the D_800F336C draw block from table D_800E2850[D_800E11EA].
 * The magic-number multiplies (0x51EB851F, 0xD20D20D3, 0x66666667) are signed
 * constant divisions by 50, 39 and 80.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0404i func_80193A6C --check-gte --uncovered --seeds 120
    e21_leaf: ok func_80193A6C__41f937c2_port.c 99 seeds, retail coverage 238/238 words
    e21_leaf: uncovered none

## Uncovered words

none

## Independent verification: confirmed

- `e21_leaf.py room_m0404i func_80193A6C --nonmatching 4A84:3B8 --check-gte --uncovered --seeds 24 --inputs "a0=0,1,2,3,-1; w(0x800E27EC)=0,1,3,7"`
  - e21_leaf: ok func_80193A6C__41f937c2_port.c 21 seeds, retail coverage 238/238 words | e21_leaf: uncovered none
- `e21_leaf.py ... --seeds 120 --inputs "a0=0,1,2,3,-1; w(0x800E27EC)=0,1,3,4,5,7; h(0x800E11EA)=0,1,2"`
  - e21_leaf: ok func_80193A6C__41f937c2_port.c 99 seeds, retail coverage 238/238 words | e21_leaf: uncovered none
- `NEW: ... --seeds 16 --inputs "a0=-2147483648,2147483647,4,0x10000,-3,2; w(0x800E27EC)=-1,2,4,0x7FFFFFFF,0x80000000; h(0x800E11EA)=0xFFFF,0x8000"`
  - e21_leaf: ok ... 16 seeds, retail coverage 61/238 words (spec omitted a0=0/1, so only the default arm ran; uncovered words 80193AF0..80193DB0 are the mode 0/1 arms, which my spec did not drive; no MISMATCH)
- `NEW: ... --seeds 40 --inputs "a0=0,1,2,4,-2147483648,0x10000; w(0x800E27EC)=-1,1,2,4,5,0x7FFFFFFF; h(0x800E11EA)=0,0xFFFF"`
  - e21_leaf: ok ... 35 seeds, retail coverage 238/238 words | e21_leaf: uncovered none
- `NEW: ... --seeds 60 --inputs "a0=1,2; w(0x800E27EC)=0x80000000,0xFFFFFFFE,2,3,4,5,6,0x7FFFFFFF; h(0x800E11EA)=0x7FFF,0x8000,3,100"`
  - e21_leaf: ok ... 51 seeds, retail coverage 200/238 words (spec omitted a0=0 and the default arm, so those words are uncovered by my own choice; no MISMATCH)

No MISMATCH, notest or skipped output in any run, and the claim's two commands reproduce exactly with 238/238 words covered and no uncovered words. The two partial-coverage results come from my own input specs leaving out a0=0 and the default arm. The claim's runs cover every arm.

I read the C beside the asm. The 50/39/80 modulo handling and the mode 2 table lookup are covered by the signed-edge runs (a0 = INT_MIN, 0x10000, -2; D_800E27EC at -1, 0x7FFFFFFF and 0x80000000; D_800E11EA at 0xFFFF and 0x8000), and none of them mismatched. I found no concrete divergence. The one residual risk is that the e21 harness's pseudo-random func_80071A54 results rarely produce negative remainders. The harness covers that only partly, so negative-value rounding is not exhaustively proven.

