# vendor3 lane — re-survey of remaining asm against khasinski C, one function at a time (2026-09-28)

Upstream khasinski/parasite-eve-decomp @ `3067919e7b82f0334b873117eef07e7157f4ed69`, attribution khasinski (Chris Hasinski). Every file carries the `vendor-derived:` header; inline asm/GTE bodies carry the `inline GTE/asm` flag; Psy-Q libgte bodies carry `psyq libgte: ... matching build only`.

## Method

Survey: every function (our `asm/` split labels) still inside an `asm` span is placed in the vendor layout. For the EXE this uses `main.yaml` rows at the same file offset. For overlays it uses byte identity against every vendor overlay bin at the same VRAM, which is the gap2.py method. Each vendor `c` hit is adapted (adapt.py) and reduced to that one function (split_fn.py), then link-checked on a flag/knob ladder. Scripts: `build/lanes/vendor3/{survey_exe,survey_ovl,try,exe_install}.py`.

The first pass wrongly rejected some bodies. This pass fixed four causes:

1. **Whole-TU compile.** Every function in the TU was compiled together, so the object was oversized and local `j` targets were off by the function's offset in the TU.
2. **Assembler warnings read as failure.** `Warning: used $at` lines on stderr landed after `LINK_EXACT`, so exact functions looked failed (func_800370BC, func_800370CC with the trapping-add inline asm).
3. **Unresolved libc names.** Psy-Q libc BIOS trampoline names (`rand`, `memset`, ...) had no symbol in this repo. They are now mapped to this repo's `func_` addresses (func_800D868C, func_800D927C, func_800D96F4).
4. **Missing static inline helpers.** split_fn now keeps `static inline` helper definitions (func_800DB6BC).
5. **Vendor names kept on alias collisions.** adapt.py sometimes kept the vendor name on a definition because two names shared an address. The definition is now renamed to ours (room_m0186i func_801918A8, room_m0412i func_80192DA8).

Local levers on the vendor GNU-as route (no new knobs):

* `RotMatrix`, `RotMatrixYXZ`, `RotMatrixZYX`, `RotMatrixY`, `RotMatrixZ`: the trailing `asm volatile("nop")` becomes an empty asm barrier, on profile `era_o2_g0_symbol_load_dest_temp` (patch 4). Without the knob they mismatch by 18 or 6 words; with the knob but the original nop, by 2 words.
* StSetRing (func_8007A214): `era_o2_g0_fill_call_delay_slot`. Without it, 6 words mismatch.
* Our splat had wrong boundaries in two places: func_8007A1E0 (0x60) = St_Dma handler template data words + StSetRing @0x8007A214 (0x2C), and func_8007E3B4 (0x24) = 0x10 function + pad + vendor-asm tail. Only the real functions were carved.

## EXE (private config build/lanes/vendor3-exe/, `results.jsonl` in integrate.py format)

`disc1_preflight.py --config build/lanes/vendor3-exe/disc1.yaml --profiles ... --deep --only <35>` -> `disc1_preflight: PASS (deep, 2241 c / 144 asm / 2 rodata)`; `leaf_strong_check.py` 35x LEAF_STRONG=EXACT. integrate.py sandbox run reproduces the private yaml/profiles exactly.

