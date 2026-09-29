# func_80035558 — EXE frame pump (Entity_FrameUpdate), inline GTE

- EXE, VRAM 0x80035558, file offset 0x25D58, size 0x72C (459 words). It is the top of the
  battle/field effect pump chain; the native port calls it at `func_80035558_port.c:672`, and it
  is on the Day-1 route. Its prototype is `void func_80035558(void)`.
- Its 35 COP2 ops form exactly one Psy-Q `gte_CompMatrix` (SetRotMatrix, 3x
  ldclmv/rtir/stclmv, SetTransMatrix, ldlv0/rt/stlvnl). There are no handwritten integer runs,
  so it is the same class as the m0348i leaves.
- Source: `src/func_80035558.c`, hand-written in this repo. The vendored khasinski tree names it
  `Entity_FrameUpdate` but carries it as `asm`. The `inline GTE/asm` tag is set, so count it
  separately from plain C.
- Profile: `era_o2_g8_force_d8009d1a0_d8009d1f4_d8009d2a4_d800bcffe_absolute` (new):
  `-O2 -G8` plus `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0,D_8009D1F4,D_8009D2A4,D_800BCFFE`.
  `-G8` keeps the actor-list globals `D_8009D20C` (0x49C($gp)) and `D_8009D254` (0x4E4($gp))
  gp-relative. Every forced symbol is load-bearing: dropping D_8009D1A0 gives 429 mismatched
  words, D_8009D1F4 409, D_8009D2A4 381, and dropping D_800BCFFE fails to link.
- YAML: `[0x25D58, asm]` became `[0x25D58, c, func_80035558]`. The span ends exactly at the
  next C leaf, `func_80035C84` (0x26484).

## Verification

```
$ MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D1A0,D_8009D1F4,D_8009D2A4,D_800BCFFE python3 tools/analysis/era_link_check.py src/func_80035558.c 0x80035558 0x72C -O2 -G8
linked .text 1840 bytes, target 0x72c, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_80035558
disc1_preflight: PASS (deep, 2243 c / 143 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_80035558
LEAF_STRONG=EXACT func_80035558 span=0x72C vram=0x80035558 compiled=0x730 failures=[none]
$ python3 tools/analysis/profile_necessity.py --only func_80035558
PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status
disc1 plan: 2388 spans (2243 c, 143 asm, 2 rodata), geometry=0x1EE000, plan=42b65e87c391
$ scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2243 plan=42b65e87c39126eefde7d9da38d81bf8f4260ef15175aec8f207224db3a6fff2 yaml=648394d857f7f3ccf4f1f7139d869581b6e1c50d73f582445a6f967f398898cd
EXACT_REBUILD_GATE=PASS plan=42b65e87c39126eefde7d9da38d81bf8f4260ef15175aec8f207224db3a6fff2 yaml=648394d857f7f3ccf4f1f7139d869581b6e1c50d73f582445a6f967f398898cd spans=[2243 c, 143 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

- `sha256(configs/USA/disc1.yaml)` = `648394d857f7f3ccf4f1f7139d869581b6e1c50d73f582445a6f967f398898cd`
- `sha256(configs/USA/disc1_build_profiles.json)` = `4e53fdd491b2bf2745355a3682f8931d8331e72aede291aa2607549abd200035`

## Levers (mismatch ladder, era_link_check words)

1. Straight C from the asm, with scalar `D_800B0CD8` in the force list: 431.
2. A `D_800B0CD8` pointer local `p` (retail keeps `&D_800B0CD8` in `$s0` across the calls
   in the gate block): with a plain assignment cc1 folded the pointer back to the symbol, and an
   `asm("" : "=r"(p) : "0"(&D_800B0CD8))` barrier gave 371.
3. A separate local for the D_8009D254 focus search (`c`, held in `$a0` as in retail), and one
   shared `a` for all the `D_8009D20C` walks.
4. Register pins (`a` in `$s1`, the `+0x1B4`/`+0x1E8` pointer in `$s0`) with the D_800B0CD8
   pointer merged into `b`: 365. Writing `+0x228..0x22C = W(+0x28..0x30) >> 16` gives retail's
   `lh` of the high half.
5. **Declare `D_800B0CD8` as an incomplete array `unsigned int D_800B0CD8[]`**, which takes it
   out of the `-G8` small-data class, and drop it from the force list. cc1 then no longer folds
   `b = (char *)D_800B0CD8` back into the symbol, and the barrier goes away: 98.
6. **Write the two RMW flag updates on `D_800B0CD8[0]` directly** (`|= 0x9000`,
   `&= 0xFFFF6FFF`) instead of through `b`/`q`. cc1 then knows they don't alias
   `D_8009D1A0` and schedules both loads before both stores, as retail does: 2.
7. **Pass `a + 0x1E8` inline to both `func_800794C4` and `gte_CompMatrix`** instead of a named
   matrix pointer. The `bnez` delay slot then takes `addiu $a0,$s1,0x1E0` as in retail: 0. At
   that point both register pins are redundant and were removed (the final source has no pins
   and no asm barriers apart from the GTE macros).

Rejected: declaration-order permutations (120 tried, no effect), `register` without `asm`, and
moving the pointer barrier before the first walk (419).
