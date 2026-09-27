# func_800236E8 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x800236E8`, file offset `0x13EE8`, size `0x72C` (ends at `0x14614`, the next row `func_80023E14`)
- Profile `era_o2_g8_force_d8009d278_d8009d254_d8009d20c_d8009d1ce_d8009d1f8_absolute`:
  `-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D278,D_8009D254,D_8009D20C,D_8009D1CE,D_8009D1F8`
- YAML: `- [0x13EE8, c, func_800236E8]` inserted after `- [0x1392C, asm]` (the asm row keeps `func_8002312C`, `0x1392C..0x13EE8`)

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_800236E8
  disc1_preflight: PASS (deep, 2045 c / 216 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_800236E8
  LEAF_STRONG=EXACT func_800236E8 span=0x72C vram=0x800236E8 compiled=0x730 failures=[none]
```
`compiled=0x730` is a 4-byte zero gas pad (`nonzero_pad=0`). `profile_necessity.py` has no
`--config` (main config only) — skipped. Manual necessity: `-O2 -G8` without the
force-absolute list → 420 word mismatches; `-O2 -G0` → 442.

## Levers

Built on the parked decompA draft (296 words, a one-instruction shift in the `m->f6 == 6`
branch: retail `lbu 0xE5($gp)` + `sll/sra` into `$s2`, `li 1` after the `lb` mode byte, and a
`sll/sra` re-truncation of `$s2` before the `func_80021278` call in the loop).

The closing lever is **declaring the hit-count local `short k`** (it was `int k`): the value is
read from the `signed char` `D_8009CE54.f1`, and a `short` home makes cc1 keep the byte
load as `lbu`+`sll 24`/`sra 24` and re-narrow it (`sll 16`/`sra 16`) for the `short` third
argument inside the loop — exactly retail. Ladder: `int k` → 296; `int k` + volatile
`unsigned char` read → 265 (fixes the load shape only); `k` reused as the `short lo`
local → 11; `short c` → 4; **`short k`, plain `k = D_8009CE54.f1`** → 0.
