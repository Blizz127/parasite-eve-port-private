# func_8002312C — EXACT (lane exeA5, 2026-09-28)

- VRAM `0x8002312C`, file offset `0x1392C`, size `0x5BC` (ends at `0x13EE8`, next row `[0x13EE8, c, func_800236E8]`)
- Battle-turn facing/target step: `switch (D_8009D254->fE)` over `jtbl_8001078C`.
- Profile `era_o2_g8_func_8002312C`: `-O2 -G8`, env `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_8001078C MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D278,D_8009D2FC`
- Source: `src/func_8002312C.c` (copy in `build/lanes/exeA5/drafts/`)
- Yaml row: `- [0x1392C, c, func_8002312C]` (replaces `- [0x1392C, asm]`; no resume row needed)

## Verification (private lane config build/lanes/exeA5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json --deep --only func_8002312C
  disc1_preflight: PASS (deep, 2095 c / 183 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA5/disc1.yaml --profiles build/lanes/exeA5/disc1_build_profiles.json func_8002312C
  LEAF_STRONG=EXACT func_8002312C span=0x5BC vram=0x8002312C compiled=0x5C0 failures=[none]
tools/analysis/era_link_check.py src/func_8002312C.c 0x8002312C 0x5BC -O2 -G8   (env above, subshell)
  linked .text 1472 bytes, target 0x5bc, word mismatches=0, nonzero_pad=0
```
(compiled 0x5C0 = body + one zero gas pad word.) profile_necessity.py has no `--config` (run after integration).

## Levers (on top of the exeA4 328/367 draft)
1. **Real prototype** `int func_80030534(Ent *, Ent *)` called as `func_80030534(p, D_8009D254)` in the `facing` inline:
   retail's "dead" `addu $5,$6,$0` in a beq slot is the second argument (`e` == `D_8009D254` in `$6`).
2. First path's fail block ends `goto r1;` to the shared `r1: r = 1;` after the DD38 call (retail `j 0x23518`); the
   DD38 arm then no longer cross-jumps into the second path's copy. 328 → 341.
3. `{ int x = func_800518A8(); r = 1; if (x == 0) break; }` (retail `beq $2,$0,end` with `r = 1` in the slot).
4. Facing arm: `r = 1` after the `func_8001A680` call; zero-code `asm volatile("")` between the `f14` store and the
   `f98 |= 0x100` (sched1 otherwise hoists the `f98` load); `{ Mdl *mm = D_8009D278->f68; D_8009CE39 += D_8009CE3A;
   D_8009D1DC = mm->f10 & 0xF; }` (retail loads `f68` early; the `$3` value also stops the tail cross-jump). → 360.
5. `m = st->f68;` reload before the first path's `m->fC & 0x3FF` test, `st` pinned `asm("$7")`. → 362.
6. `r1: r = 1; asm volatile(""); break;` keeps `r = 1` out of the `j` slot (retail fills it from the target). → 364.
7. Zero-code `asm("" :: "r"(m));` just before the reload keeps `$4` (m) live across the `(t+1)*8` index, which
   then takes `$5` as in retail (greg dump: pseudo 158 had no `$4` conflict). → 367/367, 0 word mismatches.
