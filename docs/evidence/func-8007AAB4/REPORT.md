# func_8007AAB4 — LINK_EXACT (lane exeE, 2026-09-27)

- VRAM `0x8007AAB4`, file offset `0x6B2B4`, size `0x55C` (343 words), cc1 2.8.1 band.
- libcd-style CD interrupt reader: select reg0 (`*D_8009B27C = 1`), latch the interrupt type
  from `*D_8009B288 & 7` into a **volatile stack byte**, drain up to 8 response bytes from
  `*D_8009B280`, zero-fill, acknowledge (`reg0=1, reg3=7, reg2=7`), update the status words
  `D_8009AFC4/C8` + error counter `D_8009AFCC`, `DiskError` debug prints through
  `func_80071A74` when `D_8009AFC0 > 0`, then a 5-way switch (jump table `jtbl_80011B8C`)
  that sets the sync bytes `D_8009B294[0..2]` and copies the response into
  `D_800A3460/68/70`; default prints "CDROM: unknown intr".
- Profile (new): `era_o2_g0_dispatch_80011b8c_cc1_281_fill_call_delay_slot_symbol_load_dest_temp`
  = `-O2 -G0 -mno-split-addresses`, `ERA_CC1_VER=2.8.1`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`,
  `MASPSX_DISPATCH_FOLD=jtbl_80011B8C`, `MASPSX_FILL_CALL_DELAY_SLOT=1`, `MASPSX_SYMBOL_LOAD_DEST_TEMP=1`.
- YAML: `- [0x6B2B4, c, func_8007AAB4]` + resume `- [0x6B810, asm]`.

## Verify (private lane config build/lanes/exeE/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeE/disc1.yaml --profiles build/lanes/exeE/disc1_build_profiles.json --deep --only func_8007AAB4
  disc1_preflight: PASS (deep, 2090 c / 190 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeE/disc1.yaml --profiles build/lanes/exeE/disc1_build_profiles.json func_8007AAB4
  LEAF_STRONG=EXACT func_8007AAB4 span=0x55C vram=0x8007AAB4 compiled=0x560 failures=[none]
```
Knob necessity (era_link_check, all others kept): all → 0; drop THREE_WORD_SYMBOL_STORE → 2;
drop DISPATCH_FOLD → 2; drop FILL_CALL_DELAY_SLOT → 223; drop SYMBOL_LOAD_DEST_TEMP → 3;
default 2.7.2 → 276. (`profile_necessity.py` has no `--config`; lead's gate covers it.)

## Levers / ladder
| step | words |
|---|---|
| structural draft (macro copy, cases in numeric order), 2.8.1 + fold | 223 |
| case bodies in retail emission order 3,2,1,4,5 | 212 |
| + FILL_CALL_DELAY_SLOT / SYMBOL_LOAD_DEST_TEMP | 229 (size now right) |
| copy helper as `static inline void copy8(d, s, n) { if (d) while (n--) *d++ = *s++; }` (macro/for forms 147–196) | 35 |
| `volatile unsigned char *q = D_8009B294; q[1] = q[2] = 4;` (retail bases the pair off `&D_8009B294[0]`) | 29 |
| same pointer form for the DataReady `q[1] = err ? 5 : 1` | (with next) |
| asm-label non-volatile alias `extern unsigned char *D_8009B288_nv asm("D_8009B288")` for the DataReady `*reg3 = 0` so reorg can put it in the `j` slot | 4 |
| `volatile unsigned char result[8]` + `D_8009AFC4 = result[0]; D_8009AFC8 = result[1]; err = D_8009AFC4 & 0x1D;` (volatile QI load keeps the `andi 0xff` that `err` then reads) | **0** |

Durable levers: (1) a **static inline helper** reproduces an inlined copy's register choice
where the equivalent macro does not; (2) **volatile local arrays** keep a QImode load's
separate zero-extend (`lbu` + `andi 0xff`); (3) the **asm-label alias** lever also serves to
drop `volatile` on a single store so reorg fills a `j` delay slot with it.
