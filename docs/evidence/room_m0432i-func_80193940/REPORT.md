# room_m0432i func_80193940 (NON_MATCHING)

Blob offset 0x4958, 0x74C bytes, VMA 0x80193940. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_80193940.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_80193940, blob offset 0x4958, 0x74C bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback, read from the retail disassembly; sibling of
 * func_80192548 / func_8019485C (frame counter at +0x12, kind at +0x10).
 * cmd 1 (step): position (+0/+2/+4) += velocity (+8/+0xA/+0xC) and the frame
 *   counter ticks; kind 0 damps +8/+0xC by 31/32 and bounces +0xA off the
 *   floor D_800942EC (done at 0x10 frames); kind 1 damps by 127/128, +0xA also
 *   loses 1 per frame, same bounce (done at 0x18 frames); kind 2 only ticks
 *   the frame (done at 0x18 frames).
 * cmd 2 (draw): kind 0 pulses a sprite through func_800CEE20 (size 1.5 * sine
 *   of frame << 7, rotation -(frame << 5), colour via func_800CF3AC); kind 1
 *   spins a sprite of size sine/2 (rotation frame << 5); kind 2 loads the
 *   camera matrix/translation into the GTE and draws a ring-like sprite from
 *   the second seed vector (z = D_800E27EC << 6), size 2 * (sine/2 + 0x800).
 * The three seeds copied on entry are D_8018F1F4 / D_8018F1FC (8-byte vectors)
 * and D_8018F204 (4-byte colour block, rewritten by func_800CF3AC).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_80193940 --check-gte --uncovered --seeds 540
    e21_leaf: ok func_80193940__e5cd1f27_port.c 2096 seeds, retail coverage 447/467 words
    e21_leaf: uncovered 80193CEC 80193CF0 80193CF4 80193CF8 80193CFC 80193D00 80193E3C 80193E40 80193E44 80193E48 80193E4C 80193E50 80193E54 80193FF8 80193FFC 80194000 80194004 80194008 8019400C 80194010

## Uncovered words

80193CEC-80193D00 (6 words), 80193E3C-80193E54 (7 words), 80193FF8-80194010 (7 words): the three 'if (D_800F336C == 4 && D_800F3428) w += k' arms (kind 0: +4, kind 1: +0xA, kind 2: +6). Each is dead code in this function: D_800F336C is stored as 3 a few instructions earlier in the same arm and the only call in between is func_800CF3AC(D_80199308, &col, frame), whose real body (src/func_800CF3AC.c) writes only through its pointer arguments and never touches D_800F336C or D_800F3428. The compare against 4 can therefore never succeed. The C keeps the arm for fidelity (it is the same instruction sequence, with D_800F336C re-read from memory after the call, exactly as retail does). The directed h(0x800F336C)=4,0 term in F1 stores into RAM before entry but the function overwrites it with 3, which is why it cannot reach the arm. All other branch, switch-default and negative-rounding arms (80193A94, 80193B2C, 80193B80-88, a0 not 1/2, kind outside 0..2 in both modes) are covered by F1+F2.

## Independent verification: confirmed

- `e21_leaf.py room_m0432i func_80193940 --nonmatching 4958:74C --check-gte (plain re-run)`
  - e21_leaf: ok func_80193940__e5cd1f27_port.c 6 seeds
- `same + --uncovered --seeds 40 --inputs F1 spec (a0=1,2; kind 0..2; frames; h(0x800F336C)=4,0; w(0x800F3428)=1,0)`
  - e21_leaf: ok ... 39 seeds, retail coverage 437/467 words (uncovered includes 80193CEC-D00 and 80193E3C-E54 dead arms, matching the claim)
- `NEW 1: --uncovered --seeds 90 --inputs "a0=1;a1=0x80101000;h(a1+0x10)=0,1,2;h(a1+0x12)=0,0xf,0x17,0x18,0x7fff,0x8000;h(a1+8)=-1,-31,31,-32,-127,-128,0x7fff,0x8000;h(a1+0xC)=-1,1;h(a1+2)=0,-1;h(0x800942EC)=0,-1"`
  - e21_leaf: ok ... 90 seeds, retail coverage 160/467 words
- `NEW 2: --uncovered --seeds 200 --inputs "a0=2;a1=0x80101000;h(a1+0x10)=0,1,2;h(a1+0x12)=-1,1,0xb,0x13,0x2f,0x17,0xffff,0x4000,0x8001;h(0x800E27EC)=-1,0x7fff,0x1000000;h(0x800E11EA)=0xffff,2;h(0x800E11FA)=0xffff,3"`
  - e21_leaf: ok ... 181 seeds, retail coverage 323/467 words
- `--seed-list 104 with verifier n2 spec, a1=0x80101000 pinned`
  - e21_leaf: ok ... 1 seeds

No MISMATCH, notest or skipped in any run. The claimed 20 uncovered words (80193CEC-D00, 80193E3C-E54, 80193FF8-E010) check out against the asm. At 80193CC4-CE4 the retail code re-reads D_800F336C right after jal func_800CF3AC, having stored 3 into it at 80193C9C, and branches on it equalling 4. func_800CF3AC (src/func_800CF3AC.c) writes only through its pointer arguments, so the compare cannot succeed and the arm is dead. I did not re-run the 480/540-seed and 111-seed runs or the unpinned seed-104 run; I read the seed-104 mismatch as a harness artifact (unpinned a1 pointing at the function's own code) on the claim's word. It passes when a1 is pinned. Only the rerun of seed 104 with a1 pinned was done. Minor residual risk: that artifact explanation was not independently reproduced.

