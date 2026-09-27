# Overlay lane ovl4 (subsystem ovl_* targets) — 2026-09-27

Per body: `build/lanes/ovl4/t.sh <target> <src> [flags]` (room_lane/try.sh → era_link_check vs the overlay blob, `word mismatches=0`),
`python3 tools/build/disc1_preflight.py --target <t> --deep --only <func>` → PASS, then
`flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>` (serial). Gate lines below are the last gate run that included the leaf.

## Exact

### func_8010BD4C (ovl_039F blob 0x54, 0xe4) — profile era_o2_g0 (default)
libpress-style RLE/back-ref decoder + xor-delta pass; unsigned char b fetch with int c = b copy (gives andi 0xFF + sltiu, then signed bltz loop guards); first draft after that 0

    EXACT_REBUILD_GATE[ovl_039F]=PASS plan=b4ade401d0ad8d30ee365ea04f05d71b86d901a2d6ecc2e591b003105d7e3219 yaml=9a6939ef48ffa5352da25d8aac82030cb9a02eca368a41865ba31d24f0f2493b spans=[18 c, 2 asm, 0 rodata, 2 bin] sha1_orig=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1_cand=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1=e7331867b4d1464da4d5f386d9137eaa1c98422b

### func_8010BE70 (ovl_039F blob 0x178, 0x8c) — profile era_o2_g0 (default)
three word-copy loops, c=N-1 with while(--c != -1) sentinel; first draft

    EXACT_REBUILD_GATE[ovl_039F]=PASS plan=b4ade401d0ad8d30ee365ea04f05d71b86d901a2d6ecc2e591b003105d7e3219 yaml=9a6939ef48ffa5352da25d8aac82030cb9a02eca368a41865ba31d24f0f2493b spans=[18 c, 2 asm, 0 rodata, 2 bin] sha1_orig=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1_cand=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1=e7331867b4d1464da4d5f386d9137eaa1c98422b

### func_8010BEFC (ovl_039F blob 0x204, 0x98) — profile era_o2_g0_fill_epilogue_delay_slot
copy from a0 directly (ret = a0 saved in s0; a separate source local swaps a0/a1 homes: 34w); patch 3 epilogue fill (2w without)

    EXACT_REBUILD_GATE[ovl_039F]=PASS plan=b4ade401d0ad8d30ee365ea04f05d71b86d901a2d6ecc2e591b003105d7e3219 yaml=9a6939ef48ffa5352da25d8aac82030cb9a02eca368a41865ba31d24f0f2493b spans=[18 c, 2 asm, 0 rodata, 2 bin] sha1_orig=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1_cand=e7331867b4d1464da4d5f386d9137eaa1c98422b sha1=e7331867b4d1464da4d5f386d9137eaa1c98422b

### func_8012462C (ovl_0457 blob 0x392C, 0xd8) — profile era_o2_g0 (default)
sprite init; short x/y, unsigned char w, unsigned short h (stack lhu) params; first draft

    EXACT_REBUILD_GATE[ovl_0457]=PASS plan=0cc88e18ca43129010ff1ace0d807e27a90ad9cf18426209b9a5e769c638a0f8 yaml=ff3b71a01b7002c23e55603c08e8cacb327e8fae9b8213eedf094c9c807fce9f spans=[25 c, 2 asm, 1 rodata, 2 bin] sha1_orig=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1_cand=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6 sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6

