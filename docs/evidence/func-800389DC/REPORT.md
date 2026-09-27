# func_800389DC — save-slot record load

- VRAM `0x800389DC`, size `0x1E4`, profile `era_o2_g0`. Source: `src/func_800389DC.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x800389DC 0x1E4`.


## Levers
1. Retry shape: `retry: while (read(...) == -1) {}  while ((v = poll()) != 0) if (v == -1) goto retry;` reproduces the rematerialised `la` bases at the retry label and the `$s0` reuse as the -1 constant.
2. Record copy through ONE running index `k = off + 1; b1 = buf[k++]; ...` (three spellings with explicit `off + n` give the same code); per-field `off + n` indexing without k costs 82 words.
3. `(unsigned char)slot` at the use gives the late `andi 0xFF` (in the beqz slot).

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_800389DC
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_800389DC
disc1_preflight: PASS (deep, 1844 c / 296 asm / 2 rodata)
LEAF_STRONG=EXACT func_800389DC span=0x1E4 vram=0x800389DC compiled=0x1F0 failures=[none]
```
