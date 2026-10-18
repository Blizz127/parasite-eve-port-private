# room_m0318i func_80194AAC (NON_MATCHING)

Blob offset 0x5AC4, 0x6E4 bytes, VMA 0x80194AAC. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80194AAC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80194AAC, blob offset 0x5AC4, 0x6E4 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, a0 = mode, a1 = effect record (state +8, frame +0xA,
 * strength +0xC), a2 = key pair for mode 0.
 *   mode 0: clear a1 +8/+0xA/+0xC, find the live object in the D_8009D20C
 *           list whose +0xD / +0xC bytes equal a2 words 0 / 4 (func_800CE870
 *           attach, else func_80071A74 on D_8018F1CC), func_800D3F64(0x5F5,
 *           func_800D3FD8()), func_800D3F64(0x5F6, 0x80), then
 *           return func_800CE560(D_800F33E0->+8, 0x14, 0x28, func_801944E8).
 *   mode 1: state 0 (16 frames): frame++, strength = func_80077CF4(frame<<6)/32,
 *           then state 1. State 1 (frame++, 0x20 frames): while frame < 0x19
 *           spawn a +-0x20 burst particle; odd frames a +-0x80 / +-16 spray
 *           particle; even frames a +-0x100 ring particle (state 2); then
 *           state 2. State 2 (frame++): strength = func_80077DC4(
 *           (frame<<10)/24)/32, returns 1 from frame 0x18 on.
 *   mode 2: load the D_800BCFA4 rotation/translation into the GTE, set the
 *           D_800F3368 draw block, and when strength != 0 draw func_800D004C
 *           (0x190 ring, sp+0x30 / sp+0x38 colour by D_800E27EC & 1) and
 *           func_800D0728 (0x1F4 / 0x2BC ring, strength / 2); then
 *           D_800F3372 = 0, D_800F3374 = 4. Returns 0.
 * Unknown modes / states return 0. The two colour seeds (lwl/lwr of
 * D_8018F200 / D_8018F204) live at sp+0x30 / sp+0x38 and are passed by
 * address to the ring draws.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80194AAC --check-gte --uncovered --seeds 110
    e21_leaf: ok func_80194AAC__c4be4f68_port.c 440 seeds, retail coverage 441/441 words
    e21_leaf: uncovered none

## Uncovered words

None. I intersected the uncovered lists printed by the four runs and the intersection is empty, so every one of the 441 words is executed by at least one passing run. Per run, the uncovered words are other modes' code.

