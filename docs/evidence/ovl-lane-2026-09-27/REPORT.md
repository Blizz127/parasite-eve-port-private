# Overlay lane `ovl` — 2026-09-27 fan-out
Distinct bodies matched: **49** (hand-written), parked **10**, non-C **1**.
Every leaf: `era_link_check.py --target <t> <src> <vram> <size> -O2 -G0` → `LINK_EXACT`, then `disc1_preflight.py --target <t> --deep --only <func>` → PASS, then `scripts/exact_rebuild_overlay.sh <t>` (serial, one at a time). All gates reach `RESULT=EXACT_MATCH` (retail SHA-1 reproduced); the final verify step reports only the documented *staging gap* (new `src/overlays/**` files are untracked until the owner `git add`s them).
Profile: all leaves use the default `era_o2_g0` (no profile assignments added).

## Matched bodies

| body | size | targets | description / levers |
|---|---|---|---|
| `func_8018F0F0` | 0xB4 | room_m0017i | one-shot func_8018F1C4 setup then per-frame model update chain on +8 record +0x1B4 |
| `func_801903A8` | 0x80 | room_m0049i | reads two func_800C2B10 slots into a2+0x2E/+0x24, seeds fields |
| `func_80190BF8` | 0xB8 | room_m0049i | entity init via func_800C2B10 slots, +0x10 = rand()%40 |
| `func_801914F0` | 0xB8 | room_m0049i | entity init variant (+0x28 = 0x290 on both arms) |
| `func_80191C94` | 0xB0 | room_m0049i | entity init via func_800C2B10 slots; +0x10 = rand()%40 |
| `func_80192688` | 0x10C | room_m0049i | particle spawn at model node offset (func_80078C34 transform of D_8018F01C); levers: statement order, rand() result held before the +0x12 store |
| `func_80190B54` | 0xB0 | room_m0075i, room_m0419i | two func_800C5EB0 range tests against D_8009D254->0x2A vector; lever: three int temps keep lh (plain short copy narrows to lhu) |
| `func_8018FD14` | 0x104 | room_m0137i, room_m0146i, room_m0392i, room_m0393i, room_m0401i | approach/brake toward e+0xC then func_800C6B90 step; lever: 'v > w' operand order loads v first |
| `func_801900B0` | 0xDC | room_m0137i, room_m0146i, room_m0392i, room_m0393i, room_m0401i | six-particle integrator (+0x20 pos, +0x50 vel, +0x98 alive, +0xA4 count); lever: explicit i/one/lim/two locals reproduce loop-hoist order |
| `func_8018F1D4` | 0x8 | room_m0145i, room_m0153i | Profile era_o2_g0 (default); LINK_EXACT at the room VMA. Lane ovl 2026-09-27. |
| `func_80192A3C` | 0xA0 | room_m0156i | camera/window param block D_80192C34 set + refresh from D_800F32D0->8; lever: returns &D_80192C34 (keeps $v0 live) |
| `func_8018F084` | 0xAC | room_m0269i | message handler: 0x19 registers a3, 0x200 searches actor list for (a3,a4) ids |
| `func_8018F5D4` | 0xE8 | room_m0269i | player reset variant (func_8003E0FC pose, pos<<16, D_800BCF88/=0x80); levers: $6/$4 pins for the call-argument bases |
| `func_80192D8C` | 0xB0 | room_m0273i | teardown: flags linked records =4, optional func_80020CE4, then func_80192C00 |
| `func_80193114` | 0xF8 | room_m0273i | weighted random pick over D_8019A8C8[a0][band(y)][(x>>10)&3][6]; lever: reuse y for band index, for(i=0,s=0;...) |
| `func_80194D54` | 0x118 | room_m0273i | event poll: area-type 9/0x10 proximity tests set D_8019AE9A, else func_8006DCE4 trigger; lever: if(b!=9){...}else-chain layout |
| `func_8019A1A0` | 0xF0 | room_m0273i | camera shake/param preset selector by mode a1 (0/1/2), random sign via func_80052B2C |
| `func_8019A628` | 0xE4 | room_m0273i | event state handler (spawn / periodic vector copy); levers: if-tree, SVECTOR-typed source and destination |
| `func_80192560` | 0xC0 | room_m0318i | find actor by id bytes (+0xD,+0xC) with live model, call func_800CE870; else log; lever: a1 copy pinned $7 |
| `func_80192620` | 0xF8 | room_m0318i | event callback: jitter vector / project and draw via func_800CF3AC+func_800D1DEC |
| `func_801992A4` | 0xD8 | room_m0318i | event state query twin of m0432i func_80198F54 (threshold *a2) |
| `func_80199B0C` | 0x88 | room_m0350i | actor-list search (D_8009D20C chain) for id bytes +0xC/+0xD with flag 0x10 clear; cursor D_8019A774; lever: guarded for-loop with p=q copy |
| `func_80199B94` | 0x110 | room_m0350i | event callback twin with sine-scaled parameter |
| `func_80199D50` | 0xEC | room_m0350i | event handler (spawn / copy player pos-0x140 into slot); lever: scalar *q stores keep order vs flag byte |
| `func_80199E50` | 0xDC | room_m0350i | event state handler (0: spawn via func_800CE560, 1: poll + set offset); lever: if-tree with case-1 falling into shared return 0 |
| `func_80199F2C` | 0xE8 | room_m0350i | event handler (spawn / copy vector into slot); lever: scalar *q stores |
| `func_8019A014` | 0x10C | room_m0350i | event callback: spawn func_800CEE20 effect with per-area pitch table D_800E1204; lever: int n (no ushort mask) |
| `func_8019A134` | 0xE8 | room_m0350i | event callback: frame>=4 query / spawn effect via func_800D004C with sine amplitude; levers: goto shared return 0 (no store-flag), int k + (short) casts keep lh |
| `func_8018F214` | 0x2C | room_m0401i | Profile era_o2_g0 (default); LINK_EXACT at the room VMA. Lane ovl 2026-09-27. |
| `func_8018F53C` | 0x104 | room_m0418i | init: two emitter records (func_800C4E50) + phase tables; lever: (char *) base keeps base+(off+K) unreassociated |
| `func_8018F8B0` | 0xA8 | room_m0418i | per-frame table ramps (+0x254/+0x25C x4, +0x230 x2), clamp +0x264 |
| `func_801905F8` | 0xC4 | room_m0418i | 4-slot timer/fade state machine; lever: post-increment via named old value c |
| `func_80191348` | 0x90 | room_m0418i | sine-driven drift (func_80077DC4), decaying +0xA counter; lever: copy-pointer p=a2 for final test, short n test |
| `func_80191748` | 0xE8 | room_m0418i | timed trigger: func_800C6CE0==3 at frame 0xF0 sets player flag 0x4000 and model bit31 |
| `func_80191850` | 0x120 | room_m0418i | billboard draw twin of func_80194C60 with room offsets D_8019956C/D_8019957C |
| `func_80191DB8` | 0xC0 | room_m0418i | 4-slot init: +0x28[i]=i<<11, zero vectors, +0x48[i]=rand()%10+10 (struct arrays) |
| `func_80192CCC` | 0xA4 | room_m0418i | fade in/out on a2+2 by phase a2+4; lever: if/else stores cross-jumped (reload of +4) |
| `func_80193480` | 0x128 | room_m0418i | randomise 10 phase pairs + probability gate on func_800C2B28(2); lever: *(a2 + i + 0x24) operand order |
| `func_80193FF8` | 0x98 | room_m0418i | entity init from func_800C2B50 record (+0x18/+0x1C/+0x20 position offsets) |
| `func_801943CC` | 0xCC | room_m0418i | entity init from func_800C2B50 pos (<<16) + func_800C2B10/2B28 params; lever: zero stores after the +8 copy |
| `func_80194B44` | 0xC0 | room_m0418i | decay/brighten step; lever: statement order 0x22 before 0x1C, int k for signed slti |
| `func_80194C60` | 0x114 | room_m0418i | sprite/billboard draw setup: identity MATRIX scaled by a2+0x10, translate, func_800C42A4; lever: chained-assignment direction |
| `func_80194D74` | 0x5C | room_m0418i | velocity/clamp step on a2 record; sets a1[1]=2 when a1 state 0x3C |
| `func_80193EDC` | 0x128 | room_m0428i | event handler: spawn / count up then fill slot {0,0xF0D0C0,0x200,0,0x280}; levers: goto shared return 0, $3-pinned base, scalar *hp load before the flag store |
| `func_80192B10` | 0xB0 | room_m0429i | player state reset: clear motion vectors, copy pos to +0x40, set D_800BCF88 bit 7; lever: d pinned $4 (call arg) |
| `func_80192BFC` | 0x108 | room_m0429i | teardown + player reset (clears *a0->0x10 first) |
| `func_80192D04` | 0xFC | room_m0429i | teardown + player reset twin of m0273i func_80192D8C/m0429i func_80192B10 |
| `func_801906EC` | 0xF4 | room_m0430i | effect slot setup in D_80190860[24B] by mode 1-4; levers: rand() before the +0xC store, scalar *(short *)p store orders before the D_800942EC load |
| `func_80198F54` | 0xD4 | room_m0432i | event state query (0/1) on player flag 0x10000000 and D_800E27EC>=100; levers: ==1/<2/==0 if-tree, struct-typed D_800E2368 |

