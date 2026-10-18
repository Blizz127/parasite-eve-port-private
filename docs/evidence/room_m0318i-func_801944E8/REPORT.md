# room_m0318i func_801944E8 (NON_MATCHING)

Blob offset 0x5500, 0x5C4 bytes, VMA 0x801944E8. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_801944E8.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_801944E8, blob offset 0x5500, 0x5C4 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Sibling of the m0429i/m0269i particle callbacks, read from the retail
 * disassembly (517C.s). a0 = command, a1 = effect object.
 * cmd 1: state 0 integrates the velocity (+8/+A/+C) into the position
 * (+0/+2/+4) with 31/32 damping of vx/vz and a vy gravity step that is
 * negated once the object falls below D_800942EC (16 frames); state 1 is a
 * 24-frame timer; state 2 drifts y down by 8 for 16 frames.
 * cmd 2: state 0 draws the sprite (D_800F3368 block 0x10, +2 = 1) on a
 * sin-sized glow 0xDC at {0,0,-(f<<5)}; state 1 places the object at
 * velocity-vector * cos(f*1024/24) around its position, draws an 0x24 sprite
 * with a colour curve (D_80199674) and a 0x80 tint; state 2 draws the glow
 * 0x42 (growing, CLUT y 0x0 / +4).
 * Return: cmd 1 returns 1 when the timer ends, otherwise 0.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_801944E8 --check-gte --uncovered --seeds 192
    e21_leaf: ok func_801944E8__13b7cd4a_port.c 384 seeds, retail coverage 362/369 words
    e21_leaf: uncovered 80194920 80194924 80194928 8019492C 80194930 80194934 80194938

## Uncovered words

80194920, 80194924, 80194928, 8019492C, 80194930, 80194934, 80194938 (7 words, unreached in both runs). This is the `k == 4 && D_800F3428 != 0` arm of the cmd 2 state 1 CLUT selection, which adds 8 instead of 4. Retail stores 3 into D_800F336C just before func_800CF3AC and re-reads it afterwards. Only that callee could change it to 4, and the mocked callee writes only through its stack argument, so with mocks the arm is unreachable. The `bne` at 80194918 and the +4 path are covered.
All other words in run 1's uncovered list (80194568/6C, 80194590/94, 801945A4/A8, 801946E4/E8, 801946F4/F8) are the default arms: mode outside {1,2}, and a state outside {0,1,2} in either mode. Run 2 covers them (it passes the odd a0/state values). In the other direction, run 1 covers everything run 2 lists as uncovered except the k==4 arm: the y-negation fix-up at 80194618, the mode-1 return-1 paths (8019468C/90, 801946BC/C0) and the negative-product rounding at 80194824.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0318i func_801944E8 --nonmatching 5500:5C4 --check-gte --uncovered --seeds 192 --inputs "a0=1,2; h(a1+0x10)=0,1,2; h(a1+0x12)=0,11,12,24; h(0x800F336C)=4,0; w(0x800F3428)=1,0; h(0x800942EC)=0x7000,0"`
  - e21_leaf: ok func_801944E8__13b7cd4a_port.c 192 seeds, retail coverage 352/369 words | uncovered 80194568 8019456C 80194590 80194594 801945A4 801945A8 801946E4 801946E8 801946F4 801946F8 80194920-80194938 (identical to the claim)
- `python3 tools/analysis/e21_leaf.py room_m0318i func_801944E8 --nonmatching 5500:5C4 --check-gte --uncovered --seeds 24 --inputs "a0=0,1,2,3; h(a1+0x10)=-1,0,1,2,3,0x7fff; h(a1+0x12)=0,5"`
  - e21_leaf: ok func_801944E8__13b7cd4a_port.c 24 seeds, retail coverage 356/369 words | uncovered 80194618 8019468C 80194690 801946BC 801946C0 80194824 80194920-80194938 (identical to the claim)
- `NEW A: --seeds 900 --inputs "a0=1; h(a1+0x10)=0; h(a1+8)=0x7fff,-1,-32768,0,31; h(a1+0xC)=0x7fff,-1,-32768,33,0; h(a1+0xA)=0x8000,1,0,-1; h(a1+2)=0x7fff,0,-5; h(0x800942EC)=0x7fff,-1,0"`
  - A: e21_leaf: ok ... 899 seeds, retail coverage 97/369 words (no MISMATCH; the 899 count is the harness's own, versus 900 requested)
- `NEW B: --seeds 112 --inputs "a0=2; h(a1+0x10)=1; h(a1+0x12)=-1,0,1,23,24,0x7fff,-32768; h(a1+6)=0x7fff,-1,-32768,5; h(0x800E11FA)=0,0xffff"`
  - B: e21_leaf: ok ... 112 seeds, retail coverage 148/369 words
- `NEW C: --seeds 112 --inputs "a0=2; h(a1+0x10)=0,2; h(a1+0x12)=-1,0,1,0x7fff,-32768,0x100,0x4000; h(0x800E1208)=0,0xffff; h(0x800E120A)=0,0xffff; h(0x800E11E8)=0,0x7fff"`
  - C: e21_leaf: ok ... 112 seeds, retail coverage 171/369 words
- `NEW D: --seeds 90 --inputs "a0=1; h(a1+0x10)=1,2; h(a1+0x12)=-32768,-1,0,15,16,17,23,24,25,0x7fff; h(a1+2)=0,-1,0x7fff,-32768,16"`
  - D: e21_leaf: ok ... 90 seeds, retail coverage 71/369 words
- `Static checks: read asm/overlays/room_m0318i/517C.s lines 254-647 against src/nonmatching/overlays/room_m0318i/func_801944E8.c; read src/func_800CF3AC.c for writes to D_800F336C`
  - No printed MISMATCH, notest or skipped line in any run.

No refutation found. (1) Both claimed runs reproduce exactly: 352/369 and 356/369 with the same uncovered lists. (2) The asm and the C read the same on every path I checked. Cmd 1 state 0: the /32 damping, the y-1 step, and the negation after the signed compare against D_800942EC. Cmd 1 states 1 and 2: the lh timer compares and the return values. Cmd 2 state 1: the 0x2AAAAAAB multiply, which is divide by 24 after the <<10. The call order DC4, CFB7C, CF4, the +/-0xFFF rounding and the sll4/sra16 narrowing to short. The sp+0x10..0x20 argument order of func_800CEE20. Cmd 2 state 2: sp1C is (s3+sign(s1_biased))>>1, which equals C's s3/2. (3) The 7 words at 80194920-80194938 are justified. After func_800CF3AC returns, D_800F336C can only differ from the 3 stored just before the call if that callee writes it. src/func_800CF3AC.c has no D_800F336C write (it only reads the header, builds the table and calls func_80078554), so the k==4 arm cannot be reached by any input I can construct, not only because of mocking. Seeds that set h(0x800F336C)=4 have no effect because the store of 3 precedes the call. (4) Minor, not a defect: my four extra specs each cover only the arm they target, so their coverage is low (71 to 171 words). The claim's own two runs, not mine, provide the overall coverage. Edge values in my specs: the +8/+0xC velocity at +/-32768, the y step at 0x8000, a +0x12 timer at -32768/0x7fff, a +6 factor at -32768 and the 0xffff CLUT table indices.

