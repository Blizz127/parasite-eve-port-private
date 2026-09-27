# func_800323C8 — sprite record from pool-initialized local tables (0x800323C8, 0x214)

Status: **matched** (tooling2, 2026-09-24), `LINK_EXACT` + `LEAF_STRONG=EXACT` on the live install.

- YAML: `- [0x22BC8, c, func_800323C8]` (after `[0x2285C, asm]`; ends exactly at `[0x22DDC, c, func_800325DC]`).
- Profile: `era_o2_g0_rodata_fold_800323c8` = `-O2 -G0` + `MASPSX_RODATA_FOLD=D_80010DFC,D_80010E10,D_80010E24`.
- Tooling: maspsx patch 20 (`$LC<n>` -> pool symbol) + `disc1_build.strip_rodata_fold`
  (strips cc1's 60-byte duplicate .rodata only after proving it byte-identical to the
  three pool blocks and that no relocation still points at it).

## Shape
Three initialized `unsigned char[18]` locals are block-copied (lwl/lwr) from the
EXE rodata pool, indexed by `a0 * 2`, and written into a 28-byte record of
`D_8009E928` = 8 bytes + a SPRT at +8 (u0/v0, w/h, x0/y0 = D_8009E360[k] + table),
then `func_80077AC4(D_800B0E38[k] + 16, &D_8009E928[k])`.

## Ladder (era_link_check, patch 20 on where noted)
| variant | words |
|---|---|
| flat 28-byte struct, direct indexing | 73 |
| + row pointers p/q/r, local k after the byte stores | 69 / 52 |
| + patch 20 (the 3 `la $LCn` pairs) | 46 |
| + SPRT-at-+8 typing (`SPRT *s = &D_8009E928[k].s`) | 4 (frame 0x60 vs 0x70) |
| + unreferenced `int pad[4]` | **0** |
| without patch 20 | 6 (the three `lui/addiu` pool addresses) |

The unreferenced 16-byte local only reproduces retail's frame reservation
(ra at 0x68); lead ruling 2026-09-24: accepted, same device as func_800347B4's
unused 16-byte local.

## Verification
```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_800323C8
  -> disc1_preflight: PASS (deep, 1887 c / 284 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_800323C8
  -> rodata fold: stripped 60-byte duplicate literals (D_80010DFC,D_80010E10,D_80010E24)
  -> LEAF_STRONG=EXACT func_800323C8 span=0x214 vram=0x800323C8 compiled=0x220 failures=[none]
```
