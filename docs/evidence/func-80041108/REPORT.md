# func_80041108 — EXACT (lane exeB6, 2026-09-28)

Memory-card operation state machine (switch on `D_800A0ED4[idx].f1`, `jtbl_80010F6C`).
- VRAM `0x80041108`, file `0x31908`, size `0xF18`; ends at `0x32820` = `func_80042020` (existing c row).
- Source `src/func_80041108.c` (exeB5 near-miss draft + the levers below).
- Profile `era_o2_g8_dispatch_80010f6c_force_d8009ee70_d80092224_d80092230_d800a1704_d800a1838_d800a1854_d800a1864_d800a186c_absolute`:
  `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1 MASPSX_DISPATCH_FOLD=jtbl_80010F6C
  MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009EE70,D_80092224,D_80092230,D_800A1704,D_800A1838,D_800A1854,D_800A1864,D_800A186C`.
- YAML: `- [0x31908, c, func_80041108]` (replaces `- [0x31908, asm]`).

## Verification (private config build/lanes/exeB6/)
- `disc1_preflight.py --config build/lanes/exeB6/disc1.yaml --profiles build/lanes/exeB6/disc1_build_profiles.json --deep --only func_80041108`
  → `disc1_preflight: PASS (deep, 2112 c / 170 asm / 2 rodata)`
- `leaf_strong_check.py ... func_80041108` → `LEAF_STRONG=EXACT func_80041108 span=0xF18 vram=0x80041108 compiled=0xF20 failures=[none]`
- profile_necessity.py lacks `--config`, so this is a manual knob check (`era_link_check.py src/func_80041108.c 0x80041108 0xF18 -O2 -G8`):
  full → exact; without THREE_WORD → 2; without DISPATCH_FOLD → 2; without FORCE_ABSOLUTE → LD FAILED.

## Levers (39 → 0)
1. **Case-2 clear loop ascending, on `i`**: `for (i = 0; i < 15; i++) c->e[i].state = 2;`. loop.c's dbra reversal
   rebuilds it as retail's `li 14` counter (emitted after the moved `li 2`) with the descending `c+952`/`28(ptr)`/`-68`
   giv. `i` (not `n`) gives retail's homes: counter $4 here and `i`=$4 / `n`=$5 in the count logic.
2. **Case 1 / case 13**: test `D_800A0ED4[idx == 0].f8` directly (no `n =` copy), so the load stays in temp `$2`.
3. **Case 3**: `cur = c->f6; if (c->f6 < c->f2 * 2) { n = c->f2 * 2; while (1) { int k = ...; ...; cur = c->f6 = cur + 1;
   if (cur >= n) break; } }`. The entry test spelled exactly like the later ternary lets jump threading send the
   false edge straight to the `0xFF` arm, which is what lets reorg put the `lim` copy in the beq delay slot.
   A block-local `k` (not the function-wide `r`) takes `$3`.
4. **Case 10**: `e = (CardEntry *)(c->f3 * 68 + (int)c);` (retail `addu a2,v0,s0` operand order).
5. **Case-2 dir loop**: an empty `do { } while (0);` between `c->count += dir.size >> 13;` and
   `func_80072794(&dir)`. gcc 2.7.2 sched.c makes an insn that follows a mid-block LOOP_BEG/END note depend on every
   earlier set (sched_analyze_insn `loop_notes`). The `a0 = &dir` move therefore inherits the load-latency priority
   (2) and wins the LUID tie, so it stays next to the `jal` (reorg puts it in the delay slot, as retail). Plain
   statement orders, pointer vars, `$4` pins and asm ties all left it at priority 1.
