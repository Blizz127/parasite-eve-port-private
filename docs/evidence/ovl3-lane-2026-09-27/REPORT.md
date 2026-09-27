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

## Not closed

* func_80190370 (m0022i + ~30 rooms): non-C — inline GTE (5 ctc2, 2 lwc2, 3 swc2).
* func_8019A290 (m0273i, 0x108): 2 words, needs K3b (register-base `lw 0($b)` → `lh 2($b)`); spec in build/lanes/ovl3/KNOB_REQUESTS.md.
* func_8019A21C (m0350i, 0xFC): same K3b shape.
* func_80199F84 (m0273i, 0xE4): parked at 9 words (sched1 order of `la D_8019AB70` / `sh s.x`); tried x-local orderings, arg locals, 2.8.1, -O1, sched flags.
* func_801937B4 (m0350i, 0x130): parked; compare constant 4 allocation (`$a0` filling the lhu load delay in retail) — tried if polarity, unsigned short k, direct D_800F336C index, result local.
