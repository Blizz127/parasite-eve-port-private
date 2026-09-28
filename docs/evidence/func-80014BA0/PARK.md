# func_80014BA0 — PARKED (lane exeA, 2026-09-27)

- VRAM `0x80014BA0`, file offset `0x53A0`, size `0x200`
- Best: 104 word mismatches (size exact)
- Profile tried: -O2 -G8, MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0
- Best draft: `build/lanes/exeA/drafts/func_80014BA0.c` (not in `src/`)

## Residual

Fresh decode from the asm (the goalb0 draft introduced a frame and was semantically off; 117).
Frameless leaf: turn-to-target setup (`D_8009D300->f8 & 0x20` gate, `f14` = +/-speed by the
shortest-arc test, `f18` = target) then a crossing test that snaps `f3A` or steps it.
Residual is register-copy structure: retail keeps *copies* of the heading (`$a1 -> $t3, $a2`) and
target (`$v1 -> $a0, $t0`) and re-sign-extends `$t0` (`(short)tgt`) although it is `& 0xFFF`;
the post-`L80` half uses `$a1` and `$a3` as two names for the speed. cc1's cse/combine drop
every copy and the no-op extension, so the whole body shifts.

## Levers tried

inline `adiff(tgt, ang)`; whole body as `static inline step(a0, ang)` wrapper (108);
`-O1` (107); `-fno-schedule-insns` (115).

Measured with `tools/analysis/era_link_check.py <draft> 0x80014BA0 0x200 <flags>` (profile env in a subshell).

## Round 2 (lane exeA4, 2026-09-27) — best still 49/128 aligned (lnk 116; retail 128)
Decode confirmed (draft is semantically right). New observations: retail reloads `D_8009D300` ($gp) after the `sh f8`
in every f14/f18 arm, while cc1 keeps it in a register (cse follows the d-diamond: retail's `if (tgt-ang<0) d=ang-tgt;
else d=tgt-ang;` keeps both arms and a join, cc1 folds to `d=tgt-ang; if (d<0) d=ang-tgt`). Retail's test operands are
*copies* (`$6`,`$4`) while the arms use the originals (`$5`,`$3`), so the two `tgt-ang` computations are never CSE'd.
Tried: static-inline `adiff`/`setup`/`step` split (byte-identical to the flat draft — cse removes inline param copies),
non-struct cast accessors for every Task/Actor field (49/128, lnk 118), every era cc1 (`ERA_CC1_VER` 2.6.3 48,
2.7.2-cdk 34, 2.7.2.3 49, 2.8.x 32, 2.9x 31–32). The copy structure looks like it needs something cse cannot see through.
### Round 2 progress (exeA4): 49 → 99/128 aligned
The copies are **inline-function parameter copies with narrow types** (a `short`/`unsigned short` param fed an `int`
value: cse sees a conversion, combine later reduces it to `addu x,y,$0`). Generators `build/lanes/exeA4/v14/gen*.py`.
- `start(unsigned int **a0, int ang)` inline (fl test, tgt/spd loads, `ang == tgt` early `return 1`, `f8 |= 0x20`,
  `setup(ang, tgt, spd)`), called from main as `if (start(a0, ang)) return 1;` with main `short ang`;
  `setup(unsigned short ang, short tgt, short spd)` with the diff written `if (tgt - ang >= 0) d = tgt - ang; else d = ang - tgt;`
  (retail's fall-through arm is `tgt - ang`); step code in main as ONE `||` chain falling into the done block → 89/128
  (lnk 129), draft `build/lanes/exeA4/drafts/func_80014BA0_y87.c`.
- tests 1/2 through `static inline int crossed(int ang, short cur, short spd)` (retail uses copies `$4`/`$3` there but
  `$11`/`$8` in tests 3/4) → 99/128 but lnk 136: the inline's boolean return materialises a flag (`addu $8,$0,$0` …
  `bne $8`) and `start`'s return value is tested (`bne $2,$0` after `j`) where retail returns directly. Draft
  `build/lanes/exeA4/drafts/func_80014BA0.c`. Block-local copies instead of `crossed` (85) get cse'd away.
Residual: start/crossed return-value materialisation; `f18 = (short)tgt` re-extension (retail `sll $8`/`sra` at the
store, i.e. setup's tgt param is not pre-extended); `D_8009D300` in `$10`/fl in `$9`.

## Round 3 (lane exeA5, 2026-09-28) — best still exeA4's 99/128
Flat (no-inline) narrow-copy rewrite `build/lanes/exeA5/drafts/func_80014BA0_flat.c` (67/128, lnk 121): `int a = f3A & 0xFFF`,
`unsigned short ua = a`, and inside the setup block `short sa = a; int t = …; unsigned short spd; short st = t;
unsigned short ut = t;` reproduces retail's setup block almost word-for-word (the `sa == st` test, `st - sa` sign test,
`a - t` / `t - a` arms, the four f14 arms). Differences there: retail re-extends the f18 store (`sll/sra $8`) — the stored
value must not be range-known (supports exeA4's "setup's tgt param is not pre-extended"); ours stores `$11` directly.
Main part: plain `int s2 = s; int c2 = c;` copies are CSE'd away (cse follows the `||` fall-through chain) and
`(short)c` folds to `lh`; retail keeps `lw` + `sll/sra` and the `$7`/`$8` copies — those still need an inline-param or
block-boundary source (try the no-code `L: z = 0; if (z) goto L;` boundary before tests 3/4). A `-8` stack frame
appeared in the flat form (unexplained; check `short d`).

## Round 4 (lane exeA6, 2026-09-28) — best still exeA4's 99/128 (`build/lanes/exeA4/drafts/func_80014BA0.c`)
Levers that closed func_8001C7DC / func_8001B5FC / func_8001220C this session, tried here without success:
- Tests 1/2 moved into main with `goto done` instead of the `crossed()` inline (`build/lanes/exeA6/drafts/func_80014BA0_testsmain.c`):
  89/128. Kills the boolean flag, but the test-1/2 copies (`$4=$11`, `$3=(short)cur`, `$7=$5`) vanish with it.
- Type grid over main `ang` / `start` param / `tgt` / `setup` params on that form (243 variants): best 89.
- A single nested inline chain whose results flow straight into the return (`return body(a0, f3A & 0xFFF)`
  → `setup` + `return tail(ang, f14, f18)`): 66/128 at best over a 144-variant type grid.
- Flat narrow-copy form with variable reuse (`v7` = spd in setup and f14 in the tail, `v8` = tgt copy and
  f18; retail uses `$7`/`$8` for both roles, and `v8` being set twice would explain the non-folded `(short)$8`
  re-extension), plus a `L1: z = 0; if (z) goto L1;` cse boundary after the tail copies
  (`build/lanes/exeA6/drafts/func_80014BA0_flatF1.c`): 67/128. cse still drops `a4`/`v7`/`v8` (it re-loads f18 as
  both `lh` and `lw` rather than keeping a copy). A statement-expression tail with narrow locals and `goto done`
  (4 type variants): 66–67.
- The flat forms all get an unexplained 8-byte frame (`addiu $29,$29,-8`, no stack use) that the inline forms don't.
Still open: retail's copy set looks like inline-param copies (exeA4) with no boolean materialisation. Next thing to try:
cc1 `-dj`/`-ds` on the y99 draft to see whether `cse-follow-jumps` could fold `start()`'s `ret=1; goto L` into
`L: if (ret)` (it folds only if `L` has a single use), e.g. make the early return the inline's only jump.
