# func_800DE7A8 — EXACT (lane exeD4, 2026-09-27)

VRAM `0x800DE7A8`, file `0xCEFA8`, size `0x288` (`asm/disc1/CE8A8.s`). Two-state effect
callback. State 1 drifts and decays the `a1[]` vector and ends after 32 frames. State 2 spawns
two particles through `func_800CEE20` with a scale of `D_800F336A * (D_800E27EC / 4 or 8)` and a
table-driven tone from `func_80077AA4`.

- Profile: default `era_o2_g0` (2.7.2 `-O2 -G0`, no environment), so no assignment is needed.
- YAML: `- [0xCEFA8, c, func_800DE7A8]`, carved out of the `[0xCE8A8, asm]` span. The next row is `[0xCF230, c, func_800DEA30]`.

## Verification (private config build/lanes/exeD4/)
```
disc1_preflight.py --config build/lanes/exeD4/disc1.yaml --profiles build/lanes/exeD4/disc1_build_profiles.json --deep --only func_800DE7A8
  disc1_preflight: PASS (deep, 2090 c / 189 asm / 2 rodata)
leaf_strong_check.py --config ... func_800DE7A8
  LEAF_STRONG=EXACT func_800DE7A8 span=0x288 vram=0x800DE7A8 compiled=0x290 failures=[none]
```

## Load-bearing lever (exeD best was 16 words, D_800E27EC/D_800F336A one register low)
The `func_80077AA4(…) & 0xFFFF` call is written inline as the 6th argument of `func_800CEE20`
instead of a `t = …;` statement. sched1 then keeps the return value `$v0` live across the
D_800E27EC / D_800F336A loads (the `andi` sinks to the argument setup), so global alloc gives them
`$v1`/`$a0` and the quotient `$v0`. sched2 then hoists the `andi $t0,$v0,0xFFFF` back under the
`jal`, which is retail's order. Every statement form (`int t`, `unsigned short t`, a `t & 0xFFFF`
argument, a separate `m` product local) gives 16–70 words. `$3`/`$4`/`$2` pins give the right
registers but a 22-word `andi` placement residual.
