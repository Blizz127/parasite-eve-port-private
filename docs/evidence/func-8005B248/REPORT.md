# func_8005B248 — LINK_EXACT

- VRAM `0x8005B248`, size `0x15C`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8005B248` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005B248` → `LEAF_STRONG=EXACT func_8005B248 span=0x15C … failures=[none]`
- Levers: both loops are written as `while (*p != 0) { if (bit test) break; p++; if (p >= END) …; }`
  behind an explicit `p >= END` entry guard (the natural `while (p < END)` form rotates
  differently); there are no dummy locals, because the 0x40 frame is just args + 4 saved
  regs (an `int unused[4]` adds 16); the fn-ptr `D_8009D0B4` is typed `unsigned char *(*)(short)`,
  which gives `lhu` + `sll/sra 16`. Mask 0x1FE, window source D_8009D0A4.
- Shape: `D_8009D0A0 = D_8009D0A4`; skip leading entries until one with type bit in
  mask 0x1FE; collect the run of matching entries; `func_800723A4` (qsort) with
  comparator `func_8005B124`.
