# exe func_800DE0A8 (NON_MATCHING)

SLUS file offset 0xCE8A8, 0x700 bytes, VMA 0x800DE0A8. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800DE0A8.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800DE0A8 -- EXE, VRAM 0x800DE0A8, file offset 0xCE8A8, 0x700 bytes.
 * NON_MATCHING behaviour C (proven by the e21 differential; not a byte match).
 * Three-mode effect callback (a0 = mode, a1 = effect record):
 *   mode 0: func_800CE870(D_8009D254, 1, record + 8); record +0/+2/+4 = words
 *           0x274/0x278/0x27C of the object at D_8009D254->+0x238;
 *           func_800C6D5C(D_800F3474, 0, 0); returns 0.
 *   mode 1: returns D_800E27EC >= 0x19.
 *   mode 2: f = D_800E27EC. f == 0: func_800D1AE0 with 0xA0A0A0; 1 <= f <= 5:
 *           func_800D1AE0 with 0xA0FFFF at y = 0x80 - ((f - 1) << 5). f < 0x11:
 *           draw the object's matrix scaled by {sin/3, sin/3, cos} through the
 *           func_800C70xx/71E4 draw path (two passes). f < 0x19: colour curve
 *           D_800E20CC, func_800D004C sprite (f < 0x11: func_800D0728 too) and
 *           func_800CEE20 glow. Returns 0.
 * The colour buffer at sp+0x40 is read back after func_800CF3AC (out=3 bytes);
 * its fourth byte is never written -- retail never reads it.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800DE0A8 --check-gte --uncovered --seeds 50
    e21_leaf: ok func_800DE0A8 (exe) 50 seeds, retail coverage 429/448 words
    e21_leaf: uncovered 800DE3F0 800DE3F4 800DE3F8 800DE3FC 800DE400 800DE404 800DE4F8 800DE4FC 800DE500 800DE504 800DE508 800DE50C 800DE720 800DE724 800DE728 800DE72C 800DE730 800DE734 800DE738

## Uncovered words

800DE3F0-800DE404 (6 words): the 'D_800F336C == 4 && D_800F3428' +4 arm after the first func_80077AA4 setup in the f<0x11 draw block; 800DE4F8-800DE50C (6 words): same arm in the second pass; 800DE720-800DE738 (7 words): same test with +7 in the f<0x19 glow block (the +3 arm at 800DE73C is covered). In all three, the function itself stores D_800F336C = 1 and then only mocked callees run before D_800F336C is reloaded and compared with 4, so the ==4 branch is provably unreachable from entry state (only a real callee writing D_800F336C could reach it). All other words, including mode 0/1, mode -1, mode 3, f==0, f 1..5, f<0x11, f<0x19, f>=0x19 and the negative-quotient fixup, are covered.

## Notes from the decomp agent

Three-mode effect callback (mode 0 init, mode 1 done-test f>=0x19, mode 2 draw). Retail frame layout is reproduced with one local struct { int vec[3]; int pad0; B4 buf; int pad1; MATRIX m; } because callees receive &buf (sp+0x40) and the differential compares the stack block behind the pointer; with separate locals the e21 run mismatched on the func_800D0728 stack arg. The 4 bytes at sp+0x44 (pad1) and buf byte 3 are never written by retail (retail passes them as-is; reads of buf[0..2] come after func_800CF3AC out=3). The matrix local is copied from *(D_8009D254+0x238) (8 words), scaled by func_80078CC4 with vec {sin/3, sin/3, cos} then {0x1800,0x1800,0x1000}, and its translation is replaced by words 0x274/0x278/0x27C. Constant divisions: 0x55555556 -> /3, 0x2AAAAAAB+sra2 -> /24, bgez/addiu 0x1F/sra 5 -> /32. No retail stack-never-written reads beyond the pad word and buf byte 3 noted. Only the e21 run was done; advisor was not consulted. The first run used --seeds 8 with no directed inputs (26/448) before adding the spec.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800DE0A8 --nonmatching CE8A8:700 --check-gte --uncovered --seeds 50 --inputs "a0=0,1,2,3,-1;a1=0x80130000;w(0x8009D254)=0x80110000;w(0x80110238)=0x80110400;w(0x800BCFA4)=0x80111000;w(0x800E27EC)=0,1,2,5,6,0x10,0x11,0x18,0x19,0xFFFFFFFF"`
  - e21_leaf: ok func_800DE0A8 (exe) 50 seeds, retail coverage 429/448 words | uncovered 800DE3F0..800DE404, 800DE4F8..800DE50C, 800DE720..800DE738 (19 words), exactly as claimed
- `python3 tools/analysis/e21_leaf.py exe func_800DE0A8 --nonmatching CE8A8:700 --check-gte --uncovered --seeds 200 --inputs "a0=2;a1=0x80130000,0x80130010;w(0x8009D254)=0x80110000;w(0x80110238)=0x80110400;w(0x800BCFA4)=0x80111000;w(0x800E27EC)=0..26 each,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFF0"`
  - e21_leaf: ok func_800DE0A8 (exe) 200 seeds, retail coverage 389/448 words (f swept 0..26 plus INT_MAX/INT_MIN/-2/-16, a1 two values, mode 2 only; extra uncovered words are only the mode 0/1/-1 and f==0/negative-fixup words not targeted by this spec, plus the same three 4/7 arms)
- `python3 tools/analysis/e21_leaf.py exe func_800DE0A8 --nonmatching CE8A8:700 --check-gte --uncovered --seeds 100 --inputs "a0=-2,4,0x10000,0x80000000,0,1,2;a1=0,0x80130000;w(0x8009D254)=0x80110000;w(0x80110238)=0x80110400;w(0x800BCFA4)=0x80111000;w(0x800E27EC)=3,0x11,0x18,0xFFFFFFFF"`
  - e21_leaf: ok func_800DE0A8 (exe) 100 seeds, retail coverage 415/448 words (mode values -2,4,0x10000,0x80000000,0,1,2; a1=0 and non-zero; uncovered subset of claimed ones plus a few f-specific words from the small f list)

No MISMATCH, notest or skipped in any run. Uncovered-word check against asm CE8A8.s: D_800F336C is stored by this function (sh at 800DE330 and 800DE6D8) and then reloaded (lhu at 800DE3C0, 800DE4C4, 800DE6F0) before each ==4 compare. Between store and reload only func_800CEDA8, func_80077A64, func_80078CC4 and similar callees run. A grep of asm/disc1 found D_800F336C written only in other effect callbacks (C8B88.s, CC858.s), not in those callees. So the ==4 arms at 800DE3F0-404, 800DE4F8-50C and 800DE720-738 are unreachable from entry state, as claimed. The mode 4/0x10000/0x80000000 values only return 0 (mode != 2) and match. Residual caveat: the unreachability argument assumes the mocked callees do not write D_800F336C. I did not disassemble every real callee body (func_800CEDA8 in particular), though no asm file other than effect callbacks references that symbol. Not evidence of a defect.

