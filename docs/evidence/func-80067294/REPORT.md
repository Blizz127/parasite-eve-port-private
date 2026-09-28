# func_80067294 — EXACT (lane exeH, 2026-09-28)

- VRAM `0x80067294`, file offset `0x57A94`, size `0x3E4` (249 words). Sprite-entry renderer: `e->cells`
  words give per-cell x/y/z; with flag bit 2 the position wraps modulo `e->w`/`e->h`; visible cells get
  a SPRT16 x/y and two `addPrim`s into `D_800B0E38[D_8009CDDC]`.
- Source `src/func_80067294.c`. Profile `era_o2_g0_expand_div` (existing: stock cc1 2.7.2 `-O2 -G0` +
  `MASPSX_EXPAND_DIV=1`).
- YAML: `- [0x57A94, asm]` → `- [0x57A94, c, func_80067294]` (the span ends exactly at the next row,
  `0x57E78 func_80067678`, so there is no resume row).

## Verification (private config build/lanes/exeH/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeH/disc1.yaml --profiles build/lanes/exeH/disc1_build_profiles.json --deep --only func_80067294
  disc1_preflight: PASS (deep, 2141 c / 150 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeH/disc1.yaml --profiles build/lanes/exeH/disc1_build_profiles.json func_80067294
  LEAF_STRONG=EXACT func_80067294 span=0x3E4 vram=0x80067294 compiled=0x3F0 failures=[none]
python3 build/lanes/exeH/pn.py --profiles build/lanes/exeH/disc1_build_profiles.json --only func_80067294   # profile_necessity on the private yaml
  PROFILE_NECESSITY=PASS
MASPSX_EXPAND_DIV=1 python3 tools/analysis/era_link_check.py src/func_80067294.c 0x80067294 0x3E4 -O2 -G0
  linked .text 1008 bytes, target 0x3e4, word mismatches=0, nonzero_pad=0
(without MASPSX_EXPAND_DIV: word mismatches=200)
```
(compiled 0x3F0 = 0x3E4 + gas pad.)

## Levers (aligned difflib lines against retail; the start was exeC4's clean draft at 108)
1. **Wrap math through int temps, reading `e->w`/`e->h` fresh**:
   `{ int tx = (short)sx - 320; x = (tx + e->w) % e->w; }` (same for y), then `x -= e->w - 320;`.
   Retail computes `((short)sx - 320) + w`. The one-expression form let fold reassociate it into
   `sx + (w - 320)`, and cse then shared `w - 320` with the later `x -= e->w - 320` (108 → 78; frame back to 24).
2. **Per-loop block-local `short xx, yy, zz;` and `unsigned int *o`**. As function-level variables shared by
   both loops they were global pseudos (`o` had 24 refs, the top global priority, and took `$4` ahead of
   everything else), which pushed the whole t-bank one register up. Local to each loop they are local-alloc
   qtys, and every register lines up with retail (78 → 70 with `o` alone, → 2 with all four).
3. **`o = (unsigned int *)((int)zz * 4 + (int)ot)`**. `&ot[zz]`, `ot + zz`, `zz + ot` and the byte-pointer
   form all emit `addu $4,$17,$4`. Retail's `addu $4,$4,$17` (the scaled index first) needs the integer sum
   in that order (2 → 0).
