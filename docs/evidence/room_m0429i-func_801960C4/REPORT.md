# room_m0429i func_801960C4 (NON_MATCHING)

Blob offset 0x70DC, 0x684 bytes, VMA 0x801960C4. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_801960C4.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_801960C4, blob offset 0x70DC, 0x684 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback (same family as room_m0429i func_801982A4 / room_m0269i
 * func_8019070C).  Reads four seed blocks off the stack frame: b38 from the text
 * bytes at func_8018F210, b40 (a GteShortVector) from D_8018F1CC, b48 from
 * D_8018F21C, b50 from D_8018F220.
 * cmd 0: func_800CE8F0 position into b38, clears a1+8/+0xA, flips the byte at
 *        D_800F32D0->8->0->0x18 from 1 to 2 when D_800E2368->0xD is set, returns
 *        func_800CE560(D_800F33E0->8, 0x20, 0x20, func_8019549C).
 * cmd 1: counter a1+8 ++; on odd ticks below 20 spawns a func_800CE610 puff
 *        tagged with the spawn count a1+0xA; returns 2 once a1+8 >= 8.
 * cmd 2: copies a1+0..4 into a stack vector; ticks <= 0x18 draw func_800D0728
 *        sized func_80077CF4(f*1024/24) with the camera matrix D_800BCFA4[0];
 *        ticks < 0xD flash through func_800D1AE0; ticks <= 0x20 draw two
 *        func_800CEE20 sprites (0x3000 / 0x1800) and a 0x2000 one at half
 *        opacity, then a func_800D0728 ring (0x2BC x 0x3E8); finally the
 *        D_800F3368 draw block is reset (the last 0x20 write set is common).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_801960C4 --check-gte --uncovered --seeds 180
    e21_leaf: ok func_801960C4__eb2df6c9_port.c 410 seeds, retail coverage 405/417 words
    e21_leaf: uncovered 80196524 80196528 8019652C 80196530 80196534 80196538 801965C4 801965C8 801965CC 801965D0 801965D4 801965D8

## Uncovered words

80196524-80196538 (6 words): the `if (D_800F3368.a6C == 4 && D_800F3428) w += 4` arm after the first func_800CEE20 call. Retail stored 3 into D_800F336C (a6C) a few instructions earlier and only mocked calls run in between, so a6C cannot be 4 in the differential. The C re-reads it through guest RAM exactly as retail does. | 801965C4-801965D8 (6 words): the identical `a6C == 4 && D_800F3428` arm before the third func_80077AA4 call, where a6C was again just stored as 3. Same reason. (80196190-801961A8 default arms and 80196228/2C, the *r==1 -> 2 write, are covered by the supplementary runs above; the combined spec with fixed non-null chain covers the default arms.)

