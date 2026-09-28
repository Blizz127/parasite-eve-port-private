# Overlay lane ovl3 — 2026-09-27

Verification per body: `build/lanes/ovl3/t.sh <t> <src> [flags]` (room_lane/try.sh → era_link_check, word mismatches=0),
`disc1_preflight.py --target <t> --deep --only <func>` → PASS, then `scripts/exact_rebuild_overlay.sh <t>` serially.

## K3 profile

Overlay-local profile `era_o2_g0_narrow_shifted_word_load` (`local_profiles` in each target's
`_build_profiles.json`): `-O2 -G0` + `MASPSX_NARROW_SHIFTED_WORD_LOAD=1`. `MASPSX_SYMBOL_AT_TEMP` is *not* wanted
here — the default indexed-symbol expansion already gives retail's `lui $at`/`addu`/`lh %lo($at)` (func_801911F0 is 2 words off with it).

## Carry-over (ovl2) re-gated

func_80192678 (5 targets) re-gated clean; gate lines in results.jsonl line 1.

## Exact

### func_801911F0 (m0141i 0x2208, 0x140; copies m0146i func_8019165C, m0153i func_801946D0, m0412i func_80194680, m0414i func_8019118C)
Ladder: exeD2 draft + K3 = 13 (reg alloc: retail computes `D_800E27EC-1` in `$v1` and copies to `$s0` in the `bne` delay slot);
`int v` (no `andi` before the arg move) plus a separate `t = D_800E27EC - 1;` used for the first table read and `i = t;` after the
`D_800E1204[k]` load for the second → 0. Copies are masked twins re-targeted by symbol address (lane copy of room_lane/port.py
accepting non-dispatch local profiles), each LINK_EXACT before carving.

