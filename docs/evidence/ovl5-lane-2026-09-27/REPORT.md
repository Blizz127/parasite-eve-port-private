# Overlay lane ovl5 — 2026-09-27 (room targets)

Verification per body: `build/lanes/ovl5/t.sh <t> <func> <src> [flags]` (room_lane/try.sh → era_link_check,
`word mismatches=0`), install via `build/lanes/ovl5/install.sh` (era_link_check `LINK_EXACT` +
`disc1_preflight.py --target <t> --deep --only <func>` PASS), twins via `build/lanes/ovl3/port3.py` + carve +
deep preflight (`build/lanes/ovl5/portcarve.sh`), then `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>`
serially.

## Batch 1 — m0141i family (m0141i, m0146i, m0153i, m0412i, m0414i)

### func_80190470 (m0141i 0x1488, 0xDC; twins m0146i func_80190464, m0153i func_8019060C, m0412i func_80190614, m0414i func_8019043C)
Trail-history shift (5 × 8-byte records and 5 shorts moved up one slot) then `pos += vel >> 8`, `vel.y += W(0x60)`,
floor check against `D_800942EC`. Ladder: struct `v[0].x += dx` form 49 → dx/dy/dz locals 33 → non-struct
`S(o) = *(short *)((char *)v + o)` accesses 19 (gcc 2.7 MEM_IN_STRUCT_P: with in-struct stores sched hoists the scalar
`D_800942EC` load above `sh 4($a2)`; retail keeps it after, with an unfilled load-delay nop) → statement order
`S(0)+=S(0x30)>>8; S(2)+=S(0x32)>>8; S(4)+=S(0x34)>>8; S(0x32)+=W(0x60);` 0 (brute force over the 24 orders:
0,6,6,9,15,…). `for (i = 5; i != 0; i--)` gives retail's `bnez` (`i > 0` → `bgtz`).

### func_80190278 (m0141i 0x1290, 0x1F8; twins m0146i func_8019026C, m0153i func_80190414, m0412i func_8019041C, m0414i func_80190244)
Trail draw. First draft exact on the ovl3 func_80190384 template (zero-code `asm volatile("")` after the first call,
`unsigned int i`, `c -= 0x14` at loop top, OBJ struct global `D_801921B0` with fields at +4/+5/+0xA).

Gate lines:

    EXACT_REBUILD_GATE[room_m0141i]=PASS plan=4b254669f7f0c40bf1abe5ab81af8ba4c71e101905c553173afcb7571de9eed0 yaml=907e151b93ede5165ea7c788f49dcf3c86cac2492a89feecd89c2eb940d6181d spans=[25 c, 6 asm, 0 rodata, 2 bin] sha1_orig=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1_cand=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc
    EXACT_REBUILD_GATE[room_m0146i]=PASS plan=304a05e695e850913c71f143824295dd2acb2a5394ffe83b8d753158392a643d yaml=734713415e60ee88494202a10664a24866c512c1e716833d219a300ed0edd215 spans=[44 c, 5 asm, 0 rodata, 2 bin] sha1_orig=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1_cand=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1=72a6cf583e2916e84a5d452ae90a444d19688f68
    EXACT_REBUILD_GATE[room_m0153i]=PASS plan=f7d232f4712cda8329cc771bf59b198054110c6fd5362b5c99e5f874dd5b43f2 yaml=29180a09f43616549e454e45b487c770f1a113c7d9f4324f12041889e09c29c6 spans=[76 c, 15 asm, 1 rodata, 3 bin] sha1_orig=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1_cand=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1=342c8ca376fc14e129164ce826e2a6d867602eb2
    EXACT_REBUILD_GATE[room_m0412i]=PASS plan=50ff9c87e23a2a4f8ddfcf8547c18f32c35737950fc845aed89eda24a1be06d6 yaml=a7339d38a05b828c4b1c174e5377705e6f075e4fc662cebb0bcb31f3c5897db4 spans=[69 c, 15 asm, 1 rodata, 2 bin] sha1_orig=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1_cand=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1=673097c6bd36986925a38e6a12d2b039092ed9c8
    EXACT_REBUILD_GATE[room_m0414i]=PASS plan=61f84a87f7b6ca434f706723b05cd9e1fe02a355c4edb167527ff790a599408b yaml=ca5cf72f93d1a1db2c29628cc653d2ace31e0a53289b9315d1f30bb994cf21f9 spans=[24 c, 6 asm, 0 rodata, 2 bin] sha1_orig=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1_cand=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1=c1b8f25dd4ad748e16405bccf6692dcb2bef6467

