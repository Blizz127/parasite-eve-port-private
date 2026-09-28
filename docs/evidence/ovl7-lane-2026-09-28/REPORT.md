# Overlay lane ovl7 — 2026-09-28 (room targets)

Verification per body: `build/lanes/ovl7/t.sh <t> <func> <src> [flags]` (room_lane/try.sh → era_link_check, `word mismatches=0`),
install via `build/lanes/ovl7/install.sh` (era_link_check `LINK_EXACT` + `disc1_preflight.py --target <t> --deep --only <func>` PASS)
under `flock build/lanes/ovl.lock`, then `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>` serially (`build/lanes/ovl7/gate.sh`).
No cross-room masked-hash twins were pending for any new body (`build/lanes/ovl7/pending.py` prints nothing).

## Exact (32 distinct bodies)

| body | target | size | profile | levers |
|---|---|---|---|---|
| func_8019A290 | room_m0273i | 0x108 | era_o2_g0_narrow_shifted_reg_load | K3b load-bearing (2 words without); exeD4 k3b draft |
| func_80199CA4 | room_m0350i | 0x88 | era_o2_g0 | in-room twin of func_80199B0C with distinct return symbol D_8019A7A0 (port3 SKIP: address pattern differs) |
| func_80192E64 | room_m0273i | 0xF0 | room_m0273i_dispatch_8018F1CC | switch fold; record base pinned register char *b asm($5) (9 -> 0); THREE_WORD load-bearing (1w without) |
| func_8019A4CC | room_m0273i | 0x108 | era_o2_g0 | table-word arg as a local computed before the SV stores (42 -> 0) |
| func_8018FDA8 | room_m0418i | 0x134 | era_o2_g0 | B()/H() per-array index macros (74->5 bivs); e=8 local at loop-body top gives hoisted $t3 (5->2 with temp for f); one=1 local before e keeps $t2/$t3 hoist order (2->0) |
| func_8018F3C4 | room_m0418i | 0x138 | era_o2_g0 | 32-byte struct copy (B32) for the lw/sw x4 block; global store order via climb (23->0) |
| func_80190F0C | room_m0418i | 0x140 | era_o2_g0 | SPR D_801995C8[2] 0x18-byte records; store order climbs (block1 21->9, block2 9->0) fix callee-saved constant allocation |
| func_80195210 | room_m0418i | 0x148 | era_o2_g0 | first draft: rand()%400/%300 jitter loops |
| func_801949F4 | room_m0418i | 0x150 | era_o2_g0 | p = a2 copy before the first a2 read (81->0: separate callee-saved homes); o = &D_80199570 local |
| func_801927A8 | room_m0418i | 0x15C | era_o2_g0 | H()/W() per-array index macros; first draft |
| func_801919B0 | room_m0418i | 0x164 | era_o2_g0 | sibling of 801927A8 (== 1 gate) |
| func_801923D8 | room_m0418i | 0x16C | era_o2_g0 | SPR D_801995F8[3]; first-block store order via climb (25->0) |
| func_80194090 | room_m0418i | 0x184 | era_o2_g0 | a2 + (i*8 + 0x30) keeps retail's int offset biv (97->0) |
| func_801902CC | room_m0418i | 0x194 | era_o2_g0 | t = load; t -= 100; t += m association (26->4); m pinned asm($2) (4->0) |
| func_80190460 | room_m0418i | 0x198 | era_o2_g0 | identity MATRIX stores written in retail (reverse) order (12->0) |
| func_80194214 | room_m0418i | 0x1B8 | era_o2_g0 | SV8/VECTOR history shift loop; t/x/z single-load locals (29->2); unsigned short sum operands (2->0) |
| func_80192544 | room_m0418i | 0x1C0 | era_o2_g0 | first draft; VECTOR w declared after v so the second stage copies into v's slot |
| func_80192D90 | room_m0418i | 0x1C0 | era_o2_g0 | first draft; identity matrix in retail reverse order; SV d declared between m and s |
| func_80192F50 | room_m0418i | 0x1C4 | era_o2_g0 | counter i + B()/H() macros (loop opt eliminates i: 104->7); rand result local before the 0x100 store (7->0) |
| func_80190060 | room_m0418i | 0x1E4 | era_o2_g0 | first draft in per-record 4,5,8,A,0,1,2,6 order (8018F3C4 template) |
| func_8019155C | room_m0418i | 0x1E4 | era_o2_g0 | int *e = &D_800B0E64 then volatile (reload for the arg); r/b locals place the D_80199528 store; x/y locals for the two loads (109->0) |
| func_801938FC | room_m0418i | 0x1E8 | era_o2_g0 | first draft; SV8 D_801994D8[i] global array |
| func_8018FBA4 | room_m0418i | 0x204 | era_o2_g0 | t = b; t += k; t -= c>>1 association (8->0) |
| func_801921C8 | room_m0418i | 0x210 | era_o2_g0 | first draft; H/W/V per-array macros |
| func_801906BC | room_m0418i | 0x258 | era_o2_g0 | struct-array (HS/BS) views of a2 = MEM_IN_STRUCT so the D_8009D254 load schedules above the stores (115->26); W>>16 reads for lh; separate t2 temp; tail store order climb |
| func_8018F640 | room_m0418i | 0x270 | era_o2_g0 | o used only for the first call, loop call arg (OBJ *)((char *)hp - 0xA) (91->4: retail o/iter and hp/0x1000 reg sharing); i = 0 before o (4->0) |
| func_80191B14 | room_m0418i | 0x2A4 | era_o2_g0 | RS/HS struct-array views of a2 (MEM_IN_STRUCT: 96->8->0); rand-result local before the 0x260 clear |
| func_8018F6BC | room_m0269i | 0x13C | era_o2_g0 | separate pointer locals per D_8009D254 reload (20->0) |
| func_8019721C | room_m0350i | 0x148 | era_o2_g0 | shared fail: label inside the sqrt early-return (24->25 layout), WS struct view for the flag RMW (MEM_IN_STRUCT, ->0) |
| func_8018F014 | room_m0272i | 0x164 | era_o2_g0 | first draft (RECT struct copies) |
| func_80192680 | room_m0256i | 0x188 | era_o2_g0_expand_div | EXPAND_DIV load-bearing (61w without); -c + 0x1000 (negu/addiu); (H<<1)+0x16 operand order; separate d/v locals; explicit if (v<0) v += 0x1FFF + (v>>13) and e = UH(0x2E) local before the byte stores (43->0) |
| func_801945EC | room_m0123i | 0x17C | era_o2_g0 | switch(a0); HS struct view of a1 (MEM_IN_STRUCT); a1->h[0]++ post-increment (85->0) |

