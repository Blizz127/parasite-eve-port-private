# func_8003D050 — EXACT (lane exeB6, 2026-09-28)

Model/primitive binder (part/vertex table pointers into `Obj`, GT4/G4/GT3/G3 code bytes, UV copy, part-index
table, identity matrices, `func_8003D94C` palette rebase).
- VRAM `0x8003D050`, file `0x2D850`, size `0x7DC`; ends at `0x2E02C` = `func_8003D82C` (existing c row).
- Source `src/func_8003D050.c` (exeB4 28-word draft, plain-copy form, + levers below).
- Profile: **default `era_o2_g0`** (no assignment). The old PARK profile `-O2 -G8 + FORCE_ABSOLUTE=D_8009CDDC`
  also reproduces it, but default `-O2 -G0` does too, so an assignment would be REDUNDANT.
- YAML: `- [0x2D850, c, func_8003D050]` (replaces `- [0x2D850, asm]`).

## Verification (private config build/lanes/exeB6/)
- `disc1_preflight.py --config build/lanes/exeB6/disc1.yaml --profiles build/lanes/exeB6/disc1_build_profiles.json --deep --only func_8003D050`
  → `disc1_preflight: PASS (deep, 2113 c / 169 asm / 2 rodata)`
- `leaf_strong_check.py ... func_8003D050` → `LEAF_STRONG=EXACT func_8003D050 span=0x7DC vram=0x8003D050 compiled=0x7E0 failures=[none]`
- `era_link_check.py src/func_8003D050.c 0x8003D050 0x7DC -O2 -G0` → 0 mismatches (profile_necessity lacks --config).

## Levers (28 → 0)
1. **`o->fBA = flag;` as a real `short fBA` struct member** (was `*(short *)((char *)o + 0xBA)`). gcc 2.7's
   MEM_IN_STRUCT_P alias heuristic lets a fixed-address scalar (the stack-parameter slots `a4..a7`, `out`) move past
   a varying-address *struct* store, but not past a cast-pointer store. With the field, sched1 sinks the stack-param
   loads after `sw f10` as retail does. This was the exeB4/exeB5 residual ("stack-param loads hoisted").
2. `c += m->nVerts * 4;` before `p = p0; o->f54 = p;` (copy lands after the second `lhu nVerts`).
3. `i = 0;` as the first statement (retail's `addu t2,zero,zero` in the `lbu nParts` delay slot).
4. `asm("" : : "r"(a6));` just before `if (a7 > 0)`: it raises a6's global-alloc priority, so a6/a7 take retail's
   `$25`/`$6` homes. Without it: 4 mismatches (the a6/a7 home swap).
