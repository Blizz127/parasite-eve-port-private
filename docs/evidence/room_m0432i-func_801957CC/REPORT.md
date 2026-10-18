# room_m0432i func_801957CC (NON_MATCHING)

Blob offset 0x67E4, 0x374 bytes, VMA 0x801957CC. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_801957CC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_801957CC, blob offset 0x67E4, 0x374 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback (a0 = command 1/2, a1 = effect record), a sibling of
 * room_m0429i func_801982A4. Command 1 steps the record (+8/+0xA velocity
 * and the +0x12 frame counter): state 0 runs 12 frames (+8, +0xC), state 1
 * runs 24 frames (+0x1D, +0xB). Command 2 state 0 draws via func_800D0E88
 * with size sin(f*1024/12)/8; state 1 places an 8-byte vector copied from
 * D_8018F1F4 (z = f << 5) at an orbit point and draws it through func_800CEE20.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot macro.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_801957CC --check-gte --uncovered --seeds 128
    e21_leaf: ok func_801957CC__61b5d809_port.c 124 seeds, retail coverage 221/221 words
    e21_leaf: uncovered none

## Uncovered words

None in the final run. The first run left 8019582C and 80195830 uncovered. They are the `j .L80195B1C` default arm for a0 not equal to 1 or 2, and the 128-seed run covers them with a0=0,3.

## Independent verification: confirmed

- `e21_leaf.py room_m0432i func_801957CC --nonmatching 67E4:374 --check-gte --uncovered --seeds 64 --inputs "a0=1,2;h(a1+0x10)=0,1,2;h(a1+0x12)=0,5,0x1e,0x1f,-3,0x7ff0"`
  - e21_leaf: ok func_801957CC__61b5d809_port.c 62 seeds, retail coverage 219/221 words | e21_leaf: uncovered 8019582C 80195830
- `same, --seeds 128 --inputs "a0=0,1,2,3;h(a1+0x10)=0,1,2;h(a1+0x12)=0,5,0x1e,0x1f,-3,0x7ff0,0x20,0xb,0x17"`
  - e21_leaf: ok func_801957CC__61b5d809_port.c 124 seeds, retail coverage 221/221 words | e21_leaf: uncovered none
- `NEW A: --seeds 200 --inputs "a0=1,2;h(a1+0x10)=0,1,-1,3;h(a1+0x12)=0,1,0xb,0xc,0x17,0x18,0x19,-1,-12,-13,-24,-25,0x7fff,-0x8000"`
  - A: e21_leaf: ok ... 196 seeds, retail coverage 219/221 words | uncovered 8019582C 80195830 (a0 only 1,2, so the default arm is expected to be uncovered)
- `NEW B: --seeds 200 --inputs "a0=2;h(a1+0x10)=0,1;h(a1+0x12)=1,2,3,7,0x2f,0x1ff,0x1fff,0x3fff,-1,-2,-5,-7,-0x1fff,-0x4000,-0x8000;h(a1+6)=0,1,-1,0x7fff,-0x8000,0x1000,-0x1000,0xfff,-0xfff"`
  - B: e21_leaf: ok ... 165 seeds, retail coverage 184/221 words | uncovered 8019582C..80195850 and 80195854..801958B4 (the whole command-1 arm, because a0=2 only) plus 801958D0/801958D4 (state-other under command 2, because h(a1+0x10) is 0,1 only)

["No MISMATCH, notest or skipped output in any run. The two claimed runs reproduce exactly (219/221 and 221/221).", "The uncovered words in the first claimed run, 8019582C and 80195830, are the `j .L80195B1C` default arm for a0 not 1 or 2. That arm is reachable and is covered by the second run (a0=0,3). Retail returns 0 there, and the C returns 0 as well.", "Run A added negative, boundary and overflow frame counters (0xB/0xC, 0x17/0x18, -1, 0x7fff, -0x8000) and state -1 and 3 for the commands 1 and 2 state dispatch. The signed `slti` and the `lh` compares match. It also exercised the unsigned halfword increments of +8/+0xC/+0x1D/+0xB.", "Run B stressed the (f<<10)/12 and /24 signed division, the t/8 and t*a6/4096 rounding of negative values, and the a1+6 multiplier extremes. Every seed matched. Its uncovered words are only the command-1 arm and the state-3 arm of command 2, which that spec excluded on purpose, and the other runs cover them.", "Code review found nothing that differs. The call argument order of func_800D0E88 and func_800CEE20 matches the retail stack slots. D_800F3368 +2 is stored as 2, so the +0x64 read-back gives 0x66, and the C does the same. The 8-byte struct copy of D_8018F1F4 and the low-16 mask on the func_80077AA4 result are both reproduced. No concrete divergence was found."]

