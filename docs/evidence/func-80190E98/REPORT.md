# func_80190E98 — room_m0348i (M34 alligator boss, phase 2)

- Target: `room_m0348i` (PE.IMG room m0348i chunk 2, VRAM base 0x8018EFE8)
- VRAM 0x80190E98, blob offset 0x1EB0, size 0x608 (span ends on its `jr $ra` + delay slot)
- Role: fire-breath beam callback: 16 segments; for each live (`*(signed char*)(a2+i)==1`) segment below y -650 it composes a translated identity matrix through the beam rotation (`RotMatrix({0x20,a2->h180,0})`) and the effect matrix at `a2+0x160`, draws the beam sprite into `D_80192EB0`, kills the segment when `func_8001CAB0` (floor test) fails, flags `x->h4C` on `func_800C6B90` hit, then redraws the floor glow with `D_800942EC` height and the fixed `D_8018F03C` scale at half size.
- Source: `src/overlays/room_m0348i/func_80190E98.c` — hand-written in this repo. khasinski's vendored
  tree (`vendor/khasinski-parasite-eve-decomp/configs/USA/overlays/room_m348.yaml`) carries this
  body as `asm` only, so there is no vendor-derived C.
- **Inline GTE**: the body uses Psy-Q `inline_c.h`-shaped COP2 asm macros (`gte_CompMatrix` =
  `gte_SetRotMatrix` + 3x `gte_ldclmv`/`gte_rtir`/`gte_stclmv` + `gte_SetTransMatrix` +
  `gte_ldlv0`/`gte_rt`/`gte_stlvnl`). Count it separately from plain C (same convention as the
  vendor-lane inline-GTE imports).
- Profile: `era_o2_g0` (overlay default, no assignment, no maspsx knobs)
- YAML rows: see `configs/USA/overlays/room_m0348i.yaml` (`[0x1EB0, c, func_80190E98]`)
- Masked-body twin: room_m0174i func_80191FB0 (identical modulo relocated immediates/jal targets; not ported)

## Verification

```
$ TMPDIR=$PWD/build/tmp python3 tools/analysis/era_link_check.py --target room_m0348i src/overlays/room_m0348i/func_80190E98.c 0x80190E98 0x608
LINK_EXACT (word mismatches=0, nonzero_pad=0)
$ python3 tools/build/disc1_preflight.py --target room_m0348i --deep --only func_80190E98
disc1_preflight: PASS (deep, 44 c / 1 asm / 0 rodata)
$ flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh room_m0348i
EXACT_REBUILD_GATE[room_m0348i]=PASS plan=d4caaec0a100768c70365259efa521ba3b0c0262cd1523b4ea7cbbc870a54b03 yaml=3b11c31817fd5422b2bd5739c80783287fcc5d168e5fd5847084c8b382751a50 spans=[44 c, 1 asm, 0 rodata, 2 bin] sha1_orig=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1_cand=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1=49478531f4b705ab248a8c2eb0a3b81c4cef9985
```

The gate run above integrates all three m348 lane leaves (func_80190E98, func_80191A98,
func_80191FE0) at once.

- `sha256(configs/USA/overlays/room_m0348i.yaml)` = `3b11c31817fd5422b2bd5739c80783287fcc5d168e5fd5847084c8b382751a50`
- `sha256(configs/USA/overlays/room_m0348i_build_profiles.json)` = `91719eb7e814257ac89324325a31c3639689cd19d26a901483f74097041d7536` (unchanged)

## Levers

- **Statement order `D_80192EB0.h0A = a2[i].h20;` before `m2 = m;`** (first draft had it after: 25 mismatches, a load/copy order swap; swapping closes it).
- Identity-matrix store order as in func_80191A98.
- `(short)UH(a2+i*2,0x20) >> 1` reproduces retail's `lhu`/`sll 16`/`sra 17`.
- `x` (the `func_800C2B50` return) typed `unsigned char *` (`lbu 0x44` + `sh 0x4C`).
