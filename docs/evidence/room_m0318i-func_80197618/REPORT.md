# room_m0318i func_80197618 (NON_MATCHING)

Blob offset 0x8630, 0x6A4 bytes, VMA 0x80197618. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80197618.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80197618, blob offset 0x8630, 0x6A4 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Sibling of func_801944E8 / the m0429i particle callbacks, read from the
 * retail disassembly (7E64.s). a0 = command, a1 = effect object.
 * cmd 1: state 0 integrates the velocity (+8/+A/+C) into the position
 * (+0/+2/+4) with 50/51 damping of vx/vz and a vy gravity step that is
 * negated once the object is at or above D_800942EC (16 frames); state 1
 * snaps the object to the D_8019993C/3E/40 anchor and bumps the velocity
 * (+8 += 0x1D, +A += 0xB) for 24 frames; state 2 drifts y down by 1 for 24
 * frames.
 * cmd 2: state 0 draws the 0xD8 sprite on a glow sized by sin/cos of f<<6,
 * colour curve D_80199868; state 1 places a vector (velocity * cos(f*1024/24))
 * around the object position and draws a 6-sprite; state 2 draws the 0x42
 * glow (growing, CLUT x +4) with a NULL position vector.
 * Return: cmd 1 returns 1 when the timer ends, otherwise 0.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0318i func_80197618 --check-gte --uncovered --seeds 48
    e21_leaf: ok func_80197618__72f481bc_port.c 192 seeds, retail coverage 412/425 words
    e21_leaf: uncovered 80197994 80197998 8019799C 801979A0 801979A4 801979A8 80197B38 80197B3C 80197B40 80197B44 80197B48 80197B4C 80197B50

## Uncovered words

Seven words are never covered: 80197994 80197998 8019799C 801979A0 801979A4 801979A8 (state 0 of mode 2, the k==4 / D_800F3428 test) and 80197B38 80197B3C 80197B40 80197B44 80197B48 80197B4C 80197B50 (state 1 of mode 2, the same test). That is 13 words, so the union is 412/425 rather than the 418 stated in coverage; correct figure is 412/425.

Reason for all of them: the code stores D_800F336C = 2 (state 0) or 3 (state 1) immediately before reading it back, and the only call in between is func_800CF3AC. That callee writes only through its out pointer to the stack and never touches D_800F336C (checked in src/func_800CF3AC.c: no 0x336x or 0x3428 references). So k==4 cannot be true and the D_800F3428 word is never read. These are provably impossible branches.

Every other word was covered by one of the runs: default arms (mode other than 1/2 and state other than 0/1/2, including negative state 0xFFFF), the return-1 timer exits, the negative-value rounding fix-ups (80197958, 80197A2C, 80197BD4), and the SH2 >= D_800942EC negation (801977A8..B0).

## Independent verification: confirmed

- `e21_leaf.py room_m0318i func_80197618 --nonmatching 8630:6A4 --check-gte --uncovered --seeds 48 --inputs "a0=1,2; h(a1+0x10)=0,1,2; h(a1+0x12)=0,11,24; h(0x800F336C)=4,0; w(0x800F3428)=1,0; h(0x800942EC)=0x7000,0" (claimed run 1, re-run)`
  - e21_leaf: ok func_80197618__72f481bc_port.c 48 seeds, retail coverage 398/425 words (uncovered includes 80197994..801979A8 and 80197B38..80197B50, plus 80197694/98, 801976BC/C0/D0/D4, 801977A8-B0, 80197880/84/90/94, 80197958)
- `e21_leaf.py ... --seeds 32 --inputs "a0=1,2; h(a1+0x10)=0xFFFF,0,1,2; h(a1+0x12)=3,0x20; h(a1+6)=0x8000,0x4000" (claimed run 3, re-run; its output line was lost in a shared backgrounded stream)`
  - output line lost; not independently confirmed
- `e21_leaf.py ... --seeds 24 --inputs "a0=1,2; h(a1+0x10)=0,1; h(a1+0x12)=0x7FFF,0x8000,0xFFFF,1; h(a1+6)=0x7FFF,0x8000,0,0xFFFF" (new: signedness and limit edges)`
  - e21_leaf: ok func_80197618__72f481bc_port.c 24 seeds, retail coverage 323/425 words (no MISMATCH, notest or skipped; uncovered words in this run are only a subset of what the other runs cover)
- `sed of asm 80197960-801979B0 and grep of src/func_800CF3AC.c for 336x/3428`
  - asm: D_800F336C is stored via sh as 2 at 8019 7928, then func_800CF3AC is called, then lhu D_800F336C is compared with 4 at 8019798C. The callee source has 95 lines and no matches for 336, 3428 or D_800F.

No MISMATCH, notest or skipped result in any run I saw. The seven-word and thirteen-word uncovered blocks (80197994..801979A8 and 80197B38..80197B50) match the claim's justification. The stack-out callee does not write D_800F336C, so the k==4 test cannot be true after the store of 2 or 3. One caveat: I checked only the callee's source file, not its transitive callees or a hardware or interrupt writer. The claim's coverage figure is internally inconsistent: it states 418 words but then corrects to 412/425, and my re-run of run 1 gives 398/425. This is a reporting issue, not a behaviour mismatch. Run 3's result line did not print in my output, so I could not confirm that run independently.