The 5-room family port (`func_8018FD14`, `func_801900B0` from m0137i) also carried four pre-existing m0137i C bodies (`func_8018F668`, `func_8018FA2C`, `func_8018FBF0`, `func_8018FFA0`) into m0146i/m0392i/m0393i/m0401i via `tools/analysis/room_lane/port.py` (24 port spans, each LINK_EXACT before carving). Return-0 stub `func_8018F1D4` was ported to m0153i as `func_8018F224`; m0075i `func_80190B54` to m0419i as `func_80190B98`.

Header words: m0145i/m0153i/m0401i each begin their text window with two data words (`0x00000400`, `0x00010000`) fused into the first splat function; those are now `[off, bin]` rows and the C span starts 8 bytes later.

## Gate lines (last serial run per target)

```
room_m0017i: ok orig=b40116e92e76cdcbc81a583c79ab2513c369fe64 cand=b40116e92e76cdcbc81a583c79ab2513c369fe64 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0017i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0049i: ok orig=320861e23df3605a60e159b7db5fc081aa421607 cand=320861e23df3605a60e159b7db5fc081aa421607 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0049i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0075i: ok orig=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 cand=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0075i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0137i: ok orig=6e146c37de2790b03bcb104954571437abb82c1a cand=6e146c37de2790b03bcb104954571437abb82c1a RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0137i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0145i: ok orig=8bb899c982ce00e9d43ca6807f0f8aa770f36de7 cand=8bb899c982ce00e9d43ca6807f0f8aa770f36de7 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0145i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0146i: ok orig=72a6cf583e2916e84a5d452ae90a444d19688f68 cand=72a6cf583e2916e84a5d452ae90a444d19688f68 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0146i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0153i: ok orig=342c8ca376fc14e129164ce826e2a6d867602eb2 cand=342c8ca376fc14e129164ce826e2a6d867602eb2 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0153i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0156i: ok orig=2d094b8b74b918379e1ed7c213e1337fb7fe4cac cand=2d094b8b74b918379e1ed7c213e1337fb7fe4cac RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0156i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0269i: ok orig=06c90576dba22ef80781421d5e9c0e802c0a339a cand=06c90576dba22ef80781421d5e9c0e802c0a339a RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0269i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0273i: ok orig=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a cand=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0273i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0318i: ok orig=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b cand=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0318i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0350i: ok orig=e9597b000d287033f981cee44f3db65deb8696e1 cand=e9597b000d287033f981cee44f3db65deb8696e1 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0350i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0392i: ok orig=b51734dcce487ce7fb0e79610bd81ace8f2d8462 cand=b51734dcce487ce7fb0e79610bd81ace8f2d8462 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0392i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0393i: ok orig=cef7a5f2ad37426f7894315ec54d97526f322ee8 cand=cef7a5f2ad37426f7894315ec54d97526f322ee8 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0393i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0401i: ok orig=19d2caf765e89732886d5034b92d78fd12a3e441 cand=19d2caf765e89732886d5034b92d78fd12a3e441 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0401i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0418i: ok orig=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca cand=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0418i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0419i: ok orig=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a cand=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0419i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0428i: ok orig=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c cand=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0428i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0429i: ok orig=1487c56a4d420ebe6a13bb261622428226a66034 cand=1487c56a4d420ebe6a13bb261622428226a66034 RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0429i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0430i: ok orig=475d8d7b82616497d8e98652ebbce03a55d7cccd cand=475d8d7b82616497d8e98652ebbce03a55d7cccd RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0430i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
room_m0432i: ok orig=e13a8b54116474ab22fa0ae909a264442944ae4f cand=e13a8b54116474ab22fa0ae909a264442944ae4f RESULT=EXACT_MATCH | EXACT_REBUILD_GATE[room_m0432i]=FAIL verify: staging gap — a YAML C source under src/overlays/roo
```

