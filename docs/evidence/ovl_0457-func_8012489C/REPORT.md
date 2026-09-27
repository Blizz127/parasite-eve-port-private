# ovl_0457 func_8012489C

* Overlay: `ovl_0457` (PE.IMG overlay, VRAM base 0x80120D00)
* Blob offset `0x3B9C`, VRAM `0x8012489C`, size `0x208` (130 words)
* Source: `src/overlays/ovl_0457/func_8012489C.c`
* Profile: `era_o2_g0` (default)

## What it does

Credits init: display reset (`func_80074A44(1)`, `func_80074BB8(0)`,
`func_80074D28(0)`), ClearImage of the full 0x3FF x 0x1FF VRAM rect, zeroing of
the credit state words, the two `D_80125B88` frame-buffer pointers
(`D_80011610 + ((D_80093168 - D_80093166) << 11)` and `+ 0x54000`), the XA
stream start at `+ 0x43000` (`func_8010BD4C`), image-header reads through
`D_8012682C` (`func_80071994`/`719C4`/`71964`/`71944`), two
`func_80077A64`/`func_80077AA4` tpage/clut values, both `func_80124F40` layer
inits, then `func_800755F0(&D_800BCE80[D_8009CDDC])` and `func_80074D28(1)`.

## Levers (mismatch ladder 87 -> 21 -> 11 -> 1 -> 0)

* `unsigned short *p = &D_80093168;` keeps the base in `$s0` across calls.
* `volatile` `D_80172CC4`: retail stores it twice.
* The frame pointers are computed from a named `off`/`base` pair (87 -> 21);
  the XA-start argument is written inline as
  `(char *)D_80011610 + (((*p - D_80093166) << 11) + 0x43000)` so the
  constant is added before the base (21 -> 11).
* `register int off asm("$2")` gives retail's `$v0`/`$v1` choice (11 -> 1).
* `lo = base + off; off += 0x54000;` computes the second pointer as
  `off + 0x54000` then `+ base` in place. `base + (off + 0x54000)` is
  reassociated to `(base + 0x54000) + off` (1 -> 0).

## Verification (2026-09-23)

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu python3 tools/analysis/era_link_check.py --target ovl_0457 src/overlays/ovl_0457/func_8012489C.c 0x8012489C 0x208 -O2 -G0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0457
disc1_preflight: PASS (deep, 20 c / 6 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0457
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=fb767f84d01709cab7324f6f519dd63b547740b5d6ffa98e9c857e49577e1680 yaml=e5edc904791fca7ed0b157106ec0622d08882adc775b5a99fc53b0a4a4f80ff0 spans=[20 c, 6 asm, 0 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
```
