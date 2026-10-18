# exe func_800D8388 (NON_MATCHING)

SLUS file offset 0xC8B88, 0x304 bytes, VMA 0x800D8388. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800D8388.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800D8388 — EXE, VRAM 0x800D8388, file offset 0xC8B88, 0x304 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Orbiting-effect task handler (the effect-descriptor callback with the
 * func_800D7FBC / func_800D7E78 sub-handlers), a0 = mode, a1 = effect state:
 *   mode 0 (init):  a1[0] = rand; allocates 12 slots stride 0x10 from the
 *                   task list (func_800CE560, callback func_800D7FBC) and
 *                   registers the 8-entry list a1+4 (func_800CE5AC, callback
 *                   func_800D7E78); returns the two counts summed.
 *   mode 1 (step):  every third frame below D_800E27EC == 0x2C spawns one
 *                   glint (func_800CE610 slot: pos 0, speed 0x2BC, x = a1[0],
 *                   y from the object position + random(0x1FF) - 0x28F),
 *                   a1[0] -= 0x555; below 0x46 retires the a1[1] list and
 *                   saves it in D_800E21F4; returns 1 at/after 0x46.
 *   mode 2 (draw):  loads the first D_800BCFA4 matrix into the GTE (rotation
 *                   and translation) and sets the two D_800F3368 draw blocks
 *                   (0x10 sprite id 1, 0x20 sprite id 2) around func_800CEDA8 /
 *                   func_800CE78C / func_800CE870 calls.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800D8388 --check-gte --uncovered --seeds 108
    e21_leaf: ok func_800D8388 (exe) 210 seeds, retail coverage 193/193 words
    e21_leaf: uncovered none

## Uncovered words

Main run, uncovered: 800D83BC (j .L800D8670) and 800D83C0 (its delay slot addu v0,zero,zero). This is the arm for 0 < a0 < 2 failing a0==0, i.e. a negative mode, reachable only with a0 < 0. The supplementary run with a0=-1 covers both words and returns 0. No other word is uncovered.

## Notes from the decomp agent

Function summary: func_800D8388(mode a0, state a1) is the orbiting-effect task handler (callbacks func_800D7FBC / func_800D7E78). Mode 0 (init): a1[0]=rand; r=func_800CE560(D_800F33E0[+8], 0x10, 0xC, func_800D7FBC); v=func_800CE5AC(a1+4, r, 8, 0x20, func_800D7E78); returns r+v. Mode 1 (step): if D_800E27EC < 0x2C and divisible by 3, func_800CE610(D_800F33E0[+8]) returns a slot. If the slot is non-null it sets slot+0=0, +4=0, +0xC=0x2BC, +2=a1[0], calls func_800CE870(D_8009D254, 1, buf) and sets slot+8 = buf.h1 + (rand & 0x1FF) - 0x28F. It then does a1[0] -= 0x555. After that, if D_800E27EC < 0x46 it calls func_800CE688(a1[1]), sets D_800E21F4 = a1[1] and returns 0; otherwise it returns 1. Mode 2 (draw): loads D_800BCFA4[0] rotation and translation into the GTE, fills the D_800F3368 draw block (a68/6A/6C/70/76/78 at +0/+2/+4/+8/+0xE/+0x10), calls func_800CEDA8(1), sets 6E/72/74, calls func_800CE78C(a1[1]) and func_800CE870(D_8009D254, 1, D_800E21EC), then fills the second block (0x20 / sprite 2 / D_800E11F6) and calls func_800CEDA8(1) again. Other modes return 0. Retail reads uninitialised stack: in mode 1 it loads lhu 0x1A($sp) (buf[1] of the 0x18($sp) buffer) after func_800CE870 and stores it into slot+8. If the mocked callee does not write that buffer, retail and native read different stack garbage and the run reports a false mismatch (RAM 0x8018A118 retail 85 vs native 95; reproduced when the out= term was omitted). Always pass out=800CE870:2:8, as the inputs spec does; with it the 250-seed run is clean. The C writes the faithful behaviour with buf[4] in the callee-filled buffer. The func_800CE610 null-return (s0==0) path is covered only as a fall-through to the always-covered L800D84E4 tail; no run verified that arm separately. I did not make the mocks return 0 deliberately. Callbacks are passed as extern unsigned char func_X[] arrays and func_800CE5AC takes its fifth (stack) argument as a normal fifth C argument. This differs from the retail frame: retail uses a 0x38 frame, native does not. Nothing outside the one deliverable file was edited."

## Independent verification: confirmed

- `claimed run 1: e21_leaf.py exe func_800D8388 --nonmatching C8B88:304 --check-gte --uncovered --seeds 108 --inputs "a0=0,1,2,3;w(0x800E27EC)=0,1,-3,0x2A,0x2C,0x45,0x46,0x2B,-1;out=800CE870:2:8"`
  - e21_leaf: ok func_800D8388 (exe) 102 seeds, retail coverage 191/193 words | e21_leaf: uncovered 800D83BC 800D83C0
- `claimed run 2: ... --seeds 8 --inputs "a0=-1,3,0x7FFFFFFF,2;w(0x800E27EC)=0,0x2D"`
  - e21_leaf: ok func_800D8388 (exe) 8 seeds, retail coverage 112/193 words (uncovered = mode 0/1/2 arms not selected; 800D83BC/83C0 not listed, so covered)
- `claimed run 3: ... --seeds 400 --inputs "a0=1;w(0x800E27EC)=0,3,6,-3,9,42;out=800CE870:2:8"`
  - e21_leaf: ok func_800D8388 (exe) 250 seeds, retail coverage 74/193 words
- `new A: ... --seeds 12 --inputs "a0=-2147483648,-1,4,100;w(0x800E27EC)=0,0x2B,0x2C"`
  - new A: e21_leaf: ok func_800D8388 (exe) 12 seeds, retail coverage 27/193 words (negative/INT_MIN/large a0, covers the 83BC/83C0 arm)
- `new B: ... --seeds 120 --inputs "a0=1,2;w(0x800E27EC)=0x27,0x2A,0x2B,0x2C,0x2D,0x44,0x45,0x46,0x47,-1,-2,-3,-4,0x7FFFFFFF,0x80000000,0x30,0x33,3,5,6;out=800CE870:2:8"`
  - new B: e21_leaf: ok func_800D8388 (exe) 120 seeds, retail coverage 165/193 words
- `new C: ... --seeds 20 --inputs "a0=0,2;w(0x800E11E6)=0,1,0x7FFF,0xFFFF;w(0x800E11F6)=0,0xFFFF;out=800CE870:2:8"`
  - new C: e21_leaf: ok func_800D8388 (exe) 20 seeds, retail coverage 130/193 words (D_800E11E6/11F6 table index extremes, modes 0 and 2)

No mismatch, notest or skipped line in any run. The two words uncovered in the main run (800D83BC j and 800D83C0 delay slot) are reachable only for a0 < 0. In the asm, a0 == 1 goes to .L800D8430 and a0 >= 2 goes to .L800D83C4, so a0 < 2 with a0 != 0 and a0 != 1 means a0 < 0. Run 2 and new run A, both with negative a0, did not list those two words as uncovered and matched, so the claim holds. The C returns 0 for any other mode, which matches the asm. I also read the C against the asm for loads, compares, stores, call arguments and returns. I found no divergence in the mode 0 call arguments, the mode 1 divisibility test, the `lhu 0x1A` read of buf[1], the 0x555 decrement on the spawn path only, the 0x46 return, or the mode 2 store set and ordering. Residual limit: no new run covered every arm at once, but the union of runs covers all 193 words."

