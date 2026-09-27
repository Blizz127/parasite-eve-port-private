# func_80026824 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x80026824`, file offset `0x17024`, size `0x4CC` (ends on `jr $ra` at `0x80026CE8` + delay slot; next function `func_80026CF0` at `0x174F0`)
- Profile `era_o2_g8_dispatch_800108ac_expand_div_force_d8009d2a4_d8009d254_d8009d278_d8009d2b0_d8009d1a0_d8009d1f4_absolute`:
  `-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D2A4,D_8009D254,D_8009D278,D_8009D2B0,D_8009D1A0,D_8009D1F4`,
  `MASPSX_EXPAND_DIV=1`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_800108AC`
- YAML: `- [0x17024, c, func_80026824]` + `- [0x174F0, asm]` (replaces `- [0x17024, asm]`)

Shape: battle command-menu commit (`mode` 1 = command chosen, 2 = target cursor). Pushes
`{who, cmd, (signed char)D_8009D2D8}` records onto the `D_800BE830[D_8009CE3C]` queue
(single push, the 7-entry random `0x196` push, the `0x197/0x198` special commands with a
`func_8006DE80` effect, the `s >= 0x199` push, and the mode-2 cursor push), plus the
`0x189..0x18D` sub-menu switch (jump table `jtbl_800108AC`).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_80026824
  disc1_preflight: PASS (deep, 2048 c / 215 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_80026824
  LEAF_STRONG=EXACT func_80026824 span=0x4CC vram=0x80026824 compiled=0x4D0 failures=[none]
```
`profile_necessity.py` has no `--config` — skipped. Manual necessity: without
`MASPSX_THREE_WORD_SYMBOL_STORE` → 2 mismatches; without `MASPSX_EXPAND_DIV` → 228.

## Levers (mismatch ladder)

Started from the parked decompA draft (222: control flow, jump table, frame exact; every
queue push differed in temp allocation).

1. **Queue-push idiom** (same one that closed `func_80025BD8`/`func_80026600`): a block-local
   `Q *q = &D_800BE830[D_8009CE3C];` initializer, the three field stores, then
   `D_8009CE3C++` / `D_8009D2D8--` (or `= 0`) as separate statements after the block —
   instead of a cached `k = D_8009CE3C; c = D_8009D2D8; ... D_8009CE3C = k + 1`. First
   push → 211, default push → 138.
2. `switch (s) { case 0x197: ...; case 0x198: ...; }` instead of an `if/else if` chain
   (retail tests both before either body) + the `s >= 0x199` push in the idiom → 59.
3. `0x196` random loop: `c = D_8009D2D8; g = (signed char)c; D_8009CE3C++;
   w = D_8009E000[rnd].who; q = &D_800BE830[k];` then stores (brute-forced over 144
   orders) → included above.
4. **Mode-2 push: the entry value read into a local before the pointer**
   (`void *w = D_8009E000[D_8009CE44].who; Q *q = &...;`) → 0 (every store order is 0).

Tool note: `era_link_check.py` compares only `min(len(rom), len(linked))` words, so a
*shorter* link can read `word mismatches=0` — intermediate 1216-byte links here were 3 words
short; the final link is 1232 bytes (0x4CC + 4 zero pad) and `leaf_strong_check` agrees.
