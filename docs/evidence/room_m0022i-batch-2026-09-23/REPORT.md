# room_m0022i — object-behaviour leaves (2026-09-23)

All `era_o2_g0` (default), each `LINK_EXACT` at its room VMA with
`era_link_check.py --target room_m0022i <src> <vram> <size> -O2 -G0`, and all
covered by `disc1_preflight.py --deep --target room_m0022i` and the gate below.
`leaf_strong_check.py` is EXE-only; for overlays the preflight `--deep` pass
is the strong check (size, link at VMA, terminator, no interior `jr $ra`).

Object layout used here (a room actor): `+0` state byte, `+3` flag,
`+8` context pointer (`ctx->p0->+0x18` byte pointer, `ctx+0xE` mode byte,
`ctx+0x16/+0x1A` u16 range), `+0xC` behaviour function pointer,
`+0x10` linked word pointer, `+0x14` short, `+0x16..+0x19` signed bytes.

| leaf | offset | size | what / lever |
| --- | --- | --- | --- |
| func_80190CD4, func_801917EC, func_801921D8 | 0x1CEC, 0x2804, 0x31F0 | 0x28 | `o->fn(o); return 0;` |
| func_80192200 | 0x3218 | 0x30 | set fn when `ctx->p[0xE] == (signed char)o->f16` |
| func_801920FC | 0x3114 | 0x3C | behaviour init (bytes -1/-1/-1/3, fn, zero fields) |
| func_80190980 | 0x1998 | 0x58 | state 4 + clear; a `Sub *s = &o->sub` local gives retail's `addiu v1,a0,0xC` |
| func_801914A0, func_80192090 | 0x24B8, 0x30A8 | 0x5C | same with store order `f3` then `state` |
| func_80190CFC, func_80191814 | 0x1D14, 0x282C | 0x70 | range gate; lever: `v = o->f17`, `lo`/`hi` locals loaded before the compares (15 → 0) |
| func_8018FF44 | 0xF5C | 0x7C | behaviour init (0x10000 at +0x74) |
| func_8018F500, func_80190240 | 0x518, 0x1258 | 0x88 | `func_800DFB78` dispatch. Only `if (r != 1) { if (r < 2) {…} } else call` reproduces the `==1` / `<2` / `!=0` tree with the call block last. A `switch` picks pivot 0 (27 → 17 → 0) |
| func_801908FC | 0x1914 | 0x84 | yaw update; a `Src *s` local for `q->f64` (4 → 0) |
| func_8019150C | 0x2524 | 0x90 | behaviour init incl. ctx fields |
| func_801909E8 | 0x1A00 | 0x9C | behaviour init incl. ctx fields |
| func_80192138 | 0x3150 | 0x98 | message handler `switch (unsigned a2)`; case-body order 25, 4, 28, 10 and `int` 6th arg (36 → 0) |
| func_8018FE98 | 0xEB0 | 0x9C | state 4 with the `+0xB8` branch |
| func_8018F1D4 | 0x1EC | 0xA0 | behaviour init (0x400 at +0x9C, 0x100 at +0xB0) |
| func_8018F588, func_801902C8 | 0x5A0, 0x12E0 | 0xA8 | optional update call + the range gate; `801902C8` needs `s = (char *)o + 0xC` computed before the test (2 → 0) |
| func_8018FB78 | 0xB90 | 0xE0 | yaw turn toward target / idle spin, `& 0xFFF` |
| func_80192230 | 0x3248 | 0x114 | rotation-matrix setup from `D_800966EC` (rcossin). An `int c` temporary gives `lh` (a direct short copy gives `lhu`) (1 → 0) |

Not C: `func_80192344` (0x335C) contains inline GTE (`ctc2`/`lwc2`/`mvmva`/`swc2`,
flagged handwritten), which is the `handwritten-gte-wrapper` class.

```
$ python3 tools/build/disc1_preflight.py --deep --target room_m0022i
disc1_preflight: PASS (deep, 41 c / 12 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh room_m0022i
EXACT_REBUILD_GATE[room_m0022i]=PASS plan=dbf3824d3faf0625634028f669c0f5f44f1b324d5729447738aa427ee4b6d2ea yaml=fe2d8e1f55a61bda377d9523976ffc82582d1219997b04cb6968732f06b5d54b spans=[41 c, 12 asm, 0 rodata, 2 bin] sha1_orig=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1_cand=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3
```