## Batch 2 — m0065i family (m0065i, m0087i, m0391i, m0393i, m0399i, m0401i)

### func_8018FBCC (m0065i 0xBE4, 0x42C; twins m0087i func_8018FDA4, m0391i func_8018FDB8, m0393i func_8018FDDC, m0399i func_8018FDAC, m0401i func_8018FDE4)
3-flame particle update. Ladder: VECTOR/SVECTOR pointer locals 234 (extra givs) → per-array `o + i*stride + off`
macros 3 (retail keeps four bivs o+i, o+2i, o+8i, o+16i with field displacements) → `sv.vx = VX(0x3C) >> 16` 0
(a `>>16` int read stored to a short narrows to `lh 0x3E` natively; reading the short field gives `lhu`).

### func_8018F828 (m0065i 0x840, 0x3A4; twins m0087i func_8018FA00, m0391i func_8018FA14, m0393i func_8018FA38, m0399i func_8018FA08, m0401i func_8018FA40)
3-flame draw. Ladder 121 (short t clamp: sum and compare value in swapped registers) → `int q = (rand>>3) + UH(0x32);
t = q; if (UH(0x2A) < (short)q) t = UH(0x2A);` 0 (retail computes the sum in $v0 and copies it to $s0).

Gate lines:

    EXACT_REBUILD_GATE[room_m0065i]=PASS plan=4d041c15f513b3b4e6109172c7e54b7b2f1458c89768ef801ce12a32f4072068 yaml=482583046ef468939bb6f2dfcfe2ff14ddc98a2156d75a08d25eefac9f6a8e59 spans=[12 c, 1 asm, 0 rodata, 2 bin] sha1_orig=fa62e6cdf74b19728ab31b55f190143f0ec93bc8 sha1_cand=fa62e6cdf74b19728ab31b55f190143f0ec93bc8 sha1=fa62e6cdf74b19728ab31b55f190143f0ec93bc8
    EXACT_REBUILD_GATE[room_m0087i]=PASS plan=48e866002e8f9babdbe186185cf4ca964b1d6f91048e71f53f68510bee9c1ed8 yaml=89b7bf04de2d578ce11250c18acaa294b52c2b47c75f1bfe6aa4e38c5a05ab11 spans=[57 c, 9 asm, 1 rodata, 2 bin] sha1_orig=4c6107aa1de07a456f12d3723abab64156f4e73b sha1_cand=4c6107aa1de07a456f12d3723abab64156f4e73b sha1=4c6107aa1de07a456f12d3723abab64156f4e73b
    EXACT_REBUILD_GATE[room_m0391i]=PASS plan=e9c8892195e1f6b6c41a67fa8da6aae90ae0adf604702063102a0946ec2ee4c6 yaml=54c4fe6f938e3c51f9fe9bd3cde7845d3ae0c37e519418cfc455935e8ac32392 spans=[57 c, 11 asm, 1 rodata, 2 bin] sha1_orig=327d5766f293ada3d33a96d655a20273a9b95145 sha1_cand=327d5766f293ada3d33a96d655a20273a9b95145 sha1=327d5766f293ada3d33a96d655a20273a9b95145
    EXACT_REBUILD_GATE[room_m0393i]=PASS plan=e4df48fbd98d0a319dcc04ac486ee00fdee0336fc5def87f86e1867a8289ca96 yaml=36a66e89f272ab4698a648905ae62889e45f80a7b0f3cd26118b6a86f7b2c6c0 spans=[79 c, 9 asm, 1 rodata, 2 bin] sha1_orig=cef7a5f2ad37426f7894315ec54d97526f322ee8 sha1_cand=cef7a5f2ad37426f7894315ec54d97526f322ee8 sha1=cef7a5f2ad37426f7894315ec54d97526f322ee8
    EXACT_REBUILD_GATE[room_m0399i]=PASS plan=d68da555f422e7a3c3d82c4f55199cfb93570c42ff145543cee40acb00a3d70a yaml=46eaee5b1323dbf042c3e1c6adcf2f24c2bb67c43e0d3aca712a438f0d06052b spans=[65 c, 11 asm, 1 rodata, 2 bin] sha1_orig=d05fc006543447f413ca8cd7c1c1f2de0335c6b2 sha1_cand=d05fc006543447f413ca8cd7c1c1f2de0335c6b2 sha1=d05fc006543447f413ca8cd7c1c1f2de0335c6b2
    EXACT_REBUILD_GATE[room_m0401i]=PASS plan=75d30d24e42e0d9f587e3093db00e6c1ef42d15f780d0fa6a4303e9c07951425 yaml=80a07e0c197a4fbaa20b1d810de129ec9956621a0348e9a27d053d02aea08a11 spans=[87 c, 11 asm, 1 rodata, 3 bin] sha1_orig=19d2caf765e89732886d5034b92d78fd12a3e441 sha1_cand=19d2caf765e89732886d5034b92d78fd12a3e441 sha1=19d2caf765e89732886d5034b92d78fd12a3e441