## Parked (drafts in build/lanes/ovl/drafts/park_*.c)

| body | size | target | residual |
|---|---|---|---|
| `func_80190470` | 0xDC | room_m0141i | 5-copy-trail-shift(19w;dz-load-sched) |
| `func_80192594` | 0x9C | room_m0429i | init-3word-copy(9w;return-reg) |
| `func_8019250C` | 0xA4 | room_m0273i | init-3word-copy-twin(9w) |
| `func_8018F1C4` | 0x34 | room_m0017i | stack-arg-load-order(13w) |
| `func_80199F84` | 0xE4 | room_m0273i | sin-table-lh-sra(knob1) |
| `func_8019A21C` | 0xFC | room_m0350i | sin-table-lh-sra(knob1) |
| `func_8019A720` | 0xF0 | room_m0273i | reg-swap(4w) |
| `func_80196E04` | 0x128 | room_m0350i | frame+CSE residual (8-byte frame from HImode temp) |
| `func_80197230` | 0x130 | room_m0273i | sched1 order: retail finishes the table term before the D_800F336C load (15w) |
| `func_80192DA0` | 0x134 | room_m0318i | double load of a1[3] hoisted above the a1[1] store (sched/alias) |

## Non-C

- `func_80121000` (ovl_03C5): not a function: lone lui word ahead of the 0x304 prologue (header fragment)

