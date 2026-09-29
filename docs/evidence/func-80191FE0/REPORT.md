# func_80191FE0 — room_m0348i (M34 alligator boss, phase 2)

- Target: `room_m0348i` (PE.IMG room m0348i chunk 2, VRAM base 0x8018EFE8)
- VRAM 0x80191FE0, blob offset 0x2FF8, size 0x300 (span ends on its `jr $ra` + delay slot)
- Role: ember-burst callback: 4 embers, each an identity matrix translated by `a2+i*8` (`h30/h32/h34`), composed with the effect matrix, rotated by `D_8018F004`, scaled by `a2+i*2` `h70`, coloured from `a2+i*4` bytes `0x20..0x22` and sized by `h78` into `D_80192E40`, drawn via `func_800C42A4`.
- Source: `src/overlays/room_m0348i/func_80191FE0.c` — hand-written in this repo. khasinski's vendored
  tree (`vendor/khasinski-parasite-eve-decomp/configs/USA/overlays/room_m348.yaml`) carries this
  body as `asm` only, so there is no vendor-derived C.
- **Inline GTE**: the body uses Psy-Q `inline_c.h`-shaped COP2 asm macros (`gte_CompMatrix` =
  `gte_SetRotMatrix` + 3x `gte_ldclmv`/`gte_rtir`/`gte_stclmv` + `gte_SetTransMatrix` +
  `gte_ldlv0`/`gte_rt`/`gte_stlvnl`). Count it separately from plain C (same convention as the
  vendor-lane inline-GTE imports).
- Profile: `era_o2_g0` (overlay default, no assignment, no maspsx knobs)
- YAML rows: see `configs/USA/overlays/room_m0348i.yaml` (`[0x2FF8, c, func_80191FE0]`)
- Masked-body twin: room_m0174i func_801930F8 (identical modulo relocated immediates/jal targets; not ported)

## Verification

```
$ TMPDIR=$PWD/build/tmp python3 tools/analysis/era_link_check.py --target room_m0348i src/overlays/room_m0348i/func_80191FE0.c 0x80191FE0 0x300
LINK_EXACT (word mismatches=0, nonzero_pad=0)
$ python3 tools/build/disc1_preflight.py --target room_m0348i --deep --only func_80191FE0
disc1_preflight: PASS (deep, 44 c / 1 asm / 0 rodata)
$ flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh room_m0348i
EXACT_REBUILD_GATE[room_m0348i]=PASS plan=d4caaec0a100768c70365259efa521ba3b0c0262cd1523b4ea7cbbc870a54b03 yaml=3b11c31817fd5422b2bd5739c80783287fcc5d168e5fd5847084c8b382751a50 spans=[44 c, 1 asm, 0 rodata, 2 bin] sha1_orig=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1_cand=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1=49478531f4b705ab248a8c2eb0a3b81c4cef9985
```

The gate run above integrates all three m348 lane leaves (func_80190E98, func_80191A98,
func_80191FE0) at once.

- `sha256(configs/USA/overlays/room_m0348i.yaml)` = `3b11c31817fd5422b2bd5739c80783287fcc5d168e5fd5847084c8b382751a50`
- `sha256(configs/USA/overlays/room_m0348i_build_profiles.json)` = `91719eb7e814257ac89324325a31c3639689cd19d26a901483f74097041d7536` (unchanged)

## Levers

- Matched on the first draft once the func_80191A98 identity-matrix store order was reused (reverse diagonal, chained `t` zero, chained off-diagonal zero).
- Per-field byte-offset macros (`SH(a2 + i*8, 0x30)` etc.) give retail's three strength-reduced pointer IVs (`$s1` +8, `$s0` +2, `$s2` +4); `unsigned int i` gives `sltiu`.