## Batch 3 — m0186i/m0187i, m0034i/m0174i/m0348i, m0141i family (func_8018F224)

| body | src target | twins | size | levers |
|---|---|---|---|---|
| func_801905D4 (8-particle init) | room_m0186i | m0187i func_8018F760 | 0x1A0 | pointer-arith x store keeps the D_800942EC load after it (MEM_IN_STRUCT); zero stores after the vy store |
| func_80190774 (8-particle draw) | room_m0186i | m0187i func_8018F900 | 0x190 | (SVECTOR *)(a2+0x80)+i rotation arg; asm volatile after first call |
| func_80190A58 (spawn-point init via func_80078C34) | room_m0186i | m0187i func_8018FBE4 | 0x1B4 | SVECTOR out (frame size); store order via climb |
| func_8018FC54 (debris particle update) | room_m0034i | m0174i func_8018FE9C | 0x180 | first draft; (signed char)(cnt % 3) |
| func_8018F830 (portal glow draw + exit test) | room_m0034i | m0174i func_8018FA78 | 0x424 | D_8009D254 ints read as W>>16 give lh +2 |
| func_80192F34 (4-flare init) | room_m0174i | m0348i func_80191E1C | 0x1C4 | store order via climb |
| func_801934C8 (8-sprite draw) | room_m0174i | m0348i func_801923B0 | 0x18C | a2+i*8+off fields (no SVECTOR base giv) |
| func_80190B68 (two-pass sprite draw) | room_m0174i | m0348i func_8018FA50 | 0x1E4 | first draft |
| func_80190710 (timed spawner) | room_m0174i | m0348i func_8018F5F8 | 0x2A8 | first draft |
| func_80193654 (ember emitter/update) | room_m0174i | m0348i func_8019253C | 0x30C | inverted rand&1 polarity; H6 -= 0x14; H6 -= t%20 as two statements |
| func_80190EE4 (mirrored sprite pair draw) | room_m0174i | m0348i func_8018FDCC | 0x31C | SPR D_80197460[2] array (separate externs: 168w) |
| func_80190358 (object init + 7 colour groups) | room_m0174i | m0348i func_8018F240 | 0x320 | retail store order; result store after the x/y pointer loads |
| func_80191914 (spark particle update) | room_m0174i | m0348i func_801907FC | 0x428 | sv2 declared before sv; W>>16 reads |
| func_801912E4 (spark draw + flare) | room_m0174i | m0348i func_801901CC | 0x630 | (x%3<<1); pointer-arith hA store; hA,b4,b5 order; int t=(unsigned short)call; m0348i twin re-targeted by hand |
| func_8018F224 (trail object init + 4 colour groups) | room_m0141i | m0146i func_8018F218, m0153i func_8018F3C0, m0412i func_8018F3C8, m0414i func_8018F1F0 | 0x244 | global store order: single-move climb 47->2, pair-move search 2->0 |

