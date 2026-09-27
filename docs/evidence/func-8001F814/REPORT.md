# func_8001F814 — LINK_EXACT (lane exeA, 2026-09-27) — closes the former NONMATCHING_C leaf

- VRAM `0x8001F814`, file offset `0x10014`, size `0x1B0` (ends at `0x101C4`, the next row `func_8001F9C4`)
- Profile `era_o2_g8_dispatch_800106e4_force_d8009d254_d8009d278_absolute`:
  `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1` (the gate the dispatch fold rides on),
  `MASPSX_DISPATCH_FOLD=jtbl_800106E4`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D278`
- YAML: `- [0x10014, c, func_8001F814]` replaces `- [0x10014, asm]` (span is the whole former asm row)

**Integration note:** `src/func_8001F814.c` was the registered `nonmatching-native-cut`
source (`configs/USA/disc1_nonmatching_sources.json`, evidence
`docs/evidence/pe-btl99-1f814-post-hp/REPORT.md`). The file now holds the matching leaf; the
old draft is preserved at `build/lanes/exeA/drafts/func_8001F814.orig.c`. The
`disc1_nonmatching_sources.json` entry must be dropped when this row is merged. No build or
pc_port file includes the old source (pc_port has its own `func_8001F814` port).

Shape: hit-react handler. Records the hitter's command (`fE`, `fF`, `f14` or `fF << 16`) into
`D_8009D29A/9B/9C` with `D_8009D298 = 1` (jump table over kinds 6..15), then unless
`D_8009D278->f4C & 0x12000` turns the actor by `func_800305C8(a, D_8009D254)` quadrant
(`func_8001A680(D_8009D254, 0/2/1/3)`), spawns effect `0x46A` at the actor, and converts a
pending `f98 & 0x100` into `D_8009D298 = 2`. Returns the angle as `short`.

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_8001F814
  disc1_preflight: PASS (deep, 2050 c / 213 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_8001F814
  LEAF_STRONG=EXACT func_8001F814 span=0x1B0 vram=0x8001F814 compiled=0x1B0 failures=[none]
```
`profile_necessity.py` has no `--config` — skipped.

## Levers (mismatch ladder)

1. The old blocker ("jump table in the 0x800 rodata pool") is gone: `MASPSX_DISPATCH_FOLD`
   retargets cc1's `$L` table to `jtbl_800106E4`, **but only together with
   `MASPSX_THREE_WORD_SYMBOL_STORE=1`** (the fold is applied inside that gate; fold alone
   leaves the local table — `lui 0x8002`).
2. Fresh decompile: retail re-reads `D_8009D254` in every case body (the old draft cached it)
   → 86.
3. **Aliasing classes**: `D_8009D29A/9B/9C` as array elements (`[0]`, MEM_IN_STRUCT) so the
   `fF`/`f14` actor loads stay after the byte stores → 20; `D_8009D298` as `short[1]` (still
   `$gp`-relative, now also in-struct), `register int cmd asm("$2")` for the quadrant
   (retail copies it to `$a1` in the `jal` slot), `int r` with a `short` return type → 14.
4. **Store order `D_8009D29A, D_8009D29B, D_8009D298`** in both case bodies (brute-forced
   over the 12 order × declaration combinations) → 0.
