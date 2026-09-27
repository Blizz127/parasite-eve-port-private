# func_800480AC — LINK_EXACT

- VRAM `0x800480AC`, file `0x388AC`, size `0x150`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_800480AC` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_800480AC` → `LEAF_STRONG=EXACT func_800480AC span=0x150 vram=0x800480AC compiled=0x150 failures=[none]`
- Levers:
  - `signed char D_800C0E22[]` is accessed through a pointer local `s`. A scalar
    extern goes gp-relative and fails to link under `-G8`
    (`relocation truncated to fit: R_MIPS_GPREL16`).
  - The loop bound is read as `r[0x14]` in the `for` condition. A named `n`
    local changes the `blez` test (26 mismatches).
  - The operand order is `(r + i)[0x15]` (4 mismatches otherwise).
