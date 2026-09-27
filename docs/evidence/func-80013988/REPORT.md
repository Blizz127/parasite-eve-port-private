# func_80013988 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x80013988`, file offset `0x4188`, size `0x2AC` (ends at `0x4434`, the next row `func_80013C34`)
- Profile `era_o2_g8_force_d8009d2f0_d8009d254_absolute` (existing): `-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2F0,D_8009D254`
- YAML: `- [0x4188, c, func_80013988]` replaces `- [0x4188, asm]` (span is the whole former asm row)

Shape: field-VM "walk to point" step. Loads/caches the target `(tx, tz, tol)` in the VM task
record `D_8009D300` (flag `0x20` at `+8`), computes heading `ang = (0x1400 - atan2) & 0xFFF`,
sets the velocity (`f68/f70 = -spd * cos/sin(ang) << 4`), turns `f3A` towards `ang` by at most
`tol`, and either arrives (snap to target, clear velocity, clear the flag, return 1) or
consumes 5 VM words (`D_8009CE00 -= 5`), sets `D_8009D300->f10 = 1`, returns 0.

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_80013988
  disc1_preflight: PASS (deep, 2049 c / 214 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_80013988
  LEAF_STRONG=EXACT func_80013988 span=0x2AC vram=0x80013988 compiled=0x2B0 failures=[none]
```
`profile_necessity.py` has no `--config` — skipped; manual: `-O2 -G8` without the
force-absolute list → 160 mismatches.

## Levers (mismatch ladder)

From the parked decompA draft (44; six pins `$17..$22`).

1. Tail layout: `if (x*x + z*z >= dx) { arrive; return 1; } D_8009CE00 -= 5; ...; return 0;`
   (retail falls through into the "still walking" block) and the negation inlined into the
   first multiply argument `func_8003708C(spd = -spd, func_80077CF4(ang) << 4)` (retail negates
   after the `cos` call) → 24.
2. Pins on `x/z/tx/tz` removed (they were not load-bearing; `ang $17` and `tol $18` are) → 20.
3. The pinned `ang`/`tol` made cc1 compute intermediates *into* the pinned registers. Each
   such intermediate gets its own hard-reg local: `register int t asm("$3")` for
   `0x1400 - atan2` before `ang = t & 0xFFF`; `register int cc asm("$4")` as the copy of the
   loaded heading (retail keeps the load in `$v1` = the difference and the copy in `$a0`); and
   `register int k asm("$2")` for both the `tol < d` flag (so reorg can share it through the
   delay slot) and the `cc -/+ tol` value stored back → 0.