## Durable levers found this session

1. **Alias class decides store/load order.** This cc1 marks `p[i]`, `*(p + k)` and struct-field MEMs in_struct and a
   plain `*q` (pointer variable) scalar. An in_struct store never blocks a scalar fixed-address (global) load, so the
   scheduler hoists the global above it; retail order is reproduced by picking the class: plain `*q` stores
   (`func_80199D50`, `func_80199F2C`, `func_801906EC` `*(short *)p = a2`), or a struct-typed pointee
   (`func_80198F54` `S2368 *D_800E2368`, `func_8019A628` SVECTOR source+dest).
2. **Return value keeps $v0 live**: `func_80192A3C` returns `&D_80192C34`; that is why retail avoids `$v0` for temps.
3. **`int` temps keep `lh`**: copying `short` fields through a plain `short` lvalue narrows to `lhu`; three separate
   `int` temps reproduce `lh` (`func_80190B54`); `int k` + `(short)k` casts reproduce `lh; sll 16; sra 16` (`func_8019A134`).
4. **Operand order of a comparison fixes load order**: `v > w` loads `v` first (`func_8018FD14`).
5. **`==1 / <2 / ==0` compare tree**: `if (a0 != 1) { if (a0 < 2) { if (a0 == 0) {...} } } else {...}`, with case 1
   falling into the shared `return 0` (`func_80199E50` family); `goto out` to a shared `return 0` stops a store-flag
   (`xori`/`sltiu`) fold (`func_8019A134`, `func_80193EDC`).
