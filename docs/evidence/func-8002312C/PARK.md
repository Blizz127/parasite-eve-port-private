# func_8002312C — PARKED (lane exeA, 2026-09-27)

- VRAM `0x8002312C`, file offset `0x1392C`, size `0x5BC`
- Best: 322 (decompA first-pass draft, links 0x5A0 — 7 words short)
- Profile tried: -O2 -G8, MASPSX_THREE_WORD_SYMBOL_STORE=1, MASPSX_DISPATCH_FOLD=jtbl_8001078C, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D278,D_8009D2FC
- Best draft: `build/archive/fanout/decompA/drafts/func_8002312C.c` (not in `src/`)

## Residual

Not re-attacked. decompA: control flow decoded; actor pointer home, facing-index
operand order, facing load order and return-label splits differ. Candidate levers from this lane:
the `short k` narrow-local lever that closed its neighbour `func_800236E8`, and the queue-push
idiom (`Q *q = &D_800BE830[...]` block-local init).

## Levers tried

(decompA only)

Measured with `tools/analysis/era_link_check.py <draft> 0x8002312C 0x5BC <flags>` (profile env in a subshell).

## Round 2 (lane exeA4, 2026-09-27) — 328/367 aligned (lnk 353), draft build/lanes/exeA4/drafts/func_8002312C.c
Metric: `build/lanes/exeA4/al.sh <draft> 0x8002312C 0x5BC "<profile env above>" -O2 -G8` (difflib opcode alignment).
Landed (289 → 328):
- **semantic fix**: case 7 first-path condition is `(m->f10 & 0xC0) != 0xC0 || D_8009D1DC == 1` (the decompA draft had `!= 1`;
  retail `bne $3(d),1 → second path`).
- case 6: block-local `register Ent *e6 asm("$5") = D_8009D254` and the reset arm moved out of the if/else-if chain
  (`else goto reset; e6->f98 |= 0x100; break; reset: ...`) — retail places the `|= 0x100` fall-through before the reset block.
- case 7: `register Ent *e asm("$6")`, `register Mdl *m asm("$4") = st->f68` (retail m=$4, d=$5, e=$6, st=$7, t=$8, t*8=$9, tgt=$10).
- second path written `if (who->f0->f10 <= 0 || who->f0 == 0) { fail } else { D_1DC = d-1; tail: ... }`; the shared
  `D_8009D1DC = v; func_80023008(); return r;` tail is an explicit label that case 6 `goto`s (cross-jump direction as retail).
- facing index `*(unsigned char *)(facing(tgt) + (int)D_8009D278 + 0x17/0x14)` gives retail's `addu $2,$k,$base`.
- facing prologue `{ register Ent *p asm("$4") = *tgt; s = -D_8009D27C + p->f26A; }` gives retail's `lw $4,0($10); lh D_27C; lh 618($4)`.
- first-path end `if (func_800518A8() != 0) { ...; r = 1; break; } r = 1; break;` (+1).
Residual: (1) facing arm: retail `lbu fF; nop; sll; sw f14; lw f98` — cc1 hoists the f98 load above the f14 store
(same base reg ⇒ memrefs_conflict_p proves disjoint; non-struct casts / e2 local / reordering all byte-identical);
(2) the `func_8001A680(D_254,0xC); func_8006DD38(...)` pair of the first path cross-jumps into the second path's copy
(retail keeps both, the first ending in the shared `r = 1` at 0x23518); (3) a dead `addu $5,$6,$0` in the 0x23410 beq
slot and `addu $4,$6` vs `lbu $5` order in the 0x233C8 fail block. Tried and rejected: `m = st->f68` reload in the first
path (316–317), `if (func_800518A8() == 0) { r = 1; break; }` form (327), `int v` tail (317).
