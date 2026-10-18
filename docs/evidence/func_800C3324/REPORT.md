# exe func_800C3324 (NON_MATCHING)

SLUS file offset 0xB3B24, 0x7E0 bytes, VMA 0x800C3324. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800C3324.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800C3324 — EXE, VRAM 0x800C3324, file offset 0xB3B24, 0x7E0 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Textured/shaded quad effect packet (code 0x2C, 0x28 bytes) for a model
 * effect record a0: builds the packet colour (a0[0x20..0x22], optionally scaled by
 * the 16-bit factor at a0+0x28 unless it is 0x80, clamped to 0x7FFF, >> 7), a scratchpad
 * matrix pair (0x1F80001C = copy of *D_800BCFA4, translation advanced by the
 * a0 offset vector through the rotation rows; 0x1F80003C = matrix built by
 * func_80079754/func_80078CC4 from the a0+8 angles and a0+0x10 scale), the
 * concatenation M1 = M1 * M2 and translation M1.t = clamp16(M1.t + R*M2.t) via
 * two GTE mvmva passes, an RTPT of the three D_800F33xx vertices (corner screen
 * coordinates -> packet +8/+0x10/+0x18, OTZ -> scratch +0xC, UV offsets from
 * the scratch bytes +0x10/+0x11 and D_800F345C/D), an RTPS of D_800F3328 ->
 * packet +0x20, the func_80077AA4 CLUT/tpage word -> packet +0xE, and links the
 * packet into the ordering table at (OTZ + a0->0x26).
 * Scratchpad layout used: +0x10..+0x13 bytes, +0x14 word, +0xC OTZ,
 * +0x1C M1 (rot 0x1C..0x2D, trans 0x30..0x38), +0x3C M2 (rot, trans +0x50).
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) asm macros — count separately
 * from plain C.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800C3324 --check-gte --uncovered --seeds 288
    e21_leaf: ok func_800C3324 (exe) 288 seeds, retail coverage 504/504 words
    e21_leaf: uncovered none

## Uncovered words

none. Random seeds alone left 17 words uncovered: the 0x7FFF clamp stores, the a0+0x28 == 0x80 copy arm, the three negative-sum +0xFFF fix-ups, and the D_800F337A != 0 arm (ori 2). The directed spec covers all of them.

## Notes from the decomp agent

Passed on the first compile. The deliverable is the only file I created or edited; scratch output went to /dev/shm/pe-native-34f506ce/work/exe_func_800C3324. git status also showed changes to nonmatching.yaml, ACTIVE_HANDOFF.md and docs/port/NATIVE_COMPLETION.md plus other untracked room dirs. None of those are mine.

What the function does:
- It is a GTE effect-quad packet builder for effect record a0, written as a code 0x2C, 0x28-byte packet.
- It uses scratchpad 0x1F800000 and the pointer global D_800F33B4. The scratch layout is +0x10..0x13 bytes, +0x14 word, +0xC OTZ, M1 at +0x1C (rotation 0x1C..0x2D, translation 0x30..0x38) and M2 at +0x3C (rotation, translation at +0x50).
- Colour: a0[0x20..0x22] is scaled by the halfword a0+0x28 and clamped to 0x7FFF, then >> 7, unless a0+0x28 == 0x80, in which case it is copied unchanged.
- It calls func_80079754(a0+8, S+0x3C), then func_80078CC4(S+0x3C, a0+0x10). It zeroes the M2 translation words at +0x50, +0x54 and +0x58, and copies 32 bytes (8 words) from *D_800BCFA4 to S+0x1C.
- M1.t += (M1 rotation rows . a0 vector at a0+0..4) / 4096, as plain signed integer C.
- GTE: it computes M1 = M1*M2 column by column using mvmva (macro gte_rotcol), then M1.t = clamp16(M1.t + R*M2.t) using mvmva with cv=TR and lm=0 (gte_rttrans). The 16-bit clamp is intentional retail behaviour and is done by the GTE.
- It then reloads rotation and translation, runs RTPT on D_800F3310/3318/3320, and writes the UV and flag bytes. It runs AVSZ3, stores OTZ to scratch +0xC and SXY0..2 to packet +8/+0x10/+0x18, runs RTPS on D_800F3328, and stores SXY2 to packet +0x20. D_800F337A sets or clears bit 1 of packet +7.
- It calls func_80077AA4(D_800F341C + S[0x12], D_800F341E + S[0x13]) and stores the result as a halfword at packet +0xE. It then links the packet into ordering[D_8009CDDC][OTZ + (short)a0[0x26]] and does D_8009CDD8 += 0x28.

