# func_801914A0 — room_m0348i (M34 alligator boss, phase 2)

- Target: `room_m0348i` (PE.IMG room m0348i chunk 2, VRAM base 0x8018EFE8)
- VRAM 0x801914A0, blob offset 0x24B8, size 0x448 (ends on its `jr $ra` + delay slot at 0x801918E4)
- Role: fire-breath beam per-segment update. For each of 16 segments (`a2+i*8`, fields
  `+0x60/+0x62/+0x64`) it lowers y by 150. The segment whose index equals `a1->h2 - a2->h182`
  checks `*func_800C2B28(7) == 1`; if so it kills the segment's live byte (`a2+i`) and bumps
  `a2->h184`. On the first bump (`== 1`) it builds `RotMatrix({0x20, a2->h180, 0})` with zero
  translation, composes a translated identity through it and through the effect matrix at
  `a2+0x160`, and writes the world position into the slot returned by
  `func_800C2B90(a0, 1, D_801928F0, D_80192848)` (if non-null). After the loop it sets
  `a1->b1 = 2` once `a1->h2 >= 121`.
- Source: `src/overlays/room_m0348i/func_801914A0.c`, hand-written in this repo. The vendored
  khasinski tree has this body as asm only (`room_m348.yaml`: `[0x24B8, asm, func_801914A0]`).
- **Inline GTE**: the source uses the same Psy-Q `inline_c.h`-shaped macros as func_80190E98
  (`gte_CompMatrix` twice). Count it separately from plain C.
- Profile: `era_o2_g0` (overlay default, no assignment, no maspsx knobs)
- YAML: `[0x24B8, asm]` became `[0x24B8, c, func_801914A0]`. room_m0348i now has no asm spans
  left (45 c / 0 asm / 2 bin).
- Masked-body twin: room_m0174i func_801925B8 (offset 0x35D0). It is identical apart from
  relocated immediates and jal targets, and has not been ported.

## Verification

```
$ TMPDIR=$PWD/build/tmp python3 tools/analysis/era_link_check.py --target room_m0348i src/overlays/room_m0348i/func_801914A0.c 0x801914A0 0x448
linked .text 1104 bytes, target 0x448, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --target room_m0348i --deep --only func_801914A0
disc1_preflight: PASS (deep, 45 c / 0 asm / 0 rodata)
$ flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh room_m0348i
EXACT_REBUILD_GATE[room_m0348i]=PASS plan=7bd5bf56483774d2bd1ce5f701db35f4ef788c377036af3e8b5eb5c3991ba2b0 yaml=11c270c64ed52db2ed7003600cb15c005b48d6063dd81a44ed682a2a4f988881 spans=[45 c, 0 asm, 0 rodata, 2 bin] sha1_orig=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1_cand=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1=49478531f4b705ab248a8c2eb0a3b81c4cef9985
```

(The 1104-byte linked size is the 0x448 body plus the gas 16-byte alignment pad, and the pad is zero.)

- `sha256(configs/USA/overlays/room_m0348i.yaml)` = `11c270c64ed52db2ed7003600cb15c005b48d6063dd81a44ed682a2a4f988881`.
  This supersedes the `3b11c318…` hash recorded in the func_80190E98 / func_80191A98 /
  func_80191FE0 reports. The gate above also covers those three functions.
- `sha256(configs/USA/overlays/room_m0348i_build_profiles.json)` = `91719eb7e814257ac89324325a31c3639689cd19d26a901483f74097041d7536` (unchanged)

## Levers

- The first draft matched, with 0 mismatched words. It reused two things from the earlier lane
  leaves: the identity-matrix store order (the three diagonal `0x1000` stores in reverse order,
  then the chained `t` zero, then the chained off-diagonal zero) and the `gte_CompMatrix` macro.
- `unsigned int i` gives `sltiu`. The per-field offset macros (`SH(a2 + i*8, 0x64)`,
  `SB(a2 + i, 0)`) give retail's two pointer induction variables (`$s0` +8, `$s7` +1).
- `++SH(a2,0x184) == 1`: cc1 reuses the `$v1` that already holds the `== 1` value from the
  `func_800C2B28` result check, as retail does.
