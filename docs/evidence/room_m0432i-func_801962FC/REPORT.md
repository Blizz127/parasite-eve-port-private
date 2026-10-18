# room_m0432i func_801962FC (NON_MATCHING)

Blob offset 0x7314, 0x258 bytes, VMA 0x801962FC. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_801962FC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_801962FC, blob offset 0x7314, 0x258 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback, read from the retail disassembly.
 * cmd 1, state 0: frame counter (+0x12)++, +0xE += 0x20; done (1) at 0x20 frames.
 * cmd 2, state 0: places the sprite from func_800CF844 around a vector scaled by
 * the sine of the frame (<< 5), seeds the D_800F3368 draw block (+0/+0xE/+0x10 =
 * 0x20, +2 = 2, +4 = 3, +6 = 1, +8 = D_800E2850[D_800E11FA]), picks a colour via
 * func_800CF3AC and draws with func_800CEE20. The 8-byte seed buffer copied from
 * D_8018F1F4 has its halfword at +4 overwritten with +0xE - frame * 30.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_801962FC --check-gte --uncovered --seeds 64
    e21_leaf: ok func_801962FC__a78a82f2_port.c 64 seeds, retail coverage 141/150 words
    e21_leaf: uncovered 80196350 80196354 801964CC 801964D0 801964D4 801964D8 801964DC 801964E0 801964E4

    python3 tools/analysis/e21_leaf.py room_m0432i func_801962FC --nonmatching 7314:258 --check-gte --uncovered --seeds 8 --inputs "a0=0,3,1,2,-1,0x10"
    e21_leaf: ok func_801962FC__a78a82f2_port.c 8 seeds, retail coverage 37/150 words
    e21_leaf: uncovered 80196368 8019636C 80196370 80196374 80196378 8019637C 80196380 80196384 80196388 8019638C 80196390 80196394 801963A8 801963AC 801963B0 801963B4 801963B8 801963BC 801963C0 801963C4 801963C8 801963CC 801963D0 801963D4 801963D8 801963DC 801963E0 801963E4 801963E8 801963EC 801963F0 801963F4 801963F8 801963FC 80196400 80196404 80196408 8019640C 80196410 80196414 80196418 8019641C 80196420 80196424 80196428 8019642C 80196430 80196434 80196438 8019643C 80196440 80196444 80196448 8019644C 80196450 80196454 80196458 8019645C 80196460 80196464 80196468 8019646C 80196470 80196474 80196478 8019647C 80196480 80196484 80196488 8019648C 80196490 80196494 80196498 8019649C 801964A0 801964A4 801964A8 801964AC 801964B0 801964B4 801964B8 801964BC 801964C0 801964C4 801964C8 801964CC 801964D0 801964D4 801964D8 801964DC 801964E0 801964E4 801964E8 801964EC 801964F0 801964F4 801964F8 801964FC 80196500 80196504 80196508 8019650C 80196510 80196514 80196518 8019651C 80196520 80196524 80196528 8019652C 80196530 80196534 80196538

## Uncovered words

80196350 and 80196354 (the "j .L8019653C" for a mode other than 1 or 2) are covered by the supplementary run with a0=0,3,-1,0x10, which does not list them as uncovered. 801964CC..801964E4 (7 words: the "D_800F336C == 4 and D_800F3428 != 0 then +0xA" arm) are unreachable in the differential. The retail code stores 3 to D_800F336C a few instructions earlier and reloads it after func_800CF3AC, and the mocked callee never writes that global. The C keeps the reload and the branch faithfully. The arm could only run if the real func_800CF3AC modified D_800F336C.

## Independent verification: confirmed

- `e21_leaf room_m0432i func_801962FC --nonmatching 7314:258 --check-gte --uncovered --seeds 64 --inputs "a0=1,2;h(a1+0x10)=0,1;h(a1+0x12)=0x1e,0x1f,0,5,-3,0x7ff0,0x20;h(a1+0xE)=0,0x7fe0,0xfff0"`
  - e21_leaf: ok func_801962FC__a78a82f2_port.c 64 seeds, retail coverage 141/150 words
- `e21_leaf ... --seeds 8 --inputs "a0=0,3,1,2,-1,0x10"`
  - e21_leaf: uncovered 80196350 80196354 801964CC..801964E4 (as claimed)
- `NEW: e21_leaf ... --seeds 48 --inputs "a0=2;h(a1+0x10)=0;h(a1+0x12)=0,1,-1,0x7fff,-0x8000,0x100;h(a1+6)=0,-1,0x7fff,-0x8000;h(a1+0xE)=0,-1,0x8000"`
  - e21_leaf: ok ... 8 seeds, retail coverage 37/150 words; uncovered list begins at 80196368, so 80196350/54 are covered
- `NEW: e21_leaf ... --seeds 24 --inputs "a0=1;h(a1+0x10)=0,1,-1,0x8000;h(a1+0x12)=0x1f,0xffff,0x7fff,0x8000,-0x20,0x20;h(a1+0xE)=0xffe0,0x7fff"`
  - e21_leaf: ok ... 48 seeds, retail coverage 125/150 words (only mode-2 inputs; uncovered = mode-1 arm, 80196350/54, and 801964CC..801964E4)

No MISMATCH, notest or skipped output in any run. I read the asm against the C side by side and found no divergence. The checks covered: the sh/lhu/lh signedness at +0x10, +0x12 and +0xE; the slti-based done test; the mult by lh(+6) with the bgez+0xFFF fixup then sra 12 (equal to /4096); the r*3/2 rounding; the argument order of func_800CF844, func_800CF3AC, func_80077AA4 and func_800CEE20; the stores of 0x20, 2, 3 and 1 into D_800F3368..; and the buf+4 halfword. Words 801964CC..801964E4 are the 'D_800F336C==4 && D_800F3428 != 0' arm. Retail stores 3 to D_800F336C earlier and reloads it after func_800CF3AC, so the arm is reachable only if the real callee writes D_800F336C. No input to this function can reach it with the mocked callee. The C preserves the reload and the branch, so this limit is honest. It is a residual coverage limit, not a defect. 80196350 and 80196354 are reached when a0 is a mode other than 1 or 2, and the supplementary run covers them."]

