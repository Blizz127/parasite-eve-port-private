# func_8002FAF8 — actor animation-slot switch (16-entry jump table)

- VRAM `0x8002FAF8`, file offset `0x202F8`, size `0x380` (single `jr $ra`, next span
  `[0x20678, c, func_8002FE78]`).
- Profile `era_o2_g0_dispatch_80010a88` = `-O2 -G0` + `MASPSX_THREE_WORD_SYMBOL_STORE=1`
  + `MASPSX_DISPATCH_FOLD=jtbl_80010A88`.
- Lane exeB, 2026-09-27. Draft base `build/archive/fanout/decompA/drafts/func_8002FAF8.c`
  (previously PARKED at 7 words).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json --deep --only func_8002FAF8
  -> disc1_preflight: PASS (deep, 2043 c / 216 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeB/disc1.yaml --profiles build/lanes/exeB/disc1_build_profiles.json func_8002FAF8
  -> LEAF_STRONG=EXACT func_8002FAF8 span=0x380 vram=0x8002FAF8 compiled=0x380 failures=[none]
```
`profile_necessity.py` has no `--config`; necessity checked by hand: dropping
`MASPSX_THREE_WORD_SYMBOL_STORE` → 2 mismatches, dropping `MASPSX_DISPATCH_FOLD` → 2.

## Levers (mismatch ladder)

| variant | mismatches |
|---|---|
| prior draft | 7 |
| head as `if ((f5 == 2 \|\| f5 == 4) && (y = a->f18C) != 0) b = y; else b = a;` | 5 |
| + slot insert / flag clears as bitfields (`st->u0.b.slot = i; b30 = 0; b31 = 0;`) | **0** |

1. **Merged `b = a` target.** Retail's `f5 != 4` branch shares the `y == 0` arm's
   `b = a` and reorg copies it into the delay slot (retargeted one past). The `&&`
   condition form gives both failure paths one `b = a` block; the nested-if form leaves a
   `nop`.
2. **Bitfield insert moves the `st->f0` load in sched1.** With the explicit
   `(f0 & 0xFF1FFFFF) | ((i & 7) << 21)` form, the `-dS` dump shows sched1 placing the
   `f0` load after the last use of `i*16`, so global-alloc gives `i*16` `$3` and `i` `$7`.
   Retail has `i` in `$8` and `i*16` in `$7`, i.e. the `f0` value (local `$3`) was live
   across `i*16`. Declaring `f0` as a union with a
   `{lo:21, slot:3, mid:6, b30:1, b31:1}` bitfield view and writing
   `st->u0.b.slot = i; st->u0.b.b30 = 0; st->u0.b.b31 = 0;` produces that order.
   The masks come out the same (`0xFF1FFFFF`, `0xBFFFFFFF`, `0x7FFFFFFF`, one lw / one sw).
   The mixed form (explicit insert + bitfield clears) stays at 5; the bitfield insert with
   explicit `&=` clears also matches.
3. Pins `a asm($6)`, `b asm($17)`, `y asm($4)` stay from the prior draft (dropping `y`'s pin
   → 8, dropping `a`'s pin → 206).
