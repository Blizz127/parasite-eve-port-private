# func_80030894 — DECODE NOTES, no C draft yet (lane exeB, 2026-09-27)

- VRAM `0x80030894`, file offset `0x21094`, size `0xC50` (789 words, one `jr $ra` at
  `0x800314DC`). No COP2, trap or syscall ops, so this is a C candidate. Frame 88, saves
  s0–s8 + ra.
- Retail listing with ABI register names:
  `build/lanes/exeB/drafts/func_80030894.retail.txt` (from `tools/analysis/pe_dis.sh 0x80030894 0xC50`).
- Likely profile `-O2 -G0`: every global access is absolute `lui`, with no `$gp` use.

## Shape
The HUD/menu primitive initialiser, built for both draw buffers.

```
signed char col[3]  @16(sp)  <- D_8009CD90[0..2]  (lb/sb, one byte each)
tp   = func_80077A64(0, 1, 256, 480) & 0xFFFF   -> s8 (GetTPage)
clut = func_80077AA4(304, 504)                  -> spilled halfword @32(sp) (GetClut)
s7 = 128                                         (rgb 128 constant, kept in s7 all function)
for (buf = 0; buf < 2; buf++)   // buf is an unsigned char SPILLED to 24(sp)
                                //  (sb zero / lbu / lbu+addiu+sb increment at 0x80031478)
```
Record types: `Ent28 = { DR_TPAGE (8 bytes); SPRT (20 bytes: tag, rgb+code, x, y, u, v,
clut, w, h) }`, with the SPRT fields at +8. `POLY_FT4` is 40 bytes (clut +14, tpage +22).
`func_800370DC(Ent28 *, tp)` initialises the tpage and sprite. `func_80077Bxx`/`Cxx` are
libgpu `SetPoly*`/`SetSemiTrans`/`SetShadeTex` style setters.

## Blocks inside the buf loop (retail order)
1. `p = func_8005DADC(139)`; FT4 `&D_800BE9F0[buf]` (stride 40): `func_80077BA4`; u/v from
   `p[0], p[1], p[0]+p[4], p[1]+p[5]`; `tpage = func_80077A64(0,0,448,0)` (`sh 0x800BEA06`);
   `clut = *(u16 *)(p+2)` (`sh 0x800BE9FE`); xy0..3 = 0; rgb = 0; `func_80077B04(G, 1)`.
2. `for j < 10, k < 4`: `func_800370DC(&D_800B01C0[buf][j][k], tp)`, then `.clut = clut`
   (`sh 0x800B01D6`). Strides 1400 / 140 / 28 (the same array as `func_80032B0C`). Retail
   spills `buf*12` to 40(sp) across this loop, a reload artefact.
3. `func_80077A64(0,0,0,0)`; `func_80037140(&D_8009E068[buf], tp2)` (stride 24);
   `D_8009E070[buf]` rgb=48, `func_80077B04(.,1)`; `D_8009E098[buf]` (stride 16)
   `func_80077C44` rgb 29/62/50, w=56 h=3; `D_800B00E8[buf]` (stride 36) `func_80077BC4`
   (then constant colour stores 0/70/159/200/130/255/249).
4. `func_800370DC(&D_800B6920[buf], tp)` (stride 28), sprite u=200 v=224 clut w=4 h=8.
5. Four loops over 28-byte `Ent28` tables, each `func_800370DC` + clut + w/h (+ rgb 128):
   `D_8009E0F0[buf][4]` (6×10), `D_8009E1D0[buf][5]` (6×10), `D_8009E3B8[buf][3]`
   (24×8, rgb128), `D_8009E500[buf][10]` (rgb128), `D_8009E7A0[buf][4]` (6×6),
   `D_8009E8B8[buf][2]` (6×6), `D_8009E960[buf][13]` (per entry
   `p = func_8005DADC(106+i)`, uv/clut/w/h from p), plus single records
   `D_8009E0B8`, `D_8009E2E8`, `D_8009E320`, `D_8009E460`, `D_8009E768`, `D_8009E730`,
   `D_8009E880`, `D_8009E928`, `D_8009EC38` and two G4 at `D_800B0130`/`D_800B0024`
   (stride 72), the latter with constant gradient colours.
6. `D_8009E358[buf][3]` (stride 16, `func_80077C44`, rgb = `col[i]`), `D_8009E498`/`D_8009E4A8`
   (stride 32, `func_80077C64`, rgb 224/96), `D_8009E4D8` (stride 20, `func_80077B64`).

The symbol map (every `la` / `%lo` target in order) comes from the lui/addiu pairs in the
listing. In `lnk.dis`, entries `0x800A0008` / `0x800A0001` are `addiu sX,sX,8` artefacts,
not symbols.

## Expected difficulty
Straight-line code. The risks are register pressure (s0–s8 all live, `buf`/`clut`/`buf*12`
spilled by reload) and constant scheduling of the colour `li`s into `t1`. Budget several
hours; start with blocks 1–2 and check the prologue and spill slots first.
