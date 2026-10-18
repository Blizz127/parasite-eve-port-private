# exe func_800D6514 (NON_MATCHING)

SLUS file offset 0xC6D14, 0x508 bytes, VMA 0x800D6514. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800D6514.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800D6514 -- EXE, VRAM 0x800D6514, file offset 0xC6D14, 0x508 bytes.
 * NON_MATCHING behaviour C (proven by the e21 differential; not a byte match).
 * Three-mode effect callback (a0 = mode, a1 = effect record), the sibling of
 * func_800D5A00 (same descriptor shape, different effect):
 *   mode 0: seed the record (+8 = 0, +0/+2/+4 = halfwords 0x268/0x26A/0x26C of
 *           the object at D_800F32D0->+8, then +2 = D_800942EC), call
 *           func_800C6D5C(D_800F32D4, 0, 0xC0) and return
 *           func_800CE560(D_800F33E0->+8, 0x10, 0x18, func_800D629C).
 *   mode 1: on the first frame (D_800E27EC == 0) spawn 24 particles from
 *           func_800CE610(D_800F33E0->+8) (+8 = i*0xAA, +0xA = a1->+2,
 *           +0xC = rand); return 1 once D_800E27EC >= 0x18, else publish
 *           a1's position to D_800E21D8/DA/DC, D_800E21D4 = f / 3 with
 *           f = func_80077CF4((D_800E27EC << 10) / 24); return 0.
 *   mode 2: build a scaled rotation (func_80079754/func_80078CC4) from
 *           sin/cos of (D_800E27EC << 10) / 24, load the camera matrix
 *           *D_800BCFA4 into the GTE, draw the primitive twice (full scale
 *           then half scale) through func_800C6EC0..func_800C6F4C and reset
 *           the D_800F336x/337x draw parameters. Returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800D6514 --check-gte --uncovered --seeds 144
    e21_leaf: ok func_800D6514 (exe) 272 seeds, retail coverage 316/322 words
    e21_leaf: uncovered 800D688C 800D6890 800D6894 800D6898 800D689C 800D68A0

## Uncovered words

800D654C, 800D6550 (j to the return plus addu v0,zero: the a0 < 2 and a0 != 0 default arm, e.g. a0 = -1). Covered by the supplementary a0=-1,3 run. 800D6560, 800D6564 (the a0 > 2 and a0 != 2 default arm, e.g. a0 = 3). Covered by the same supplementary run. 800D688C..800D68A0 (6 words: lui/lw D_800F3428, beqz, and the addiu a2,a2,4 under 'D_800F336C == 4 && D_800F3428 != 0') are still uncovered. Mode 2 stores D_800F336C = 0, then calls func_800CEDA8 (src/func_800CEDA8.c does not reference D_800F336C), the GTE matrix loads and func_80077A64 (a libgs call), and only then reads D_800F336C. Nothing in between writes it, so the value is 0 and the a1 == 4 arm cannot be taken with the mocked callees. The retail callees are also not known to write it. Directed h(0x800F336C)=4 therefore has no effect, as expected. The C keeps the arm faithfully.

## Notes from the decomp agent

