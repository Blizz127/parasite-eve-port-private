# func_8005EED4 — EXACT (lane exeF, 2026-09-28)

- VRAM `0x8005EED4`, file offset `0x4F6D4`, size `0x2CC` (179 words), `asm/disc1/4F6D4.s`.
- Profile `era_o2_g8` (`-O2 -G8`, stock cc1 2.7.2). Default `-O2 -G0` gives 177 word mismatches
  (profile hand-checked; `profile_necessity.py` has no `--config` and rejects the private manifest).
- YAML row: `- [0x4F6D4, c, func_8005EED4]` replaces `- [0x4F6D4, asm]` (next row `0x4F9A0` is
  already `func_8005F1A0`, so no resume row).

## Verification
```
python3 tools/analysis/era_link_check.py src/func_8005EED4.c 0x8005EED4 0x2CC -O2 -G8
  linked .text 720 bytes, target 0x2cc, word mismatches=0, nonzero_pad=0
  LINK_EXACT
python3 tools/build/disc1_preflight.py --config build/lanes/exeF/disc1.yaml --profiles build/lanes/exeF/disc1_build_profiles.json --deep --only func_8005EED4
  disc1_preflight: PASS (deep, 2130 c / 155 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeF/disc1.yaml --profiles build/lanes/exeF/disc1_build_profiles.json func_8005EED4
  LEAF_STRONG=EXACT func_8005EED4 span=0x2CC vram=0x8005EED4 compiled=0x2D0 failures=[none]
```

## Levers (mismatch ladder, from the 2-word parked draft build/lanes/exeC/drafts/func_8005EED4.c)
The old residual was the y+1 / u1 add order. From `cc1 -dS` ready lists: at T-45 both sched1 and
sched2 see the x add, u1 add and y add at priority 2 and pick by LUID; the y add's LUID came from
the y-store line (combine folded the add into the HI truncation there), which is after the u1 line.
1. `short x; short y;` — each add gets its own truncation insn at its source line, so the
   tie resolves x < y < u1 (retail order). Alone: 31 (register allocation cascade: `lo` went to
   a0, the f+1 value to v1, `v` to a1, because sched1 changed pseudo lifetimes).
2. `asm("" : : "r"(v));` right after `v = row * 12;` — an output-less asm is volatile, so it is a
   sched barrier (u chain ahead of v chain, lo dies before v is born → lo=v1) and adds refs to
   v (local-alloc gives v v1 ahead of f+1). With int u,v: 3 (a separate QI truncation insn for u
   landed after the barrier and pushed `addiu a1,a1,1` ahead of the two `lw`s).
3. `unsigned char u; unsigned char v;` — no truncation insns → 0 (LINK_EXACT).
   (uchar u,v without the asm: 31; uchar u,v on the int-x/y draft: 2.)
