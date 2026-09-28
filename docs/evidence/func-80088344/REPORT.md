# func_80088344 — LINK_EXACT (lane exeE, 2026-09-27)

- VRAM `0x80088344`, file offset `0x78B44`, size `0x63C` (399 words, no calls).
- AKAO voice update (sibling of func_80088980): three LFO blocks (pitch `0x1C`, volume `0x20`,
  pan `0x24` wave tables), volume/pan mix into `+0x118/+0x11A` through `D_8009B8F8` per
  `D_8009D2C0` output mode, the `fl & 0x800` paired-voice copy/subtract against
  `D_800B8AC0 + U16(0x5C)*0x11C`, and the pitch write to `+0x10C` scaled by `D_8009D2CE`.
- Profile: default `era_o2_g0` (cc1 2.7.2 `-O2 -G0`; retail's 32-byte empty frame is the
  2.7.2 temp-slot signature). No assignment needed.
- YAML: `- [0x78B44, c, func_80088344]` + resume `- [0x79180, asm]` (func_80088980, still asm).

## Verify (private lane config build/lanes/exeE/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeE/disc1.yaml --profiles build/lanes/exeE/disc1_build_profiles.json --deep --only func_80088344
  disc1_preflight: PASS (deep, 2089 c / 190 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeE/disc1.yaml --profiles build/lanes/exeE/disc1_build_profiles.json func_80088344
  LEAF_STRONG=EXACT func_80088344 span=0x63C vram=0x80088344 compiled=0x640 failures=[none]
tools/analysis/era_link_check.py src/func_80088344.c 0x80088344 0x63C -O2 -G0
  word mismatches=0
```
Default profile, so profile_necessity has nothing to assign (its `--config` is not supported
for a private yaml; the lead's gate covers it).

## Levers / ladder
| variant | words |
|---|---|
| structural draft from the func_80088980 shape, separate `r`/`pan`/`k` | 368 |
| one variable for the LFO results **and** `pan` (retail `$a3` throughout; the case-4 temp stays separate) | 368 → layout identical, regs aligned |
| block 1 sample as a named local `{ int s = *PTR(0x1C)++; pan = (U16(0x92) * s) >> 16; }` | 12 |
| block 2 intermediate assigned to the same variable (`pan = ((vol*(U16(0xA6)>>8))<<9)>>16; pan = (pan * *PTR(0x20)++) >> 15;`) and the `D_8009D2CE` byte also read into it | **0** |

Durable lever: **one reused C variable is one global pseudo** — retail's `$a3` shared by the
LFO results, the block-2 intermediate, pan and the pitch-scale byte means the original
reused one local for all of them; separate locals leave `vol` in `$a3`/`$t0` and shift
`fl`/`mflo` to `$t2`/`$t3` (368 words).
