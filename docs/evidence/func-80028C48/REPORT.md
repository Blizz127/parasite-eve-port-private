# func_80028C48 — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x80028C48`, file offset `0x19448`, size `0x24C` (ends on `jr $ra` at `0x80028E8C` + delay slot; next row `func_80028E94` at `0x19694`)
- Profile: default `era_o2_g0` (`-O2 -G0`, no environment) — no assignment needed
- YAML: `- [0x19448, c, func_80028C48]` replaces `- [0x19448, asm]` (span is the whole former asm row)

Shape: actor step. Snapshots `a->fE / f98 bit 9 / f14..f1C` into the sub-record's
`0xBC..0xC8` block the first time (`fBC == 1` gate, skipped when `D_8009D278->f4C & 0x80000`
and the `f1C[(f0>>17)&0x70]` byte is set), faces the actor (`func_8001A680(a, 1)` when
`func_800305C8(D_8009D254, a)` lies in `(0x400, 0xC00)`, else `0`), sets `f1C = 0x10000`,
`f98 |= 0x1000`, and for `f0 & 0x100000` runs the `fA4` knockback mode (1/2/3 set speed
400/70/20 and sub-mode 2/5) then adds `speed * cos/sin(heading+0x800) << 4` to `f28/f30`.

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_80028C48
  disc1_preflight: PASS (deep, 2044 c / 216 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_80028C48
  LEAF_STRONG=EXACT func_80028C48 span=0x24C vram=0x80028C48 compiled=0x250 failures=[none]
```
`profile_necessity.py` lacks `--config` (main config only) — skipped; the leaf is on the
default profile (`-O2 -G8` gives 127 mismatches).

## Levers (mismatch ladder)

1. First draft → 88: range test folded (`r > 0x400 && r < 0xC00` → `addiu -1025 / sltiu 2047`),
   and `case 0` inside the switch reordered the dispatch tree.
2. `if (s->fA4.f0 == 0) return;` before `switch (m->f0) { case 1/2/3 }`, per-case
   `s->fA4.f2 = K; break;` (cross-jumped into retail's shared `sh 0xA6`), and both trig
   calls take `m->f4` (cse turns the first into the `sll/sra` of the just-stored value) → 83.
3. `k = 0; if (..) if (..) k = 1;` → store-flag `slti`; `if (&&) f(a,1); else f(a,0);` → still
   range-folded (89).
4. **Nested `if (r > 0x400) { if (r < 0xC00) { f(a, 1); goto next; } } f(a, 0); next:`** → 0:
   no range fold, and jump2 cross-jumps the two calls into retail's single `jal` with
   `a1 = 1` in the branch delay slot and `a1 = 0` on the fallthrough.
