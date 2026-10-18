# room_m0273i func_80194E6C (NON_MATCHING)

Blob offset 0x5E84, 0x77C bytes, VMA 0x80194E6C. The span is `asm` in `configs/USA/overlays/room_m0273i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0273i/func_80194E6C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0273i — func_80194E6C, blob offset 0x5E84, 0x77C bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Written from the retail disassembly (asm/overlays/room_m0273i/5E84.s).
 * Slime (m0273i) drip/trail particle callback; a1 = particle record:
 *   +0..+4 position (x,y,z), +8/+A/+C velocity-angle words, +0xE landed flag,
 *   +0x10..+0x2F x trail, +0x30..+0x4F z trail, +0x50..+0x5F y trail
 *   (8 shorts each, ring index +0x60, fill count +0x62).
 * Mode 1 (update): while landed (+0xE) it counts the trail down (+0x62) and
 * returns 1 when it reaches zero.  Otherwise it spins the angle (+8/+A,
 * +0xC grows away from zero by 2), builds a RotMatrix from {+8, +A, 0},
 * rotates the D_8019ACA4 vector with it (translation = position, GTE mvmva)
 * into the position, lands on the floor line D_800942EC - 0x80, or within
 * 0x140 of the player (D_8009D254 +0x1FC/+0x204; then averages the position
 * with the player's +0x238 record into D_8019AEAC, sets D_8019AEB2, the
 * player flag 0x4000 and the hit latch bit), logs the landing point into the
 * D_8019AEB8/C8/D8 ring, and pushes two rotated trail points (D_8019ACAC /
 * D_8019ACB4) into the record's ring.
 * Mode 2 (draw): state 0 (not landed) draws the glow head at the particle
 * (sprite size from the D_800966EC curve of age D_800E27EC); then both trail
 * rows (2 points each) are drawn with fading brightness.
 * Layout: each ring slot owns two x shorts at +0x10+4*ring and two z shorts
 * at +0x30+4*ring (the pair of rotated trail points), one y short at
 * +0x50+2*ring.  Retail quirk kept: the draw descriptor y (sp+0x5A) is never
 * reset between ring entries, only advanced by 0x800 per trail point.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0273i func_80194E6C --check-gte --uncovered --seeds 240
    e21_leaf: ok func_80194E6C__5b84a50c_port.c 720 seeds, retail coverage 467/479 words
    e21_leaf: uncovered 80195374 80195378 8019537C 80195380 80195384 80195388 801954F4 801954F8 801954FC 80195500 80195504 80195508

## Uncovered words

12 words, all in the two `D_800F336C == 4` tails of mode 2. Retail only does `a1 += 4` when `D_800F336C == 4 && D_800F3428 != 0`.
- 80195374..80195388 (6 words: `lui; lw D_800F3428; nop; beqz; nop; addiu a1,a1,4`). This arm is provably dead. Retail stores `D_800F336C = 3` (sh at 0x801952C4) and reloads it with `lhu` at 0x80195350, with no call or other store in between. The `bne v0,4` is therefore always taken.
- 801954F4..80195508 (6 words, the same test inside the per-point loop). `D_800F336C = 2` is stored before the loop, and the reload happens after calls to `func_800CEE20` and `func_80077AA4`. I read the C for both callees in the tree. `src/func_800CEE20.c` only reads `D_800F336C` (line 140). `src/func_80077AA4.c` does not mention it. Neither writes it, so this arm is dead in practice. I have not audited writes made by anything those two callees call in turn. The e21 harness mocks callees and never writes globals, so it cannot reach this arm either way. My C keeps the per-iteration reload and the `==4 && D_800F3428` test, so it is behaviour-equivalent even if the arm were reachable.

## Independent verification: confirmed

- `e21_leaf.py room_m0273i func_80194E6C --nonmatching 5E84:77C --check-gte --uncovered --seeds 96 --inputs <claim run 1, mode 1 update>`
  - claim run 1 (the 96-seed command run exactly as given): e21_leaf: ok func_80194E6C__5b84a50c_port.c 96 seeds, retail coverage 260/479 words
- `same, --seeds 240 --inputs "a0=2; a1=0x80180800; h(0x8018080E)=0,1; h(0x80180862)=0,1,2,5; h(0x80180860)=0,3,7; w(0x800E27EC)=0,5,0x10,0x11,0x20; h(0x800966EC)=-1000,1000"`
  - claim mode-2 run (240 seeds): e21_leaf: ok func_80194E6C__5b84a50c_port.c 240 seeds, retail coverage 226/479 words
- `same, --seeds 12 --inputs "a0=1,0,3; a1=0x80180800; h(0x8018080E)=1; h(0x80180862)=0,1,5,0x7FFF"`
  - claim 12-seed run: e21_leaf: ok func_80194E6C__5b84a50c_port.c 12 seeds, retail coverage 39/479 words
- `NEW A: --seeds 128 --inputs "a0=1; a1=0x80180800; h(0x8018080E)=0; h(0x80180862)=0,7,8; h(0x8018080C)=0,1,-1,0x7FFE,-0x8000; h(0x8018080A)=0x7FFF,-1; h(0x80180860)=0,3,7; h(0x80180802)=-0x8000,0x7FFF,-129,-128; h(0x800942EC)=0,0x7FFF; h(0x8019AEEC)=-1,0,1; h(0x80180A0E)=3,4; r(0x8005186C)=0x13F,0x140; h(0x8019AEE8)=0,0x7FFF,-1; w(0x8009D254)=0x80180A00; w(0x80180A00)=0x80180E00; w(0x80180C38)=0x80180F00; w(0x800F32D0)=0x80180000; w(0x80180008)=0x80180100,0; w(0x80180100)=0x80180200,0"`
  - NEW A (mode 1 boundary values: 0x7FFF/-0x8000 velocity, -129/-128 y floor edge, 0x13F/0x140 player-distance edge, ring counter 0/0x7FFF/-1): e21_leaf: ok func_80194E6C__5b84a50c_port.c 128 seeds, retail coverage 200/479 words
- `NEW B: --seeds 400 --inputs "a0=2; a1=0x80180800; h(0x8018080E)=0; h(0x80180862)=1,2,8,0x7FFF,-1; h(0x80180860)=0,0x100,-1; h(0x80180808)=0x7000,-0x7000; h(0x8018080A)=0x7FFF,-0x8000; w(0x800E27EC)=0,0x10,0x11,-1; h(0x800966EC)=-0x8000,0x7FFF,5; h(0x800E11EA)=0,1; w(0x800F3428)=0,1"`
  - NEW B (mode 2, extreme sprite-curve values, trail counts, D_800E11EA 0/1, D_800F3428 0/1): e21_leaf: ok func_80194E6C__5b84a50c_port.c 320 seeds, retail coverage 223/479 words (80 seeds reported 'oracle fault: step limit', all on the retail oracle side; no MISMATCH, notest or skipped line)

No MISMATCH, notest or skipped line in any of the five runs. I could not refute the claim.

Uncovered-word justifications, checked against asm/overlays/room_m0273i/5E84.s:
- 80195374..80195388 is dead. 801952C4 stores D_800F336C = 3 (the 0x3 is loaded at 801952BC). 8019534C reloads it with lhu at 80195350. The only code between the two is the sh stores, sp stores and branches, with no jal. The jal sites are 80194F18, 80195058, 8019538C, 801953C8, 8019550C and 80195550. The bne against 4 is therefore always taken.
- 801954F4..80195508 is not provably dead from the asm alone. A jal to func_800CEE20 lies between the store and the reload. I grepped src/func_800CEE20.c and src/func_80077AA4.c. Neither assigns D_800F336C. func_800CEE20 only reads it at line 140. Its callees func_80077A64, func_8079754, func_80078CC4 and func_800786E4 do not mention D_800F336C in src/, and func_80077AA4 does not reference it either. So the arm is dead in practice. The port keeps the per-iteration reload and the ==4 && D_800F3428 test, so it stays equivalent even if a callee did write it. This is a reasoned argument, not a run: the harness mocks callees and cannot reach the arm.

Run caveats:
- NEW B had 80 oracle faults ('step limit'). They come from my extreme inputs h(0x80180862)=0x7FFF/-1 and h(0x80180860)=-1 making the retail loop run past the step limit. They are not port mismatches.
- run_s1.log and run_s2.log in the work dir contain MISMATCH lines for an older port hash (bb6a3517). The current port is 5b84a50c, so these were not counted against it.
- Only the coverage union is from the claimant; I did not recompute it. The four extra runs I did could not reach the 12 words either.