New local profiles (target build_profiles.json): room_m0273i `era_o2_g0_narrow_shifted_reg_load` (K3b, func_8019A290),
room_m0273i `room_m0273i_dispatch_8018F1CC` (THREE_WORD + DISPATCH_FOLD=jtbl_8018F1CC, func_80192E64); room_m0256i assigns the
inherited EXE profile `era_o2_g0_expand_div` (func_80192680).

## Durable levers (new this lane)

1. **MEM_IN_STRUCT struct-array views** — `typedef struct { short h[N]; } HS; #define H(o) ((HS *)a2)->h[(o)/2]` (and WS/RS/BS
   variants) make the object stores in-struct, so gcc 2.7 sched may hoist scalar-global loads (D_8019956C, D_8009D254, D_800F32D0, D_8009D1CC)
   above them, as retail does (801906BC 115→26→0, 80191B14 96→0, 8019721C, 801945EC, 801946D4 22→11).
2. **Object pointer anchors**: when retail keeps `&OBJ.hA` in a callee-saved reg and addresses the object as `hp - 0xA`, write the
   loop call as `(OBJ *)((char *)hp - 0xA)` and use the plain object pointer only before the loop (8018F640 91→0).
3. **Loop-body constant locals** (`one = 1; e = 8;` at the loop top) reproduce retail's hoisted-constant registers and their order (8018FDA8).
4. **Association via in-place temps**: `t = load; t -= 100; t += m;` (801902CC) / `t = b; t += k; t -= c >> 1;` (8018FBA4);
   explicit `if (v < 0) v += 0x1FFF; ... (v >> 13)` for an in-place /8192 (80192680).
