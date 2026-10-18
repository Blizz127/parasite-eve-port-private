# room_m0318i func_8019326C (NON_MATCHING)

Blob offset 0x4284, 0x3CC bytes, VMA 0x8019326C. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_8019326C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_8019326C, blob offset 0x4284, 0x3CC bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, a0 = mode, a1 = effect record (+0/+2/+4 position, +6 phase
 * flag, +8 / +0xA / +0xE velocity-like shorts, +0xC frame counter).
 *   mode 1, +6 == 0: seed +0/+2/+4 from D_80199904/06/08; +0xA += 0x20 +
 *           (f << 2); +8 = sin((f << 11) / 24) / 32; +0xE = (f << 12) / 24
 *           (f = D_800E27EC). On odd f spawn a particle via func_800CE610
 *           (D_800F33E0->+8): +6 = 1, copy +0/+2/+4/+8/+0xE. Returns 1 once
 *           f >= 0x18, else bumps +0xC and returns 0.
 *   mode 1, +6 != 0: +0xA += 3, +8 = +8 * 3 / 4; returns 1 once f >= 4, else
 *           bumps +0xC and returns 0.
 *   mode 2: RotMatrix {0, +0xA, 0} -> stack matrix at sp+0x18, ScaleMatrix by
 *           {0x400, (+0xE) / 4, 0x400}, translation = a1 +0/+2/+4 (ints), set
 *           the D_800F336C draw block (3, 1, D_800E2850[D_800E11FA]), load the
 *           D_800BCFA4[0] matrix into the GTE, then draw with the colour
 *           func_80077A64 | table word, CLUT func_80077AA4 and a size pair
 *           derived from +8 (/3 and /2). Returns 0.
 *   other : returns 0.
 * The stack vector at sp+0x38 is {0x400, +0xE / 4, 0x400}; word 0x44 is never
 * written (matches retail).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_8019326C --check-gte --uncovered --seeds 60
    e21_leaf: ok func_8019326C__bebaf1a5_port.c 55 seeds, retail coverage 236/243 words
    e21_leaf: uncovered 80193568 8019356C 80193570 80193574 80193578 8019357C 80193580

## Uncovered words

80193568..80193580 (7 words: bne a0!=4 fall-through, D_800F3428 load, beqz, j, addiu a1,8): the `D_800F336C == 4 && D_800F3428` arm (a1 += 8). Retail writes D_800F336C = 3 a few instructions earlier and the only intervening call, func_80077A64, is mocked and writes no RAM, so in the differential the compare is always 3 != 4 and the arm cannot be reached. The C reproduces the branch exactly, and the reads of D_800F336C and D_800F3428 are in place. The default mode arm (80193298/8019329C, mode not 1 or 2, returns 0) is covered by a0=0,3.

## Independent verification: confirmed

- `e21_leaf.py room_m0318i func_8019326C --nonmatching 4284:3CC --check-gte --uncovered --seeds 60 --inputs "a0=1,2,0,3;h(a1+6)=0,1;h(a1+8)=0xFFF1,0x1234,7;w(0x800E27EC)=0,1,3,4,5,0x17,0x18,25,0xFFFFFFF0;w(0x800F3428)=1,0;h(0x800F336C)=4,0"`
  - e21_leaf: ok func_8019326C__bebaf1a5_port.c 55 seeds, retail coverage 236/243 words | e21_leaf: uncovered 80193568 8019356C 80193570 80193574 80193578 8019357C 80193580
- `e21_leaf.py ... --seeds 40 --inputs "a0=1,2;h(a1+6)=0,1;w(0x800E27EC)=0,1,3,4,5,0x17,0x18,25"`
  - e21_leaf: ok ... 30 seeds, retail coverage 234/243 words | uncovered 80193298 8019329C 80193568..80193580 (expected: no a0=0,3 in this spec)
- `e21_leaf.py ... --seeds 150 --inputs "a0=1,2,0,5,-1;h(a1+6)=0,1,0x7FFF,0x8000;h(a1+4)=0,0x7FFF,0x8000;h(a1+10)=0,0xFFFF;w(0x800E27EC)=0x80000000,0x7FFFFFFF,2,6,12,0xFFFFFFFF;h(0x800F336C)=3"`
  - e21_leaf: ok ... 136 seeds, retail coverage 236/243 words | uncovered 80193568 8019356C 80193570 80193574 80193578 8019357C 80193580
- `e21_leaf.py ... --seeds 120 --inputs "a0=1,2,0,5;h(a1+6)=0,1,0x8000;h(a1+4)=0,0x8000;h(a1+10)=0,0xFFFF;w(0x800E27EC)=0x80000000,0x7FFFFFFF,2,6;h(0x800F336C)=3"`
  - e21_leaf: ok ... 111 seeds, retail coverage 236/243 words | uncovered 80193568..80193580 only
- `e21_leaf.py ... --seeds 64 --inputs "a0=2,1;h(a1+6)=0,1,2,0xFFFF;h(a1+8)=0,0x8000,0x7FFF,0xFFFF;h(a1+10)=0,0x8000;w(0x800E27EC)=0,0xFFFFFFFF"`
  - e21_leaf: ok ... 54 seeds, retail coverage 208/243 words | uncovered extra 80193354..801933B8 (my narrow spec skipped the a0=1 path) plus 80193568..80193580; no MISMATCH, no notest, no skipped

No MISMATCH, notest or skipped in any run. The only uncovered words in the full runs are 80193568..80193580. Retail asm shows D_800F336C is stored as 3 at 801934A8/801934AC (the only path to 80193560 passes through it, other store at 80193220 is on a different path), then straight-line code calls func_80077A64, then reloads D_800F336C at 8019353C and compares to 4 at 80193560. Unless func_80077A64 (GPU load routine) writes D_800F336C, the compare is always 3 != 4, so the 7-word arm is unreachable by constructible input; the claim's justification holds. The 80193298/8019329C default arm is covered with a0=0 or 5. The claim's second quoted command piped into itself is odd, but the result is the same when run directly. The residual risk is only that the mocked func_80077A64 hides a real-hardware write to D_800F336C, which I consider implausible. The C source was not read side by side in full detail, but the directed and edge-value runs (signed 0x8000/0x7FFF halfwords, 0x80000000/0x7FFFFFFF/0xFFFFFFFF words) all matched retail."]

