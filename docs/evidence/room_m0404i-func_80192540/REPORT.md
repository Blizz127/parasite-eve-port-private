# room_m0404i func_80192540 (NON_MATCHING)

Blob offset 0x3558, 0xE28 bytes, VMA 0x80192540. The span is `asm` in `configs/USA/overlays/room_m0404i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0404i/func_80192540.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0404i — func_80192540, blob offset 0x3558, 0xE28 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback a0 = mode (1 step, 2 draw), a1 = object, state at
 * +0x10 (0..4), frame at +0x12, position x/y/z at +0/+2/+4, velocity at
 * +8/+0xA/+0xC.
 * mode 1 (jump table 0x8018F1DC):
 *   0: ballistic fall with +-8 jitter on x/z and y += 0x40 until the floor
 *      D_800942EC - 0x10, then state 1 (velocity doubled, vy = 0); every odd
 *      frame spawns a state-2 spark from func_800CE610(D_800F33E0[2]);
 *      func_800C6B90(a1, 0x6E) near the floor gates the 0x3D000000 tag
 *      rewrite of the root word; returns 1 when func_8001CAB0 reports clear.
 *   1: same with +-16 jitter and a frame limit of 0x20.
 *   2: 127/128 decay, gravity -1 with a floor bounce; at frame 0x10 -> state 4.
 *   3: 511/512 decay, gravity -3 with a floor bounce; ends at frame 0xC.
 *   4: every 16th odd frame spawns a state-3 spark; ends at frame 0x2E.
 * mode 2: SetRot/SetTrans from D_800BCFA4[0], then draw by state:
 *   0/1: sprite sized sin(D_800E27EC << 9) / 4 + 0x1000, then (1 in 4) the
 *        func_800D004C burst; 2: curve colour D_80193EFC at f * 16 / 12; 3: size
 *        sin(f << 6) * 4 / 9 + 0x1000; 4: size from sin(f * 2048 / 46) with
 *        a second func_800CEE20 and a func_800D004C burst.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0404i func_80192540 --check-gte --uncovered --seeds 160
    e21_leaf: ok func_80192540__ec5e3e36_port.c 1271 seeds, retail coverage 906/906 words
    e21_leaf: uncovered none

## Uncovered words

none in the union. Words covered only by dedicated rows: 801925D8/801925DC (default arm for a0 not 1 or 2, row x with a0=0,3) and 80192E80 (negative fix-up of func_80077CF4(..)/4 in mode 2 states 0/1, row x with r(0x80077CF4)=-0x1235 and a0=2, h10=0/1). Mode 1 default states and mode 2 default states (state >4 / 0xFFFF) are covered by rows x and m2b.

## Independent verification: confirmed

- `Re-ran all 8 claimed rows (s0,s1,s2,s4,m2a,m2b,x,r) via the work-dir run.sh using the saved *.spec files: python3 tools/analysis/e21_leaf.py room_m0404i func_80192540 --nonmatching 3558:E28 --check-gte --uncovered --seeds N --inputs "<spec>" (seeds 128,128,96,128,160,120,64,160); then python3 un.py v_*.log (intersection of uncovered sets)`
  - s0: e21_leaf: ok func_80192540__ec5e3e36_port.c 128 seeds, retail coverage 212/906 words
- `NEW n1 (mode 1, states 0-4, frame in 0x7FFF/0xFFFF/0x1F/0x1D, y in 0x7FFF/0x8000/0xFFF0/0x100, floor D_800942EC in 0x7FFF/0x8000/0, vx 0x8000/0x7FFF, vy 0x8000/5, 8001CAB0 0/1) --seeds 480`
  - s1: e21_leaf: ok ... 128 seeds, retail coverage 187/906 words
- `NEW n2 (mode 2, states 0-4, frame 0x7FFF/0x8000/0xFFFF/1/0xB/0x2D, D_800F336A 0x8000/0x7FFF/1/-1, D_800F3368 0xFFFF/0x8000/1, D_800F336C 0/4/5/0x20, D_800F3428 0/1, D_800E27EC 0xFFFFFFFF/0x80000000/0x1FF/1, func_80077CF4 result -1/-9/0x7FFF/-0x8000, rand 0/3) --seeds 600`
  - s2: e21_leaf: ok ... 96 seeds, retail coverage 163/906 words
- `NEW n3 (mode 1 states 0/1, frames 0-3, extreme x/z/vx/vz/y wraparound, rand results 0/0xF/0x1F/0x35/0xFFFFFFFF, CE610 0/ptr) --seeds 400`
  - s4: e21_leaf: ok ... 128 seeds, retail coverage 177/906 words

No MISMATCH, notest, or skipped in any of the 11 runs, and no uncovered word that the claim wrongly calls unreachable. Findings:
- Signedness of the compares matches the C. The asm uses lh then slti for the frame limits at 0x80192A68 (0x20), 0x80192B20 (0x10), 0x80192BFC (0xC) and 0x80192DD0 (0x2E). The C uses SH(a1,0x12). The jump-table bound is sltiu 5 at 0x801925E8 and 0x80192E5C. My n1 frame values 0x7FFF and 0xFFFF exercise these edges.
- The 0x801925D8/DC arm (a0 not 1 or 2) is reached by row x with a0=0,3, as the claim says.
- Caveats: the 8 rows are only exhaustive where seeds >= the product of the lists. s1, s4, m2a and x use fewer seeds than their list products, so they sample. The union coverage figure still reproduces.
- Row x reports 60 of 64 seeds surviving. The 4 skips are oracle-side, as the claim says, not mismatches.
- I did not change the C file.

