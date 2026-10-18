# room_m0318i func_80192A7C (NON_MATCHING)

Blob offset 0x3A94, 0x324 bytes, VMA 0x80192A7C. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80192A7C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80192A7C, blob offset 0x3A94, 0x324 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, a0 = mode, a1 = effect record, a2 = parameter block.
 *   mode 0: a1 +8 = func_80071A54() (RNG); walk the D_8009D20C list (stop at
 *           the D_8009D254 sentinel / null) for a node whose +0 pointer has
 *           a positive +0x10 and whose bytes +0xD/+0xC equal a2 +0 / +4, then
 *           func_800CE870(node, 0, a1); else printf D_8018F1CC. Seeds a1 +2,
 *           the 0x5F3/0x5F4 params, registers func_80192718 (pool at
 *           D_800F33E0->+8) and func_80192620 (a1 +0xC); returns the sum of
 *           both call results.
 *   mode 1: while D_800E27EC < 0x20 and odd, spawn a particle (+0xC=0xC8,
 *           +2 = a1 +8) and drop a1 +8 by 0x555; returns 0 and latches
 *           a1 +0xC into D_80199900 while D_800E27EC < 0x46, else 1.
 *   mode 2: load the D_800BCFA4[0] rotation/translation matrix into the GTE,
 *           set up the D_800F3368.. draw-state shorts, latch a1 position
 *           into D_801998F8/FA/FC; returns 0.
 * a3 is not an input: retail never sets $a3 before the not-found printf, so the
 * 4th printf argument is whatever $a3 holds after func_80071A54 returns (callee
 * scratch, TTY text only). The C passes only the three defined arguments.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80192A7C --check-gte --uncovered --seeds 96
    e21_leaf: ok func_80192A7C__e068cdb9_port.c 315 seeds, retail coverage 201/201 words
    e21_leaf: uncovered none

## Uncovered words

None in the union of the four runs. Each single run leaves the other modes' words uncovered, which is expected.

## Independent verification: confirmed

- `L='python3 tools/analysis/e21_leaf.py room_m0318i func_80192A7C --nonmatching 3A94:324 --check-gte --uncovered' ; the four claimed runs (--seeds 72/40/16/12 with the claimed --inputs), rerun exactly`
  - claimed run 1: e21_leaf: ok func_80192A7C__e068cdb9_port.c 48 seeds, retail coverage 92/201 words
- `NEW A: $L --seeds 64 --inputs 'a0=0;a1=0x80130000;a2=0x80120000;w(a2+0)=5,0x205;w(a2+4)=7,0x307;w(0x8009D20C)=0x80110000,0;w(0x8009D254)=0x80110200,0x80110000;w(0x80110000)=0x80110100;w(0x80110004)=0x80110200;w(0x80110110)=0,5;h(0x8011000C)=0x0507,0x0705;w(0x80110200)=0x80110300;w(0x80110204)=0;w(0x80110310)=1;h(0x8011020C)=0x0507'`
  - claimed run 2: e21_leaf: ok func_80192A7C__e068cdb9_port.c 35 seeds, retail coverage 54/201 words
- `NEW B: $L --seeds 40 --inputs 'a0=1;a1=0x80130000;w(0x80130008)=0x80000000,0x7FFFFFFF,0x554;w(0x8013000C)=0,0xFFFFFFFF;w(0x800E27EC)=1,0x1F,0x21,0xFFFFFFFF,0x45,0x47'`
  - claimed run 3: e21_leaf: ok func_80192A7C__e068cdb9_port.c 10 seeds, retail coverage 82/201 words
- `NEW C: $L --seeds 16 --inputs 'a0=2;a1=0x80130000;h(0x80130000)=0,0xFFFF,0x8000;h(0x80130002)=0,0x7FFF;h(0x800E11FA)=0,0xFFFF'`
  - claimed run 4: e21_leaf: ok func_80192A7C__e068cdb9_port.c 12 seeds, retail coverage 26/201 words
- `python intersection of the four claimed runs' uncovered word lists`
  - intersection of the four uncovered lists: set() (empty), so the 201/201 union claim reproduces

No MISMATCH, notest or skipped line appeared in any run, and the union of uncovered words is empty, so no reachable word is called unreachable. A side-by-side read of the asm and the C found no divergence. (1) The mode 0 loop order matches: null head check, sentinel compare, +0 null test, +0x10 signed blez, then the lbu 0xD/0xC compares. The key words a2+0 and a2+4 are compared against unsigned bytes, as in retail. (2) In mode 0 the printf is called with a0 only. Retail leaves a1 and a2 holding the key words, which the C passes as args 2 and 3. The $a3 difference is documented in the C header and reaches only TTY text. (3) The mode 0 sh to a1+2 sits in the jal delay slot before func_800D3FD8, and the C stores it before the call, so the order matches. (4) Mode 1 uses signed slti for 0x20 and 0x46, and the C's int compares match. (5) Mode 2 indexes D_800E2850 with an unsigned short (lhu then sll 1), and the C matches. (6) Dispatch: negative a0 and a0>2 return 0, as in the C default arm. Caveats: this is dynamic differential testing, and the GTE inline asm is only checked via --check-gte. My scratch outputs went to /dev/shm/pe-native-34f506ce/work/m0318i_func_80192A7C/. I briefly wrote A.txt through D.txt into the worktree root by mistake and moved them back. I made no other changes, and the C file is untouched."

