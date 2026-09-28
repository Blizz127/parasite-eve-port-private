# vendor2 lane — multi-function khasinski TUs (2026-09-28)

Upstream https://github.com/khasinski/parasite-eve-decomp @ `3067919e7b82f0334b873117eef07e7157f4ed69`, attribution khasinski (Chris Hasinski); imported under the owner's 2026-09-28 authorization. Every file carries the `vendor-derived:` header; inline-GTE bodies also carry the `inline GTE/asm` flag; Psy-Q libgte bodies (per `configs/USA/psyq_classification.tsv`) carry `psyq libgte: ... matching build only`.

## Finding

The 13 "byte-mismatch" and 4 "multi_function_tu" EXE rows, and the four overlay rows, were **not** real code mismatches. The earlier lane compiled the whole vendor TU and linked it with the target function placed at its retail VMA. Two things went wrong. The extra functions made the object larger than the span (`word mismatches=0`, oversized). And local `j` targets are relocated against the section start, which moved every in-function `j` by the function's offset inside the TU (for example, the room_m0273i `j .L80193F14` read as `0x8019406C`, off by 0x158). None of these TUs has a real cross-function jump: in retail the `j` stays inside its own function.

Fix: `build/lanes/vendor2/split_fn.py` keeps one function definition and turns every other definition in the adapted TU into a prototype. Each function is then carved as its own `c` span at the vendor symbol boundaries, so the deep-interior-return guard is satisfied without change. One more fix: in `func_80193CB8`, adapt.py had put a wrong `asm("func_80193CB8")` label on a two-declarator `extern`. It was removed by hand. The function never references it, and the file was re-verified as LINK_EXACT.

## EXE (private config build/lanes/vendor2-exe/, all `era_o2_g0` default)

Commands: `python3 tools/build/disc1_preflight.py --config build/lanes/vendor2-exe/disc1.yaml --profiles build/lanes/vendor2-exe/disc1_build_profiles.json --deep --only <24 funcs>` -> `disc1_preflight: PASS`; `python3 tools/analysis/leaf_strong_check.py --config ... <24 funcs>` -> 24x LEAF_STRONG=EXACT. profile_necessity.py has no --config (skipped; default profile, no assignment).

