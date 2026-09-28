# ovl14 lane (2026-09-28) — subsystem overlays

## ovl_0457 func_80124C04 (VRAM 0x80124C04, blob 0x3F04, 0x33C) — EXACT

16x16 4bpp glyph blit (credits text) into the current D_80125B88 frame (BGR24, 960-byte rows),
additive-saturating each nonzero nibble's D_80172C9C palette intensity. Profile `era_o2_g0` (default).
Previously parked (ovl4/ovl10, best 171w).

Mismatch ladder (era_link_check words):
- ovl10 draft (int j index `row[j+2]`): 178
- single row cursor `q` with `q[2]/q[1]/q[0]`, `q += 3` (cc1's loop giv q+1 reproduces retail's two bivs): 64 (regs only)
- block-local `k` inside the pixel macro: 58
- declare `g, r, b` (g first) — global-alloc order gives g=$a0, r=$a1: 18
- outer increments `src += 64, yy++, row += 960`: 16
- `src = D_80172C98; src += …` (load straight into src): 10
- `register unsigned short *src asm("$11")` (src/i allocation swap otherwise): 2
- named `lo = gl & 0xF; hi = gl & 0xF0;` temps (both `andi` before the `sll`s — sched1 reverse-list LUID tie order): **0**

Verify:
- `python3 tools/build/disc1_preflight.py --target ovl_0457 --deep --only func_80124C04` → `disc1_preflight: PASS (deep, 27 c / 0 asm / 1 rodata)`
- `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh ovl_0457` →
  `EXACT_REBUILD_GATE[ovl_0457]=PASS plan=77fa8a7958d1778bf6cc88dc6a7c914b0ed45c3cbc934ba20ea3df7330e79f4a yaml=7b9b8561a546bf2949b061f13518b743183e13de5b42e1a853511823eb1f77ef spans=[27 c, 0 asm, 1 rodata, 2 bin] sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6`

## ovl_0700 func_8018F55C (VRAM 0x8018F55C, blob 0x56C, 0x3D0) — EXACT

Camera-path spline sample. Profile `era_o2_g0_expand_div` (three `%` by `n-2`). Previously parked (ovl8, ~140 diff lines).

Mismatch ladder (words):
- ovl8 park draft: 146
- count read through `volatile unsigned short` (combine can't fold the zero-extend load into `lh` → retail `lhu` + `sll/sra`): 142
- `short ya, yb` (int `ya0`/`yb0` copies; the int→short copy keeps ya0/ya in separate callee-saved regs, no asm barrier): 48
- path pointer and `pts++` in one variable (`p`=`pts` share $t1): 45
- `seg %= m; if (seg < 0) seg += n - 2; i0 = seg;` (seg reuses $a1, m in $a0): 15
- `*(int *)((char *)tab + (idx << 2))` (retail `addu a1,a2,a1` operand order): 14
- `register int sa asm("$2") = ya` for the rot.vz / vy differences: 2
- product `register int pr asm("$3")` (mflo straight into $v1): **0**

Verify:
- `python3 tools/build/disc1_preflight.py --target ovl_0700 --deep --only func_8018F55C` → `disc1_preflight: PASS (deep, 44 c / 8 asm / 1 rodata)`
- `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh ovl_0700` →
  `EXACT_REBUILD_GATE[ovl_0700]=PASS plan=329879e3f085ffb23f727c92539af2ddf4b680875aa6074c32e40d06cea33662 yaml=368a46ebde9a474c81a4d227ed43a4be81e07406f28ebdaf5fa6d8a225ea37e7 spans=[44 c, 8 asm, 1 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de`

## ovl_0700 func_80195F6C (VRAM 0x80195F6C, blob 0x6F7C, 0x52C) — EXACT

Double-buffered POLY_G4 gradient / DR_TPAGE init. Profile `era_o2_g0` (default). Previously parked (ovl8, 395 diff lines).

Lever: the ovl8 draft wrote the field stores through the call-argument pointer (`setRGB0(&D[i], …)` →
`(&D[i])->r0`), so the address was built on the `&D[i]` pseudo and cse'd into `off(base)`.
Writing the stores as array elements (`D[i].r0 = …`) gives `(plus (mult i 36) (const sym+off))`,
which cse1 does not merge with the argument pointer; loop strength reduction then turns `i*36`
into the $s1 giv, yielding retail's `lui $at / addu $at,$at,$s1 / sb %lo(sym+off)($at)`. 395 → **0** words, first try.

Verify:
- `python3 tools/build/disc1_preflight.py --target ovl_0700 --deep --only func_80195F6C` → `disc1_preflight: PASS (deep, 45 c / 8 asm / 1 rodata)`
- `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh ovl_0700` →
  `EXACT_REBUILD_GATE[ovl_0700]=PASS plan=a938c4922252822d1c77a466ef619c48c078547a9a3d6895fead45187fc66c7c yaml=f8b9c95c5c8c7442a1bf7878c89ebc8ea2d9995e2937038bd97d365b12ee52f5 spans=[45 c, 8 asm, 1 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de`