| func | span | profile | inline asm | lib | vendor path |
|---|---|---|---|---|---|
| func_800370BC | 0x278BC+0x10 | era_o2_g0 | yes | game | src/main/math/math_fixed.c |
| func_800370CC | 0x278CC+0x10 | era_o2_g0 | yes | game | src/main/math/math_fixed.c |
| func_80072714 | 0x62F14+0x10 | era_o2_g0 | yes | libapi | src/main/psyq/libapi/critical_section.c |
| func_80072724 | 0x62F24+0x10 | era_o2_g0 | yes | libapi | src/main/psyq/libapi/critical_section.c |
| func_80078EB4 | 0x696B4+0x10 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078EC4 | 0x696C4+0x10 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078ED4 | 0x696D4+0x10 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078EE4 | 0x696E4+0x20 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F04 | 0x69704+0x14 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F18 | 0x69718+0x14 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F2C | 0x6972C+0xC | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F38 | 0x69738+0x14 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F4C | 0x6974C+0x18 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F64 | 0x69764+0x14 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F78 | 0x69778+0x14 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078F8C | 0x6978C+0x14 | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078FA0 | 0x697A0+0xC | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078FAC | 0x697AC+0xC | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80078FB8 | 0x697B8+0xC | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg03.c |
| func_80079024 | 0x69824+0xC | era_o2_g0 | yes | libgte | src/main/psyq/libgte/reg13.c |
| func_800794C4 | 0x69CC4+0x28C | era_o2_g0_symbol_load_dest_temp | yes | libgte | src/main/psyq/libgte/RotMatrix.c |
| func_80079754 | 0x69F54+0x28C | era_o2_g0_symbol_load_dest_temp | yes | libgte | src/main/psyq/libgte/RotMatrixYXZ.c |
| func_800799E4 | 0x6A1E4+0x28C | era_o2_g0_symbol_load_dest_temp | yes | libgte | src/main/psyq/libgte/RotMatrixZYX.c |
| func_80079C74 | 0x6A474+0x198 | era_o2_g0_symbol_load_dest_temp | yes | libgte | src/main/psyq/libgte/RotMatrixY.c |
| func_80079E14 | 0x6A614+0x198 | era_o2_g0_symbol_load_dest_temp | yes | libgte | src/main/psyq/libgte/RotMatrixZ.c |
| func_8007A214 | 0x6AA14+0x2C | era_o2_g0_fill_call_delay_slot |  | game | src/main/psyq/libcd/StSetRing.c |
| func_8007E3B4 | 0x6EBB4+0x10 | era_o2_g0 |  | libcard | src/main/memcard/CardPatchFunctions.c |
| func_800CF658 | 0xBFE58+0xA0 | era_o2_g0 | yes | game | src/main/engine/Gte_BuildAndLoadRotationMatrix.c |
| func_800CFB7C | 0xC037C+0x178 | era_o2_g0 | yes | game | src/main/engine/FieldEng_RotationOffset.c |
| func_800D4704 | 0xC4F04+0x14C | era_o2_g0 | yes | game | src/main/engine/FieldEng_ObjectLifecycle.c |
| func_800D868C | 0xC8E8C+0x2EC | era_o2_g0 | yes | game | src/main/engine/FieldEng_OrbitingBurst.c |
| func_800D927C | 0xC9A7C+0x2D8 | era_o2_g0 | yes | game | src/main/engine/FieldEng_HelicalEffect.c |
| func_800D96F4 | 0xC9EF4+0x398 | era_o2_g0 | yes | game | src/main/engine/FieldEng_DampedSpark.c |
| func_800DB25C | 0xCBA5C+0x398 | era_o2_g0 | yes | game | src/main/engine/FieldEng_LineDiamond.c |
| func_800DB6BC | 0xCBEBC+0x3E0 | era_o2_g0 | yes | game | src/main/engine/FieldEng_DiamondEmitter.c |

## Overlays (under build/lanes/ovl.lock)

| target | func | span | profile | vendor path |
|---|---|---|---|---|
| room_m0186i | func_801918A8 | 0x28c0+0x240 | era_o2_g0 | src/overlays/room_m186/RoomLib_RotateMotion_801918A8.c |
| room_m0350i | func_801992A0 | 0xa2b8+0x41c | era_o2_g0_expand_div | src/overlays/room_m350/RoomEffect_FadingModel.c |
| room_m0412i | func_80192DA8 | 0x3dc0+0x47c | era_o2_g0_expand_div | src/overlays/scene_e02/RoomLib_HandlerC.c |

Gate lines (yaml= is the sha256 of the committed-candidate yaml):

* `EXACT_REBUILD_GATE[room_m0186i]=PASS plan=f66ecbb17678535be3c4d5403cc2269a6663facc6aa7a87c7204c8cd437275e6 yaml=c1d1286b97da167148a2cc8f278b6c4ed97f6c48bab40cadcfcfbe39361d7c17 spans=[85 c, 0 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e`
* `EXACT_REBUILD_GATE[room_m0350i]=PASS plan=d8a6044b31d8cfb96d0bcc3e89983fd303fa2d24a20935c77e95eb65872f8e9a yaml=e901743826ce8d1b49dbfe72c0f1b65118bbbaa50c6af96f50e3f9d02590ae25 spans=[112 c, 3 asm, 1 rodata, 2 bin] sha1_orig=e9597b000d287033f981cee44f3db65deb8696e1 sha1_cand=e9597b000d287033f981cee44f3db65deb8696e1 sha1=e9597b000d287033f981cee44f3db65deb8696e1`
* `EXACT_REBUILD_GATE[room_m0412i]=PASS plan=93892cb5a330d9d8b7951374e785fc0736c094addb80daf6cc7238393be6d383 yaml=d6c47b4614477619cfaceb52f6fe3b026008ea8ea27a7d5d83ed52cb58ef9e47 spans=[82 c, 3 asm, 1 rodata, 2 bin] sha1_orig=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1_cand=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1=673097c6bd36986925a38e6a12d2b039092ed9c8`

## What remains

EXE: 249 functions still in asm spans at survey time. Vendor `c` 122: 33 exact here (+2 recovered from our mis-bounded splat functions), 52 are Psy-Q BIOS `jr $t2` trampolines (the vendor also emits them as top-level asm via `PSYQ_BIOS_TRAMPOLINE`), 37 are 4-byte alignment pads inside vendor TUs. Vendor `asm` 65, vendor `pad` 57, vendor `data` 1, straddling 4.
Overlays: 228 functions in asm spans. Vendor `asm` 186 rows (184 distinct bodies). Vendor `c` 9: 3 exact, 6 are 4-byte pads. No vendor coverage 32 (ovl_039F, ovl_03C5, ovl_03C9, ovl_03D2, room_m0413i, room_m0429i have no vendor counterpart). Vendor databin 1.
