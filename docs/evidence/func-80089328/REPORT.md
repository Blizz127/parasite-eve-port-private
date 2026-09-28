# func_80089328 — EXACT (lane exeE2, 2026-09-28)

AKAO per-tick dispatcher: two channel-group pushes through `func_8008900C`, the voice walk
calling `func_80088980` over 0x11C-stride records, then the `D_8009D2C4` flag handlers.

- VRAM `0x80089328`, file offset `0x79B28`, size `0x3FC` (next row `[0x79F24, c, func_80089724]`).
- Profile: default `era_o2_g0` (cc1 **2.7.2** `-O2 -G0`), no knobs.
- YAML: `- [0x79B28, c, func_80089328]` (replaces `- [0x79B28, asm]`).
- Source: `src/func_80089328.c` (copy `build/lanes/exeE2/drafts/func_80089328.exact.c`).

## Verification (private config build/lanes/exeE2/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeE2/disc1.yaml \
  --profiles build/lanes/exeE2/disc1_build_profiles.json --deep --only func_80089328
  -> disc1_preflight: PASS (deep, 2103 c / 177 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeE2/disc1.yaml \
  --profiles build/lanes/exeE2/disc1_build_profiles.json func_80089328
  -> LEAF_STRONG=EXACT func_80089328 span=0x3FC vram=0x80089328 compiled=0x400 failures=[none]
ERA_CC1_VER=2.7.2 tools/analysis/era_link_check.py src/func_80089328.c 0x80089328 0x3FC -O2 -G0
  -> word mismatches=0
```
`profile_necessity.py` has no `--config` (main yaml only). The leaf uses the default profile.

## Levers / ladder (exeE draft = 40 on this harness; PARK best 32)
1. Drop the `register unsigned int b asm("$17")` pin and the `bt` barrier; plain
   `b = a & t[1].fC & ~m;` -> 32. Only callee-saved swaps are left.
2. One `asm("" : : "r"(v));` in the voice-walk loop body (loop-weighted refs lift `v` over
   `bit` / the `v+0x38` giv: s1 = v, s0 = giv, s2 = bit), plus three `asm("" : : "r"(b));`
   uses just before the second group (lift `b` over `m`/`d`: s1 = b, s2 = m and d) -> 12.
   Each lever alone gives 12 in the final draft, so both are load-bearing.
3. **No `e` local**: write `~(w->fC & (b | m))` and `~(b | m)` inline instead of
   `e = b | m`. The second group's local-alloc then matches (w `$a2`, b|m `$v1`, f4 `$a1`) -> **0**.
