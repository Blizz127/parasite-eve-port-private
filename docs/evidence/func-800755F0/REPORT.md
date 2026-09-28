# func_800755F0 — EXACT (lane exeE3, 2026-09-28)

VRAM `0x800755F0`, file `0x65DF0`, size `0x4F8` (318 words). PutDispEnv-shaped display
environment commit (`D_80095744->f(0x10)` GP1 pushes, shadow compare against the saved
`0x14`-byte env at `D_800957B8`, `func_80071A34` copy-back). Supersedes `PARK.md` — that park
used cc1 2.7.2; this function is in the 2.8.1 band and matches there.

Profile `era_o2_g0_cc1_281_symbol_load_dest_temp` (existing): `ERA_CC1_VER=2.8.1`,
`-O2 -G0 -mno-split-addresses`, `MASPSX_SYMBOL_LOAD_DEST_TEMP=1`.
YAML: `- [0x65DF0, c, func_800755F0]` (next row `[0x662E8, c, func_80075AE8]` exists).

## Verify (lane private config build/lanes/exeE3/)
- `ERA_CC1_VER=2.8.1 MASPSX_SYMBOL_LOAD_DEST_TEMP=1 python3 tools/analysis/era_link_check.py src/func_800755F0.c 0x800755F0 0x4F8 -O2 -G0 -mno-split-addresses`
  → `linked .text 1280 bytes, target 0x4f8, word mismatches=0, nonzero_pad=0`
- `disc1_preflight.py --config build/lanes/exeE3/disc1.yaml --profiles build/lanes/exeE3/disc1_build_profiles.json --deep --only func_800755F0`
  → `disc1_preflight: PASS (deep, 2112 c / 170 asm / 2 rodata)`
- `leaf_strong_check.py --config ... --profiles ... func_800755F0`
  → `LEAF_STRONG=EXACT func_800755F0 span=0x4F8 vram=0x800755F0 compiled=0x500 failures=[none]`
- Profile necessity (manual): without `MASPSX_SYMBOL_LOAD_DEST_TEMP` → 12 mismatches;
  2.7.2 `-O2 -G0` (+patch 3) → 278. (`profile_necessity.py` has no `--config`.)

## Ladder (2.8.1 first draft 284 → 0)
1. 2.8.1 alone fixed the PARK's prologue residual (`la s2` before `lbu`, `lui s0,0x800` in the
   `bnez` slot).
2. Shadow RECT read as `(short)` of a `volatile unsigned short` field → retail's `lhu`+`sll`/`sra`
   (a plain `short` gives `lh`).
3. `c = pad0 ? h < 289 : h < 257; if (!c)` — single test of a ternary value (276 → ~157).
4. `CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))` for all eight clamps —
   reproduces the `li a1,K` delay-slot ladders and the `move a0,a1` joins.
5. `x0 = env->screen.y` (reuse of `x0` as the y temp) → retail's `lh a0,10(s1)`.
6. Reload of `env->pad0` for the clamp-arm test via `((volatile DISPENV *)env)->pad0`
   (cse-skip-blocks otherwise carries the index read across the `if (sw)` block).
7. `d = -T[0] + T[1]` (T0 loaded first), `d` computed before `x0`.
8. GP1 words written `Y | C | X`; the first and last with `register int yy asm("$3")` for the
   Y term; `register int x1 asm("$3")`.
9. Empty `asm("" : : "r"(d))` after `x1 = x0 + d` and `asm("" : : "r"(env))` before
   `return env` (global-alloc priority; each is load-bearing: 120 / 48 without).
