# room_m0318i func_80193638 (NON_MATCHING)

Blob offset 0x4650, 0x374 bytes, VMA 0x80193638. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80193638.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80193638, blob offset 0x4650, 0x374 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Handler dispatched on mode a0 with a state block a1:
 *   0: a1[8] = func_80071A54(); func_800CE870(D_8009D254, 1, a1);
 *      a1 halfword +2 = D_800942EC; while D_800B0E64 is set,
 *      func_8006DF50(D_800B0E64, 0x5EF, func_800D3FD8(), 0x80, 0x7F) then
 *      func_8006DF50(D_800B0E64, 0x5F0, 0x80, 0x80, 0x7F); if
 *      D_800E2368[0xD] is set, bump the byte at (**(D_800F32D0 +8))[0x18]
 *      from 1 to 2; func_800C6D5C(D_800F32D8, 0, 0); returns
 *      func_800CE560(D_800F33E0->+8, 0x10, 0x18, func_8019326C).
 *   1: frame counter D_800E27EC: func_80020D50() below 0x2B; every sixth
 *      frame below 0x28 take a node from func_800CE610(D_800F33E0->+8),
 *      clear its halfwords +6/+0xC, set +0xA = a1[8], and lower a1[8] by
 *      0x555 + (func_80071A54() & 0xFF); func_80020DD0() at 0x2B; at 0x2C
 *      with D_800E2368[0xD] set and the node word tagged 0x01000000, set
 *      bit 0x4000 in (*D_8009D254)[0x4C] and rewrite the node word to
 *      0x19000000 tag then bit 31; returns 1 once the counter reaches 0x40.
 *   2: D_80199904/06/08 = the first three halfwords of a1; returns 0.
 *   other modes return 0.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80193638 --check-gte --uncovered --seeds 200
    e21_leaf: ok func_80193638__68b58263_port.c 710 seeds, retail coverage 221/221 words
    e21_leaf: uncovered none

## Uncovered words

None in the union. Each single run lists the arms that run does not drive:
- mode 0: the a0==1 arm (801937B4..80193968, the per-frame body) and the a0==2 arm.
- mode 1: the a0==0 init arm and the a0==2 arm.
- default run (a0=-1,3): 80193668/8019366C and 8019367C/80193680, the a0<2 and a0>2 return-0 arms.
- mode 2: only the a0==2 arm.

