# room_m0432i func_80194F60 (NON_MATCHING)

Blob offset 0x5F78, 0x86C bytes, VMA 0x80194F60. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_80194F60.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_80194F60, blob offset 0x5F78, 0x86C bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback, modes 0/1/2 on a point record a1, read from the
 * retail disassembly (sibling of room_m0432i func_8019485C and room_m0429i
 * func_801960C4/func_80196CCC).
 * mode 0: plays sound 0x5DC (func_800D3FD8() result) and 0x5DD, reads the world
 *   position of D_8009D254 into *a1 (func_800CE870), flips the byte at
 *   D_800F32D0->8->0->0x18 from 1 to 2 when D_800E2368->0xD is set, and arms
 *   func_8019485C through func_800CE560(D_800F33E0->8, 0x14, 0x3C, ...).
 * mode 1: frame counter D_800E27EC. func_80020D50() below 0x1C; two state-0
 *   sparks (+-32 velocity) up to 0x10; a state-1 spark (+-20/+-16/+-20) up to
 *   0x20 and, on odd frames, a state-2 spark (+-24); func_80020DD0() at 0x1C;
 *   at 0x1D (with D_800E2368->0xD set and the top flag bits of
 *   ***(D_800F32D0+8) == 0x01000000) sets bit 0x4000 in *D_8009D254->0x4C and
 *   rewrites/sets bits in the record word; returns 2 from frame 0x20.
 * mode 2: loads the camera matrix D_800BCFA4[0] into the GTE, fills the
 *   D_800F3368 draw block (size 0x20) and, up to frame 0x20, draws a
 *   func_800CEE20 sprite and func_800D004C / func_800D0728 rings sized from
 *   func_80077CF4; finally re-seeds a72 = 0 / a74 = 8.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
 * Retail frame: sp+0x30 = D_8018F1FC (8 bytes), +0x38 vector, +0x40 =
 * D_8018F210 (4 bytes), +0x48 colour (written by func_800CF3AC).
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_80194F60 --check-gte --uncovered --seeds 96
    e21_leaf: ok func_80194F60__bace993d_port.c 390 seeds, retail coverage 532/539 words
    e21_leaf: uncovered 80195650 80195654 80195658 8019565C 80195660 80195664 80195668

## Uncovered words

80195650-80195668 (words 80195650, 80195654, 80195658, 8019565C, 80195660, 80195664, 80195668): the D_800F336C == 4 && D_800F3428 != 0 arm of the CLUT index (w += 0xB instead of += 7) in mode 2. Unreachable under the e21 mocks: the function itself writes D_800F336C = 3 (at 8019556C) before the compare at 80195648, and the only call in between (func_800CE870) is mocked and writes only through its stack-vector argument, so D_800F336C is always 3 there; the retail words 80195650..80195658 are the load of D_800F3428 that only executes on the equal-4 path. The C keeps the arm faithfully (if (a6C == 4 && D_800F3428 != 0) w += 0xB; else w += 7).

## Independent verification: confirmed

- `e21_leaf.py room_m0432i func_80194F60 --nonmatching 5F78:86C --check-gte --uncovered --seeds 8`
  - e21_leaf: ok func_80194F60__bace993d_port.c 8 seeds, retail coverage 46/539 words
- `... --seeds 96 --inputs "a0=1; ...; w(0x800E27EC)=0x10,0x11,0x1B,0x1C,0x1D,0x1F,0x20,0x21; ...; r(0x80071A54)=-7,5,100"`
  - e21_leaf: ok func_80194F60__bace993d_port.c 50 seeds, retail coverage 299/539 words
- `... --seeds 48 --inputs "a0=0; ..." (claimed mode-0 spec)`
  - e21_leaf: ok func_80194F60__bace993d_port.c 35 seeds, retail coverage 93/539 words
- `... --seeds 4 --inputs "a0=3,0xFFFFFFFF"`
  - e21_leaf: ok func_80194F60__bace993d_port.c 4 seeds, retail coverage 51/539 words
- `... --seeds 32 --inputs "a0=2; w(0x800E27EC)=0x10,0x11,0x20,0x21; r(0x80077CF4)=-100,-33,100,4000; w(0x800F32D0)=0x80120000"`
  - e21_leaf: ok func_80194F60__bace993d_port.c 24 seeds, retail coverage 218/539 words
- `NEW n1: ... --seeds 24 --inputs "a0=2; w(0x800E27EC)=0x10,0x21; r(0x80077CF4)=-1,0,1,0x7FFFFFFF,0x80000000,-32768; r(0x80071A54)=-1,0,1,-0x8000; w(0x800F32D0)=0x80120000"`
  - NEW n1: e21_leaf: ok func_80194F60__bace993d_port.c 17 seeds, retail coverage 212/539 words
- `NEW n2: ... --seeds 12 --inputs "a0=1; w(0x800F32D0)=0x80120000; w(0x80120008)=0x80121000; w(0x80121000)=0x80122000; w(0x80122018)=0x80123000; h(0x80123000)=0xFFFF,0x7FFF,0x8000; w(0x800E2368)=0x80124000; w(0x8009D254)=0x80125000; w(0x80125000)=0x80126000; h(0x8012400C)=0xFFFF,0x8000; r(0x80071A54)=-1,1"`
  - NEW n2: e21_leaf: ok func_80194F60__bace993d_port.c 6 seeds, retail coverage 247/539 words

No MISMATCH, notest or skipped in any of the 7 runs, and the coverage figures match the claim exactly. n1 and n2 completed fewer seeds than I requested (17 of 24 and 6 of 12), so the harness appears to cap seeds at the number of distinct combinations. I did not check this.

The claim's uncovered justification for 80195650-80195668 holds up in the asm. 0x8019556C does `addiu v0,zero,3`, and the following instructions store it to D_800F336C. Between that store and the compare at 0x80195648 (`bne a0,v1(4)`), the only calls are func_80077CF4 (twice) and func_800CE870, all mocked in e21. The a0 reload at 0x80195630 is `lhu D_800F336C`. So under the mocks the value is always 3 and the 0xB arm cannot run. It could only run if one of those callees really wrote D_800F336C, which is unlikely but not proven. The C keeps the arm: `a6C == 4 && D_800F3428 != 0` gives `w += 0xB`, else `w += 7`, which matches the asm (`+0xB` at 0x80195668, `+7` at 0x8019566C).

The first run of the claimed 96-seed spec finished with 50 seeds, and the claimed commands ran correctly after one restart. The first chain was interrupted by a harness timeout, and I killed a duplicate chain I had launched. Those interruptions did not affect the results.

Residual risk: none of my new inputs targeted the 0xB arm, which is unreachable under the mocks anyway. n1 and n2 added signedness and edge-value coverage (r(0x80077CF4) at 0x7FFFFFFF, 0x80000000, -1 and -32768; r(0x80071A54) at -0x8000; halfwords 0xFFFF, 0x7FFF and 0x8000) and all matched."