5. **Copy local made before the first read** (`p = a2;` at the top) splits two callee-saved homes (801949F4, 80193150).
6. **Counter-biv elimination**: a `for (i...)` loop with `B(off) (a2+i)[off]` macros whose counter loop.c eliminates gives retail's
   `p < a2 + 0x20` exit test (80192F50); `a2 + (i*8 + 0x30)` keeps a separate int offset biv (80194090).
7. Identity-MATRIX seeds are stored in reverse order in retail (m22,m11,m00,t2,t1,t0,m21,...); VECTOR/MATRIX locals are laid out in
   declaration order (declare an extra VECTOR after v to get retail's reused copy slot, 80192544).

## Parked (12)

| body | target | best words | notes |
|---|---|---|---|
| func_8019A21C | room_m0350i | 3 | K3b 5->3; int w=p[0] (lh once); residual: andi 0x3F00 scheduled before la D_800966EC (retail after); 20 index spellings, climb, -fschedule-insns, K4 all 3+ |
| func_80197230 | room_m0273i | 10 | four pinned $3, a1 pinned $17, asm use of t, $4-pinned first arg; residual post-call (short)c sign-extend; see PARK.md |
| func_80194470 | room_m0273i | 21 | ovl7 recipe from 80197230 gives 29/49; unchanged from ovl5 |
| func_80194498 | room_m0418i | 39 | hp = &O.hA / o = hp - 5 anchors (136->77); retail keeps b1/b2 absolute and o in s0 (reusing a1), hp in s5; climb no gain; ovl7 retry: o only for the first two calls, loop arg hp-0xA (95->39). Residual: retail keeps &hA in $v0 then copies to $s5 after the b1/b2 stores, and b0=0x40 stays sb -0xA($s5) (ours folds back to absolute) |
| func_80193150 | room_m0418i | 3 | p = a2 copy at top (38->15 alloc), h local (132->15), nested A64/AA4 call args (15->9), h pinned $7 + climb (9->3). Residual: lh a2+2 scheduled before lh D_800942EC (retail after); volatile fixes order but emits lhu+sll/sra |
| func_80190914 | room_m0418i | 30 | structure exact; residual is callee-saved allocation (retail c shares $s0 with the D_801987E4 address, a2+0x9C shares $s1 with a2); p = a2 copy, asm uses of c, 720 declaration permutations all 30-35 |
| func_801946D4 | room_m0418i | 11 | W>>16 reads (lh), t8/t4 locals + climb, WS struct view (MEM_IN_STRUCT; 22->11), opaque asm pointer e for the D_800942EC reload. Residual: la e lands first in the %1024 join block so ASPSX fills the bgez delay slot differently; li 1024 order |
| func_80193488 | room_m0428i | 41 | k==4 sprite family (see func-80197230 PARK): sched1 hoists the D_800F336C load; li 4 gets $2 not $3. -fno-schedule-insns 76 |
| func_8019330C | room_m0428i | 90 | cc1 frame has vars=8 (spill slot) vs retail none (0x30 vs 0x28 frame); disappears only when the a1[1]=0 store is removed; switch compare tree (retail beq 1 / slti 2 / bnez) not reproduced by switch or if-chain forms |
| func_80193A80 | room_m0350i | 27 | k==4 family: retail sched1 moves the D_800F336C load only into the table-load delay slot; ours hoists it to the top; -fno-schedule-insns 59, -mcpu variants 27-64 |
| func_80190B98 | room_m0418i | 179 | first draft only; retail re-reads a2[0xA9] after its zero test (no CSE), hoists the /100 magic constant, and loads D_800942EC through a pointer reg 3x |
| func_80193C68 | room_m0418i | 38 | 65 descriptor-seed stores generated from the disassembly; residual = retail hoists the shared constants (0x40,1,0x80,0x80,0x20,2) to the block top in distinct regs; statement-order climb 56->45->38 (slow: ~7w per 6-min step) |

func_80197230 PARK.md updated (15→10w, RTL-dump mechanism); new PARK: docs/evidence/func-8019A21C/PARK.md.

## Gate lines (final per target)

    EXACT_REBUILD_GATE[room_m0123i]=PASS plan=5f2a413f70760f5d15d5915a07c84afe06810d826189955ba8c933549b92b59c yaml=7aff0187b4448df0c08f7712b5b17a3020cef5d1c1b2c7dfb818ba0338535e01 spans=[48 c, 11 asm, 1 rodata, 2 bin] sha1_orig=7dbc62ff99461836f22d884ef04718af387e04cd sha1_cand=7dbc62ff99461836f22d884ef04718af387e04cd sha1=7dbc62ff99461836f22d884ef04718af387e04cd
    EXACT_REBUILD_GATE[room_m0256i]=PASS plan=bbf12b2a01a279619ff091216e9ada7044b3b42ea3f6c91f48f88ffaa387bb42 yaml=74c3d4a250c9bd3f0204891c745938348de6f5a9fcfaedc0b754c3bba455d03b spans=[55 c, 10 asm, 1 rodata, 2 bin] sha1_orig=618fb891329d4ac903ec617a210c4b1e0af742d1 sha1_cand=618fb891329d4ac903ec617a210c4b1e0af742d1 sha1=618fb891329d4ac903ec617a210c4b1e0af742d1
    EXACT_REBUILD_GATE[room_m0269i]=PASS plan=f1fbc635e53c91baeb51261eb939eef6c913e6eb72d9756409ae7af50f7caaf2 yaml=0aa44c12811b424fe551c15825eed196d1d59aec56f72e105d55e548728a515e spans=[11 c, 3 asm, 0 rodata, 2 bin] sha1_orig=06c90576dba22ef80781421d5e9c0e802c0a339a sha1_cand=06c90576dba22ef80781421d5e9c0e802c0a339a sha1=06c90576dba22ef80781421d5e9c0e802c0a339a
    EXACT_REBUILD_GATE[room_m0272i]=PASS plan=652da28d4f1a13ee405ca99d629436b7e42a5554ff8011548f5daf758e4fd9c3 yaml=a0bf9474578e41a21591586f68e84c588c7ed7a6abb9b301a71b6e111966b1e7 spans=[6 c, 0 asm, 0 rodata, 2 bin] sha1_orig=9684da6b0a4c7708f7faa6a05b90134a4e9f1039 sha1_cand=9684da6b0a4c7708f7faa6a05b90134a4e9f1039 sha1=9684da6b0a4c7708f7faa6a05b90134a4e9f1039
    EXACT_REBUILD_GATE[room_m0273i]=PASS plan=ba83ca0077c55e8b2c41d1076b358a7f119a38ba469782760bedf05b956e006e yaml=5c71fd53d70da9e41ea93f6e0be9b7d1d2f6e7533f0f849f0a40f8487059531b spans=[69 c, 19 asm, 1 rodata, 2 bin] sha1_orig=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1_cand=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a
    EXACT_REBUILD_GATE[room_m0350i]=PASS plan=d0eca1a172c44d206efd47f9fe83a76cf45f6a8dd09816eaad89f5cf551fcaae yaml=9d0702c51d9a6998bb38be6788540923959f75b871a90be78901f0267c399033 spans=[57 c, 13 asm, 1 rodata, 2 bin] sha1_orig=e9597b000d287033f981cee44f3db65deb8696e1 sha1_cand=e9597b000d287033f981cee44f3db65deb8696e1 sha1=e9597b000d287033f981cee44f3db65deb8696e1
    EXACT_REBUILD_GATE[room_m0418i]=PASS plan=1b4703a179883ead264449e5e8f9dfefbf9cf10c29bdb4ad77dfc8f4f6149a53 yaml=827734bd490ee1995416c49239db609f56faefa8eb0448fefb16fcd560cd5d77 spans=[130 c, 18 asm, 1 rodata, 2 bin] sha1_orig=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca sha1_cand=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca sha1=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca
