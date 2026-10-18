# room_m0428i func_801928CC (NON_MATCHING)

Blob offset 0x38E4, 0x2C0 bytes, VMA 0x801928CC. The span is `asm` in `configs/USA/overlays/room_m0428i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0428i/func_801928CC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0428i — func_801928CC, blob offset 0x38E4, 0x2C0 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Particle callback. cmd 1: integrates the position shorts (+4 += +6, +8 += +0xC,
 * +0xA += +0xE), grows the velocities (+0xC += 0x30, +0xE += 0x20), and while the
 * +0x12 flag is set tests the distance (func_8005186C) from the *D_8009D254
 * actor (+0x1FC/+0x204) to the point record *a1 against (short)+8 / 4; inside it
 * sets bit 0x4000 in the actor flags and bit 31 of **(D_800F32D0+8), clears the
 * flag. Then +0x10 -= 10 and, below 10, the D_801941C8 counter is decremented
 * and 1 is returned (callback done). cmd 2: builds a yaw matrix from sin/cos of
 * (short)+4 (func_80077DC4/CF4, func_80071A44 memset, func_80078CC4 scale by the
 * {x,z,x} vector from +8/+0xA), places it at *a1 and draws through
 * func_800C6EF8/6FA0/71E4/6F4C on D_801941C4 with size min((short)+0x10, 0x80).
 * Retail reads sp+0x4C after func_80071A44 (memset of sp+0x40 .. 0x10 bytes).
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0428i func_801928CC --check-gte --uncovered --seeds 96
    e21_leaf: ok func_801928CC__ae5f844a_port.c 62 seeds, retail coverage 176/176 words
    e21_leaf: uncovered none

## Uncovered words

none

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0428i func_801928CC --nonmatching 38E4:2C0 --check-gte --uncovered --seeds 96 --inputs "a0=1,2; h(a1+0x10)=0x80,0x100,0x12,0x8; h(a1+0x12)=0,1,3"`
  - e21_leaf: ok func_801928CC__ae5f844a_port.c 62 seeds, retail coverage 176/176 words
- `python3 tools/analysis/e21_leaf.py room_m0428i func_801928CC --nonmatching 38E4:2C0 --check-gte --uncovered --seeds 300 --inputs "a0=1,2; h(a1+0x10)=0x80,0x100,0x12,0x8; h(a1+0x12)=0,1,3" (started, not finished)`
  - e21_leaf: uncovered none
- `NEW1: ... --seeds 400 --inputs "a0=1; h(a1+0x12)=0,1,0xFFFF; h(a1+8)=0x8000,0xFFF0,0xFFFD,0xFFFC,0,3,4,0x7FF0,0x7FFF; h(a1+0xC)=0,0xFFFF,5,0x30; h(a1+0x10)=0,0xA,0x14,0x15,0x8000,0xFFFF,0x12" (started, not finished)`
  - NEW1, NEW2 and the 300-seed claimed run did not print a result line in my session. The machine was heavily loaded (load average about 12) and the runs were still going when I stopped. No MISMATCH, notest or skipped line appeared.

The only completed run is the claimed 96-seed spec. It matched, with 176/176 retail words covered and no uncovered words. My two new edge-value runs (negative and boundary values for the h(a1+8)>>2 compare, +0x10 and the 0x80 clamp, odd mode values) did not finish, so they are unverified. The seed counts I used (400 and 300) are also below the product of the value lists (756 and 960), so those specs would only have sampled the combinations. This verdict therefore rests on that one run plus a static read of the C against the asm. The static read found no divergence. (1) Mode 1 matches the asm: halfword adds, +0x30 and +0x20, `lh` on +0x12, `lh` on +8 with the +3 correction for negatives before `>>2`, signed `slt`, the |= 0x4000 at +0x4C, the bit-31 OR through D_800F32D0+8, `lhu`-10 stored back, a signed `<10` test, the D_801941C8 decrement, and return 1 or 0. (2) Mode 2 matches the asm: the sin/cos shorts at sp+0x10 to +0x20 with the negated sin at +0x1C and 0x1000 at +0x18, a 16-byte memset, `lh` loads of +8/+0xA/+8, sw[3] read as zero from the memset, the matrix translation words at 0x24/0x28/0x2C from *a1, `lh` against 0x81 for the clamp to 0x80, and the same call order. (3) Any mode other than 1 or 2 returns 0. No coverage gap was found, since the asm contains no reachable word outside the two modes.

