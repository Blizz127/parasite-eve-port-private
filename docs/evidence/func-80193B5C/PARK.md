# func_80193B5C (ovl_0700, blob 0x4B6C, 0x5AC) — PARKED (ovl14, 2026-09-28), first draft

Map marker overlay: fade value (`short` param, returned) steps ±8 toward ±0xFF, then per
marker (10) a LINE_F2 + two POLY_GT4 labels (RotTransPers-style func_80079274 projection,
SetSemiTrans, GetClut/GetTPage, addPrim into ot[10]/ot[9]). Structure is decoded in
`build/lanes/ovl14/drafts/park_func_80193B5C.c` (352 words; frame 0xD0 vs 0xA0).
Residual: register-pressure/spill set. Retail spills fade (0x50), i (0x58), `lvl<<16`
(0x60), `lvl>>3` byte (0x68), `i*16` (0x70) and rematerialises the hoisted 0xFF000000 / 0x80 /
0x7F constants via `$t3`; the draft keeps 0x80 in `$s6`, spills lvl as a HImode slot and the
`i*13(+12)` / `i*52` givs instead. Next step: match loop.c movables (threshold) so the same
constants hoist, then global-alloc priorities.
