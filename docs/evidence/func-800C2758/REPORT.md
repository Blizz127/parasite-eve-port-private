# func_800C2758 — EXACT (lane exeE3, 2026-09-28)

VRAM `0x800C2758`, file `0xB2F58`, size `0x398` (230 words). Script-VM step. Default era profile
(`era_o2_g0`, cc1 2.7.2 `-O2 -G0`, outside the 2.8.1 band). Supersedes `PARK.md` (was 96 / 76).

YAML: `- [0xB2F58, c, func_800C2758]` (next row `[0xB32F0, c, func_800C2AF0]` already present).

## Verify (lane private config build/lanes/exeE3/)
- `python3 tools/analysis/era_link_check.py src/func_800C2758.c 0x800C2758 0x398 -O2 -G0`
  → `linked .text 928 bytes, target 0x398, word mismatches=0, nonzero_pad=0`
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeE3/disc1.yaml --profiles build/lanes/exeE3/disc1_build_profiles.json --deep --only func_800C2758`
  → `disc1_preflight: PASS (deep, 2111 c / 171 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_800C2758`
  → `LEAF_STRONG=EXACT func_800C2758 span=0x398 vram=0x800C2758 compiled=0x3A0 failures=[none]`
- `profile_necessity.py` has no `--config` option (uses main configs); leaf uses the default
  profile, so no assignment to prove.

## Load-bearing levers (mismatch ladder, from the PARK draft at 96)
1. `int done` with loop test `while (!(unsigned char)done)` — keeps the `li s7,1` / separate
  `li v0,1; sb` in the else arm AND retail's `andi v0,s7,0xff` loop test (96 → 53).
2. Init order `done = 0; ret = 0;` (prologue `move s7,zero` before `move s6,zero`) (53 → 49).
3. `signed char sd` shared by the 0x1000 / 0x2000 arms (`move a1,s2` from raw `hi`, sign-extend
  at use) (→ 26 with the other fixes).
4. **Shared cross-block index variables make the pseudo global, which stops local-alloc tying
  the `hi - K` temp to the `andi` result.** `int r = (unsigned char)(hi - 0x20/0x30)` shared by
  the `+=`/`-=` arms (index in `$a0`), `unsigned char u = hi - 0x40/0x50` shared by the
  var-move arms (26 → 1). Each separate in-arm expression is a one-block pseudo that
  `combine_regs` ties to its andi output, giving the wrong local qty priority.
5. Prototype `func_800C2B90(Obj *, unsigned char, int, int)` + passing `lo` (the implicit
  conversion reads `lo`'s register `$s4`; `lo & 0xFF` gets CSE'd onto `op`) (1 → 0).
  The earlier `hz` local and 16 empty asm uses were only needed before lever 4/5; the final
  source has neither.
