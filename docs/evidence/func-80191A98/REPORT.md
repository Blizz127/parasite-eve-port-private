# func_80191A98 — room_m0348i (M34 alligator boss, phase 2)

- Target: `room_m0348i` (PE.IMG room m0348i chunk 2, VRAM base 0x8018EFE8)
- VRAM 0x80191A98, blob offset 0x2AB0, size 0x318 (span ends on its `jr $ra` + delay slot)
- Role: fire-breath floor-glow callback: copies the actor matrix (`*(MATRIX*)(*x+0x238)+0xA0`) into the effect, composes an identity+offset(0,100,-100) matrix, rotates by `D_8018F004`, scales by `a2->h30`, submits two glow slots `D_80192E80[2]` via `func_800C4FC4`.
- Source: `src/overlays/room_m0348i/func_80191A98.c` — hand-written in this repo. khasinski's vendored
  tree (`vendor/khasinski-parasite-eve-decomp/configs/USA/overlays/room_m348.yaml`) carries this
  body as `asm` only, so there is no vendor-derived C.
- **Inline GTE**: the body uses Psy-Q `inline_c.h`-shaped COP2 asm macros (`gte_CompMatrix` =
  `gte_SetRotMatrix` + 3x `gte_ldclmv`/`gte_rtir`/`gte_stclmv` + `gte_SetTransMatrix` +
  `gte_ldlv0`/`gte_rt`/`gte_stlvnl`). Count it separately from plain C (same convention as the
  vendor-lane inline-GTE imports).
- Profile: `era_o2_g0` (overlay default, no assignment, no maspsx knobs)
- YAML rows: see `configs/USA/overlays/room_m0348i.yaml` (`[0x2AB0, c, func_80191A98]`)
- Masked-body twin: room_m0174i func_80192BB0 (identical modulo relocated immediates/jal targets; not ported)

## Verification

```
$ TMPDIR=$PWD/build/tmp python3 tools/analysis/era_link_check.py --target room_m0348i src/overlays/room_m0348i/func_80191A98.c 0x80191A98 0x318
LINK_EXACT (word mismatches=0, nonzero_pad=0)
$ python3 tools/build/disc1_preflight.py --target room_m0348i --deep --only func_80191A98
disc1_preflight: PASS (deep, 44 c / 1 asm / 0 rodata)
$ flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh room_m0348i
EXACT_REBUILD_GATE[room_m0348i]=PASS plan=d4caaec0a100768c70365259efa521ba3b0c0262cd1523b4ea7cbbc870a54b03 yaml=3b11c31817fd5422b2bd5739c80783287fcc5d168e5fd5847084c8b382751a50 spans=[44 c, 1 asm, 0 rodata, 2 bin] sha1_orig=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1_cand=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1=49478531f4b705ab248a8c2eb0a3b81c4cef9985
```

The gate run above integrates all three m348 lane leaves (func_80190E98, func_80191A98,
func_80191FE0) at once.

- `sha256(configs/USA/overlays/room_m0348i.yaml)` = `3b11c31817fd5422b2bd5739c80783287fcc5d168e5fd5847084c8b382751a50`
- `sha256(configs/USA/overlays/room_m0348i_build_profiles.json)` = `91719eb7e814257ac89324325a31c3639689cd19d26a901483f74097041d7536` (unchanged)

## Levers

- **Identity-matrix store order is load-bearing** (the only residual of the first draft, 10 words): retail's `sh`/`sw` order comes from `m[2][2]=m[1][1]=m[0][0]=0x1000` as three separate stores in reverse order, then the chained `m.t[0]=m.t[1]=m.t[2]=0;`, then the chained off-diagonal zero `m[0][1]=...=m[2][1]=0;`, then `t[0]=0; t[1]=100; t[2]=-100` (duplicate stores kept). Found by a 90-variant ordering sweep (`build/lanes/m348/perm.py`); natural forward order = 10 mismatches, chained ones = 138 (dropped store).
- `D_80192E80` declared as a 2-element 0x18-byte slot array: cc1's related-value CSE keeps `&D_80192E80[0].h14` in `$s2` and derives `[0]` / `[1]` as `$s2-0x14` / `$s2+4`, as retail does.
- `D_8018F004` typed as an 8-byte **byte** struct → lwl/lwr/swl/swr unaligned copy.
