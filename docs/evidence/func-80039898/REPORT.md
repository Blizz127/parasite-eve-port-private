# func_80039898 — font cursor reset to class 3

- VRAM `0x80039898`, size `0xD8`, profile `era_o2_g0`. Source: `src/func_80039898.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80039898 0xD8`.
- Prior: parked by lane2 (store-then-reload: exact only with a memory-clobber barrier, ruled out)

## Levers
1. D_80091A1C typed as the font state struct (cursor byte +3 = 0x80091A1F, data pointer +0xC = 0x80091A28).
2. Resolves the lane2 PARK: the store-then-reload of the cursor needs NO memory clobber. The slot search is a `static inline int find_slot(...)` whose `int` result is stored into the `unsigned char` cursor; cse cannot forward an int into the zero-extended byte reload, so the tail `cur_attr()` re-reads the cursor exactly as retail. (Storing under a separate `D_80091A1F` symbol lets sched2 move the reload ABOVE the store: wrong code, 14 words.)
3. The inline's parameter copies are what give retail's otherwise-unused 16-byte frame.
4. find_slot uses `goto found; ... r = 255; goto done; found: r = i; done:` so reorg steals `r = i` into the beq slot (return-from-loop spelling: 19 words).
5. `unsigned char sel` (the class search result) gives the in-block `andi a1,a2,0xff`.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80039898
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80039898
disc1_preflight: PASS (deep, 1849 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_80039898 span=0xD8 vram=0x80039898 compiled=0xE0 failures=[none]
```
