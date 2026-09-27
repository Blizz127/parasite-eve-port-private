# ovl_0457 func_80123CAC

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x2FAC`, VRAM `0x80123CAC`, size `0x15C`
* Source: `src/overlays/ovl_0457/func_80123CAC.c`
* Profile: `ovl_0457_dispatch_801232AC_80123424` (local profile in `configs/USA/overlays/ovl_0457_build_profiles.json`:
  `-O2 -G0`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_801232AC,jtbl_80123424`)

## What it does

Credits glyph width for an ASCII byte. Letters come from the `u8 D_80125B1C[][4]` table, indexed by the character itself (lowercase uses `[c - 0x20][m | 1]`, mode 2 folds to uppercase and uses column 2). Punctuation widths come from two switches whose identical `return K` blocks GCC cross-jumps together.

## Levers

* The table symbol is `D_80125B1C` indexed by the raw character. Declaring the table at its first used row costs an `addiu -0x41`.
* The `'~'` case must be listed with `default`: the tables span 94 entries (`0x21..0x7E`). Without it, the bound becomes `sltiu 32`.

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
$ MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_801232AC,jtbl_80123424 LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_80123CAC.c 0x80123CAC 0x15C -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, 22 c / 5 asm / 1 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=8f0a13a963c37dd220b6d7d91e3c903a58f95cdb5db81cb2c82b5fc1f7833bfd yaml=de0dcc67a3595632978e5fa6aa0c2f542d4450293290872af8baa8bdf8b5a07c spans=[22 c, 5 asm, 1 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
