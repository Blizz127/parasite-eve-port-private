# func_80015DAC — PARKED (lane exeA, 2026-09-27)

- VRAM `0x80015DAC`, file offset `0x65AC`, size `0xB64`
- Best: 277 raw (decompA reported ~110 aligned; draft links 0xB60, one word short)
- Profile tried: -O2 -G8, MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_DISPATCH_FOLD=jtbl_800101B0, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254,D_8009D20C,D_800B0E64,D_800B0CEA,D_800B0DBE,D_800B0DB8,D_800B0DB9,D_800B0DFC,D_800B0DCE
- Best draft: `build/archive/fanout/decompA/drafts/func_80015DAC.c` (not in `src/`)

## Residual

Script-opcode switch (100..410). Not re-attacked in this lane beyond re-measuring:
the decompA draft needs `D_800B0DCE` added to the force-absolute list to link at all. Residuals per
decompA: 300 arg-load order, 352 then-branch null test threaded into the loop exit, 400 loop copies
not coalesced / `lui 0xff` hoist, 406 compare placement.

## Levers tried

(decompA) see `build/archive/fanout/decompA/results.jsonl`.

Measured with `tools/analysis/era_link_check.py <draft> 0x80015DAC 0xB64 <flags>` (profile env in a subshell).

## Round 3 (lane exeA3, 2026-09-27) — 703/729 aligned (lnk 728), draft build/lanes/exeA3/drafts/func_80015DAC.c
Same env as above (decompA draft + D_800B0DCE absolute). Landed:
- case 352: the `*a[2]==0` arm reads `D_8009D254` into `register unsigned char *q asm("$2")`, tests q, then `p = q`
  (retail's `beq $2,$0 / addu $4,$2,$0`); with p itself the test cross-jumps into the loop-exit `if (p == 0)`.
- case 400: `for (i = 0; i < hdr >> 22; i++)` indexing `q[i]` (the bound becomes a hoisted copy like retail's `$8`,
  no `&q->hA` giv).
Residual: case 300 arg-load order (D_800B0E64 load late), case 400 register numbering (i/n/giv $5/$7/$4 vs
retail $6/$8/$7 — retail keeps the giv separate from q, so `lui $4,0xff` fills the `beq` slot), case 406 `la
D_800B0CE9` placed before the `sh` (retail after). Pinning i/h reaches 706 but links 3 words long.

## Round 4 (lane exeA4, 2026-09-27) — 712/729 aligned (lnk 728, 1 short), draft build/lanes/exeA4/drafts/func_80015DAC.c
Landed on the exeA3 draft (703):
- case 300: first argument through an asm-label array alias `extern unsigned char *E64a[2] asm("D_800B0E64");`
  (`func_8006DF50(E64a[0], …)`) — retail loads `D_800B0E64` first (`lui $4 / lw $4` before the `a[k]` pointer loads);
  a scalar alias or a block-local copy does not move it (703 → 707).
- case 400: `register int h asm("$5")` for the matched `hA` (707 → 712). Pinning `i` to `$6` as well links 4 words long.
Residual: case 400 loop — retail keeps the strength-reduced giv separate from `q` (`addu $7,$4,$0` pre-loop; n in `$8`,
giv `$7`, so `lui $4,0xff` fills the in-loop `beq` slot); cc1 ties the giv to q's register (this is the missing word).
Pinned `nn asm("$8")` (709), explicit pinned pointer walk `g asm("$7")` (701–703) do not help. Case 406: `la D_800B0CE9`
still hoisted into the `lw`→`sh` load-delay slot (retail after the `sh`); array/scalar aliases, `(*c)++` via a local,
`x = x + 1` all identical; `-fno-schedule-insns2` 696, `-fno-schedule-insns` 553.
