# func_80014BA0 — matched (vendor-derived, 2026-09-28)

- VRAM `0x80014BA0`, file offset `0x53A0`, size `0x200`.
- Source: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69
  `src/main/main/Camera_StepAngleFade.c`, imported under the owner's authorization
  (provenance header in `src/func_80014BA0.c`, upstream tree in `vendor/khasinski-parasite-eve-decomp/`).
- Supersedes `PARK.md` (lane exeA's own draft stopped at 104 word mismatches).
- Gate (batch 17, with `func_8001220C`):
  `TMPDIR=$PWD/build/tmp bash scripts/exact_rebuild.sh` →
  `EXACT_REBUILD_GATE=PASS plan=be6eb4d4a58b… yaml=5c951c8272d6… spans=[2144 c, 147 asm, 2 rodata]
  sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, `VERIFY_SWEEP=PASS leaves=2144`.
