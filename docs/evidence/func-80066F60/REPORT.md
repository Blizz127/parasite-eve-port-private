# func_80066F60 — matching C leaf (lane exeC4, 2026-09-27)

- VRAM `0x80066F60`, file offset `0x57760`, size `0x268` (154 words); source `src/func_80066F60.c`
- YAML rows: `- [0x574E8, asm]` (unchanged) then `- [0x57760, c, func_80066F60]`; next row
  `[0x579C8, c, func_800671C8]` unchanged (0x57760 + 0x268 = 0x579C8).
- Profile: default `era_o2_g0` (`-O2 -G0`), cc1 2.7.2 — no assignment needed.
- Role: two-pass sprite + DR_MODE primitive builder (16-byte sprite records at `a1`, 8-byte draw-mode
  records at `a1 + n*32`), clamps `D_800BD024` to the max texture x.

## Verification (lane private config build/lanes/exeC4/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC4/disc1.yaml --profiles build/lanes/exeC4/disc1_build_profiles.json --deep --only func_80066F60
  -> disc1_preflight: PASS (deep, 2094 c / 185 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_80066F60
  -> LEAF_STRONG=EXACT func_80066F60 span=0x268 vram=0x80066F60 compiled=0x270 failures=[none]
python3 tools/analysis/era_link_check.py src/func_80066F60.c 0x80066F60 0x268 -O2 -G0  -> word mismatches=0 / LINK_EXACT
```

## Levers (prior best 141 → 0; ladder 152 → 44 → 27 → 22 → 2 → 0)
1. **Array indexing instead of pointer walkers** for the sprite / record / position arrays
   (`sp[i]`, `rec[i]`, `pos[i]`, with `Spr`/`Rec` structs): loop.c then strength-reduces one giv per
   array whose value is the element base, so fields keep their 0..14 displacements (the pointer-walker
   form made offset givs `p+14`, `rs+4`, `q+4` and spent an extra callee-saved register).
2. **Draw-mode walker as a pointer with a no-code barrier** `asm("" : "=r"(dm) : "0"(dm));` at the loop
   top: dm is no longer a biv, so no `dm+4` giv (index form spills the base instead).
3. **`extern unsigned char *volatile D_800B1624`**: retail reloads the pointer for each of the two
   `+0x38`/`+0x3A` reads.
4. `c = D_800BD024;` read first (retail loads it before the frame is built); `unsigned int k`
   (`sltiu`); `pos`, `rec`, `k` declared in that order (spill slots 0x18/0x20/0x28); separate `w1` for
   the first call (w in `a1`); guarded preheader `i = 0; if (i < n) { sp = ...; dm = ...; for (; i < n; i++) ...}`
   (dm/sp copies after the `beqz`).
5. **`asm("" : : "r"(a0), "r"(a0), "r"(a0), "r"(a0));` at the end of the inner loop** raises the
   global-alloc priority of `a0` above `dm` (a0 → s2, dm → s3); exactly four operands (3: 22, 5+: 26).
6. **`Spr *s = &sp[i];` at the loop top, all sprite fields through `s->`**: the sprite giv is then
   discovered at its first insn instead of at the post-call `clut` store, so loop.c emits the giv
   increments in retail's rec / pos / sprite order (2 → 0).