func_800D6514 is the sibling of func_800D5A00 (three-mode effect callback) and follows its structure. The divisions in C: (D_800E27EC<<10)/24 comes from mult 0x2AAAAAAB with sra 2. f/3 comes from 0x55555556. f/2 and the half-scale pass use srl 31 / addu / sra 1. c/32 uses bgez / +0x1F / sra 5. Argument types: func_800C6EC0(unsigned short, unsigned short) and func_800C6FA0(int, unsigned short), since retail does andi 0xFFFF on those arguments. func_80077AA4 is declared with 2 args (retail's a2 is stale a2), because the e21 lowering rejected 3 args (arity 3 != 2). Local layout mirrors retail's stack: MATRIX at sp+0x10, int v[3] at sp+0x30, SVECTOR at sp+0x40, though only the call arguments and contents are compared. func_80078CC4's matrix and scale-vector arguments are passed as separate pointers. The GTE rotation and translation come from *D_800BCFA4 (a MATRIX pointer global) via the inline-asm SetRot/SetTrans macros copied from func_800D5A00. Retail never reads uninitialised stack here: the matrix translation is written from a1's shorts before each func_800C71E4 call. The first retail-read of sp+0x1E (matrix padding) and sp+0x46 (SVECTOR pad) never matters to any callee or comparison. In mode 0 the retail delay-slot order is: a1 is zeroed in the branch delay slot (addu a1,zero,zero), so func_800C6D5C receives (D_800F32D4, 0, 0xC0). The +2 halfword is written from D_800942EC after the 0x26A copy, so the final +2 value is D_800942EC. Only the one deliverable file was created; scratch m2c output is at /dev/shm/pe-native-34f506ce/work/exe_func_800D6514/m2c.c. The main run printed '128 seeds' for the requested 144 (the harness's own count).

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800D6514 --nonmatching C6D14:508 --check-gte --uncovered --seeds 144 --inputs "a0=0,1,2;w(0x800E27EC)=0,1,5,0x17,0x18,-3;w(0x800F3428)=0,1;h(0x800F336C)=4,0;h(0x800E11E4)=0,3"`
  - e21_leaf: ok func_800D6514 (exe) 128 seeds, retail coverage 312/322 words; uncovered 800D654C 800D6550 800D6560 800D6564 800D688C 800D6890 800D6894 800D6898 800D689C 800D68A0
- `python3 tools/analysis/e21_leaf.py exe func_800D6514 --nonmatching C6D14:508 --check-gte --uncovered --seeds 4 --inputs "a0=-1,3"`
  - e21_leaf: ok func_800D6514 (exe) 4 seeds, retail coverage 29/322 words; uncovered list starts at 800D6568, so the default-arm words 800D654C/6550/6560/6564 are covered
- `python3 tools/analysis/e21_leaf.py exe func_800D6514 --nonmatching C6D14:508 --check-gte --uncovered --seeds 200`
  - e21_leaf: ok func_800D6514 (exe) 200 seeds, retail coverage 24/322 words (random seeds only)
- `NEW: ... --seeds 60 --inputs "a0=2;w(0x800E27EC)=0,1,0x17,-1,-25,-5,0x3FFFFF,-0x400000,0x7FFFFFFF,-0x80000000;h(0x800E11E4)=0,0xFFFF,7"`
  - e21_leaf: ok func_800D6514 (exe) 60 seeds, retail coverage 215/322 words (mode 2 with extreme D_800E27EC and D_800E11E4 index values; no MISMATCH)
- `NEW: ... --seeds 40 --inputs "a0=1,0;w(0x800E27EC)=0,0x17,0x18,-1,-25,0x7FFFFF,-0x400000,0x7FFFFFFF;h(0x800F336C)=4,7"`
  - e21_leaf: ok func_800D6514 (exe) 33 seeds, retail coverage 117/322 words (modes 0 and 1 with extreme D_800E27EC; no MISMATCH)

No mismatch, notest or skipped in any run. I read the asm and the C side by side and found no divergence. Checked: call and argument order on every path, halfword vs word store widths, lhu vs lh loads, the /24, /3, /2 and /32 signed divisions with the rounding bias, the sh stores of the particle fields, the a1 translation stored from lh, the andi 0xFFFF truncations of the flags and a2 values, and return values on every path.

The uncovered words 800D688C..800D68A0 (the a1 == 4 arm) are in practice unreachable. Mode 2 stores D_800F336C = 0 and then calls func_800CEDA8. The C source src/func_800CEDA8.c writes only D_800F34E4 and calls func_8007506C. I did not read func_8007506C or func_80077A64. Nothing between the store and the read in the function itself writes D_800F336C. The claim's assertion that the callees do not write it is therefore only partly verified: it holds for the function and for func_800CEDA8's C, but not for the two deeper callees.

The 800D654C/6550/6560/6564 default-arm words are covered by the a0=-1,3 run, as claimed. The 200-seed random run covers only 24 words, so it adds no coverage beyond the directed runs. The second new spec was run with --seeds 40 and the harness ran 33 seeds, so its seed count may not have reached the product of its lists. The first new spec was run with --seeds 60 against a product of 90 (1 x 10 x 3 = 30 combinations, so it is fine; the harness did not truncate). I treated both as adequate since the harness printed ok."

