# ovl11 lane — room bodies (2026-09-28)

Lane ovl11 (rooms), continuing ovl9. Profiles: default `era_o2_g0` (-O2 -G0) except two leaves on the existing K3b profile `era_o2_g0_narrow_shifted_reg_load` (MASPSX_NARROW_SHIFTED_REG_LOAD=1; knob load-bearing: 2 / 4 words without). No new knobs.
Per leaf: `tools/analysis/room_lane/ldiff.py`/`era_link_check.py --target <t> src/overlays/<t>/<f>.c <vram> <size> -O2 -G0` -> 0 word mismatches / `LINK_EXACT`,
`python3 tools/build/disc1_preflight.py --target <t> --deep --only <f>` -> `disc1_preflight: PASS`, then
`flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>` -> `EXACT_REBUILD_GATE[<t>]=PASS`.

## Exact (33 distinct bodies, single-copy)

| target | function | size | profile | levers |
|---|---|---|---|---|
| room_m0273i | func_8019A068 | 0x138 | default | empty 'case 2: break;' reproduces cc1's slti/beqz balanced switch tree (a0>=2 -> default without an ==2 test); case 0 returns func_800CE560's result (j to epilogue without v0 set) |
| room_m0273i | func_80193B5C | 0x15C | default | first-try: short loop counter, v[4] block, &tbl[i]/&tbl[i+1] args |
| room_m0273i | func_80192C00 | 0x150 | default | D_8009D248 read into a local before the RMW block; D_800BCF88 |= via pointer local |
| room_m0273i | func_80197A48 | 0x174 | default | D_8019AEFC as struct {x,y,z} (MEM_IN_STRUCT keeps load/store pairs ordered); D_800F3368 struct |
| room_m0273i | func_801936F0 | 0x180 | default | pad[2] frame; empty case 2; (t<<10)/12; int local v forces lh for the short store |
| room_m0273i | func_801933A0 | 0x19C | default | table base local t = D_800966EC for the +1 element (la base, index-first addu) |
| room_m0350i | func_801938E4 | 0x19C | default | in-arm return 0 + two asm volatile("") barriers around the D_8019A79C clear; D_8009D254 pointer local |
| room_m0350i | func_80193BCC | 0x1A0 | default | register s asm($16)/i asm($17) (7->0); a>=0x29 return 2 else break (xori/sll fold avoided) |
| room_m0273i | func_801939B4 | 0x1A8 | default | array-indexed pointer reads ((void**)X)[2] are in_struct so flag stores sink below them |
| room_m0350i | func_801976C8 | 0x184 | default | func_801938E4 shape: struct source vector + asm volatile barriers around the flag clear, in-arm return 0 |
| room_m0273i | func_80197648 | 0x1B0 | default | short cooldown counter; struct source vector; first try |
| room_m0350i | func_80195064 | 0x1B4 | default | table+count+flags as one struct D_8019A7A8 (count and table share a base); natural 0x28 frame |
| room_m0350i | func_8019784C | 0x1B8 | default | table+count as one struct: in_struct count load keeps the last e[] store inside the loop (51->0) |
| room_m0273i | func_80198CD4 | 0x1C0 | default | state struct D_8019AF74; i pinned $17; for (i = 0, s = t; ...) init order (2->0) |
| room_m0273i | func_80199A90 | 0x1C0 | default | s/i pinned $16/$17; shared D_8019AF74 struct |
| room_m0273i | func_8019706C | 0x1C4 | default | SoA state struct D_8019AF0C (x/y/z arrays, count 0x48, flag 0x5D) |
| room_m0273i | func_801945A8 | 0x224 | default | both player fields read before the && test (unsigned short b, (short)b compare); array-indexed pointer reads |
| room_m0350i | func_80194CFC | 0x208 | default | D_8019A7A0 state block as one struct (seed + counter + flag) keeps the counter RMW after the particle stores (67->0) |
| room_m0350i | func_80192C34 | 0x218 | default | += / -= 0x1000 clamp (14 vs 74), D_800F33E0 read into q before the a1 init stores, table value in t; case-2 store order from retail |
| room_m0350i | func_8019569C | 0x240 | default | e[j] = v[j] copy loop keeps lw (12), sin-table word index ((k-7)<<7)&0xF80 hoists the whole address (9->0) |
| room_m0350i | func_80195378 | 0x1EC | default | func_80195064 shape with struct D_8019A7B8 {SV t[9]; short n;} |
| room_m0350i | func_8019360C | 0x1A8 | default | fresh r local for the second player-pointer read (7->0); s/i pinned |
| room_m0273i | func_80194284 | 0x1EC | default | short loop counter, SVECTOR copy + y jitter; first try |
| room_m0350i | func_801928D4 | 0x208 | default | first try; r local for D_800F33E0 before a1 init stores |
| room_m0273i | func_801949EC | 0x170 | default | asm volatile("") after the first call keeps the s0 copy before la a0 (3->0) |
| room_m0273i | func_80195BD0 | 0x240 | default | SoA state struct D_8019AEB8 {x[8],y[8],z[8],n,..,f}; e[4] before e[3] store order; asm volatile after CE560 |
| room_m0273i | func_801993F0 | 0x178 | default | flag-byte base pointer b = &S.f pinned $3 with q = b-0x18 / v = b-0x1E reproduces retail's CSE base; v pinned $16 |
| room_m0273i | func_80193F30 | 0x1F8 | default | h = D_8019AE8C pointer local; asm volatile after CE560; short i; first-pass exact |
| room_m0123i | func_801929FC | 0x1E0 | default | t%6==0 && t<0x28 then separate t>=0x28 return 2 (jump threading reproduces retail) |
| room_m0188i | func_80194588 | 0x214 | default | uninitialised stack SVECTOR copied (retail source bug); >=10 polarity for the e[8] flag |
| room_m0350i | func_801944F0 | 0x164 | era_o2_g0_narrow_shifted_reg_load | sin-table base local tb shared by both calls (67->44); *(int *)p >> 21 + K3b gives lh 2(p)/sra 5 (44->0; 2 without knob) |
| room_m0273i | func_8019353C | 0x1B4 | era_o2_g0 | pad[2] frame (48->36); table word w loaded before the scale division, shifted after (36->0) |
| room_m0273i | func_80194B5C | 0x1F8 | era_o2_g0_narrow_shifted_reg_load | b declared before a (stack order), ix = &D_8019AE98 pointer local (s1), *(int *)p >> 21 + K3b (4 without knob) |

