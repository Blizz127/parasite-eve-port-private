# room_m0432i func_8019702C (NON_MATCHING)

Blob offset 0x8044, 0x510 bytes, VMA 0x8019702C. The span is `asm` in `configs/USA/overlays/room_m0432i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0432i/func_8019702C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0432i — func_8019702C, blob offset 0x8044, 0x510 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect-object callback, read from the retail disassembly; same family as
 * func_801962FC / func_801957CC (frame counter at +0x12, kind at +0x10).
 * cmd 1 (step): kind 0 grows +0xE by 0x20 per frame (done at 0x20 frames);
 *   kind 1 waits for D_80199504 != 0 (frame counter still counts);
 *   kind 2 moves +8 by 8 and +0xA by 0xC per frame (done at 0x10 frames).
 * cmd 2 (draw): kind 0 places the sprite from func_800CF844 around a vector
 *   scaled by the sine of frame << 5, seeds the D_800F3368 draw block and
 *   draws with func_800CEE20 (size 1.5 * cosine, colour via func_800CF3AC);
 *   kind 1 loads the camera rotation/translation into the GTE and draws a
 *   0xA0-wide sprite through func_800D2370 (colour byte 0x96 on even frames,
 *   0x80 on odd frames, x origin 0x80 or 0xC0 by bit 0 of +0xA);
 *   kind 2 loads the camera matrix and draws a ring through func_800D0E88
 *   sized cosine(frame << 6) / 8.
 * The 8-byte seed copied from D_8018F1F4 has its halfword at +4 overwritten
 * with +0xE - frame * 30 (kind 0). The two 4-byte colour blocks D_8018F21C /
 * D_8018F220 are copied to the stack on entry and passed to func_800D0E88.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0432i func_8019702C --check-gte --uncovered --seeds 64
    e21_leaf: ok func_8019702C__5fcda57b_port.c 181 seeds, retail coverage 317/324 words
    e21_leaf: uncovered 801972F0 801972F4 801972F8 801972FC 80197300 80197304 80197308

## Uncovered words

Main run (305/324) leaves 19 words uncovered; the supplementary runs cover all of them except the last group below.
- 801970CC/801970D0, 801970F4/801970F8, 80197108/8019710C: `j .L80197518` + delay slot on the cmd 1 default arms (kind not 0/1/2, i.e. `return 0`). Covered by run 2 (kind 3, -1, 0x7fff, mode 0/3/-1).
- 801971B4/801971B8, 801971C4/801971C8: cmd 2 kind-dispatch default arms (kind not 0/1/2). Covered by run 2.
- 80197150/80197154: cmd 1 kind 1 with D_80199504 != 0, which returns 1. Covered by run 3 (h(0x80199504)=0,1,0x100,-1).
- 801971F8, 80197378: negative-product rounding fix-up (`addiu 0xFFF`) in kind 0 and the odd-frame colour arm in kind 1. Covered in the 305-word run, listed as uncovered only in run 2.
- 801972F0..80197308 (7 words): `lui/lw D_800F3428; beqz; j; addiu a1,0xA`, the `D_800F336C == 4 && D_800F3428 != 0` clut +0xA arm. UNREACHABLE: D_800F336C is stored 3 a few instructions earlier and the only call in between is func_800CF3AC, which does not write it, so `a0 == 4` is never true. The directed h(0x800F336C)=4 term is overwritten by the store, so the arm cannot be driven.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0432i func_8019702C --nonmatching 8044:510 --check-gte --uncovered --seeds 60 --inputs "a0=1,2;h(a1+0x10)=0,1,2;h(a1+0x12)=0,5,0x1f,0x7fff,-3,0x20;h(a1+0xE)=0,0x7fe0;h(a1+0xA)=0,1,0x7fff;h(0x800F336C)=4,0;w(0x800F3428)=1,0;h(0x80199504)=0,1"`
  - e21_leaf: ok func_8019702C__5fcda57b_port.c 51 seeds, retail coverage 305/324 words; uncovered 801970CC 801970D0 801970F4 801970F8 80197108 8019710C 80197150 80197154 801971B4 801971B8 801971C4 801971C8 801972F0 801972F4 801972F8 801972FC 80197300 80197304 80197308 (identical to claim)
- `python3 tools/analysis/e21_leaf.py room_m0432i func_8019702C --nonmatching 8044:510 --check-gte --uncovered --seeds 800 --inputs "a0=1,2;h(a1+0x10)=0,1,2;h(a1+0x12)=0,1,0x1e,0x1f,0x20,-1,0x7fff,0x8000;h(a1+0xE)=0,1,-1,0x7fff,0x8000;h(a1+0xA)=0,-1,0x7fff,0x8000;h(a1+0x6)=0,1,-1"`
  - e21_leaf: ok func_8019702C__5fcda57b_port.c 727 seeds, retail coverage 304/324 words; uncovered 801970CC 801970D0 801970F4 801970F8 80197108 8019710C 80197150 80197154 801971B4 801971B8 801971C4 801971C8 801971F8 801972F0..80197308
- `python3 tools/analysis/e21_leaf.py room_m0432i func_8019702C --nonmatching 8044:510 --check-gte --uncovered --seeds 600 --inputs "a0=1,2,0,3;h(a1+0x10)=0,1,2,3;h(a1+0x12)=1,2,3,0x10,0x1f,0x20;h(a1+0xE)=0x7fe0,0xffe0,-1,5;h(a1+0xA)=0,7,-7;h(a1+0x8)=0,1,-1,0x100;w(0x800F3428)=1,0;h(0x800F336C)=4,3"`
  - e21_leaf: ok func_8019702C__5fcda57b_port.c 581 seeds, retail coverage 311/324 words; uncovered 801970F4 801970F8 80197150 80197154 801971B4 801971B8 801972F0 801972F4 801972F8 801972FC 80197300 80197304 80197308

No MISMATCH, notest or skipped in any run. The default arms and the D_80199504 arm that the main run leaves uncovered are all cases of mode/kind or D_80199504 values the claim says are covered by its other runs. I did not re-run those runs; this is by the claim only. My cmd 1 kind 3 inputs did not reach 801970F4/F8, but the claim's -1/0x7fff kinds are not contradicted. 801971F8 (negative-product rounding) was covered in my third run, consistent with the claim.\n\nThe 7-word 801972F0..80197308 arm is unreachable. At 801972A4 the asm stores 3 to D_800F336C (sh). The only call before the reload at 801972D0 is func_800CF3AC, which takes (D_8019946C, &col, frame) and, per src/func_800CF3AC.c, writes only through its header pointer and its out argument. It does not write D_800F336C. So the bne at 801972E8 (a0 != 4) is always taken. The claim holds as far as static reasoning goes. The remaining caveat is that the callee's data pointer D_8019946C is room-local, and I did not prove it can never alias D_800F336C.\n\nThe harness treats func_800CF3AC as a stub (out=800CF3AC:1), so callee-driven effects on this word are not exercised either way. The first of my two runs had its --seeds capped at 727 of the 1440-point product, and the second at 581."]

