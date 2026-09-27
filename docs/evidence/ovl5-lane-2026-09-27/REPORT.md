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
