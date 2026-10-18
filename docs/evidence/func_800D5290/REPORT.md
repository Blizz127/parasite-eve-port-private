# exe func_800D5290 (NON_MATCHING)

SLUS file offset 0xC5A90, 0x608 bytes, VMA 0x800D5290. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800D5290.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800D5290 -- EXE, VRAM 0x800D5290, file offset 0xC5A90, 0x608 bytes.
 * NON_MATCHING behaviour C (proven by the e21 differential; not a byte match).
 * Three-mode effect callback (a0 = mode, a1 = effect record); the EXE twin of
 * the room callbacks room_m0429i func_801982A4 / room_m0269i func_8019070C:
 *   mode 0: seed the record (rand into +8, +0/+2/+4 = halfwords 0x268/0x26A/
 *           0x26C of the object at D_800F32D0->+8), then
 *           func_800CE560(D_800F33E0->+8, 0x10, 0xC, func_800D5010); its result
 *           is the return value.
 *   mode 1: while D_800E27EC < 6 spawn two particles from
 *           func_800CE610(D_800F33E0->+8) (position = record +0/+2/+4, speed
 *           from sin/cos(record +8) * (rand & 15) + 0x2B / 4096, random drift
 *           words, record +8 += 0x955); returns D_800E27EC >= 0x18.
 *   mode 2: set the camera matrix *D_800BCFA4, set up the D_800F3368 draw block,
 *           draw the glow with func_800CEE20 around a stack vector
 *           {0, 0, f << 4, 0} (f = D_800E27EC), and while D_800E27EC < 9 a
 *           func_800D004C sprite; for 4 <= f <= 16 draw func_800D0728 through
 *           a stack vector {record x, D_800942EC, record z}; then restore the
 *           draw block (D_800F3374 = 0x40). Returns 0.
 * Retail copies the 4 bytes at D_800C22BC to a stack buffer (sp+0x40) passed to
 * func_800D004C / func_800D0728; the vector at sp+0x38 has its fourth halfword
 * (sp+0x3E) never written -- retail passes it as-is.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800D5290 --check-gte --uncovered --seeds 300
    e21_leaf: ok func_800D5290 (exe) 548 seeds, retail coverage 379/386 words
    e21_leaf: uncovered 800D564C 800D5650 800D5654 800D5658 800D565C 800D5660 800D5664

## Uncovered words

800D564C..800D5664 (7 words: the `D_800F336C == 4 && D_800F3428 != 0` arm, the +5 CLUT-offset path). Provably unreachable. Retail stores 0 to D_800F336C (sh $zero at 0x800D5530) and then calls func_800CEDA8(0). src/func_800CEDA8.c touches only D_800B0E18, D_800B0E1C and D_800F34E4, so D_800F336C is still 0 when it is re-read at 0x800D562C. The `bne v0,4` branch is therefore always taken and the 4/D_800F3428 arm cannot run. The C keeps the arm faithfully. In the supplementary run only, 800D52E8/52EC/52FC/5300 (mode default arms: mode<0 and mode>=3 return 0) were not covered, but the first run (a0=-1, 3) covers them.

## Notes from the decomp agent

