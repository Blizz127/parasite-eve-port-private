# Lane ovl13 — room bodies (2026-09-28)

Continuation of ovl..ovl11 on room_* targets. Every leaf: `era_link_check.py` LINK_EXACT at the target VMA, `disc1_preflight.py --target <t> --deep --only <f>` PASS (via `build/lanes/ovl13/install.sh`), then `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>` → `EXACT_REBUILD_GATE[<t>]=PASS`.

**33 distinct bodies exact** (34 rows incl. ported twins); 4 re-parked with notes.

| target | function | size | profile | levers |
|---|---|---|---|---|
| room_m0273i | func_80194128 | 0x15C | era_o2_g0_narrow_shifted_word_load | loop counter reused for the table value (s0); *(int *)p >> 21 + K3 (2 without knob) |
| room_m0273i | func_80195E10 | 0x168 | era_o2_g0 | pinned four $3 + asm volatile use after t (21->); input-only asm("" : : "r"(s)) after table read (5->0); u/k pinned; rodata C4 local copy |
| room_m0273i | func_801974DC | 0x16C | era_o2_g0 | pad[2] frame (12->0); li-after k==4, no pins |
| room_m0350i | func_80195218 | 0x160 | era_o2_g0_narrow_shifted_word_load | first try; t>=0 guard; K3 |
| room_m0350i | func_80195B64 | 0x17C | era_o2_g0 | first try |
| room_m0350i | func_80197A04 | 0x194 | era_o2_g0 | first try |
| room_m0350i | func_80196C64 | 0x1A0 | era_o2_g0_narrow_shifted_word_load | p=&D_800F3368 via asm output barrier (96->16), pad[2] + asm volatile after first store (->0), K3 (6 without) |
| room_m0273i | func_80192F5C | 0x18C | era_o2_g0_expand_div | MASPSX_EXPAND_DIV for the (t<<12)/per div guards (86->); base read before the object pointer (->0) |
| room_m0273i | func_8019320C | 0x194 | era_o2_g0 | short x/y loop scalars; (unsigned)b >> 5 for srl; SV copy declared before v[4]; pad[2] (67->0) |
| room_m0273i | func_80194470 | 0x138 | era_o2_g0 | t copied to r via asm output barrier (addu s0,v1); pinned four $3 + asm volatile before the D_800E1204 read (64->0) |
| room_m0273i | func_80199950 | 0x140 | era_o2_g0 | PARK RESOLVED (4w): s0/s1 as short locals (sext-at-use); asm volatile then o = k << 1 then pinned four $3 |
| room_m0273i | func_80197230 | 0x130 | era_o2_g0 | PARK RESOLVED (15w): short c; pinned four $3 + asm volatile after t; first try |
| room_m0350i | func_80194F04 | 0x160 | era_o2_g0 | PARK RESOLVED (8w): asm volatile fences after the m load and after four = 4 |
| room_m0350i | func_80193A80 | 0x14C | era_o2_g0 | PARK RESOLVED (27w): o = k << 1 then asm volatile then pinned four $3 (3 without the fence order) |
| room_m0428i | func_80193488 | 0x168 | era_o2_g0 | PARK RESOLVED (41w): short c, o/fence/four lever, separate us local (88 inline) |
| room_m0428i | func_8019330C | 0x17C | era_o2_g0 | PARK RESOLVED (90w): r = D_800F33E0 first; a1[0]++ then a1[1] = 0 then n = a1[0] (27->0) |
| room_m0318i | func_80192DA0 | 0x134 | era_o2_g0 | PARK RESOLVED (34w): a1[1] -= a1[3]; a1[3]--; k between v.x/v.y; pinned four; int k (andi otherwise) |
| room_m0350i | func_80196E04 | 0x128 | era_o2_g0 | PARK RESOLVED (37w): short h; no outer x local; asm output barrier on p before *p - 1 (removes 8-byte frame) |
| room_m0429i | func_80192594 | 0x9C | era_o2_g0 | PARK RESOLVED (9w): separate word globals; $3-pinned last copy temp |
| room_m0273i | func_8019250C | 0xA4 | era_o2_g0 | func_80192594 shape; first try |
| room_m0273i | func_80199F84 | 0xE4 | era_o2_g0_narrow_shifted_word_load | PARK RESOLVED (9w): a1+8/a1+0xA into locals + asm volatile before s.x store; K3 (2 without) |
| room_m0273i | func_8019A720 | 0xF0 | era_o2_g0 | PARK RESOLVED (4w): a1[1] -= a1[3]; a1[3]++; idiom |
| room_m0350i | func_80194654 | 0x168 | era_o2_g0 | PARK RESOLVED (30w): pp = D_800F32D0 read between a1[1] = 1 and a1[0]-- |
| room_m0428i | func_80192E60 | 0x1B8 | era_o2_g0 | SV declared before v[4]; v stores in index order (10->0) |
| room_m0428i | func_80193A58 | 0x1CC | era_o2_g0 | k==4 li-after: four pinned $4 (78->5), o = k << 1 pinned $3 (5->0) |
| room_m0273i | func_80192664 | 0x1D4 | era_o2_g0 | first try; S32 struct copy for the 8-word block |
| room_m0273i | func_80198B1C | 0x1B8 | era_o2_g0 | first try: short s0/s1, (unsigned)(b+0x1000)>>5, o/fence/four, pad[2] |
| room_m0273i | func_80195984 | 0x24C | era_o2_g0_narrow_shifted_word_load | us local for call 1, inline func_80077AA4 for call 2 (36->0); K3 |
| room_m0273i | func_801977F8 | 0x250 | era_o2_g0 | D_800966EC[(t << 6) & 0xFC0] int-array index (keeps t<<8 un-CSEd); pc/pv/one/tb locals; w pinned $3 (121->0) |
| room_m0350i | func_8019404C | 0x1D0 | era_o2_g0_narrow_shifted_word_load | first try; pad[2]; K3 |
| room_m0350i | func_801996BC | 0x1D0 | era_o2_g0 | twin of m0428i func_80193A58; first try |
| room_m0174i | func_80192A00 (+room_m0348i:func_801918E8) | 0x1B0 | era_o2_g0 | PARK RESOLVED (40w): base via asm volatile output from &D_8019749C placed after the call (21->0); absolute externs for other fields |
| room_m0123i | func_80194F68 | 0x1AC | era_o2_g0 | rodata SV init; /24 (mult 0x2AAAAAAB + sra 2); separate r for the call result (14->0) |

