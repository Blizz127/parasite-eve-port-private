# room_m0318i func_80195904 (NON_MATCHING)

Blob offset 0x691C, 0x41C bytes, VMA 0x80195904. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80195904.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80195904, blob offset 0x691C, 0x41C bytes.
 * NON_MATCHING behaviour C written from the retail asm (2026-10-18).
 * Particle callback, same shape as room_m0429i func_80199C28 (halfword
 * layout: x y z spin at +0..+6, velocity at +8/+0xA/+0xC, kind at +0x10,
 * frame at +0x12). Mode 1 steps a particle: drift, damping (127/128 for
 * kind 0, 31/32 and a floor bounce for kind 1); done after 24 frames.
 * Mode 2 draws it through func_800CEE20 as a sprite sized by sin/cos of
 * frame*1024/24 (kind 0: 32x32 sprite, kind 1: 64x64 sprite).
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80195904 --check-gte --uncovered --seeds 216
    e21_leaf: ok func_80195904__73ca5372_port.c 216 seeds, retail coverage 263/263 words
    e21_leaf: uncovered none

## Uncovered words

none

## Independent verification: confirmed

- `Claimed cmd 1: e21_leaf.py room_m0318i func_80195904 --nonmatching 691C:41C --check-gte --uncovered --seeds 216 --inputs "a1=0x80110000; a0=1,2,0; h(a1+0x10)=0,1,2; h(a1+0x12)=0,10,22,23,24,30; h(0x800F336C)=4,0; w(0x800F3428)=1,0"`
  - e21_leaf: ok func_80195904__73ca5372_port.c 216 seeds, retail coverage 263/263 words | e21_leaf: uncovered none
- `Claimed cmd 2: same with --seeds 192 and the mode-1 negative/floor-bounce spec`
  - e21_leaf: ok ... 192 seeds, retail coverage 113/263 words (uncovered = mode-2 words plus 80195924..34 default-mode arm, all covered by cmd 1)
- `Claimed cmd 3: same with --seeds 128 and the default-arm/kind 7/frame 0xFFF0 spec`
  - e21_leaf: ok ... 128 seeds, retail coverage 165/263 words (uncovered = mode-1 words, covered by cmd 1)
- `NEW1: --seeds 288 --inputs "a1=0x80110000; a0=1,2; h(a1+0x10)=0,1,2,3; h(a1+0x12)=21,22,23,24,25; h(a1+6)=0xFFFF,0,1,0x8000; h(0x800F336C)=4,0; w(0x800F3428)=1,0"`
  - e21_leaf: ok ... 288 seeds, retail coverage 261/263 words; uncovered 80195930 80195934 (the default-mode j/nop arm, reachable only with a0 not in {1,2}; covered by cmd 1 a0=0)
- `NEW2: --seeds 256 --inputs "a1=0x80110000; a0=1,0; h(a1+0x10)=0,1,4,5; h(a1+8)=0xFF9B,0xFF9D,0,1; h(a1+0xA)=0xFF9B,0,101,0x8000; h(a1+4)=0x8000,0x7FFF,0xFFFF,1"`
  - e21_leaf: ok ... 256 seeds, retail coverage 116/263 words (uncovered = mode-2 body 80195AB8..80195D00 since a0=1,0 only; covered by cmd 1)

No MISMATCH, notest or skipped in any run. Side-by-side read of asm vs C found no discrepancy: signed lh/sra damping with 127/128 and 31/32 round-toward-zero, floor-bounce compare uses the updated +2 against D_800942EC with signed slt, frame +0x12 is stored with lhu+1 and tested with lh (slti 0x18), division by 24 via 0x2AAAAAAB matches (x<<10)/24, kind1 uses func_80077CF4(t*2)/32 and +0x1000, the 5-argument stack layout (0x10..0x20) and the D_800F3428 gating of clut+8/+6 vs +4/+2 match. Edge values (negative velocities, 0x8000/0xFFFF p[3], frames 21-25, kinds 0-5, a0=0) all passed. The only uncovered words in my targeted runs were arms my inputs deliberately excluded and the main claimed run covers 263/263."]

