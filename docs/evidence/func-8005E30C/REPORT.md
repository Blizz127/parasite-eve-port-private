# func_8005E30C — pad event dispatch

- VRAM `0x8005E30C`, size `0x1D8`, profile `era_o2_g8`. Source: `src/func_8005E30C.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x8005E30C 0x1D8`.


## Levers
1. `-G8` for the queue globals (0x36C..0x37C($gp)).
2. The queue pop is a `static inline pop_event(Ev *out)` with nested `if (head) { walk; if (e) {unlink; *out = *e;} else zero } else zero` — both zero arms are laid out at the end exactly as retail (the early-return spelling: 95 words). The two zero arms differ in retail ($a3-based vs $sp-based stores) and cc1 reproduces that.
3. `switch (ev.type)` with cases 1,2 (shared window loop, `|= 0x20000` for repeat) and 4 (release with bit 0x20 -> handler(w, 0x10000)).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_8005E30C
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_8005E30C
disc1_preflight: PASS (deep, 1843 c / 297 asm / 2 rodata)
LEAF_STRONG=EXACT func_8005E30C span=0x1D8 vram=0x8005E30C compiled=0x1E0 failures=[none]
```