## Re-parked

| target | function | best words | residual |
|---|---|---|---|
| room_m0273i | func_80193870 | 5 | k load/li 4 register swap ($3/$4) once p is pinned $3; levers: short s0/s1, x0..x2 temps in retail store order, (unsigned)b>>5, asm volatile fences, four/k pins, climb |
| room_m0273i | func_8019A398 | 24 | retried ovl13 (pinned four + fences, split w/<<16/>>21, pad[2]): best 32 vs prior 24; li 4 / sra / k-load schedule |
| room_m0418i | func_80193150 | 3 | ovl13 retry: lh a2+2 scheduled before the D_800942EC load; g local, fences, unpinned h, volatile, climb — no change |
| room_m0186i | func_80190048 | 4 | ovl13 retry: -0x2A constant li order; pinned $5 constant local gives 21 |

## Durable levers (new this lane)

1. **k==4 sprite family, li-before variant**: `register int four asm("$3")` + `asm volatile("")` fence placed where retail's `li 4` sits (after the D_800E27EC read, or after `o = k << 1`), plus `o = k << 1` as a named local when retail computes the index before the li (func_80197230, 80194470, 80199950, 80193A80, 80193488, 80194F04).
2. **Short-typed scalars for sext-at-use**: when retail keeps a value unextended and emits `sll 16 / sra 16` at each use, declare the local `short` (not `int` + cast) (func_80199950, 80197230, 80193488, 8019320C, 80198B1C).
3. **Input-only asm barrier** `asm("" : : "r"(s))` after a table read stops cc1 hoisting the D_800F336C load above it without changing allocation (func_80195E10).
4. **Store-order invalidation for a reload**: `a1[0]++; a1[1] = 0; n = a1[0];` reproduces retail's post-store `lh` reload (func_8019330C); `pp = D_800F32D0` read between two stores fills a load delay (func_80194654).
5. **Anchored base register**: `asm volatile("" : "=r"(b) : "0"(&SYM))` after a call gives retail's callee-saved base for a few fields while the rest stay absolute `lui $at` (func_80192A00; 40w park).
6. **Int-array sin index** `D_800966EC[(t << 6) & 0xFC0]` (combine turns it into `(t<<8)&0x3F00` but CSE keeps a separate `t << 8`) (func_801977F8).
7. `$3`-pinned last copy temp lets `return 0` fill the load delay slot (func_80192594, 8019250C).
8. `a1[1] -= a1[3]; a1[3]++;` is the retail double-`lhu` idiom (80195E10, 8019A720, 80192DA0).

## Final gate lines (latest per target)

- `EXACT_REBUILD_GATE[room_m0123i]=PASS plan=638b994ffd21b2934a842f8d1ec7da950b8ca8e3a07b491bc6a2133934c3c5bc yaml=b079e78d8f8ac84f65dfda360740a98e9b9d8a3d19174daf`
- `EXACT_REBUILD_GATE[room_m0174i]=PASS plan=fb6573a0ee4d728ccc10b70602579794ce0974ad86660ec446ff3450b8336fe2 yaml=29868f3900eb4df4087f6b075c6a99fe6bbc6a5b054bc3ee`
- `EXACT_REBUILD_GATE[room_m0273i]=PASS plan=55ca91eb40a1d519a383dd2664fa59e46f3c537e55f07ac0e49e108a73b8838b yaml=8e07ebefbf0439751f5f2ce8b3d6e000e0b8535d0b4aebcb`
- `EXACT_REBUILD_GATE[room_m0318i]=PASS plan=741d0d584f3100155f23f9f819df0b8e7abe211609f450b60bc3f21d10e805a8 yaml=52e348f04fb3de39854248f0b328a691bffc9a27743d1cbe`
- `EXACT_REBUILD_GATE[room_m0348i]=PASS plan=bc52238d796f3aa863a5f1320085c03a2a848bdaa132db59622181bbac16e41d yaml=67ad9c8d50ddceb684101a8a6034869bca1029d316443ad0`
- `EXACT_REBUILD_GATE[room_m0350i]=PASS plan=1a0a49d7fdfeb15602588b2e8c40dd4c5203ec7602f8987775dc7093f78a9083 yaml=13a47a306eb5abeec8ed6b495cbb6b638540e57552877063`
- `EXACT_REBUILD_GATE[room_m0428i]=PASS plan=bd16bab5bb7d2b14247d70cdf2f21913c838a46b956ba67dbc873881d165dd81 yaml=af35f3fe51e3f394b0391e57f700df15d82aa4ce60b41709`
- `EXACT_REBUILD_GATE[room_m0429i]=PASS plan=931845a452b0ba9a2ac3ade47a46aa200d193c590b85fcda75f18c4cfef2ce8f yaml=cdb797bfcdfa499b898274e17e133be548f30e4573b2a882`
