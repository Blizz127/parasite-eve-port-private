# func_80044444 — item-list input handler

- VRAM `0x80044444`, size `0x3AC`, profile `era_o2_g8`. Source: `src/func_80044444.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80044444 0x3AC`.


## Levers
1. `-G8` for D_8009CF00/CF5C/CF8C/CF90/CF94/CEFC.
2. Return value `r` in $s3; every success/fail arm is written `func_...(); r = 1;` (call FIRST): with `r = 1;` first, jump.c cross-jumps the two `func_80052634()` tails (retail keeps them apart and reorg drops `li s3,1` into each jal slot).
3. The early fail arm `goto out;` to the shared `out: return r;` so the constant is not folded into `li v0,1` (2 words).
4. Use-item result: `x = rec ? (rec->b6 < 10 ? func_80057D18 : func_80057D30)(id) : 0; v = x; func_800512AC(0, &v);` gives one `sw v0,16(sp)` in the call slot (assigning `v` directly: 5 extra words).
5. `self->f24 == 13 || self->f24 == 52` flags; cancel routing `t == 13 || t == 14` / `t == 51 || t == 52` become retail's `addiu/sltiu 2` range tests.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80044444
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80044444
disc1_preflight: PASS (deep, 1864 c / 290 asm / 2 rodata)
LEAF_STRONG=EXACT func_80044444 span=0x3AC vram=0x80044444 compiled=0x3B0 failures=[none]
```
