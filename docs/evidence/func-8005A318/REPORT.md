# func_8005A318 — EXACT (lane exeC5, 2026-09-28)

- VRAM `0x8005A318`, file offset `0x4AB18`, size `0xCE4` (825 words), frame `0xA0`, `asm/disc1/4AB18.s`.
- Source `src/func_8005A318.c` (drafted from retail control flow; no prior draft).
- Profile: existing `era_o2_g8_force_d800c0e20_d800c0e22_absolute`
  (`-O2 -G8`, `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E20,D_800C0E22`) — add the assignment.
- YAML: `- [0x4AB18, c, func_8005A318]` replaces `- [0x4AB18, asm]` (next row
  `- [0x4B7FC, c, func_8005AFFC]` already exists; 0x4AB18 + 0xCE4 = 0x4B7FC).

## What it does
Inventory transfer between the two item lists (`who`, `who == 0`): either moves a slot byte
(`slot >= 0`; class match on `& 0xE0`, else first empty `& 0x1F` slot, else
`func_8004CC50(7, 0)`) or merges the three shorts at +0xE/+0x10/+0x12 and the count at +0xA
(capped at 999, overflow sent to the type-descriptor record). If `cnt < 999` it consumes a
type-12/13 item (key list `D_800C1EB8[100]`, or the carried list), then (flags bit 0 clear)
folds the source item's count into its descriptor, removes it, and re-picks the equipped
weapon (`D_800C0E20`, type 8) / armour (`D_800C0E22`, type 9).

## Verification (private lane config build/lanes/exeC5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC5/disc1.yaml --profiles build/lanes/exeC5/disc1_build_profiles.json --deep --only func_8005A318
  disc1_preflight: PASS (deep, 2103 c / 177 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC5/disc1.yaml --profiles build/lanes/exeC5/disc1_build_profiles.json func_8005A318
  LEAF_STRONG=EXACT func_8005A318 span=0xCE4 vram=0x8005A318 compiled=0xCF0 failures=[none]
  (0xCF0 = 0xCE4 + 12 bytes of trailing 16-byte alignment pad)
MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800C0E20,D_800C0E22 era_link_check.py src/func_8005A318.c 0x8005A318 0xCE4 -O2 -G8
  word mismatches=0
```
Profile necessity (by hand): no force → LD FAILED; only `D_800C0E20` or only `D_800C0E22` →
LD FAILED; `-O2 -G0` → 779 mismatches.

## Levers (aligned-diff ladder)
1. First draft with macro-expanded helpers: 255. **Selector as a `static inline` taking
   `unsigned int k`** — retail keeps the `sltiu k,2` test on `who == 0`; a macro lets fold drop it.
   Separate short-lived counters for the slot-move loops: 219.
2. **`slot` reused as every later loop counter** and for `slot = sel(who)` (retail keeps them all
   in `$s1`), CAP-style clamp `(x > 999) ? 999 : x` (retail recomputes the sum through copies): 124.
3. Block-local `v, w` in each lookup copy (retail's `move a1,v1` `w` copy survives), class-loop
   counter + `cls == (…)` order, `(src + slot)` / `(dst + j)` operand order: 73.
4. `typedesc()` inline for the merge block only, block-local `t` per open-coded copy (retail's
   `t` in `v1`), `int unused[2]` in the last block (retail's frame has one more dead 8-byte
   ST_REGS slot; declaring it there keeps `tmp` at `0x10`), `register int amt asm("$4")`: 43.
5. Block-local `e, w, v` in each copy of the remove body (the `sh zero` then falls into the
   `bnez` delay slot): 28. Open-coded selector for the final `slot = …` (no retval copy): 17.
   func_8005332C-shaped inline `lookup()` + fresh `pp` local for the weapon-9 loop: 9.
   `eight = 8` hoisted constant for the walk: 5.
6. Final block (`car` = `&D_800C0E48` kept in `$s0` across the calls):
   - cse's jump equivalence makes the EQ's op0 canonical; `car` as a plain pseudo is turned back
     into the constant, as a `$16` pin it gets substituted for the walker; the working form is
     both tests written `D_8009D048 == car` (identical operands, so `thread_jumps` pairs them and
     the first test's failure branches straight to the epilogue, as retail does) plus
   - **`asm volatile("" : "=r"(car) : "0"(car));` at the end of the call block** — kills the
     `D_8009D048 == car` equivalence only on the path through the calls, so the second test and
     the walker use `car` / the reloaded `D_8009D048`. A non-volatile barrier there is hoisted
     above the calls by sched1 (breaks the delay-slot fill); `volatile`, a `"memory"` clobber or
     an `"m"(D_800C0E22)` input all pin it: 0 (LINK_EXACT).
