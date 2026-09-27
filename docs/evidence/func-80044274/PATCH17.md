# func_80044274 — re-spelling on maspsx patch 17 (2026-09-23, tooling2)

The matched leaf previously needed `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009CF90,D_8009CF94`
plus an alias store `(&D_8009CF8C)[1] = D_8009CF88;` because retail's `la`
of the small-data words is absolute (0x80044360 `lui $a0,0x800A` /
`addiu $a0,-0x3070`) while the store stays gp-relative (`sw $v0,0x220($gp)`).
New maspsx patch 17 (`MASPSX_LA_ABSOLUTE_SMALL_DATA=1`) emits `la` of a
small-data symbol as the absolute pair (ASPSX < 2.80 behaviour), so the
plain spelling `D_8009CF90 = D_8009CF88;` works with no forced symbols.

- Proposed source: `build/fanout/tooling2/drafts/func_80044274.c`
  (differs from `src/func_80044274.c` only in the header comment and that
  one store); lead to copy over `src/func_80044274.c` at merge.
- Proposed profile: `era_o2_g8_la_absolute_small_data` = `-O2 -G8` +
  `MASPSX_LA_ABSOLUTE_SMALL_DATA=1`; the old
  `era_o2_g8_force_d8009cf90_d8009cf94_absolute` profile had only this leaf
  and is removed in the private profiles.

| spelling / profile | mismatches |
|---|---|
| plain store, -O2 -G8, no knob | 57 |
| plain store, force CF90+CF94 absolute | 54 (store goes absolute) |
| plain store, patch 17 | **0 (LINK_EXACT)** |

```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_80044274 --source-override func_80044274=build/fanout/tooling2/drafts/func_80044274.c
  -> disc1_preflight: PASS (deep, 1834 c / 304 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --source-override func_80044274=build/fanout/tooling2/drafts/func_80044274.c func_80044274
  -> LEAF_STRONG=EXACT func_80044274 span=0x1D0 vram=0x80044274 compiled=0x1D0 failures=[none]
```