6. **Loop hoist order via explicit locals** (`i = 0; one = 1; lim = &D; two = 2;` in `func_801900B0`).
7. **Chained assignment direction** is store order: `m00 = m11 = m22 = K` stores m22 first (`func_80194C60`).
8. **Named old value for post-increment compare** (`c = x; x = c + 1; if (c == 8)`, `func_801905F8`).
9. **Call-argument base pins** (`register T *d asm("$4")` / `asm("$6")`) for bases later passed to a call
   (`func_80192B10`, `func_8018F5D4`, `func_80192D04`).
10. **`(char *)` base** keeps `base + (off + K)` unreassociated (`func_8018F53C`); `*(a2 + i + 0x24)` gives `addu base,i`.

## Knob request

`build/lanes/ovl/KNOB_REQUESTS.md`: retail narrows `int >> 21` on the sin/cos table `D_800966EC` to `lh +2; sra 5`
(address shows as `D_800966EE`). None of the nine vendored cc1 builds does this; 75 overlay asm functions reference
`D_800966EE` (incl. the 5-copy m0141i `func_801911F0` and the ~30-room m0022i `func_80190370`).

## Staging

New sources (75 files = 49 hand-written bodies + 26 port spans): `build/lanes/ovl/NEW_SRC_FILES.txt`.
Changed configs: `build/lanes/ovl/CHANGED_CONFIGS.txt`. After `git add` of those, re-running
`scripts/exact_rebuild_overlay.sh <target>` should turn each `staging gap` into `EXACT_REBUILD_GATE[<target>]=PASS`
(the build half already reproduces the retail SHA-1 for all 21 targets).

## Addendum (continuation, same day)
After the lead's approval every new `src/overlays/**` file was `git add -N`'d and all changed targets re-gated serially: **every one now reports `EXACT_REBUILD_GATE[<id>]=PASS`** (32 room targets; lines below).
New distinct bodies: `func_8018F004` (m0012i, +7 rooms), `func_8018F9F8` (m0126i, +5 rooms), `func_8018F838` and `func_8018F230` (m0137i, +4 rooms each). Levers: SV-typed third call result keeps the return-value test on `$v0` (`func_8018F838`); `case 2: ...; break;` into the shared `return 0` keeps the constant in `$v0` (`func_8018F9F8`); per-descriptor field order (`func_8018F230`).
Additional parks: m0065i `func_8018F828` (6-copy, 82w), m0141i `func_80190278` (60w), `func_8018F224` (47w). Status `blocked-knob-K3` (the `lh SYM+2; sra 5` narrowing, now routed to exeD): m0141i `func_801911F0`, m0273i `func_80199F84`/`func_8019A290`, m0350i `func_8019A21C`/`func_801927A4`/`func_801937B4`.
Hazard: writing new src files while a gate's split runs trips the split's git-status check (the m0273i/m0418i/m0428i failures were caused that way); only draft under `build/` while gates run.

### Gate lines (final)

