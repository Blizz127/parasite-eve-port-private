# func_80036448 — EXACT (lane exeB4, 2026-09-27)

- VRAM `0x80036448`, file offset `0x26C48`, size `0x980` (608 words): the entity-pair contact pass
  (clears the `0x2040000` flags, then for each pair tests a scaled sphere, an optional f21C/f220
  plane circle, pushes apart via `func_80035F54`, links contact records through `func_80012700`, and
  runs the hitbox × hitbox `f194` callbacks).
- Profile: default `era_o2_g0` (`-O2 -G0`), so no assignment is needed.
- YAML: `- [0x26C48, asm]` → `- [0x26C48, c, func_80036448]` (the next row `0x275C8` is already c).
- Source: `src/func_80036448.c` (copy in `build/lanes/exeB4/drafts/`).

## Verification (private lane config build/lanes/exeB4/)
- `python3 tools/analysis/era_link_check.py src/func_80036448.c 0x80036448 0x980 -O2 -G0` → `word mismatches=0`
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeB4/disc1.yaml --profiles build/lanes/exeB4/disc1_build_profiles.json --deep --only func_80036448`
  → `disc1_preflight: PASS (deep, 2089 c / 189 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config ... --profiles ... func_80036448`
  → `LEAF_STRONG=EXACT func_80036448 span=0x980 vram=0x80036448 compiled=0x980 failures=[none]`
- profile_necessity: default profile, nothing to assign (run the repo-wide check after integration).

## Levers (starting from build/archive/fanout/decompC/drafts/func_80036448.c, 732 words / 515 mismatches)
1. **One shared LINK pair through a `goto`**: the plane-miss arm jumps into the simple branch's link
   code (`goto link;`), and the simple branch is laid out first (`if (!a->f1AC || !b->f1AC)`). The plane
   arm is written `if (!(r3 < d2)) { push } else { ... goto link; }` (732 → 620 words).
2. **Decode fix**: the `+ 10000` test compares the plane values (`(r3+ra)^2 + 10000 < d2`), not `r2`/`d`.
3. **Goto contact-list walk** (`top: if (c == 0) goto out; ...; c = c->f24; if (!found) goto top; out:`):
   retail's loop is not inverted, and `found = 0` stays out of the `beq` slot. `while`/`do` forms get
   inverted and jump-threaded. Also `t = x->fA4; if (t) { c->f24 = t; t->f28 = c; }` (no reload).
4. **`int w[4]` stack array** for vx/vz/dx/dz: retail keeps them in memory (a dead second `sw` after the
   call, reloads of dx/dz). **Reuse `d`** as the dot-product sum (`d = w[0]*w[2]; d += w[1]*w[3];`).
5. **Hitbox loops on `ha`/`hb` field access only** (no separate `qa`/`qb` pointers): loop.c then reduces the
   `+2/+4/+6/+8` address givs into the one `+6` register retail has. Also no explicit `f3 == 0` pre-test
   (the for-loop's own test is the one retail has).
6. **Squares as separate statements** (`x0 = ...; x0 = x0 * x0; x1 = ...; ...; d = x0 + x1 + x2;`) for all
   three distance sums. This gives retail's late sum and its temporary registers.
7. **One accumulator variable `r2`** for all three radius chains: `r2 = (..)/4096; r2 += rb; r2 *= r2;`,
   and the plane (`r3`) and hitbox (`rr`) radii reuse `r2` too. Retail keeps each chain in `$6`.
Ladder (word mismatches): 515 → goto link 531 (size 620) → goto walk 447 (608) → w[4] 432 → field-only
hitbox 376 → d reuse 76 → decode fix 61 → r2 accumulate 45 → split squares 11 → r2 reuse 0.
