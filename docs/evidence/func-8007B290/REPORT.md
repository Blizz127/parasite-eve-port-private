# func_8007B290 — EXACT (lane exeG, 2026-09-28)

VRAM `0x8007B290`; file `0x6BA90`, size `0x2C8`.
Source `src/func_8007B290.c`. Supersedes PARK.md (kept for the lever history).

## Verify (private config build/lanes/exeG/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeG/disc1.yaml --profiles build/lanes/exeG/disc1_build_profiles.json --deep --only func_8007B290
  -> disc1_preflight: PASS (deep, 2140 c / 151 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeG/disc1.yaml --profiles build/lanes/exeG/disc1_build_profiles.json func_8007B290
  -> LEAF_STRONG=EXACT func_8007B290 failures=[none]
python3 build/lanes/exeG/pn.py --profiles build/lanes/exeG/disc1_build_profiles.json --only func_8007B290   (profile_necessity.py against the private yaml)
  -> PROFILE_NECESSITY=PASS
```

## Mechanism and levers
The PARK residual was the printf `$4 = &D_80011B28` set. From the `cc1 -dS` trace (2.8.1):
at T-5 (just above the call) the ready list held only the priority-1 `$4 = fmt` set and the
priority-2 `$5 = pseudo` copy of `D_800A3480` (`$6/$7` are loads, so latency keeps them queued).
The copy won, fmt was issued later in backward time (earlier in the code), `$4` was live across the
`intstr[a]` chain, and local-alloc put the chain in `$7`.

1. **`char **pn = &D_800A3480;` and pass `*pn`**: the argument becomes a `(mem (reg))` that
   expand_call keeps as a MEM and loads straight into `$5` after the stack store (combine folds
   `pn` back to the symbol, so the code is unchanged). `$5` is now a latency-queued load like
   `$6/$7`, fmt is picked at T-5, and the chain gets `$4` as in retail.
2. **Prologue `li $4,-1`**: same as func_8007B010 (the pinned `result` is `$19` here):
   `{ int m1; __asm__("" : "=r"(m1) : "0"(-1), "r"(result)); … func_80073A44(m1) … }`.

Ladder: 7 (PARK) → 3 (lever 1) → 0 (lever 2). Profile `era_o2_g0_cc1_281_fill_call_delay_slot`.