Things to know:
- func_80077AA4 is declared argument-less, `extern int func_80077AA4();`. Retail passes only a0 and a1, with a2 and a3 as leftover registers. The differential's call-argument comparison passed with this declaration, so it is not logging a2 or a3 for this call.
- The three callees (func_80079754, func_80078CC4, func_80077AA4) are written as `extern void func_80079754(); extern void func_80078CC4(); extern int func_80077AA4();`, with no prototypes.
- Retail reads no stack it never wrote, so there is nothing to disclose on that point. It writes scratchpad and main RAM only.
- Retail reloads D_800F33B4 and D_8009CDD8 repeatedly. The C mirrors that with `S = D_800F33B4` re-reads and a final `D_8009CDD8 += 0x28`. The retail order is also kept: the OT entry is read first and the final OT-entry store comes after the D_8009CDD8 store.
- The whole scratchpad is excluded from the differential's RAM comparison. Its contents are checked indirectly through the GTE registers and the packet bytes.
- I did not edit configs/, nonmatching.yaml or docs. The orchestrator should add `B3B24:7E0` for func_800C3324 and the inputs_spec above to nonmatching.yaml.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800C3324 --nonmatching B3B24:7E0 --check-gte --uncovered --seeds 288 --inputs "h(a0+0x28)=0x80,0x100,0x4000,0; w(a0+0x20)=0xFFFFFFFF,0x01010101,0x00FF0000; h(0x800F337A)=0,1; h(a0+0)=0x5000,0xB000; h(a0+2)=0x4000,0xC000"`
  - e21_leaf: ok func_800C3324 (exe) 288 seeds, retail coverage 504/504 words / e21_leaf: uncovered none (reproduces the claim; the earlier 40-seed and 96-seed runs were not re-run)
- `python3 tools/analysis/e21_leaf.py exe func_800C3324 --nonmatching B3B24:7E0 --check-gte --uncovered --seeds 288 --inputs "h(a0+0x26)=0x7FFF,0x8000,0,1; h(a0+4)=0x8000,0x7FFF,0x4000; h(a0+2)=0x8000,0x7FFF; h(a0+0)=0x8000,0x7FFF; h(a0+0x28)=0xFFFF,0x81; w(a0+0x20)=0xFFFFFFFF,0x00010001"`
  - e21_leaf: ok func_800C3324 (exe) 216 seeds, retail coverage 494/504 words / e21_leaf: uncovered 800C34C8 800C34CC 800C34D0 800C34D4 800C34D8 800C34DC 800C34E0 800C39A8 800C39AC 800C39B0 (spec 2 never sets a0+0x28 == 0x80 or D_800F337A != 0, so those arms are expected to be uncovered here)
- `python3 tools/analysis/e21_leaf.py exe func_800C3324 --nonmatching B3B24:7E0 --check-gte --uncovered --seeds 64 --inputs "h(a0+0x24)=0xFFFF,0x0F0F,0x10F0,0; h(0x800F345C)=0xFFFF,0,0x0101,0xFF00; h(a0+0x28)=0x80,0x81; h(0x800F337A)=0,1"`
  - e21_leaf: ok func_800C3324 (exe) 64 seeds, retail coverage 497/504 words / e21_leaf: uncovered 800C33F4 800C33F8 800C344C 800C34A4 800C35CC 800C362C 800C368C (spec 3 never sets the clamp or negative-sum inputs, so the 0x7FFF stores and +0xFFF fix-ups are expected to be uncovered here)

No MISMATCH, notest or skipped output in any run. I could not refute the claim.

Claimed run reproduced: I re-ran only the 288-seed command, and it gave 504/504 words with nothing uncovered. The 40-seed and 96-seed commands were not re-run.

Asm vs C comparison found no divergence:
- The colour scale uses `mult` on `lbu` and `lhu` values, which are never negative. The `sltu` clamp against 0x7FFF and the `srl 7` match the C's unsigned `v > 0x7FFF`.
- The a0+0x28 == 0x80 arm copies the three bytes unscaled, as the C does.
- The `+0xFFF` fix-up for negative sums followed by `sra 12` equals C `d / 4096`, with signed `lh` operands (SH) and the three products summed in `int`. The a0+0/2/4 offset values 0x8000, 0x7FFF, 0x5000, 0xB000 and 0xC000 all ran clean.
- The packet byte stores (+0xC, +0xD, +0x14, +0x15, +0x1C, +0x1D, +0x24, +0x25) wrap through `sb`. Spec 3 exercised a 0xFF byte at D_800F345C/D, but those bytes are only read through the h() terms, so a 0xFF value in one byte and 0 in the other is covered only partly.
- The OT link uses `lh` at a0+0x26, so a negative offset such as 0x8000 is signed, and the C's SH matches. It was covered by spec 2.
- The `ordering[]` base at 0x800B0E38 plus the index `<<2`, the packets base at 0x800B0E58 (`D_800B0E58`) plus `D_8009CDD8`, and the +0x28 advance of D_8009CDD8 are all present in the C.
- func_80079754 and func_80078CC4 are called in the same order with the same arguments.

Caveats, none of them a refutation:
- The `b()` byte-width term is not supported by e21_diff, which accepts only `a<N>`, `h()` and `w()` terms. Byte fields therefore had to be driven through halfword terms.
- In the 288-seed claimed run, the earlier part of the C file's claim that 17 words need a directed spec is consistent with what I saw: spec 2 and spec 3 each left a subset uncovered, and the claimed spec covered all.

