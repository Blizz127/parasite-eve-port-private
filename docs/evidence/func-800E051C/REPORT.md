# func_800E051C — EXE park-effect glow sprite (inline GTE)

- EXE, VRAM 0x800E051C, file offset 0xD0D1C, size 0x2EC (187 words). It is a leaf with no calls,
  and its prototype is `void func_800E051C(void)`.
- Role: draws one camera-facing POLY_FT4 (len 9, code 0x2E) from the scratchpad block that
  func_800E026C / func_800E03A0 publish. The matrix comes from `*(MATRIX **)0x1F800034` and
  the vertex from 0x1F800024, then `rtps`. SXY is stored to 0x1F800000 and SZ to 0x1F800004.
  The half-size is `h = ((size<<5) * *D_800BCFA8) / ((sz<<4) + *D_800BCFA8) >> 1`. The prim is
  linked at `ordering[bank][(sz>>2) - 8]`.
- Source: `src/func_800E051C.c`, hand-written in this repo (the vendored khasinski tree has no
  C for it). The `inline GTE/asm` tag is set: it uses Psy-Q `inline_c.h`-shaped macros
  (`gte_SetRotMatrix`, `gte_SetTransMatrix`, `gte_ldv0`, `gte_rtps`, `gte_stsxy`,
  `gte_stsz`). Count it separately from plain C.
- Profile: `era_o2_g0_expand_div` (`-O2 -G0`, `MASPSX_EXPAND_DIV=1`), which is load-bearing.
  Without the knob the leaf has 64 mismatched words; with it, 0.
- YAML: `[0xD0D1C, asm]` became `[0xD0D1C, c, func_800E051C]` + `[0xD1008, asm]` (the
  `D_800E0808…` data that follows).

## Verification

```
$ MASPSX_EXPAND_DIV=1 python3 tools/analysis/era_link_check.py src/func_800E051C.c 0x800E051C 0x2EC -O2 -G0
linked .text 752 bytes, target 0x2ec, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800E051C
disc1_preflight: PASS (deep, 2242 c / 144 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800E051C
LEAF_STRONG=EXACT func_800E051C span=0x2EC vram=0x800E051C compiled=0x2F0 failures=[none]
$ python3 tools/analysis/profile_necessity.py --only func_800E051C
PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status
disc1 plan: 2388 spans (2242 c, 144 asm, 2 rodata), geometry=0x1EE000, plan=93fe51ebc3fb
$ scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2242 plan=93fe51ebc3fb8ace043c13064f80cbe71630c9e2adeb75ca300944396bd6cddc yaml=ad3a09fbc1e21d9e13bb253d524a209b7a84007cb5d2faaf1e234742d74dba56
EXACT_REBUILD_GATE=PASS plan=93fe51ebc3fb8ace043c13064f80cbe71630c9e2adeb75ca300944396bd6cddc yaml=ad3a09fbc1e21d9e13bb253d524a209b7a84007cb5d2faaf1e234742d74dba56 spans=[2242 c, 144 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

- `sha256(configs/USA/disc1.yaml)` = `ad3a09fbc1e21d9e13bb253d524a209b7a84007cb5d2faaf1e234742d74dba56`
- `sha256(configs/USA/disc1_build_profiles.json)` = `17e123b21d66cbca5009a99433f3728347958806b74d2ffe88771b68c5f3ab77`
  (`func_800E051C` added to `era_o2_g0_expand_div`)

## Levers (mismatch ladder, era_link_check words)

1. Flat constant-address scratch reads (`*(T *)0x1F8000xx`) plus flat `D_800B0E58[]` /
   `D_800B0E38[]` arrays: 181. cc1 CSE'd the repeated u/v addresses into registers, and the
   base fold was missing.
2. **`RenderBufferPrefix` struct for `D_800B0E38`** (`.ordering[bank]` / `.packets[bank]`;
   packets at +0x20): 153. This reproduces retail's `&D_800B0E58` base in `$a3` and the
   `addiu $a3,-0x20` fold.
3. A **local scratch base pointer** instead of constant addresses: 92. This keeps the `$a2`
   base for `0x34` / `0`, while the other offsets fold back to `lui`+offset macro loads as in
   retail.
4. **Psy-Q `addPrim` with the `P_TAG` bitfield macros** (`setaddr`/`getaddr`): this evaluates
   the OT element twice, as retail does, so the OT base and `sz` are reloaded after the tag store.
5. **`POLY_FT4` and a `Scratch` struct** reached through `Scratch *s = (Scratch *)0x1F800000`:
   vertex stores become struct stores, so cc1 reloads `sx`/`sy` after each store as retail
   does. This also produces retail's 8-byte frame with no saves by itself. At this point the only
   residual was the matrix load, handled in step 6.
6. **Pass `s->mat` directly to both `gte_SetRotMatrix` and `gte_SetTransMatrix`**, with no
   `m` local: 0. A `MATRIX *m = s->mat` local turns the load into a `lui`+`lw` constant form
   in `$v0` instead of retail's `lw $t2,0x34($a2)`.

Rejected: `volatile` scratch reads (they force the addresses into registers and leave the load
delay slots unfilled), `-O1` (177), and the cc1 2.8.1 compiler (186).

## Classification fix

`docs/evidence/non-c-matchable/MANIFEST.json` listed this span as counted
`handwritten-gte-wrapper` with the stale path `asm/disc1/C5060.s`. The entry is removed:
`handwritten-gte-wrapper` counts go 107 → 106 and words 19958 → 19771, and `total_spans` goes
1695 → 1694. That was a minimal edit; the file was not regenerated. `nonmatchable_spans.py`
itself has no per-span entry for this function, so it needed no change. The span disappears from
its output once the asm is re-split. Note that the rest of MANIFEST.json is broadly stale against
the current split (a fresh `--json` run differs in about 80 entries). Regenerating it is left
to the lead, because `route_coverage.py` consumes it.
