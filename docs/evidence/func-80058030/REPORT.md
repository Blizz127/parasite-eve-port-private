# func_80058030 — inventory entry swap

- VRAM `0x80058030`, size `0x30C`, profile `era_o2_g8_force_d800c0e20_d800c0e22_absolute`. Source: `src/func_80058030.c`.
- Retail disassembly: `tools/analysis/pe_dis.sh 0x80058030 0x30C`.


## Levers
1. New profile `era_o2_g8_force_d800c0e20_d800c0e22_absolute`: the equipped weapon/armor indices D_800C0E20/E22 are scalar signed chars kept absolute (as arrays cc1 caches their base in a register).
2. List-select prologue as in func_8005CAEC (D_8009D048 = D_800C0E48 etc.).
3. `static inline void swap16(short *p, short *q) { *p ^= *q; *q ^= *p; *p ^= *q; }`: three of the four expansions are cross-jumped into retail's shared tail.
4. Stock-side swaps take a local base `st = D_800A1FD4; swap16(&st[i], ...)` so the `la` is emitted before `i*2` (retail's slot/nop pattern); the special-item check pointer uses `register short *bs asm("$3") = D_800A1FD4; p = &bs[i];` (4 words otherwise: $v0/$v1 swap).
5. Refusals `goto fail;` -> `fail: r = 0; out: return r;` so both exits share `move v0,s4`.

## Verification (decompA private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json --deep --only func_80058030
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompA/disc1.yaml --profiles build/fanout/decompA/disc1_build_profiles.json func_80058030
disc1_preflight: PASS (deep, 1865 c / 289 asm / 2 rodata)
LEAF_STRONG=EXACT func_80058030 span=0x30C vram=0x80058030 compiled=0x310 failures=[none]
```
