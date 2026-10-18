# room_m0429i func_801972EC (NON_MATCHING)

Blob offset 0x8304, 0x600 bytes, VMA 0x801972EC. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_801972EC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_801972EC, blob offset 0x8304, 0x600 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, modes 1/2 on a record a1, read from the retail disassembly.
 * Mode 1 steps the record by state (+0x10): 0 adds +8/+0xA/+0xC to +0/+2/+4
 * and damps +8/+0xC by 31/32 (bounces +0xA when +2 >= D_800942EC), done at
 * frame (+0x12) 0x10; 1 does the same with 127/128 and +0xA - 1, done at
 * 0x18; 2 only counts to 0x18. Mode 2 draws: state 0 a func_800CEE20 glow
 * sized func_80077CF4(f << 7) * 3 / 2 (draw block D_800F3368 set to 0x20,
 * +2 = 2); state 1 the same with f*1024/24 through func_80077DC4 and the
 * stack vector buf3; state 2 loads the GTE rotation/translation from
 * D_800BCFA4[0] and draws func_800D0728 sized func_80077CF4(f*1024/24),
 * timed by func_80077DC4(f*1024/24) / 32.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_801972EC --check-gte --uncovered --seeds 400
    e21_leaf: ok func_801972EC__489fd292_port.c 785 seeds, retail coverage 384/384 words
    e21_leaf: uncovered none

## Uncovered words

Main-run misses, all dispatch fall-through arms and all covered by the supplementary run (a0=0,1,2,3; state=3,0xFFFF,7): 8019739C/801973A0 are the mode default (mode not 1 or 2). 801973C4/801973C8 are mode 1 with state below 2 but not 0 or 1 (negative). 801973D8/801973DC are mode 1 with state above 2. 8019759C/801975A0 are mode 2 with a negative state. 801975AC/801975B0 are mode 2 with state above 2. No word is left unexplained.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0429i func_801972EC --nonmatching 8304:600 --check-gte --uncovered --seeds 400 --inputs "a0=1,2;h(a1+0x10)=0,1,2;h(a1+0x12)=0,5,0x30;h(0x800F336C)=4,0;w(0x800F3428)=1,0"`
  - e21_leaf: ok func_801972EC__489fd292_port.c 386 seeds, retail coverage 374/384 words | e21_leaf: uncovered 8019739C 801973A0 801973C4 801973C8 801973D8 801973DC 8019759C 801975A0 801975AC 801975B0 (identical to claim)
- `python3 tools/analysis/e21_leaf.py room_m0429i func_801972EC --nonmatching 8304:600 --check-gte --uncovered --seeds 24 --inputs "a0=0,1,2,3;h(a1+0x10)=3,0xFFFF,7"`
  - e21_leaf: ok func_801972EC__489fd292_port.c 24 seeds, retail coverage 85/384 words; uncovered list starts at 801973E0, so the 10 words are covered (identical to claim)
- `NEW A: ... --seeds 300 --inputs "a0=1;h(a1+0x10)=0,1,2;h(a1+0x12)=0xF,0x10,0x17,0x18,0xFFFF;h(a1+8)=0xFFF1,5,0x8000;h(a1+0xC)=0xFFF1,7;h(a1+2)=0,0x7FFF,0x8000"`
  - e21_leaf: ok func_801972EC__489fd292_port.c 299 seeds, retail coverage 165/384 words (mode-1 only; uncovered are mode 2 arms and the dispatch defaults, as expected)
- `NEW B: ... --seeds 100 --inputs "a0=2;h(a1+0x10)=0,1,2;h(a1+0x12)=0,1,0xFFFF,0x17,0x18;h(0x800F336C)=4,0,5;w(0x800F3428)=1,0"`
  - e21_leaf: ok func_801972EC__489fd292_port.c 91 seeds, retail coverage 260/384 words (mode-2 only; uncovered are mode 1 arms and the dispatch defaults, as expected)

No mismatch, notest or skip in any run. I also read the asm against the C by hand and found no divergence. The shifts and divisions match: /32 and /128 on signed values, /24, /160 and the x3/2 round-toward-zero. The loads match: lh versus lhu, and the negate of +0xA done on the value just stored. The calls match, including the stack argument order for func_800CEE20 and func_800D0728 and the DC4-before-CF4 order in state 2. The five uncovered-word justifications check out: each pair is a 'j .L801978C4' plus nop on a dispatch fall-through arm, which only a mode or state outside the main spec can reach. The supplementary run covers them. Minor note: the new runs got 299 and 91 seeds, one below the requested 300 and 100, because the harness drops one seed. Each run still exceeds its spec's product (270 and 90). The ops list does not drive func_80077AA4's or func_800CF3AC's results over a wide range, so those paths are only lightly tested.

