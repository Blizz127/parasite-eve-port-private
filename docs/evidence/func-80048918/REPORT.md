# func_80048918

- VRAM `0x80048918`, file offset `0x39118`, size `0x60C` (387 words)
- Profile: `era_o2_g8_force_d800c0e10_d800a1960_absolute` (`-O2 -G8`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E10,D_800A1960`); cc1 2.7.2
- Source: `src/func_80048918.c`
- Carved from the former `[0x39118, asm]` span (it ran exactly to `func_80048F24` at `0x39724`).

## Verification (decompC private config, session 2026-09-24)

```
python3 tools/build/disc1_preflight.py --config build/fanout/decompC/disc1.yaml --profiles build/fanout/decompC/disc1_build_profiles.json --deep --only func_80048918
disc1_preflight: PASS (deep, 1961 c / 255 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompC/disc1.yaml --profiles build/fanout/decompC/disc1_build_profiles.json func_80048918
LEAF_STRONG=EXACT func_80048918 span=0x60C vram=0x80048918 compiled=0x610 failures=[none]
python3 tools/analysis/era_link_check.py src/func_80048918.c 0x80048918 0x60C -O2 -G8  (+ the two forced-absolute symbols) -> LINK_EXACT
```

## What it does

Inventory-root menu builder (khasinski TU name `Menu_StepInventoryRoot`): looks up the
item record for `a1` (`func_8005332C`), builds the widget tree with the
`func_80062D2C` (create) / `func_8006322C` (attach) / `func_80063158` (position) API,
installing per-widget handlers at `+0x2C`/`+0x30`, seeds the gp-relative menu state
(`D_8009CFC4 = a1`, `D_8009CF34 = (a1 == -2)`, `D_8009CF38 = (a1 == -3)`, `D_8009CFD4`,
`D_8009CF18 = (a0 != 0x200)`, `D_8009CFC8/CFCC` from the item record's `+14 & 3` / `+12`),
optionally pre-selects the list row whose `func_800556E8(i)` equals `a2`, fills the
7-entry `D_800A18D8`/`D_800A18B4`/`D_800A18FC` tables from `D_800C0E28` via
`func_8005B91C`, and finally dispatches on the root widget's `+0x24` kind (48/54 →
`func_80049008`, else `func_80048F24`).

## Load-bearing levers (mismatch ladder: link error -> 54 -> 13 -> 0)

1. `-G8` with **`D_800C0E10` and `D_800A1960` forced absolute** (both are addressed with
   `lui` in retail; under plain `-G8` the link aborts with `small-data section too large`).
2. **Struct-member widget fields** (`w->f2C = func_…`, `o->f44`, …) instead of the sibling
   leaves' `*(void **)(w + 0x2C)` casts: retail hoists the gp loads of `D_8009CF30`/`D_8009CF1C`
   *above* the preceding handler-pointer stores and drops the last store into the branch
   delay slot — only possible when the stores are `MEM_IN_STRUCT_P` (2.7's alias rule).
3. **Three distinct loop variables**: the row search (`i`, lands in `$s0`, its `i = 0` fills the
   `bltz a2` delay slot), the 7-entry table loop (`j`, `$s1`) and the countdown clear
   (`k`, a temp `$a0`); one shared `i` costs 9 words and a different frame.
4. **Explicit four-pointer table loop** (`*pd = *ps++; *pf = 0; func_8005B91C(j, *pd, pb, 0);
   pd++; pb++; pf++;`) with `unsigned short D_800C0E28[]` (retail `lhu`) — indexed
   `&D_800A18B4[j]` is not strength-reduced by cc1 (`sll/addu` per iteration).
5. **`j = 0` initialised between `ps` and the other pointers** (`for (j = 0, pd = …, pb = …,
   pf = …; …)`): flips the `j`/`pd` allocation to retail's `$s1`/`$s0` (last 13 words).
6. `neg = (unsigned int)o->f44 >> 31` for retail's `srl s0,s0,31` sign test.