Written from the retail disassembly, with m2c skipped. It follows the structure of the twin func_801982A4 (room_m0429i) and src/nonmatching/exe/func_800D5A00.c. Modes: 0 seeds the record and returns func_800CE560(D_800F33E0[2], 0x10, 0xC, func_800D5010). 1 runs 2 particle spawns while E<6 and returns E>=0x18. 2 sets the GTE camera matrix, draws with func_800CEE20 and func_800D004C (E<9), draws with func_800D0728 for 4<=E<=16, and resets the D_800F3368 draw block, ending with D_800F3374=0x40. Other modes return 0. Retail stack layout is vector A at sp+0x30, vector B at sp+0x38 and a 4-byte buffer at sp+0x40, copied from D_800C22BC. In the mode-2 func_800D0728 call, vector B's 4th halfword (sp+0x3E) is never written, so retail passes it as stack garbage. B.y is first set to the record's +2 halfword and then overwritten by D_800942EC (retail does the same, sh at 0x3A). Divisions are plain C: the retail sequence is a magic 0x2AAAAAAB multiply, i.e. /24, /12, /6-style signed division, and the differential matches them with extreme and negative E. The GTE registers matched on all seeds under --check-gte. No stack-read-before-write was flagged by the differential other than the unwritten B.w noted above. Both runs exited ok. The only non-zero shell exit was a trailing grep in my command, not e21_leaf. D_800F32D0 and D_800F33E0 are `int *` (read as [2], i.e. +8). D_800942EC, D_800E11F4 and D_800E11E4 are unsigned short (lhu). The nonmatching.yaml entry is `[exe, func_800D5290, 0xC5A90, 0x608]`. I did not edit it per the rules.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800D5290 --nonmatching C5A90:608 --check-gte --uncovered --seeds 200 --inputs "a0=0,1,2,3,-1; w(0x800E27EC)=0,1,5,6,8,9,0x10,0x11,0x17,0x18,-3,0x2000000; h(0x800F336C)=4,0; w(0x800F3428)=1,0"`
  - e21_leaf: ok func_800D5290 (exe) 186 seeds, retail coverage 379/386 words; uncovered 800D564C 800D5650 800D5654 800D5658 800D565C 800D5660 800D5664
- `python3 tools/analysis/e21_leaf.py exe func_800D5290 --nonmatching C5A90:608 --check-gte --uncovered --seeds 300 --inputs "a0=2,1,0; w(0x800E27EC)=3,4,8,9,12,16,17,-4,-1,0x7FFFFFFF,0x80000000,0x400000; h(a1+2)=0,5,0xFFFA,0x8000,0x7FFF; w(a1+8)=0,0x7FFFFFFF,0x80000000,-5"`
  - e21_leaf: ok func_800D5290 (exe) 266 seeds, retail coverage 375/386 words; uncovered 800D52E8 800D52EC 800D52FC 800D5300 800D564C..800D5664
- `NEW1: ... --seeds 60 --inputs "a0=0,1,2,3,-1,4; w(0x800E27EC)=0x40000,-0x40000,0x1FFFFF,0x200000,-0x200000,0x3FFFFF,1,-1,0x7FF,0x800"`
  - NEW1: e21_leaf: ok func_800D5290 (exe) 55 seeds, retail coverage 331/386 words (uncovered includes 800D564C..5664 and 800D5744..5800, which are other arms not reached by that a0 set)
- `NEW2: ... --seeds 144 --inputs "a0=0,1,2; w(0x800E27EC)=31,32,33,-31,-32,-33,0x3F,0x40; w(0x800F3428)=0,1; h(0x800F336C)=0,4,1"`
  - NEW2: e21_leaf: ok func_800D5290 (exe) 129 seeds, retail coverage 327/386 words (same style of uncovered words)

No MISMATCH, notest or skipped in any run. I re-ran both claimed commands and they reproduced exactly. Two new specs (E near 2^18/2^21 and +/-1 boundaries, a0 including 4, and E near multiples of 32 with D_800F336C and D_800F3428 varied) also passed. The new runs covered less than the claimed runs because their inputs were narrower, so they add little coverage. The unreachability claim for 800D564C..5664 holds at the instruction level. 800D5530 stores 0 to D_800F336C. Only func_800CEDA8, func_80077DC4 and the stack writes run before the re-read at 800D562C. func_800CEDA8 touches only D_800B0E18, D_800B0E1C and D_800F34E4, and calls func_8007506C. func_80077DC4 is a pure table lookup. The two local-address writes, sh to 0x2($s2) at 800D5588 and the $sp stores, cannot alias D_800F336C. One residual caveat: func_8007506C dispatches through the D_80095744 handler pointer (+0x8), which is an unseen host or asm callee. The claim's evidence does not rule out that handler writing D_800F336C. I judge that implausible, since it is a display-slot handler. The claim's 'provably' is therefore slightly strong, but the 7 words are not reachable by any input I could construct."]

