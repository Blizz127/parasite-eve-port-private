# func_8001CE88 — PARKED (lane exeA, 2026-09-27)

- VRAM `0x8001CE88`, file offset `0xD688`, size `0x2E8`
- Best: 152 word mismatches, but aligned it is a 2-word shift (two missing register copies); everything else matches
- Profile tried: -O2 -G8, MASPSX_EXPAND_DIV=1
- Best draft: `build/lanes/exeA/drafts/func_8001CE88.c` (not in `src/`)

## Residual

Nearest polyline segment within `D_8009CE2C` (bounding-box rejects, perpendicular
distance, projection range checks), returns the segment index or -1. Retail loads `p[i].x`/`p[i].z`
into caller-saved temps (`$v1`, `$a3`), stores the spilled `short` prev-point from the temp, and
copies into `$s3`/`$s1` right before the first compare of each axis; cc1 loads straight into
`$s3`/`$s1` (2 words shorter, everything after shifts).

## Levers tried

landed: one reused `d` for `x±lim`/`z±lim`/distance/dot/sums (all `$s0` in retail).
Tried: `x0 = load; x1 = x0`, int temps, chained assignment, `q->` pointer, `unsigned short` / `int`
prev, and hard-reg temps `register int tx asm("$3")` (cc1 ICE: unrecognizable `minus:SI (reg:SI)
(reg/v:HI)` insn), `register short` temps (149).

Measured with `tools/analysis/era_link_check.py <draft> 0x8001CE88 0x2E8 <flags>` (profile env in a subshell).

## Round 2 (2026-09-27)

- Scheduler rungs: `-fno-schedule-insns` 136, `-fno-schedule-insns2` 179.
- Hard-register temporaries (`register int tz asm("$7")`, with or without `int` copies of the `short` params) crash
  era cc1 with `unrecognizable insn (minus:SI (reg/v:SI) (reg/v:HI))`. The crash happens whenever a hard-reg local
  coexists with the `short` prev/param variables.
- exeC's greedy line-order search (`build/lanes/exeC/lsearch.py`) reported 16. That number is an artifact: it moved
  `continue;` out of its `if`, the link came out tiny, and `era_link_check.py` compares only `min(len)` words. A
  lane-local copy that rejects wrong-size links and only moves plain statements (`build/lanes/exeA/lsearch.py`) found
  70 at the correct size. That result leaves `ox` uninitialised on the first iteration, so it is not a valid program
  and was discarded.

## RESOLVED (lane exeA2, 2026-09-27)
Exact. See REPORT.md (int temp copy-first plus `short x1/z1`).