## Final gate lines (latest per target)

- `EXACT_REBUILD_GATE[room_m0123i]=PASS plan=ce4976363405cbd50cc411869170e50fddaf902cfbf90f9f4ebd8923769db596 yaml=b9b03af3073886e526de09f43bfcd96a1e04c0cc4c3d2c5e08b3234407f00717 spans=[49 c, 12 asm, 1 `
- `EXACT_REBUILD_GATE[room_m0188i]=PASS plan=e4201dcfec445e266fbba3abafa4541fd1981828c444fbedbbbab6e5dc891a62 yaml=0d28134b0ab02b2e7e1643586c1b9ee1fccd51c4e0629b5fbd52e31d285aab2b spans=[47 c, 11 asm, 1 `
- `EXACT_REBUILD_GATE[room_m0273i]=PASS plan=f9e7a12ffd2907acd2e370441bc2a6dbabadde89bfb04b3b1f7becd4a5f90391 yaml=81328d792bd0f1845af2b1d3b1a4543267021862cfed3c54c58452e516298d88 spans=[91 c, 32 asm, 1 `
- `EXACT_REBUILD_GATE[room_m0350i]=PASS plan=15f0c236b26b76f05f82a4bbd4df7f59c543259357d71d4b7d7927a708763e28 yaml=7bce87399cdfdfe62dc1d5391b1c854f832f063e1552adfd96ac549595942426 spans=[76 c, 21 asm, 1 `

## Parked

| target | function | best words | residual |
|---|---|---|---|
| room_m0273i | func_80199950 | 4 | table word x loads straight into $s1 (retail lw $v1 then addiu $s1,$v1,0x800 in the second lw's delay); levers: pad[2] frame (8 unused bytes), combined asm output barrier (o,s0), volatile table reads, asm-label volatile aliases for D_800E27EC/D_800F336C, pin subsets, climb |
| room_m0273i | func_80193870 | 15 | two lh table reads + 3 int->short copy interleave order; volatile p, barriers, -fno-schedule-insns (33), -O1, 2.8.1 tried |
| room_m0350i | func_80194654 | 30 | only a1[0]-- RMW is scheduled above the a1[1] = 1 store (retail keeps store first; the rest is +1-word branch offsets). Tried pointer forms, asm volatile between, volatile casts, source reorder, explicit c temp |
| room_m0350i | func_80197B98 | 70 | retail div by li 0x40 (cc1 folds % 64 to shifts; opaque d via asm barrier + MASPSX_EXPAND_DIV reproduces the div+guards); one SVECTOR slot at sp+0x30 whose address is NOT CSEd/hoisted across the two calls (single v: sp+48 goes to an s-reg; two block-scoped v: no CSE but two slots, frame 0x50 vs 0x48) |
| room_m0350i | func_80198ABC | None | same shape as func_80197B98 (div by li 0x80 + shared SVECTOR slot); not drafted |
| room_m0273i | func_801947CC | 61 | first draft: constant 1 shared into $a3 (a0==1 compare + last arg), case-2 q[] loads hoisted before the SVECTOR stores, D_800966EE>>5 emits lhu+sll/sra21 instead of lh+sra5 |

## Durable levers (CE560/CE610 emitter-controller family + sprite drawers)

1. Dispatch `beq 1 / slti 2 / beqz default / bnez a0` = `switch` with cases 0, 1 and an EMPTY `case 2: break;` when retail has no case-2 body.
2. Declare the room state block (tables + count + flags) as ONE struct: addresses share a CSE base and in_struct aliasing keeps load/store order (func_8019784C 51->0, func_80194CFC 67->0).
3. Copied source vectors as a struct `{x,y,z}` (MEM_IN_STRUCT) keep each lhu/sh pair in order (func_80197A48).
4. `((void **)D_800F33E0)[2]` (array-indexed = in_struct) lets scalar flag stores sink below the pointer loads (func_801939B4 91->0).
5. `asm volatile("")` right after `r = func_800CE560(...)` keeps the `$s0` copy before the next argument setup (func_801949EC, 80195BD0, 80193F30).
6. An `int` local between a short table read and a short store gives `lh` instead of `lhu` (func_801936F0).
7. `pad[2]` unused locals reproduce retail 8-byte frame slack when present (func_801936F0, 8019353C); some frames are natural (func_80195064).
8. When retail derives addresses from a flag byte (`addiu $s1,$v1,-0x18`), a pinned `unsigned char *b = &S.f` with `b - off` pointers reproduces it (func_801993F0).
9. Sprite drawers: `(short)tbl >> 5` read as `*(int *)p >> 21` + K3b gives retail lh 2(p)/sra 5 (cc1 alone emits lhu/sll/sra); load a table word early into `w` and shift it later to match the mult/mflo interleave (func_8019353C); a pointer local for an index global (`ix = &D_8019AE98`) reproduces its callee-saved base.