| func | span | inline GTE | lib | vendor path / symbol | strong |
|---|---|---|---|---|---|
| func_8003F758 | 0x2ff58+0x40 | yes | game | src/main/gpu/Gte_LightingState.c `Gte_SetBackColor` | span=0x40 vram=0x8003F758 compiled=0x40 failures=[none] |
| func_8003F798 | 0x2ff98+0x68 | yes | game | src/main/gpu/Gte_LightingState.c `Gte_SetLightColor` | span=0x68 vram=0x8003F798 compiled=0x70 failures=[none] |
| func_800661A4 | 0x569a4+0x28 | yes | game | src/main/render/Render_GteScreenOffset.c `Render_SetGteScreenOffset` | span=0x28 vram=0x800661A4 compiled=0x30 failures=[none] |
| func_800661CC | 0x569cc+0x20 | yes | game | src/main/render/Render_GteScreenOffset.c `Render_ResetGteScreenOffset` | span=0x20 vram=0x800661CC compiled=0x20 failures=[none] |
| func_80078344 | 0x68b44+0x50 | yes | libgte | src/main/psyq/libgte/average.c `LoadAverage12` | span=0x50 vram=0x80078344 compiled=0x50 failures=[none] |
| func_80078394 | 0x68b94+0x50 | yes | libgte | src/main/psyq/libgte/average.c `LoadAverage0` | span=0x50 vram=0x80078394 compiled=0x50 failures=[none] |
| func_800783E4 | 0x68be4+0x88 | yes | libgte | src/main/psyq/libgte/average.c `LoadAverageShort12` | span=0x88 vram=0x800783E4 compiled=0x90 failures=[none] |
| func_8007846C | 0x68c6c+0x88 | yes | libgte | src/main/psyq/libgte/average.c `LoadAverageShort0` | span=0x88 vram=0x8007846C compiled=0x90 failures=[none] |
| func_800784F4 | 0x68cf4+0x60 | yes | libgte | src/main/psyq/libgte/average.c `LoadAverageByte` | span=0x60 vram=0x800784F4 compiled=0x60 failures=[none] |
| func_80078554 | 0x68d54+0x7C | yes | libgte | src/main/psyq/libgte/average.c `LoadAverageCol` | span=0x7C vram=0x80078554 compiled=0x80 failures=[none] |
| func_80079034 | 0x69834+0x28 | yes | libgte | src/main/psyq/libgte/pipeline.c `LightColor` | span=0x28 vram=0x80079034 compiled=0x30 failures=[none] |
| func_8007905C | 0x6985c+0x28 | yes | libgte | src/main/psyq/libgte/pipeline.c `DpqColorLight` | span=0x28 vram=0x8007905C compiled=0x30 failures=[none] |
| func_80079084 | 0x69884+0x3C | yes | libgte | src/main/psyq/libgte/pipeline.c `DpqColor3` | span=0x3C vram=0x80079084 compiled=0x40 failures=[none] |
| func_800790C0 | 0x698c0+0x24 | yes | libgte | src/main/psyq/libgte/pipeline.c `Intpl` | span=0x24 vram=0x800790C0 compiled=0x30 failures=[none] |
| func_800790E4 | 0x698e4+0x28 | yes | libgte | src/main/psyq/libgte/pipeline.c `Square12` | span=0x28 vram=0x800790E4 compiled=0x30 failures=[none] |
| func_8007910C | 0x6990c+0x28 | yes | libgte | src/main/psyq/libgte/pipeline.c `Square0` | span=0x28 vram=0x8007910C compiled=0x30 failures=[none] |
| func_80079134 | 0x69934+0x20 | yes | libgte | src/main/psyq/libgte/pipeline.c `AverageZ3` | span=0x20 vram=0x80079134 compiled=0x20 failures=[none] |
| func_80079154 | 0x69954+0x24 | yes | libgte | src/main/psyq/libgte/pipeline.c `AverageZ4` | span=0x24 vram=0x80079154 compiled=0x30 failures=[none] |
| func_80079178 | 0x69978+0x58 | yes | libgte | src/main/psyq/libgte/pipeline.c `OuterProduct12` | span=0x58 vram=0x80079178 compiled=0x60 failures=[none] |
| func_800791D0 | 0x699d0+0x58 | yes | libgte | src/main/psyq/libgte/pipeline.c `OuterProduct0` | span=0x58 vram=0x800791D0 compiled=0x60 failures=[none] |
| func_80079228 | 0x69a28+0x18 | yes | libgte | src/main/psyq/libgte/pipeline.c `Lzc` | span=0x18 vram=0x80079228 compiled=0x20 failures=[none] |
| func_800CE8F0 | 0xbf0f0+0xE4 | yes | game | src/main/engine/FieldEng_MatrixVectors.c `FieldEng_TransformMatrixPoint` | span=0xE4 vram=0x800CE8F0 compiled=0xF0 failures=[none] |
| func_800CE9D4 | 0xbf1d4+0x114 | yes | game | src/main/engine/FieldEng_MatrixVectors.c `func_800CE9D4` | span=0x114 vram=0x800CE9D4 compiled=0x120 failures=[none] |
| func_800CEAE8 | 0xbf2e8+0xA4 | yes | game | src/main/engine/FieldEng_MatrixVectors.c `FieldEng_RotateVector` | span=0xA4 vram=0x800CEAE8 compiled=0xB0 failures=[none] |

## Overlays (main configs, under build/lanes/ovl.lock)

Per target: `disc1_preflight.py --target <id> --deep --only ...` PASS, then `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <id>`.

| target | func | span | profile | vendor path |
|---|---|---|---|---|
| room_m0273i | func_80193CB8 | 0x4CD0+0x278 | era_o2_g0 | src/overlays/room_m273/RoomEffect_ThresholdModelTriple.c |
| room_m0350i | func_8019421C | 0x5234+0x2D4 | era_o2_g0_expand_div | src/overlays/room_m350/RoomEffect_ModelShellLayers.c |
| room_m0350i | func_80196A14 | 0x7A2C+0x250 | era_o2_g0 | src/overlays/room_m350/RoomEffect_DualBallistic.c |
| room_m0350i | func_80197B98 | 0x8BB0+0x210 | era_o2_g0_expand_div | src/overlays/room_m350/RoomEffect_RadialBurst.c |

Gate lines:

* `EXACT_REBUILD_GATE[room_m0273i]=PASS plan=4fdc6b8abef8f2fa6b0e4cb3207731c433d11a61a6685cc13528b63289823d97 yaml=5c6cd4120808546388c10eb504e402c928e42210fc920fb499ff3e342f46a67a spans=[121 c, 8 asm, 1 rodata, 2 bin] sha1_orig=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1_cand=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a`
* `EXACT_REBUILD_GATE[room_m0350i]=PASS plan=d3b959629a0aa58870d675e0ca920ceb9e256dc0ecd669850a1bdcda54e317f9 yaml=4fd1a27449feaac4990bd2d5f99481280b6f8fa2df7e04c04ca97763f3d373cc spans=[111 c, 4 asm, 1 rodata, 2 bin] sha1_orig=e9597b000d287033f981cee44f3db65deb8696e1 sha1_cand=e9597b000d287033f981cee44f3db65deb8696e1 sha1=e9597b000d287033f981cee44f3db65deb8696e1`

`MASPSX_EXPAND_DIV` is load-bearing for func_8019421C and func_80197B98 (without it: first mismatch 0x80194344 / 0x80197cb8). This matches the vendor `--expand-div` marker.