Climb = `build/lanes/ovl5/climb.py` (parallel hill-climb over statement order between //P< //P> markers; `PAIR=a|b` mode tries all two-line moves).

Gate lines (final per target):

    EXACT_REBUILD_GATE[room_m0186i]=PASS plan=b76567a79235405f8930fd8544cc637bc7fe25305043dca491826156d61c97b2 yaml=6ddf37382709221fcb39e43a2cd52dc71f27c85855928f0a59be07aae82bdd21 spans=[74 c, 10 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e
    EXACT_REBUILD_GATE[room_m0187i]=PASS plan=650a7d4fa85cd2c2060fa173ea952a52ce62118efd3f2c0ab42105533deb03e8 yaml=44f777336f8014ae83d50e3feecb4c169922c0e898ee414121705e3ad78ec140 spans=[17 c, 1 asm, 0 rodata, 2 bin] sha1_orig=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1_cand=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1
    EXACT_REBUILD_GATE[room_m0034i]=PASS plan=8374d41fc8beae75913ba317d2536d8db076a2904bc01b10b107a1d68a320c28 yaml=be3b7df8c0086e2d1bb4de9edb10b4031271bc5cf647a72d2ade5fe7a0c2f6f9 spans=[18 c, 1 asm, 0 rodata, 2 bin] sha1_orig=2d09f16a1668c9133294b92bc3bd6acc36052d16 sha1_cand=2d09f16a1668c9133294b92bc3bd6acc36052d16 sha1=2d09f16a1668c9133294b92bc3bd6acc36052d16
    EXACT_REBUILD_GATE[room_m0174i]=PASS plan=c2e2b848e4067d6639662feb5ef9132c00b6e7440fd636d584ca5a4905ffcf98 yaml=d01cf2c5da4f5caf2939c14b996f3af024d3433735f2bfab5449d5d57c9465a9 spans=[104 c, 11 asm, 1 rodata, 2 bin] sha1_orig=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1_cand=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01
    EXACT_REBUILD_GATE[room_m0348i]=PASS plan=c4649c07ce760811fd626e2ac539f4fec9b9a6a210f334687affa90f2a10205e yaml=fe069536789500bb755d8850394fe51274209f628210d36e71cc125cc7fec353 spans=[40 c, 2 asm, 0 rodata, 2 bin] sha1_orig=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1_cand=49478531f4b705ab248a8c2eb0a3b81c4cef9985 sha1=49478531f4b705ab248a8c2eb0a3b81c4cef9985
    EXACT_REBUILD_GATE[room_m0141i]=PASS plan=a127b524bab150f8940ac59a07fe1c68800204826c984fe9b69436fcc451e826 yaml=244ff2963b90cacf01bd9aedb96dc38862b1f5189e04b75ca2980e8c1a5fcc4f spans=[26 c, 5 asm, 0 rodata, 2 bin] sha1_orig=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1_cand=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc
    EXACT_REBUILD_GATE[room_m0146i]=PASS plan=49fa6a9fece77ddd3e533b49b2f363c0b332641b31a080d77e7c843bd8b88840 yaml=2f5e1f5fad62b49c381db1293c1955c09f6e0e1581c06896eb413396013bb84f spans=[45 c, 4 asm, 0 rodata, 2 bin] sha1_orig=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1_cand=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1=72a6cf583e2916e84a5d452ae90a444d19688f68
    EXACT_REBUILD_GATE[room_m0153i]=PASS plan=b7e2e0cf6bd45359f2dcc0d33ff63f3098f87f777c61dd961198c8dd77e7ce18 yaml=ec5c2884ca229b14c7ee697feccfbdce09ffbeb22d4bc0883b7d976a4fe7aecc spans=[77 c, 14 asm, 1 rodata, 3 bin] sha1_orig=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1_cand=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1=342c8ca376fc14e129164ce826e2a6d867602eb2
    EXACT_REBUILD_GATE[room_m0412i]=PASS plan=77e6b8b09faab00a3fb1cc6e06dd7a3de10a466a59663cbe2746b8514a828d54 yaml=5c4715295f251578f39500961d3f7dbf5e4d19019e928f8fa351a94d22ee770a spans=[70 c, 14 asm, 1 rodata, 2 bin] sha1_orig=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1_cand=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1=673097c6bd36986925a38e6a12d2b039092ed9c8
    EXACT_REBUILD_GATE[room_m0414i]=PASS plan=08437d128886994c4cf4c6af888dcd3cdf65a31ef3e1f449a1690c67fca0296f yaml=06448f285d8cf1dd5785997445e044c23e836bd4e72bf3882396809dd5c9cc96 spans=[25 c, 5 asm, 0 rodata, 2 bin] sha1_orig=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1_cand=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1=c1b8f25dd4ad748e16405bccf6692dcb2bef6467

## Batch 4 — m0075i/m0419i, m0203i/m0205i/m0231i, m0245i, m0273i

| body | src target | twins | size | levers |
|---|---|---|---|---|
| func_801906D0 (beam flicker/advance) | room_m0075i | m0419i func_80190714 | 0x1B4 | first draft |
| func_80190884 (two sprite descriptors from model matrices) | room_m0075i | m0419i func_801908C8 | 0x270 | h4 store before p |
| func_80190410 (beam segment draw) | room_m0075i | m0419i func_80190454 | 0x2C0 | explicit (i << 2) keeps base-first addu |
| func_8018FF74 (beam segment init) | room_m0075i | m0419i func_8018FFB8 | 0x49C | first draft |
| func_8018F3DC (beam init + 2x10 sprite arrays) | room_m0075i | m0419i func_8018F420 | 0x338 | per-iteration SPR *p = &arr[i]; global order via climb |
| func_80193CDC (3-mode effect controller) | room_m0203i | m0231i func_8019442C | 0x2E4 | fresh local for the D_800F33E0 read before *a1 = 0 |
| func_801928EC (3-mode effect controller) | room_m0205i | m0231i func_80192940 | 0x328 | in-struct hA store; D_800F3368 block as one struct + climb |
| func_80193244 (3-mode controller (in-room twin)) | room_m0245i | m0245i func_80194AAC | 0x1A8 | in-struct a1 store; SH(e,6) copy first |
| func_80195F78 (3-mode controller (in-room twin)) | room_m0273i | m0273i func_80197360 | 0x17C | D_8019AEB2 own scalar extern; D_800F3368 struct + climb |

Gate lines:

    EXACT_REBUILD_GATE[room_m0075i]=PASS plan=5676b0f7b830777295239f17a29e416684784850e8d7b9675426abfd185e28aa yaml=9b439c7a2431721e47280f74dd60f014d2005c9c50b4152d949c26d1dd61a9ee spans=[65 c, 10 asm, 1 rodata, 2 bin] sha1_orig=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 sha1_cand=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 sha1=ca34d1321e158763e9ee8b3cc9df0b3d94d28978
    EXACT_REBUILD_GATE[room_m0419i]=PASS plan=10db8325fa5526acced0ea5df60deef340b293474c5bf0738a7af32edfa81d6a yaml=96e9638e88fc34e38f980305b8a626d1a87a7db95a2aef31459ffdbdf4d2055d spans=[67 c, 11 asm, 1 rodata, 2 bin] sha1_orig=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a sha1_cand=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a sha1=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a
    EXACT_REBUILD_GATE[room_m0203i]=PASS plan=c65cc0bc9c2c845fd14f49ad000aaef02357424e593a665085ab149c1eb4c6a4 yaml=2b7858d7946cf4af8c8f997980ca137462f9b1f84fd4df6d67e0d0b25fc4bf5d spans=[47 c, 9 asm, 1 rodata, 2 bin] sha1_orig=6f9743ef89e5fb16a761b32a00d167402203c376 sha1_cand=6f9743ef89e5fb16a761b32a00d167402203c376 sha1=6f9743ef89e5fb16a761b32a00d167402203c376
    EXACT_REBUILD_GATE[room_m0205i]=PASS plan=cb16bb4b1a84d35f877fc4b85759d3f50dca5ce6492d4e476c75971a1032b88b yaml=68351815e6c8593025099db8c00daba8f842ddacd871540e3e8df512253622f7 spans=[46 c, 10 asm, 1 rodata, 2 bin] sha1_orig=0c281041b45ca7461ec98c2536143f14a63ae43e sha1_cand=0c281041b45ca7461ec98c2536143f14a63ae43e sha1=0c281041b45ca7461ec98c2536143f14a63ae43e
    EXACT_REBUILD_GATE[room_m0231i]=PASS plan=2b349ea5aea4fc9dcf5bf2147f676d236d70b87649c8746d35d09e2927713836 yaml=8da358010777d4919e57d05627a9bb916d126ca6e44f11446ac7430d0b30745e spans=[49 c, 10 asm, 1 rodata, 2 bin] sha1_orig=94400edc11b5d5e300c179482d99935456f0f918 sha1_cand=94400edc11b5d5e300c179482d99935456f0f918 sha1=94400edc11b5d5e300c179482d99935456f0f918
    EXACT_REBUILD_GATE[room_m0245i]=PASS plan=2db982e6c05bfdc94b752e168c72573c187472360c92a02545459d881bd52ad5 yaml=fa7c2ab26381071a120531ca86c92a4aafe444b8690e089f5adab91539d12de3 spans=[48 c, 12 asm, 1 rodata, 2 bin] sha1_orig=512b5a7c48188edf45d84476a9d02265a51d1708 sha1_cand=512b5a7c48188edf45d84476a9d02265a51d1708 sha1=512b5a7c48188edf45d84476a9d02265a51d1708
    EXACT_REBUILD_GATE[room_m0273i]=PASS plan=6e5d62df5d7a15ab7bd7049400cb08f670a246aff70aceb318161a36eb44b89e yaml=3c788fa950ff5b9107ef18eff83b9a28d8f94dc7028b09f4737efc74391b3197 spans=[66 c, 20 asm, 1 rodata, 2 bin] sha1_orig=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1_cand=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a

## Parked

See `docs/evidence/func-{80197230,8019253C,801924F4,80190048,80192A00}/PARK.md` and results.jsonl `parked` lines (incl. retries of ovl3's 8018F0D0 5w / 8018FB70 7w).