### func_801927A4 (m0350i 0x37BC, 0x130)
First draft exact: `(D_800966EC[..] >> 16) * 4 + 0x800` (K3 N=16 drops the shift → `lh D_800966EE`) and
`(short)D_800966EC[..] >> 5` for the second read (retail keeps `lw`+`sll 16`+`sra 21`; K3's window stops at the `sll`).

Gate lines:

    EXACT_REBUILD_GATE[room_m0141i]=PASS plan=ca71d3ed4a8debfc98c2013ea9cdda52eb00638182f663a4d285bfd4bc1a49f4 yaml=d89df7835144c549fcca795e0729d03caf90cf03afae7a7edacca6a6ea1d02a6 spans=[23 c, 6 asm, 0 rodata, 2 bin] sha1_orig=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1_cand=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc
    EXACT_REBUILD_GATE[room_m0146i]=PASS plan=236adc1601a8d2fc611dba65133994e63c618a9137696fbe3bb9545ef892c45e yaml=855d3c7fa0a409a8b81843ce6d2d1962d5a8ae5ef2ed70787de5fab9167d96e4 spans=[42 c, 6 asm, 0 rodata, 2 bin] sha1_orig=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1_cand=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1=72a6cf583e2916e84a5d452ae90a444d19688f68
    EXACT_REBUILD_GATE[room_m0153i]=PASS plan=da6013deba2fa586973e69fab83725b4cdc2e4a425a34ba6206ec549fe3bd3a5 yaml=e0a03bcc6cb104906586732c4750249b0cf393ff1f9995117b20bf636d56c3bc spans=[74 c, 16 asm, 1 rodata, 3 bin] sha1_orig=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1_cand=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1=342c8ca376fc14e129164ce826e2a6d867602eb2
    EXACT_REBUILD_GATE[room_m0412i]=PASS plan=f671abcf38688039c2ecfd00d2fd283f5c03da419e7473b8f45074621bfc7a27 yaml=7dd625f486889033fbb04ee57d5f6d7d54b0a9c9485e51f7a8f7b4d34c0724f0 spans=[67 c, 16 asm, 1 rodata, 2 bin] sha1_orig=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1_cand=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1=673097c6bd36986925a38e6a12d2b039092ed9c8
    EXACT_REBUILD_GATE[room_m0414i]=PASS plan=d3c7da1b96b2bd759790a13d8b1a576258af7a982424c6352cc519135eea16cd yaml=a1b3ef268ae1f364ab53f795d33e789f8ac1b09565a018604b8350516fc10c49 spans=[22 c, 7 asm, 0 rodata, 2 bin] sha1_orig=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1_cand=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1=c1b8f25dd4ad748e16405bccf6692dcb2bef6467
    EXACT_REBUILD_GATE[room_m0350i]=PASS plan=95df836831629f72b5e2d6f345856df647d6e6973b6c838584d0420fac7e2770 yaml=82d8420b0e390926b1e5a5afd151e86c83719363cbef185b9608fd4c269a512e spans=[55 c, 13 asm, 1 rodata, 2 bin] sha1_orig=e9597b000d287033f981cee44f3db65deb8696e1 sha1_cand=e9597b000d287033f981cee44f3db65deb8696e1 sha1=e9597b000d287033f981cee44f3db65deb8696e1

### ovl_039F func_8010C1EC / C27C / C308 / C39C (0x4F4..0x738, libgpu-style DMA pair + timeout waits)
All four first-draft LINK_EXACT: `volatile unsigned int *` register pointers; C308/C39C are
`volatile int cnt = 0x100000; while (*REG & MASK) if (--cnt == -1) { func_8010C448(msg); return -1; } return 0;`
on `era_o2_g0_fill_call_delay_slot` (without it the `%lo` addiu stays before the jal: 2 words);
C1EC/C27C on `era_o2_g0_fill_epilogue_delay_slot` (2 words without).

    EXACT_REBUILD_GATE[ovl_039F]=PASS plan=1b5f808be54e494e720a12ffdb352f0a9a329f869fbac7246fbd9e67375fd5ae yaml=cb224999b210821e8114ca917e731a7f8372e09589a052343b7e96c4098c7662 spans=[15 c, 3 asm, 0 rodata, 2 bin] sha1_orig=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1_cand=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1=e7331867b4d1464da4d5f386d9137eaa1c98422b

### func_8018F3AC (m0167i 0x3C4, 0x144; masked twin m0186i func_8018F3CC)
Object init with a 32-byte struct copy and two colour-block global groups. Load-bearing: global store
order `D4, D5, DA, D8, D0, C4, C5, D1, ...` (retail's emitted order costs 7 words — cc1 places the byte-0x80
`li` before its first use; brute-forcing the D8/D0 source slots gave 0).

    EXACT_REBUILD_GATE[room_m0167i]=PASS plan=dd3f56db460cbfa1e6d6167074916c53a5baa16b974029321175ab53dd327c8c yaml=0a8156a65dedc7c60d9ccb8f0bb844e971cc6766191629e99945cf5d8ff13918 spans=[56 c, 9 asm, 1 rodata, 2 bin] sha1_orig=8414175491c6dc4ee53bde60f2801265aa64bcfc sha1_cand=8414175491c6dc4ee53bde60f2801265aa64bcfc sha1=8414175491c6dc4ee53bde60f2801265aa64bcfc
    EXACT_REBUILD_GATE[room_m0186i]=PASS plan=aede56f93ed43365128808a8b259f5205ec6d67bf45fbacb860cac835b5a5407 yaml=87d2354295ae33c0ff0649a58d71d980d4a6d9a9f247d426d5d5f84e78730bbd spans=[69 c, 12 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e

### func_80190904 (m0186i 0x191C, 0x154; byte-identical twin m0187i func_8018FA90)
Ladder 78 → 55 (V4 array indexing instead of a stepping pointer: one giv) → 37 (vel-add locals first, z before
vel.y) → 0: the rot store `*(short *)((char *)&v[i] + 0x84) += rand() % 512` is written with pointer
arithmetic, not `v[i + 16].z` — gcc 2.7's MEM_IN_STRUCT_P heuristic lets sched hoist the scalar
`D_800942EC` load above an in-struct store, but not above a plain pointer store (retail keeps the order).

    EXACT_REBUILD_GATE[room_m0186i]=PASS plan=ea00f6ddbaf85ba02ba9f54abbdd195eb45b1ef87e3175b4364f08b57e5dd591 yaml=7e9d042d10a46a63d333b76bececdf2e79ea4c6f747e2c32b95a2bcfa1140415 spans=[70 c, 13 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e
    EXACT_REBUILD_GATE[room_m0187i]=PASS plan=53b2a6c5aec65fa4fb3633f182e537768010aa28d234f898331e0f3ab749c9ef yaml=c770484e3ce19a5f63d6869a609575da8c0965c38860b2cd6a61671c92c7a8ba spans=[13 c, 4 asm, 0 rodata, 2 bin] sha1_orig=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1_cand=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1

### func_80190384 (m0186i 0x139C, 0x158; masked twin m0187i func_8018F510)
Ladder 83 (stepping SVECTOR pointer: extra giv) → 8 (p[i] indexing) → 0. The last 8 words were prologue
order: gcc 2.7 sched1 does not end blocks at calls, so the loop setup (`i = 0`, `la D_80194370+10`) floated
above the first `jal`; retail stops it right after `func_800C2B50`. A zero-code `asm volatile("")` after the
first call reproduces that (a `label:` does not). Same residual class as ovl2's parked m0141i func_8018FB70.

    EXACT_REBUILD_GATE[room_m0186i]=PASS plan=aab5d4f9244c5f4249a1b90c1e2ba8ff69e94af49dba63fe216be5c7d2463d01 yaml=fde3ac2dadd5f9dd8a0d04238694d4230775826b5ee2752cc1e0c35fe42c7fa9 spans=[71 c, 12 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e
    EXACT_REBUILD_GATE[room_m0187i]=PASS plan=d7d4b7b207b685d47591973a4bfa38e7cdc20729815c000340fec5a0a07f4fe7 yaml=97bd40b6531d97395ab4d521c3fee9ed792a23d0a91688d1765f74a1606c6b02 spans=[14 c, 3 asm, 0 rodata, 2 bin] sha1_orig=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1_cand=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1

## Not closed

* func_80191C44 (m0141i, 5 copies): parked 50w (one-block sext CSE). func_80192E78 (m0145i, 5 copies): parked 14w (case-2 reg alloc).
* func_8018F0D0 (m0063i + m0358i): parked 6w (case-10 empty-if + sw slot order).

* func_80190370 (m0022i + ~30 rooms): non-C — inline GTE (5 ctc2, 2 lwc2, 3 swc2).
* func_8019A290 (m0273i, 0x108): 2 words, needs K3b (register-base `lw 0($b)` → `lh 2($b)`); spec in build/lanes/ovl3/KNOB_REQUESTS.md.
* func_8019A21C (m0350i, 0xFC): same K3b shape.
* func_80199F84 (m0273i, 0xE4): parked at 9 words (sched1 order of `la D_8019AB70` / `sh s.x`); tried x-local orderings, arg locals, 2.8.1, -O1, sched flags.
* func_801937B4 (m0350i, 0x130): parked; compare constant 4 allocation (`$a0` filling the lhu load delay in retail) — tried if polarity, unsigned short k, direct D_800F336C index, result local.