```
EXACT_REBUILD_GATE[room_m0012i]=PASS plan=2f16f2ec9b2db4dc215b744b4f8225441ae5494504889ed7a37a2c114b42c16b yaml=65d99f65cfcb3a4f56b2201aee1855a20be762f72d94059c7b87468
EXACT_REBUILD_GATE[room_m0014i]=PASS plan=8aebae26c8045158dfea1798df0b2dfb472b9358642fe089d7b7143c1c90962c yaml=8734a3d69a9bf4e1635d8b941b8d8d8c971ebd74dae7c31e4c89b4f
EXACT_REBUILD_GATE[room_m0017i]=PASS plan=a97e66246cd641af2204064270dcb6a2586422ffb17d0c73e761b4c78a6325a9 yaml=d8d40eaac88262e48d7a6846b547e6a608ac995cb187873215a7b3a
EXACT_REBUILD_GATE[room_m0049i]=PASS plan=fe0977bafca5af662236ae7baffb8c33fc91073c6a5987f3edfdecd326fa1cff yaml=2fc3fd0c0b642ceed9cc8e42589bd6755812640f2c706a53f80e0c9
EXACT_REBUILD_GATE[room_m0075i]=PASS plan=eaffe30d8353f19f44d31f070909e9b4f2efdaec408fb41ef822d5ace9c5e540 yaml=b51a2a1e1e4a56239af7fd1f9e252514b2db4529dab23acbe27cbd2
EXACT_REBUILD_GATE[room_m0113i]=PASS plan=31282964c59fa3832aaf455a3a63f89dd22042687fd2dff99a2d698d9695efd6 yaml=2049495b43670d53fb18679ed913f82304f1d2ccce23e61dfa72dec
EXACT_REBUILD_GATE[room_m0126i]=PASS plan=c322f2a72e4fb076080b75a5c53cf6a363334cca0850aebebc5b72f9d403dbff yaml=7b3e3facaa7f8d302a976cbe0565cd4f8d6fc69f57cb339819d85cd
EXACT_REBUILD_GATE[room_m0137i]=PASS plan=d6d05a946e2a1a268b9087cbc2a5e31da21fcf704cd94e84dca73275a737f313 yaml=cdff3a1bea6806027a051dc670e2db43fb0a8466b54fe65a5488f17
EXACT_REBUILD_GATE[room_m0141i]=PASS plan=8e72e775ef3352dfdc881263973ef255ffca73e0a6962784928e8a19be44ed66 yaml=b13f077f4a7360cfb36625ef826ea1021fba6122ba3a1d2f0b4834c
EXACT_REBUILD_GATE[room_m0145i]=PASS plan=d069935370d6430b73d71c92a813b3df3f707af9c7444cbde4949aba494d8a48 yaml=0228ffc399baaecd0d7e249631036c967d17717a3f9530e7c1a6a59
EXACT_REBUILD_GATE[room_m0146i]=PASS plan=8bae2eb1e97ea5eeb03b673c468b3ff03a5d851959bed2a1fba05eeabdf0390f yaml=b7495c6c8c1bba3a59df05045a2ab881dc668295d3c8902ee5158f1
EXACT_REBUILD_GATE[room_m0152i]=PASS plan=1a8d16b3b1ed0a88155011bd31585008c7e54c71bf602b0e91de62eadacc13ab yaml=8314b80a9dd0c994bd10bc5e6177db55abed09afeb55f424b7e0daf
EXACT_REBUILD_GATE[room_m0153i]=PASS plan=c8e549b52ed8b9e1c01327f09f1d9eee31a37256d041205eab007fa04da5323c yaml=f9c60726928de6c97616624d6095bcb758571966c9bb3c7483d59ef
EXACT_REBUILD_GATE[room_m0156i]=PASS plan=42b0f1440e1a1d4e1e297b8a4af3c7d8a23fc49fe4632ba72dc2f76b3b9ad0b8 yaml=dfb4f48b1442bae21c6340b67948e8e264026dc2f17ce2d6c764cb9
EXACT_REBUILD_GATE[room_m0263i]=PASS plan=01d6330145910bac398911fe4431db1526c2a4ca5a51ad5cd6bbeb7d4d526601 yaml=dab81a71f2eb0b78a80b533dd0798bfa94790f8d2ac3f46fa5f8045
EXACT_REBUILD_GATE[room_m0269i]=PASS plan=363286d8ff0ffce95a1711819074ee6fdc9fbca58bb795893e7ed9c5908feee0 yaml=99aaeb77e35e3c62a3ac7965e11895c7783ab1be26a41566901d5dc
EXACT_REBUILD_GATE[room_m0273i]=PASS plan=7279d98e3c9bf390f41eb0934259fac4e368b0a154f124a59b1b82e1eb4dcfaf yaml=9b75c52ebb7507d2af95575e2f7893ee190eabaacae071e04f9befe
EXACT_REBUILD_GATE[room_m0318i]=PASS plan=55f0703ab8077afc956cbd16ff0f979a540dc0d2077f9e32aa2705a0e3c31c6f yaml=ff7b76dee0cbfb669ee895467bdc316ed6e24da14e40f1d5cb1ec39
EXACT_REBUILD_GATE[room_m0350i]=PASS plan=9a99fc5e04771be391a3c0c708b8df84d139fd3b0c486c2088331247ef2de561 yaml=e0fee74d0093a983cbf7036c55e767cfd0667f07613d61ef097576c
EXACT_REBUILD_GATE[room_m0392i]=PASS plan=4df9a09938bc2d9908768098fa1f6ab83b971bb6cd733f663e1d7816cc11961f yaml=2e1dabcc268c71729b2fc2e802cb68ab5014411fc3892a63b6b1c44
EXACT_REBUILD_GATE[room_m0393i]=PASS plan=d5535dd1fe6060821e19d51514e5b7b5ea4fd1bd87ef75b94ad4c0cc290da5cb yaml=37d54b662fbdc5a279aa3e3c7999e463850b488f1ef9a4a77823004
EXACT_REBUILD_GATE[room_m0401i]=PASS plan=9bc851959436d9b274db75b7cccb1238c45e80b99c78e6d0f8bdcef707c0a1df yaml=834c4cc5b6c2f503970bdb37465d70bf8381282e8cae6a427418984
EXACT_REBUILD_GATE[room_m0412i]=PASS plan=749230a28a3b37021948936620fcc9fafac608e200560e93704f6089e95999fc yaml=ce615f2af614b2faeace54b76741060712491cc7d5fc20ac899eb2e
EXACT_REBUILD_GATE[room_m0414i]=PASS plan=c57cd0c92ccf566f1595e846ede7c97d92fdb700cde50d250d9fdc0f80cd101a yaml=0b10f68d4e98c0040cfce1c119ca44d5acd311aedf225d68c65a37c
EXACT_REBUILD_GATE[room_m0418i]=PASS plan=d30b9a49185bd2936831029f79dc3e7fa544fe7bfe564222b5dcc66430fbc8fd yaml=631d87964203c224da8c78bb6aa1b5da44b3e7385bfb3d6484339f6
EXACT_REBUILD_GATE[room_m0419i]=PASS plan=9b2e3bc1bd94016dabd0d43e4e59923a8cc5a66b2952357e570fceae619ba515 yaml=94471801862c605f0648bcf08dcf64377cf36eec54274e05c2c92ac
EXACT_REBUILD_GATE[room_m0421i]=PASS plan=99797d3c58379e32a1575546ba2b0510be391aa0d9765cfdf979323e24699c9a yaml=d66fc6925c032ddafe97d77b57617c78c3bc24f0ca6dc68d7e3a96a
EXACT_REBUILD_GATE[room_m0424i]=PASS plan=156127239c4457d1e04be9d2289c320e5bdce4f0579c1e8d9663a6f7e38fa38d yaml=b78767933d83634c4b7a20bb9fb680ce10a77ebe5c1463ba90a1ada
EXACT_REBUILD_GATE[room_m0428i]=PASS plan=58693e5a18d4037c57450636a1e95d267837ac108fae04ce6a7d7c49e3ed7c78 yaml=649bb579af53a825a2861044b95b29dd22cb878de86cbb1018b9778
EXACT_REBUILD_GATE[room_m0429i]=PASS plan=71675420df6f7fd3b228644bb1ef6492b7a972424a9864c4a03190e8f533b413 yaml=959efb5f4ab6b8aec7e04285af2bcb80e3420b17e9d02e6e0a7f78b
EXACT_REBUILD_GATE[room_m0430i]=PASS plan=e29192128ced236247d56a3655691c272f18acd1f993566ccf1f35c9d63c1456 yaml=d10b9763e8787eee9a3bd585c7408c814b31428878d2b6f4a07c910
EXACT_REBUILD_GATE[room_m0432i]=PASS plan=047ad7abc6ea20558baca38ba1933e2ef0f753b294682ea3fe7011f79035dd5e yaml=a2103f7c2964be8021ea9d47e633ad2b84a10861438fff2a794fc7a
```

Totals: {'exact': 53, 'parked': 11, 'blocked-knob-K3': 6, 'non-c': 1}
