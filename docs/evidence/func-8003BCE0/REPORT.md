# func_8003BCE0 — per-face vertex-colour relight (GT4/GT3/G4/G3 double-buffered prims)

- VRAM `0x8003BCE0`, file offset `0x2C4E0`, size `0x3D4`. Carved from `[0x2C17C, asm]` as
  `[0x2C17C, asm]` / `[0x2C4E0, c, func_8003BCE0]` / `[0x2C8B4, asm]`.
- Profile: default `era_o2_g0` (`-O2 -G0`), no knobs.
- Lane exeB, 2026-09-27. Draft base `build/archive/fanout/decompA/drafts/func_8003BCE0.c`
  (previously PARKED at 199: "retail keeps `q = p + buf*size` per iteration in loops 1-3").

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_8003BCE0
  -> disc1_preflight: PASS (deep, 2047 c / 215 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_8003BCE0
  -> LEAF_STRONG=EXACT func_8003BCE0 span=0x3D4 vram=0x8003BCE0 compiled=0x3E0 failures=[none]
```
(`compiled=0x3E0` includes the gas alignment pad; `nonzero_pad=0`.)

## Levers (mismatch ladder)

| variant | mismatches |
|---|---|
| prior draft (`GT4 *q = (GT4 *)p + buf;` declared inside each loop) | 199 |
| other spellings of `q` (`p + buf*sizeof`, `&((T *)p)[buf]`, int casts, `p` incremented twice) | 197–199 |
| **one function-scope `void *q;`** reused by all four loops, accessed as `((GT4 *)q)->c0` etc. | 11 |
| + loop increments in body order `p += ...; i++; f++;` (`for (i = 0; i < n;)`) | **0** |

1. **A shared user variable stops strength reduction.** gcc-2.7 `strength_reduce` takes
   `copy_cost` off a giv's benefit when the giv is a user variable that is not replaceable
   and the biv is not eliminable. A `q` that lives across four loops is not replaceable, so
   in loops 1-3 (where `p` stays live) the giv falls under the threshold. `q` is then
   recomputed per iteration (`addu $a1,$t1,$t6`), as in retail. In the last loop `p` is dead,
   so the biv is eliminable and `q` is reduced — also as in retail. The face-pointer givs
   (`f+10`, offsets −6..0) are still reduced.
2. **Latch order.** Retail's latch is `p += 104; i++; lw hdr; f-giv += 12; …; f += 12`.
   Only the body order `p, i, f` reproduces it (`i++` first → 8, for-clause `i++` → 11).
