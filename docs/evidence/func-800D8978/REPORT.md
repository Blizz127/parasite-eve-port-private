# func_800D8978 — EXACT (lane exeD5, 2026-09-28)

VRAM `0x800D8978`, file `0xC9178`, size `0x1F4` (`asm/disc1/C8B88.s`). FieldEng effect dispatcher
(case 1 field updates + `D_800E27EC < 24` break, case 2 pos/vec + func_800CEE20). Supersedes `PARK.md`
(6-word residual, lanes exeD and exeD3).

- Source: `src/func_800D8978.c` (copy: `build/lanes/exeD5/drafts/func_800D8978.final.c`)
- yaml: `[0xC8B88, asm]` stays; add `- [0xC9178, c, func_800D8978]` before `[0xC936C, c, func_800D8B6C]`
- Profile: default `era_o2_g0`.

## Verification (private config build/lanes/exeD5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD5/disc1.yaml --profiles build/lanes/exeD5/disc1_build_profiles.json --deep --only func_800D8978
  -> disc1_preflight: PASS (deep, 2104 c / 177 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD5/disc1.yaml --profiles build/lanes/exeD5/disc1_build_profiles.json func_800D8978
  -> LEAF_STRONG=EXACT func_800D8978 span=0x1F4 vram=0x800D8978 compiled=0x200 failures=[none]
```

## Lever: one `register int t asm("$3")` for both the +8 value and the +4 counter
```c
register int t asm("$3");
t = D_800E21FA;
t += (-D_800E27EC * 900) / 24;
*(short *)((char *)a1 + 8) = t;
t = *(unsigned short *)((char *)a1 + 4);
... +6 = D_800E21F8; +0xA = D_800E21FC; +2 += 0x80; +4 = t + 1;
```
(The case-2 `func_80077AA4` result moves to a separate `int r`.)
Diagnosis came from the sched ready-list trace in cc1's `-dS`/`-dR` dumps. sched2's tie-break uses the LUIDs that
sched1 left behind. sched1's register-pressure rule (`adjust_priority` → `birthing_insn_p`: a SET of a live
reg with `REG_N_SETS == 1` gets max priority) had lifted the single-set `t` load above sh +6 and pushed the +2
load after the 21FC load.
- With a hard-register destination set several times, the `t` load is not a birth. It keeps its normal priority,
  which it gets through the `$3` anti-dependence on `sh $3,8`. It therefore lands right after `sh +8` and before
  the 21F8 load (retail `lhu $3,4($16)` first).
- Statement order `+2 += 0x80` before `+4 = t + 1` fixes the (+2, 21FC) pair in sched2.
- `t = D_800E21FA; t += div;` gives `addu $3,$3,$4` (the 21FA temp in $3). Writing it as one expression costs 5
  (operand/register swap), and `t = div; t += D_800E21FA` costs 12.
Ladder: the old draft was 6. A `$3` pin on t alone gives 3, and the t load stays a birth (the pin's hard reg is set
once). Pin plus `t++` gives 13 (no dependence, so the load floats above the div chain). A volatile store/load gives
3–13. An asm memory barrier gives 99 (it kills the D_800E27EC CSE). The shared `$3` pin for the +8 value gives 5,
and the split `t = D_800E21FA; t += …` gives 0.
