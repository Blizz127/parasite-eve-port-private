# room_m0429i func_80198914 (NON_MATCHING)

Blob offset 0x992C, 0x8E4 bytes, VMA 0x80198914. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_80198914.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_80198914, blob offset 0x992C, 0x8E4 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, modes 0/1/2 on a record a1, read from the retail
 * disassembly. Mode 0 clears a1+8/+0xA, plays sound 0x5DF/0x5E0 and arms
 * func_801982A4 (0x14, 0x3C); returns func_800CE560's result. Mode 1 (once,
 * while the global frame counter D_800E27EC < 4) spawns 4 + 7 + 5 puffs through
 * func_800CE610 (copy a1+0..4; kind 0: random +-16 velocity, kind 1: random
 * velocity and size (r & 0x3FF) + 0x5DC, kind 2: size (r & 0xFF) + 0x100) and
 * returns 1 from frame 0x1C on. Mode 2 (frame < 0x15 only) draws one
 * func_800CEE20 glow, a func_800D004C ring, three func_800D0728 rings around the
 * func_800CE9D4 world vector and three func_800D3BC8 sprites, each with the
 * D_800F3368 draw block re-seeded; then (every frame) stores the centre in
 * D_8019B678 and the 0x20 draw block.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_80198914 --check-gte --uncovered --seeds 120
    e21_leaf: ok func_80198914__408cc114_port.c 190 seeds, retail coverage 545/569 words
    e21_leaf: uncovered 80198D64 80198D68 80198D6C 80198D70 80198D74 80198D78 80198FC4 80198FC8 80198FCC 80198FD0 80198FD4 80198FD8 80199064 80199068 8019906C 80199070 80199074 80199078 801990E0 801990E4 801990E8 801990EC 801990F0 801990F4

## Uncovered words

24 words, in four 6-word groups, all the same construct: the `if (D_800F336C == 4 && D_800F3428 != 0) a1 += 4` arm (lui/lw D_800F3428, beqz, addiu a1,a1,4) after each of the four `D_800E1204[D_800F336C]` lookups. 80198D64-80198D78 (before func_80077AA4(0x90)), 80198FC4-80198FD8 (0xA0), 80199064-80199078 (0xB0), 801990E0-801990F4 (0xC0). Reason: in mode 2 the function stores D_800F336C = 3 at entry and no real code writes it again before these tests (only mocked callees in between), so `== 4` can never be true; directed h(0x800F336C)=4 cannot take effect because the entry store overwrites it. The C keeps the test faithfully. The earlier negative-mode / mode>2 default words (80198994-80198998, 801989A8-801989AC) are covered by a0=-1,3.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0429i func_80198914 --nonmatching 992C:8E4 --check-gte --uncovered --seeds 120 --inputs "a0=0,1,2,-1,3; w(0x800E27EC)=0,3,4,0x14,0x15,0x1C,-1; out=800CE9D4:2"`
  - e21_leaf: ok func_80198914__408cc114_port.c 89 seeds, retail coverage 545/569 words | e21_leaf: uncovered 80198D64..80198D78 80198FC4..80198FD8 80199064..80199078 801990E0..801990F4 (identical to the claim)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80198914 --nonmatching 992C:8E4 --check-gte --uncovered --seeds 8 --inputs "a0=1; w(0x800E27EC)=0,5; r(0x800CE610)=0,0x80140000"`
  - e21_leaf: ok func_80198914__408cc114_port.c 6 seeds, retail coverage 175/569 words (no mismatch; uncovered list is just the other modes)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80198914 --nonmatching 992C:8E4 --check-gte --uncovered --seeds 40 --inputs "a0=2; w(0x800F3428)=0,1,-1,0x10000; w(0x800E11EA)=0,1,5,-1,0x7fff; w(0x800E27EC)=0,4"`
  - NEW run, mode 2 with D_800F3428 values 0/1/-1/0x10000 and D_800E11EA edge values: e21_leaf: ok func_80198914__408cc114_port.c 28 seeds, retail coverage 376/569 words; uncovered is only the mode 0/1 code plus the same 24 words 80198D64-78, 80198FC4-D8, 80199064-78, 801990E0-F4. No MISMATCH, notest or skipped.

No refutation. Only the claimed 24-word uncovered set remains, and it is unreachable for the stated reason. At 80198CD8 mode 2 does `sh $v0, D_800F336C` (the C has a6C = 3), so the 'D_800F336C == 4' test at 80198D64, 80198FC4, 80199064 and 801990E0 cannot be true unless a real callee writes it in between. The callees in between are func_80077DC4, func_80077AA4, func_800CEE20, func_800D004C, func_80077CF4, func_800D0728, func_800CE9D4 and func_800D3BC8. They are mocked in the harness, so a retail-side write by one of them is not tested; I did not check them. This is a coverage limit, not a found divergence. The third run is mode 2 only (a0=2). It covers the D_800F3428 nonzero variants, but those words stay uncovered. I did not run my own spec on the other modes. The claimed first run already covers a0=-1,3 and the other modes, and I only reran its result.

