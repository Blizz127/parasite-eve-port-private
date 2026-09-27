# func_8001C164

- VRAM `0x8001C164`, file offset `0xC964`, size `0x4B0` (300 words)
- Profile: `era_o2_g8`; maspsx env: none; cc1 2.7.2
- Source: `src/func_8001C164.c`
- Carved from the former `[0xB640, asm]` span: `[0xB640, asm]`, `[0xC964, c, func_8001C164]`, `[0xCE14, asm]`.

## Verification (decompC private config, session 2026-09-24)

```
python3 tools/build/disc1_preflight.py --config build/fanout/decompC/disc1.yaml --profiles build/fanout/decompC/disc1_build_profiles.json --deep --only func_8001C164
disc1_preflight: PASS (deep, 1960 c / 256 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompC/disc1.yaml --profiles build/fanout/decompC/disc1_build_profiles.json func_8001C164
LEAF_STRONG=EXACT func_8001C164 span=0x4B0 vram=0x8001C164 compiled=0x4B0 failures=[none]
python3 tools/analysis/era_link_check.py src/func_8001C164.c 0x8001C164 0x4B0 -O2 -G8   -> LINK_EXACT
```

## What it does

Recursive segment-vs-triangle walker: `func_8001C164(tri, exclude, x0, z0, x1, z1)`
walks the three edges of `tri` (vertex indices at `+2/+4/+6` in the 22-byte `EntA`
record with 4-byte `x,z` vertices, or `+8/+10/+12` in the 28-byte `EntB` record with
6-byte `x,y,z` vertices; `D_8009D1D8 != 0` selects the B layout, `D_8009D1FC->f18` is
the vertex table, `->f1C` the record table). For each edge it box-rejects against the
segment's x/z extents, then checks the two 32-bit cross-product ratios
`n = ez*(x0-cx) - ex*(z0-cz)` and `n2 = dx*(z0-cz) - dz*(x0-cx)` against
`den = dz*ex - dx*ez` (same sign and `|n| <= |den|`, `den != 0`). On a crossing it
takes the neighbour record (`+14/+16/+18` or `+20/+22/+24`), skips it if it is
`exclude`, returns it if `func_8001C614(nb, x0, z0)` says the point is inside, else
recurses with `exclude = tri` and returns the first non-null result. Returns 0 when
no edge leads anywhere.

## Load-bearing levers (mismatch ladder 192 -> 178 -> 16 -> 0)

1. **Declare `int dx, dz` before the four `short` extents** — stack-slot order
   (`dx@32`, `dz@40`, spilled `zmin@48` as HImode `sh`/`lhu`). 192 -> 178.
2. **Compute the first cross product `n` before `den`, through named `ax = x0-(short)cx`
   / `az = z0-(short)cz` locals** (`n = ez*ax - ex*az; den = dz*ex - dx*ez;`). This
   alone fixed the entire callee-saved allocation (`i`->s1, `cz`->s2, `cx`->s3,
   `t`->s4, `z0`->s5, `x0`->s6) and the `mult` operand order. 178 -> 16.
3. **Vertex address as integer arithmetic**: `(VtxB *)(idx * 6 + (unsigned int)base)`.
   Pointer form (`base + idx`, or `idx + base` — cc1 canonicalizes both to base-first)
   emits `addu base,base,idx`; retail has `addu idx,idx,base` (sum lands in the index
   temp). The neighbour-record address, by contrast, IS the pointer form
   (`(EntA *)base + idx` -> `addu s0,base,idx`). 16 -> 0.
4. `-G8` is load-bearing (both globals gp-relative; `-G0` = 249 mismatches).
5. Per-arm duplicated neighbour/call tail inside `if (D_8009D1D8 == 0) … else …`
   (retail keeps both copies with different exit layouts).
6. `unsigned short` vertex fields + explicit `(short)` casts at every compare give
   retail's `lhu` + `sll/sra`; loop counter `unsigned short i` gives `andi 0xffff`/`sltiu 3`.
