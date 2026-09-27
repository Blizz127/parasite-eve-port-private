# ovl_0457 func_80123B1C

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x2E1C`, VRAM `0x80123B1C`, size `0x190`
* Source: `src/overlays/ovl_0457/func_80123B1C.c`
* Profile: `ovl_0457_dispatch_80122FBC_80123134` (local profile in `configs/USA/overlays/ovl_0457_build_profiles.json`:
  `-O2 -G0`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_80122FBC,jtbl_80123134`)

## What it does

Credits font glyph index for an ASCII byte. It has two layouts selected by `D_80172CAC`. Letters and digits are range-mapped. Punctuation goes through a 94-entry switch (`c - 0x21`) whose cases fall through a chain of `i++`, from base `-1`, then `+0x1A` or `+0x5C`.

## Levers

* A single result local with an if/else-if chain and one `return` gives retail's `j end` + shared `andi v0,v1,0xFF` return. Early returns duplicate the tail (71 mismatched words).
* The fallthrough `case X: i++;` chain (one macro used for both switches) reproduces the run of `addiu v1,v1,1`.

The two switch jump tables live in the pre-text region of the blob. They are
now a `[0x22A4, rodata]` span (`asm/overlays/ovl_0457/data/22A4.rodata.s`,
the pool). The build strips the leaf's own cc1 `.rodata` copy after proving
its relocations match the pool block (`strip_dispatch_rodata`; the compiled
object's `.rodata` is 0 bytes after the build). Tooling that made this
possible for overlays: `local_profiles` in an overlay build-profile manifest
(`disc1_plan.py` / `disc1_preflight.py`), and the pool parser accepting
splat's `.word .L<vram>` form (`disc1_build.py`).

## Verification (2026-09-23)

```
$ MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80122FBC,jtbl_80123134 LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80123B1C.c 0x80123B1C 0x190 -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, 22 c / 5 asm / 1 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=8f0a13a963c37dd220b6d7d91e3c903a58f95cdb5db81cb2c82b5fc1f7833bfd yaml=de0dcc67a3595632978e5fa6aa0c2f542d4450293290872af8baa8bdf8b5a07c spans=[22 c, 5 asm, 1 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