### func_8018F2F4 (ovl_03D2 blob 0x304, 0x174) — profile era_o2_g0_no_cse_skip_blocks
double-buffer swap + LoadImage/DrawSync/VSync/PutDrawEnv loop; RECT struct copy (align-2 -> lwl/lwr block move); flag==0 ternary polarity; -fno-cse-skip-blocks is load-bearing (cse reuses the block-move dest pseudo for &r2 across the if: 41w with it on; also stops the &r2 loop hoist)

    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=bdefe5ffda189b96049fa4a2b409547df7a39c857b203cfaef4ba5118592018c yaml=a866be17f97351b8f382ca8f39627508711cd038ff47b196d5a3d36a7722f8c8 spans=[15 c, 4 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_8018FBC0 (ovl_03D2 blob 0xBD0, 0x144) — profile era_o2_g0 (default)
task spawn via shared static inline new_task(kind) (list unlink/append + zeroing through char* casts = non-in-struct stores); Pos[k] pointer before the call; f2C=k after the indexed img load (store-after-load order); unused int pad[2] reproduces the 8-byte frame

    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=bdefe5ffda189b96049fa4a2b409547df7a39c857b203cfaef4ba5118592018c yaml=a866be17f97351b8f382ca8f39627508711cd038ff47b196d5a3d36a7722f8c8 spans=[15 c, 4 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_8018FD04 (ovl_03D2 blob 0xD14, 0x118) — profile era_o2_g0 (default)
same inline; caller k=4 local passed to the inline puts li 4 in block 1 (li 4 at entry: 39->7->0); p=D_801D0D8C before the call; zeroing via ((int*)t)[k] is still in-struct in this cc1 (18w) — char* arithmetic needed

    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=bdefe5ffda189b96049fa4a2b409547df7a39c857b203cfaef4ba5118592018c yaml=a866be17f97351b8f382ca8f39627508711cd038ff47b196d5a3d36a7722f8c8 spans=[15 c, 4 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_8018FE1C (ovl_03D2 blob 0xE2C, 0x248) — profile era_o2_g0 (default)
two inlined spawns with distinct t/p/k locals (shared locals: 140w); first spawn f28 before f10 (8w)

    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=bdefe5ffda189b96049fa4a2b409547df7a39c857b203cfaef4ba5118592018c yaml=a866be17f97351b8f382ca8f39627508711cd038ff47b196d5a3d36a7722f8c8 spans=[15 c, 4 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_801922F4 (ovl_03D2 blob 0x3304, 0x204) — profile era_o2_g0 (default)
display-env restore: DISPENV (align-2 lwl/lwr) + DRAWENV (word loop) struct copies in a signed char loop, two ClearImage rects; pointer local p=&D_800B0DBA keeps retail's register base for the flag test + clear (absolute form: 127w)

    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=8f853cd5e86019aa4897cb5f6a5f77bcefe6ffe838293e70e87c6c8587e8597f yaml=69ab14769b73842d9b536a8f555bf7f7df5d0db5513449e4b4e9b672c49c81ec spans=[16 c, 5 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

### func_8018F05C (ovl_0700 blob 0x6C, 0x2e8) — profile era_o2_g0 (default)
camera setup: two MATRIX copies from D_8018EFF4, VectorNormal/SquareRoot0/ratan2/RotMatrix chain; cameras typed as VECTOR structs (vx via cse'd base register, vy/vz absolute - matches retail); m1.t stores in t0,t1,t2 order (t0,t2,t1: 4w)

    EXACT_REBUILD_GATE[ovl_0700]=PASS plan=c9a9f932f1eb039e9da6b85a51c9b21f6500eb23dc45020b07341d5c958fe231 yaml=d142ee90945ade546698d2a23346f938afb9eff4c0a61065f1da460119100822 spans=[36 c, 11 asm, 0 rodata, 2 bin] sha1_orig=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1_cand=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de

## Non-C

* func_8010C4CC (ovl_039F, 0x30): asm/overlays/ovl_039F/7CC.s: 1 spimdisasm '/* handwritten instruction */' ops (trapping add/addi, e.g. 'addi $at,$a0,-0x1' / 'add $t1,$t1,$t1'); cc1 emits addu/addiu only — libpress MDEC VLC hand-asm
* func_8010C4FC (ovl_039F, 0x364): asm/overlays/ovl_039F/7CC.s: 56 spimdisasm '/* handwritten instruction */' ops (trapping add/addi, e.g. 'addi $at,$a0,-0x1' / 'add $t1,$t1,$t1'); cc1 emits addu/addiu only — libpress MDEC VLC hand-asm
* func_8010C89C (ovl_039F, 0x360): asm/overlays/ovl_039F/7CC.s: 58 spimdisasm '/* handwritten instruction */' ops (trapping add/addi, e.g. 'addi $at,$a0,-0x1' / 'add $t1,$t1,$t1'); cc1 emits addu/addiu only — libpress MDEC VLC hand-asm

## Parked (best drafts under build/lanes/ovl4/drafts/)

* **func_80195BC8** (ovl_0700, 0x174) best 48w: pure sched1 order of 28 independent global stores (two 0x30-byte Obj records at D_8019C05C/D_8019C08C + mode/shift shorts at D_8019C050); retail loads the second camera (D_8019C330) early reusing $t1/$t0. Tried: 40 permutations of field-group order/s-m placement/record order (best dzcr+ms-at-end=48), distinct scalar externs (71w), struct Ctl/Obj layout (49w), retail store order literally (83w)
* **func_80195994** (ovl_0700, 0x234) best 40w (unsigned char params) / 82w (int params + a&=0xFF): same record-fill as func_80195BC8 split by two func_8018F55C calls. unsigned char params: callee-saved priority puts a2 in $s6 not $s3 and the shift uses the unmasked value; int params + in-place `a1 &= 0xFF` gives retail's $s2/$s3/$s6 allocation but sched1 hoists the andi above the outer jal (retail: right after it). Tried K&R params, `s1 = a1 &= 0xFF`, pointer local for D_801D0260
* **func_801F1070** (ovl_03C9, 0x18C) best 15w: sibling of matched func_801F0BA0. Residual: signed /32 temp register (retail tmp=$v1 from call result, sra into $s0 in the bgez slot; ours copies into $s0 first) + compare constant `li 4` placed before the D_800E1204 indexed load in retail (ours after; $a0/$v1 vs $v1/$v0). Same wall as ovl3's func_801937B4. Tried: c/=32 split (15), direct c=f()/32 (57), explicit if(c<0)c+=31 (57), temp d (57), cy/r statement order (29), k local for D_800F336C (57/15), four=4 local (cse'd), -fno-schedule-insns (29), 2.8.1
* **func_801F15DC** (ovl_03C9, 0x1B4) best 62w: sibling of func_801F1070 with the same li-4 / D_800F336C register residual (retail $a0 for D_800F336C, hoisted early by sched2 over the sp-based RECT stores; ours gets $v1 and cannot move) + r.w/r.h store order. Tried: c/=32 split, k int/unsigned short local, pointer-arith table read, per-arm table load
* **func_801214D4** (ovl_03C5, 0x1F0) best 98w: movie slice advance; residual: cse reuses the align-2 RECT block-move source address register (D_801228F4) for the following slice.x/w accesses, retail keeps absolute %lo(D_801228F4) loads/stores; also retail reloads cur (lbu twice). Tried struct vs separate-symbol globals, 2.8.1 (+-mno-split-addresses)
* **func_80124C04** (ovl_0457, 0x33C) best 171w: glyph blit with 4-nibble saturating RGB add; control flow/polarity right (if (m != 0) {PIX x4} else skip). Residual: loop strength reduction — retail keeps two bivs (p=row+1 with offsets 0/1, q=row), ours adds a third giv for p+1 (pointer form) or collapses to one pointer (row[j+k] index form, 178w)
