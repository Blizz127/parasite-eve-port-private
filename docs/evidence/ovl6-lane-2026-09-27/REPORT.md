# Overlay lane ovl6 (subsystem ovl_* targets) — 2026-09-27

Continuation of ovl4 (`docs/evidence/ovl4-lane-2026-09-27/REPORT.md`). Per body:
`build/lanes/ovl6/t.sh <target> <src> [flags]` (room_lane/try.sh → era_link_check vs the overlay blob,
`word mismatches=0`), `python3 tools/build/disc1_preflight.py --target <t> --deep --only <func>` → PASS, then
`flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>`.

## Exact

### func_80191DC8 (ovl_03D2 blob 0x2DD8, 0x1f0) — profile era_o2_g0_no_schedule_insns
Double-buffered slice advance (RECT copy, buffer toggle, x += w, submit or flip entry table).
Mismatch ladder (era_link_check words): first draft 101 → separate RECT shorts + volatile first cur
read 45 → `-fno-schedule-insns` 21 → x/w `unsigned short` temps read before the cur store 16 →
`D_801D147C` char-array read for the entry `.b` (not in-struct: stays after the x store) 12 →
`v = w*h` before the tab load 2 → asm barrier on the loaded tab pointer (stops update_equiv_regs
sinking the load past the /2 fixup) 0. Levers: one struct for tab[2]/cur/ent[2]/idx gives retail's
`lw -8(&cur)` fold; a whole-block struct incl. the RECT makes cse reuse the block-move source (103w).

    preflight: disc1_preflight: PASS (deep, 17 c / 6 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=35ce1be4e60b186d744e6e8a11b78ffa1edb2bc3a0d84e2dbc4d40eb4837e2d2 yaml=9c03eb5cce4e8feff3ba18448128a01921309348416cc785f94f5622ec33a3c6 spans=[17 c, 6 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_801918F8 (ovl_03D2 blob 0x2908, 0x26c) — profile era_o2_g0 (default)
Split-screen window setup (func_800749D8/func_80074924 on the 20-byte D_800BCE80 / 92-byte D_800BCDC8
records, f4 = f4*2/3 rescale). First draft: record structs, signed char params, short v[2] local.

    preflight: disc1_preflight: PASS (deep, 18 c / 7 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=7b90f5bc69108a7c8d4f969e893cd1d0416827da278a53f88d942227cc072370 yaml=a3faecbdbc7692b7a5f97dda4e81ba9f65d57a35edfd47e2754bb1bc526cfe4e spans=[19 c, 6 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_80191B64 (ovl_03D2 blob 0x2B74, 0x264) — profile era_o2_g0 (default)
Movie frame poll (func_8007C484 retry x2000, fade-out over the last 16 frames, RECT clear on size change).
Ladder: first draft 52 → RECT declared inside the if block (retail frame puts addr/hdr at 0x10/0x14, r at
0x18) 43 → func_800719E4 (exit) `noreturn` + `u = frame + 16` local 22 → `s->x0 = s->x1 = ...`
(x1 stored first) 16 → per-branch `r.h = 0x1E0` (retail cross-jumps the identical tail store) 3 →
`v = u - t; v = 14 - v;` 0.

    preflight: disc1_preflight: PASS (deep, 19 c / 6 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=7b90f5bc69108a7c8d4f969e893cd1d0416827da278a53f88d942227cc072370 yaml=a3faecbdbc7692b7a5f97dda4e81ba9f65d57a35edfd47e2754bb1bc526cfe4e spans=[19 c, 6 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_80192CE8 (ovl_03D2 blob 0x3CF8, 0x2b0) — profile era_o2_g0 (default)
Movie playback driver. Ladder: first draft 149 → int slot param, (short) cast at the func_801924F8 call,
frame pointer q local 154 → goto-formed frame loop (a do/while lets loop.c hoist &D_800B0DBA into $s0)
151 → plain `unsigned char D_800B0DBA` tested through p with `q = (short *)(p + 2)` (a struct member
`--` stabilises the address into a register) 152/structure right → a second pointer local for the final
D_800B0CD8 clear (one shared pointer pseudo holds $a1 and blocks the return-value copy fill) 0.

    preflight: disc1_preflight: PASS (deep, 20 c / 5 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=5899c6f2eb993ae1feeee8582fe4f5450e45b57f9ee38b1d72eb8a53a060a7af yaml=5500b75ddf78015a2d94da1fc800452ab69f446bd82b8d3fea3b3711c4796012 spans=[20 c, 5 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_801214D4 (ovl_03C5 blob 0x7D4, 0x1f0) — profile era_o2_g0_no_schedule_insns (ovl4 PARK resolved)
Same body as ovl_03D2 func_80191DC8 (line-by-line masked compare of the two retail disassemblies: only
symbol addresses differ). Ported by symbol remap (D_801D1470 struct → D_801228D8, RECT shorts
D_801D148C.. → D_801228F4.., D_801D147C → D_801228E4, D_801D1494 → D_801228FC, D_801D0DC0 →
D_801223F8, func_801918F8 → func_80121004). era_link_check: word mismatches=0 on the first try.

    preflight: disc1_preflight: PASS (deep, 6 c / 3 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03C5]=PASS plan=c47b7bc5205fb827d25915fb363ca09ebb3a6cb038553c39daaa91789a8bef2c yaml=6c3f4affae741efdc2e3e59f41c48b45affc1c7d31894f4cbb5a5c2511752b72 spans=[6 c, 3 asm, 0 rodata, 2 bin] sha1_orig=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1_cand=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736

## Parked (best drafts under build/lanes/ovl6/drafts/park_*.c)

* **func_801F15DC** (ovl_03C9, 0x1B4) best 10w (was 62w in ovl4): `int k = D_800F336C;` followed by an
  empty `asm("" : "=r"(k) : "0"(k))` barrier (stops update_equiv_regs sinking the load to its use:
  62 → 15), `r.h = 1` after the step*50 store (15 → 11), plain `c = func_80077DC4(t) / 32` (10).
  Residual (3 spots): the /32 temp gets `$a2` not `$a1` and its `sra` lands in the `bne` slot instead of
  after `lhu 0xC`; retail's `li 4` sits before the D_800E1204 indexed load. Both are sched1 priority
  choices in one block. Tried: every order of {k load, c/32, x, y, w, h, step*50} (840 variants, sched1
  on: best 10; `-fno-schedule-insns`: best 34), K4 MASPSX_HOIST_BLOCK_LI (no change), `four` local
  (+barrier 59), barrier on c (64), unsigned/short k, cy types, table typing, ternary/goto compare
  forms, -O1, 2.8.1. The `li 4`-before-table-load shape recurs in func_801F1070/11FC/1790 (all asm).
* **func_80191FB8** (ovl_03D2, 0x33C; same body as ovl_03C5 func_801216C4) best 173w (shifted by a
  1-word block-layout diff): `-fno-schedule-insns` gets the entry/param copy right (sched1 on: 194);
  case 1/2 computing all `p + K` sums into locals before the stores gives retail's order. Residual:
  register allocation of the constant temps (retail reuses `$a2` = the dead `n` register for every
  `lui/ori` constant; sums in a1/a0/v1/v0, p in a3); pins `p asm("$7")`, `k asm("$6")` get closer
  (175) but not exact. Also the default-case `j` delay-slot/label placement differs.
* **func_80192934** (ovl_03D2, 0x314; same body as ovl_03C5 func_80122040) best 151w: goto-formed
  retry/frame/wait loops + `-fno-cse-follow-jumps -fno-cse-skip-blocks`. Residual: retail uses
  6 saved regs (s0 n, s1 frame, s2 = the `1` compare constant hoisted by loop.c out of a REAL outer
  retry loop, s3 end flag, s4 = &D_801D1464 tab base computed as `(&curA) - 8`). A real outer loop
  (for/do) hoists `1` but also hoists the D_801D0DDC address (block-copy dest pseudo reused by cse):
  7 regs; goto keeps 5. A `Mov *m` / `void **tab` pointer local adds a spilled 16-byte slot (vars=24).
  func_801924F8 (0x43C) contains the same frame/wait loop and will share the fix.
* **func_8018F958** (ovl_03D2, 0x268) best 94w: blend blit (two passes of per-byte
  max((src*k)>>8, dst)); `(t >= c) ? t : c` pixel form (retail's `move`+conditional-move shape),
  `if (w < 0x55) cnt = w; else cnt = 0x54;`, goto-rotated inner loop in pass 2 with a `w` countdown.
  Residual is pure global register allocation (d/p, k/w/i rotated among $t1..$t9); 60 random
  declaration orders all give 94 (declaration order is not the lever).
