# room_m0432i func_80198348 (NON_MATCHING)

Blob offset 0x9360, 0xBFC bytes, VMA 0x80198348. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_80198348.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_80198348, blob offset 0x9360, 0xBFC bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback, modes 0/1/2 on a record a1 (kind +0x10, frame +0x12,
 * position +0/+2/+4), a2 = spawn parameter block; read from the retail
 * disassembly (same family as func_8019702C / func_801978EC).
 *   mode 0: clears kind/frame, plays the D_800B0E64 voice (0x5DA) when it
 *           exists, looks up the two sprite handles D_80199508 / D_8019950C
 *           through func_8006E498 (keys 0xC58C4704 / 0xC5CC4704, each reset
 *           with func_800C6D5C), copies the a2 position and registers the
 *           func_801981B0 child through func_800CE560 (its result is returned).
 *   mode 1: frame +0x12 counts up. On frame 6, if func_800C6B90(a1, a2->8)
 *           hits and the D_800E2368 record has byte +0xD set and the
 *           D_800F32D0 object's flag word reads 0x01xxxxxx, the D_8009D254
 *           state gets bit 0x4000 and the object's flag word is rewritten
 *           (0x21 in bits 24..29) and gets bit 31. While the frame is < 7,
 *           four particles are taken from the func_800CE610 pool at
 *           D_800F33E0[2] and scattered with four func_80071A54 draws.
 *           Returns 1 from frame 0x28 on.
 *   mode 2: loads the camera GTE matrix from D_800BCFA4[0]; frames < 4 flash
 *           the screen white (func_800D1AE0 colour 0xFFFFFF, 0x6E), frames
 *           4..5 and 6..12 fade with buf D_8018F228 / sine; sets the
 *           D_800F3368 draw block and draws a 0x2000 sprite (func_800CEE20),
 *           two glows (func_800D004C / func_800D0728), then two scaled-matrix
 *           sprites through func_800C6EF8..6F4C on D_80199508, and, for frame
 *           < 25, a third on D_8019950C. Always ends by resetting the draw
 *           block; returns 0.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_80198348 --check-gte --uncovered --seeds 168
    e21_leaf: ok func_80198348__12a7f1c4_port.c 643 seeds, retail coverage 741/767 words
    e21_leaf: uncovered 80198660 80198670 80198674 80198678 801986A0 801986B0 801986B4 801986B8 80198ABC 80198AC0 80198AC4 80198AC8 80198ACC 80198AD0 80198C38 80198C3C 80198C40 80198C44 80198C48 80198C4C 80198DEC 80198DF0 80198DF4 80198DF8 80198DFC 80198E00

## Uncovered words

80198660, 80198670-80198678, 801986A0, 801986B0-801986B8 (8 words): the break 7 / break 6 divide-trap guards in the mode 1 particle loop. The divisor is the constant 0x1000 held in $s2, never 0 or -1, so the guards cannot fire. | 80198ABC-80198AD0, 80198C38-80198C4C, 80198DEC-80198E00 (18 words, three arms of 6): the 'D_800F336C == 4 && D_800F3428 != 0 -> +4' clut adjustment. Retail stores 3 to D_800F336C (via sh in the func_80077A64 call setup) and re-reads it with lhu after that call. The only call in between is func_80077A64, which src/func_80077A64.c shows is a pure bit-packer (returns ((tp&3)<<7)|((abr&3)<<5)|... and writes no memory), so the ==4 test can never be true: provably impossible. The C keeps the test for fidelity. The 'h(0x800F336C)=4' spec term cannot reach it either, because the store of 3 overwrites it first.

## Independent verification: confirmed

- `Re-ran the four claimed e21_leaf.py commands exactly as given (seeds 40 / 168 / 160 / 8; mode 0 / 1 / 2 / default arm).`
  - Claimed run 1 (mode 0): e21_leaf: ok func_80198348__12a7f1c4_port.c 40 seeds, retail coverage 106/767 words
- `New spec A: e21_leaf.py room_m0432i func_80198348 --nonmatching 9360:BFC --check-gte --uncovered --seeds 130 --inputs "a0=2;a1=0x80101000;h(a1+0x12)=-1,-2,-7,-0x8000,0x7FFF,0x28,0x27,0x18,0x19,0x1F,0x20,3,4,5,6,13;h(0x800E11FA)=0,1;h(0x800E120A)=0,0x7FFF;w(0x800F3428)=1,0"`
  - Claimed run 2 (mode 1): e21_leaf: ok func_80198348__12a7f1c4_port.c 168 seeds, retail coverage 178/767 words
- `New spec B: same command, --seeds 110 --inputs "a0=1;a1=0x80101000;a2=0x80102000;h(a1+0x12)=-0x8000,0x7FFE,-1,4,5,6,0x26,0x27,0x28;r(0x800C6B90)=0,1;w(0x800E2368)=0x80180400;w(0x8018040C)=0x100,0;w(0x800F32D0)=0x80180000;w(0x80180008)=0x80180100;w(0x80180100)=0x80180200;w(0x80180200)=0x01000000,0x3F000000,0x41000000;w(0x8009D254)=0x80180500;w(0x80180500)=0x80180600;w(0x800F33E0)=0x80150000;r(0x800CE610)=0x80140000;h(a1+0)=0,-1,0x7FFF"`
  - Claimed run 3 (mode 2): e21_leaf: ok func_80198348__12a7f1c4_port.c 131 seeds, retail coverage 550/767 words
- `New spec C: same command, --seeds 8 --inputs "a0=4,0x100,0x10000,0x80000000,-2,-0x80000000;a1=0x80101000;a2=0x80102000"`
  - Claimed run 4 (default arm): e21_leaf: ok func_80198348__12a7f1c4_port.c 8 seeds, retail coverage 59/767 words
- `Read asm lines 13-60, 182-263, 575-625 and 388-400 of asm/overlays/room_m0432i/9360.s, and src/func_80077A64.c, against src/nonmatching/overlays/room_m0432i/func_80198348.c.`
  - New spec A (mode 2 edge frames, negative and limit values): e21_leaf: ok func_80198348__12a7f1c4_port.c 109 seeds, retail coverage 550/767 words

["No refutation found.", "Break guards (80198660, 80198670-78, 801986A0, 801986B0-B8): asm lines 184-186 show $s2 = addiu 0x1000 and $s3 = addiu 0x800, set before the loop at 801985DC and 801985E4. The guards are bnez $s2 and $s2 == -1, so they cannot fire. The mfhi - $s3 arithmetic matches the C expression r % 0x1000 - 0x800.", "clut +4 arms (80198ABC-AD0, 80198C38-C4C, 80198DEC-E00): asm 586-607 shows retail does addiu $v0,0,3 then sh to D_800F336C, then jal func_80077A64, then lhu D_800F336C into $a1, then bne $a1,4. The 3 is stored unconditionally, and src/func_80077A64.c is a pure bit-packer with no memory writes. $a1 therefore always reads 3 and the ==4 branch is unreachable. The justification holds.", "Caveat: spec B asked for 110 seeds against a list product of 324, so it sampled the space rather than covering the product. It still printed ok. Spec A's 130 requested seeds gave 109 survivors after oracle filtering.", "Caveat: the 26 uncovered words are justified by static reading of the asm. No run can exercise them."]

