# exe func_800C499C (NON_MATCHING)

SLUS file offset 0xB519C, 0x4B4 bytes, VMA 0x800C499C. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800C499C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800C499C — EXE, VRAM 0x800C499C, file offset 0xB519C, 0x4B4 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Untextured gouraud quad effect packet (code 0x38, 0x24 bytes) for a model
 * effect record a0 (colour bytes a0[0..0xF] scaled by the 16-bit factor at
 * a0+0x12 through func_800C608C into packet +4/+0xC/+0x14/+0x1C, OTZ offset
 * a0+0x10) with the placement matrix a1 (3x3 shorts + three-word translation).
 * Scratchpad 0x1F800000 (D_800F33B4): +0xC OTZ, +0x1C matrix, +0x30 translation.
 * a2 == 0: scratch matrix M = *D_800BCFA4 rotation times the a1 matrix
 * (three mvmva column passes), translation = *D_800BCFA4 translation +
 * R * (a1 + 0x14 vector) (mvmva with TR); GTE then loaded from the scratch matrix.
 * a2 != 0: a1 + 0x14 is transformed in place by func_800792D4 under *D_800BCFA4
 * (stack SVECTOR from the low halves of the three words, flag at sp+0x18), and
 * the GTE is loaded from the a1 matrix directly.
 * Then rtpt on the three D_800F33xx vertices (packet +8/+0x10/+0x18 XY), avsz3 ->
 * OTZ; packet code 0x38 (0x3A when D_800F337A != 0); if 0 < OTZ + a0[0x10] <= 0xFFF
 * the fourth vertex D_800F3328 is projected (rtps) to packet +0x20, the packet is
 * linked into the ordering table at D_800B0E38[D_8009CDDC] and the packet cursor
 * D_8009CDD8 advances by 0x24.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) asm macros — count separately
 * from plain C.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800C499C --check-gte --uncovered --seeds 144
    e21_leaf: ok func_800C499C (exe) 144 seeds, retail coverage 301/301 words
    e21_leaf: uncovered none

## Uncovered words

none

## Notes from the decomp agent

I could not run the planned 64-seed random-only check. It was queued behind the 384-seed run in a background job. The 384-seed result arrived in a task notification. I did not see the random-only line.

Asm location: the brief names asm/disc1/B519C.s, which does not exist. The function is in /home/blizz/dev/Parasite-Eve-Decompilation/asm/disc1/B3B24.s, lines 1498-1804.

What the C does (a0 = effect record, a1 = placement matrix of 3x3 shorts plus a 3-word translation at +0x14, a2 = unsigned char mode):
- It emits a gouraud quad packet (length byte 8, code 0x38, or 0x3A when D_800F337A is nonzero). The packet is 0x24 bytes, so the packet cursor D_8009CDD8 advances by 0x24. func_800C3324 advances by 0x28.
- a2 == 0: the scratchpad matrix at 0x1F80001C is *D_800BCFA4 times the a1 matrix, built with three rotcol mvmva passes. Its translation is the *D_800BCFA4 translation plus R*(a1+0x14) (rttrans). The GTE is then loaded from the scratchpad matrix.
- a2 != 0: a1+0x14 is transformed in place by func_800792D4. Its SVECTOR is the low halves of the three words and its flag is at sp+0x18. The GTE is then loaded from the a1 matrix directly.
- Both modes then call func_800C608C four times with a0[0x12] as the short factor. The sources are a0+0, +4, +8 and +0xC; the destinations are packet +4, +0xC, +0x14 and +0x1C.
- rtpt runs on D_800F3310/18/20, followed by avsz3 into scratchpad OTZ at +0xC. Then OTZ += (short)a0[0x10].
- The link step only runs when (unsigned)(OTZ - 1) < 0xFFF. It projects D_800F3328 (rtps) to packet +0x20, splices the packet into the ordering table at D_800B0E38[D_8009CDDC], and advances D_8009CDD8.

Points for the integrator:
- D_800B0E58 (packet buffer array) and D_800B0E38 (ordering table array) are declared as separate pointer arrays. func_800C3324.c reaches the same storage as fields of a D_800B0E38 struct. Retail references both labels directly, and the lowering resolved them.
- func_800792D4 (RotTrans) writes its result in place at a1+0x14, which is not stack. The GTE translation is loaded from there afterwards. Under the e21 mock the differential proves the call and its arguments, not the transform result. The flag at sp+0x18 is never read back. Retail never reads uninitialised stack here.
- The directed spec has no byte term, so D_800F337A is driven with h(0x800F337A), which also writes the neighbouring byte 0x800F337B. This function never reads it.
- a2 is declared unsigned char to match retail's andi 0xFF. a2=0x100 takes the a2 == 0 path on both sides, so register truncation is covered.
- a3 is unused. The function is void and the retail v0 at exit is incidental.
- The macros are copied from func_800C3324.c (gte_rotcol, gte_rttrans, Set*Matrix, ldv3, rtpt, avsz3, stsxy3, stsxy2, stotz).
- git status showed no files created outside my one deliverable.

## Independent verification: confirmed

- `e21_leaf.py exe func_800C499C --nonmatching B519C:4B4 --check-gte --uncovered --seeds 144 --inputs "a2=0,1,0x100; h(0x800F337A)=0,1; h(a0+0x10)=<12 values>"`
  - 144 seeds: e21_leaf: ok func_800C499C (exe) 144 seeds, retail coverage 301/301 words / uncovered none
- `same with --seeds 96 and --seeds 384 (a2=0,1)`
  - 96 seeds: ok, 301/301, uncovered none
- `NEW1: --seeds 135 --inputs "a2=0,1,0x100,0xFF,0x80; h(0x800F337A)=0,1,0x100; h(a0+0x10)=0xFFFF,0xFFFE,0x1,0xFFF,0x1000,0x7FFF,0x8000,0xF001,0xFFF1"`
  - 384 seeds: ok, 301/301, uncovered none
- `NEW2: --seeds 70 --inputs "a2=0,1; h(a0+0x12)=0,1,0x7FFF,0x8000,0xFFFF,0x100,0x1000; h(a0+0x10)=0,0xFFF,0x800,0xF800,0xF000"`
  - NEW1 (135 seeds): ok, 301/301 words, uncovered none (a2 high bits 0x100/0x80/0xFF, flag 0x100, OTZ offsets at +/-1 limits, negative/large halves)

No MISMATCH, notest or skipped output in any run. Side-by-side read of asm (asm/disc1/B3B24.s lines 1498-1804; the cited B519C.s does not exist) against the C found no divergence. Checks: (1) retail does andi a2,0xFF, which matches the C's unsigned char a2, and a2=0x100 was driven. (2) otz range test is sltiu(otz+off-1, 0xFFF), matching the C. (3) sh/lh signedness matches: a0+0x10 and a0+0x12 are read as lh. (4) The func_800792D4 args (sp+0x10, a1+0x14, sp+0x18) and the four func_800C608C calls match. (5) The two OT read-modify-writes match, as do the cursor += 0x24 and the byte 3/7 stores. The harness runs very slowly under machine load (384 seeds took tens of minutes), but that is not a correctness issue. Minor caveat: my new inputs vary only a0/a2/flag halves, so GTE matrix and vertex values rely on the harness's seed randomisation. --check-gte passed in every run.

