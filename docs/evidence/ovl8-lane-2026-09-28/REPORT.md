# ovl8 lane (2026-09-28) — subsystem overlays

Scope: ovl_039F, ovl_03C5, ovl_03C9, ovl_03D2, ovl_0457, ovl_0700 (rooms = ovl7).
Per-leaf fast check: `tools/analysis/room_lane/try.sh <target> <file.c> <vram> <size> -O2 -G0`
(`word mismatches=0`), then `python3 tools/build/disc1_preflight.py --target <t> --deep --only <f>`
PASS, then `flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <t>`.

## ovl_03C9 — func_801F1070 (0x3878, 0x18C) and func_801F15DC (0x3DE4, 0x1B4): ovl4/ovl6 parks closed

Both are siblings of the matched func_801F0BA0; default profile `era_o2_g0`.
Preflight: `disc1_preflight: PASS (deep, 10 c / 4 asm / 0 rodata)` (each `--only`).
Gate: `EXACT_REBUILD_GATE[ovl_03C9]=PASS plan=a0bf801a59c7… yaml=3143ed563a69… spans=[10 c, 4 asm, 0 rodata, 2 bin] sha1=1d0e972cf9446dc700c82a8597292ced45958ef4`

**Lever 1 — the "li 4 before the table load" wall (both functions; same shape at 5 retail sites
in ovl_03C9 and ovl3's room func_801937B4).** Retail:
`sll $v0,$a0,1 / li $v1,4 / lui $at / addu / lhu $a1,D_800E1204 / bne $a0,$v1`. cc1 always
emits the compare constant after the load (the jump expander forces it into a pseudo after the
statement), and sched1's register-birth boost keeps it there; local-alloc then gives it `$v0`,
which the load's index uses, so sched2 cannot hoist it. Declaring the constant as a hard-register
local (`register int four asm("$3"); … four = 4; if (k == four && …)`) removes the anti-dependence
and sched2 (load priority 5 > li priority 1) reproduces retail's order.
Ladder (15DC): best ovl6 draft 10w → + `four` pin 5w; without the pin in the final form 60w.
func_801F1070: ovl4 best 15w → 0 with `four` pin + `k = D_800F336C` local + direct `f(t) / 32`.

**Lever 2 (15DC) — write the signed /32 out on a reused variable.** Retail copies the call
result into `$a1`, adjusts it in place, and the `sra $s0,$a1,5` sits in the `lhu 0xC` load-delay
slot. `cy = func_80077DC4(t); if (cy < 0) cy += 31; c = cy >> 5;` (cy is the later CLUT row
variable, also in `$a1`) gives exactly that: cy is set more than once, so sched1 does not give
the sra the "birthing insn" max priority, and the adjust temp shares cy's register.
Ladder: `c = f(t) / 32` 5w (temp `$a2`, sra in the `bne` slot) → `c = f(t); c /= 32` 6w →
`cy = f(t); c = cy / 32` 3w → written-out form on cy 0w. The ovl6 `asm("" : "=r"(k) : "0"(k))`
barrier on the D_800F336C load must be REMOVED in the final form (4w with it: it outranks the
sra for the stall slot).

## ovl_0700 — func_80195994 (0x69A4, 0x234) and func_80195BC8 (0x6BD8, 0x174): ovl4 parks closed

Default profile `era_o2_g0`. Preflight each `--only`: `disc1_preflight: PASS (deep, 38 c / 10 asm / 0 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_0700]=PASS plan=5cf084ac653c… yaml=528f73097bfc… spans=[38 c, 10 asm, 0 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de`

**Lever — alias classes decide which stores may cross the pointer loads.** The ovl4 draft
modelled D_8019C050 as one `Ctl` struct (mode/shift shorts + two Obj records) with `short *`
params: every store was then a fixed-address *struct* store and every `a1[i]` a varying-address
*scalar* load, so cc1 pinned all stores in source order against the loads (48w). Retail's order
shows the Obj stores pinned but the mode/shift stores moving freely. Typing the params as
`SVECTOR *` and the four mode/shift words as separate scalar `short` globals (D_8019C050/52/54/56)
— a fixed-address scalar cannot alias a varying struct access in cc1's rule — plus separate Obj
struct globals (D_8019C05C / D_8019C08C) reproduces it.
Ladder (BC8): Ctl + short* 48w → int params `a2 &= 0xFF` in place 47w → SVECTOR/scalar/Obj
typing 36w → shift-count store before the shift store (`s = a; m = 1 << a;`) 3w → first-record
camera words read directly (a reused `x/y` local in the first half left `lw` ahead of the
first `lh`) 0w. Final form reads both camera arrays directly.
**func_80195994** (same records, two func_8018F55C calls): same typing (82w → 20w); a fresh int
local from the masked param keeps the `andi` after its call (in-place `a1 &= 0xFF` makes it a
call-crossing pseudo that sched1 hoists over the calls) (→15w with Z/S/M order); prototyped
`unsigned char a1, a2` params copied into `int s1/s0` locals give the p/a2 callee-saved order
(`$s4`/`$s3`) (→0w; using the params directly without the locals is 32w).

## ovl_0700 — func_80192030 (0x3040, 0x31C): map-exit destination chooser (new)

Local profile `ovl_0700_dispatch_8018F034` = `-O2 -G0` + `MASPSX_THREE_WORD_SYMBOL_STORE=1` +
`MASPSX_DISPATCH_FOLD=jtbl_8018F034` (table entries read from the blob at 0x44:
0x801920FC … 0x80192200 → cases 0..9). The table sits inside the old `[0x0, bin]` header, so the
yaml gains a `[0x44, rodata]` pool row (without it the gate fails
`ERROR: pool source has no jtbl_8018F034 block`).
Preflight `--only func_80192030`: `disc1_preflight: PASS (deep, 39 c / 10 asm / 0 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_0700]=PASS plan=74dc25bf673f… yaml=29428cf5c74d… spans=[39 c, 10 asm, 1 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de`
Ladder: first draft (fold only) 7w (fold silently needs the three-word knob; jtbl word wrong) →
+ THREE_WORD 6w (D_8009D280 seed store after the map-id load, constant in `$v0`) → store
`D_800A77F4 = 999` before `D_8009D280 = 0xA9400048` 0w (24/120 statement permutations exact).
The per-destination `D_8009D280 = X` / `f = 0` arms are plain source; cc1's cross-jumping
produces retail's two shared store tails.

## ovl_0700 — func_8019234C (0x335C, 0x3F4): overlay main loop (new)

Default profile `era_o2_g0`. Preflight `--only`: `disc1_preflight: PASS (deep, 40 c / 9 asm / 1 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_0700]=PASS plan=19afd5044af7… yaml=aefbc8c38b0c… spans=[40 c, 9 asm, 1 rodata, 2 bin] sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de`
Shape: `% 3000000` of func_80071A54 (magic 0x165E9F81 >> 18), ordering-table relink scan
(`(ot[i] | 0x80000000) == &ot[i-1]`, strength-reduced to two byte-offset givs), DB double buffer
of 0x78 bytes (`int; u_long *ot; DRAWENV 0x5C; DISPENV 0x14`).
Ladder: first draft 38w → `func_80194108(D_8019C02A)` argument + `scan = 1; i = 0xFFF;` before
the `db = D_8019C9C0` load (reorg then steals both constants into the func_80037870 guard's
delay slots, as retail) 31w → `db->ot[i]` read in place instead of an `ot` local 28w (pure
register rotation left) → the buffer swap through its own `nx` local instead of reusing `db`
0w (reusing `db` raises its refs so it is allocated before the loop givs).

## ovl_03D2 — func_80190660 (0x1670, 0x354): title/logo fade (new)

Profile `era_o2_g0_no_cse_skip_blocks` (existing). Preflight `--only`: `disc1_preflight: PASS (deep, 22 c / 5 asm / 0 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=24985a3659b8… yaml=cc30aae53236… spans=[22 c, 5 asm, 0 rodata, 2 bin] sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32`
Shape: two LoadImage calls over a TIM pair at D_80193254+D_80193278, double-buffered
`DR_TPAGE tp[2][2]` + `SPRT sp[2][2]`, 480-frame loop (fade in 0..0x1F, hold, fade out
0x188..0x1A7), per-frame LoadImage of the buffer's `img` rect (x*3/2, w*3/2, y+0xF0 on buffer 0),
PutDrawEnv/PutDispEnv; env struct `{DRAWENV; DISPENV; RECT img}` (isrgb24 at 0x6D).
Ladder (diff lines): first draft 151 → fade value in `v`, `c = v;` + `asm("" : "=r"(c) : "0"(c))`
(cc1 otherwise targets c in every arm, so c overlaps t and needs a 9th saved reg) 89 → sprite
field store order x,y,w,len,code,v,u,clut,h / x,w,len,code,y,v,u,clut,h 77 → `sz` local read
before `p += 8` 71 → `i >= 0x188` arm first + chained `BC[0]->isrgb24 = BC[1]->isrgb24 = 0` 65 →
`-fno-cse-skip-blocks` (the `&r` argument was a loop-invariant pseudo hoisted into `$s3`) 41 →
chained store for `= 1` too 26 → `t = tp[i & 1]` after the D_801D11C4 update 0.

## ovl_03C9 — func_801ED858 (0x60, 0x3EC): effect handler (new)

Default profile `era_o2_g0`. Preflight `--only`: `disc1_preflight: PASS (deep, 11 c / 4 asm / 0 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_03C9]=PASS plan=26b948b0648f… yaml=3227056e0560… spans=[11 c, 4 asm, 0 rodata, 2 bin] sha1=1d0e972cf9446dc700c82a8597292ced45958ef4`
Exact on the first draft by applying the func_801F15DC levers: `case 1` guards
`if (o->wait >= 2) break; if (o->wait < 0) return 0;` (two distinct exit targets in retail),
`if (o->step < 0x2C) break; return 1;`, inner `switch (o->wait)` with `default: return 0`, the
phase-0 CLUT row read straight from D_800F336C (natural `li 4` after the load, `+4` no else) and
the phase-1 row through `k` + `register int four asm("$3")` (retail `li $v1,4` hoisted).

## ovl_0700 — func_80193478 (0x4488, 0x470): path step for the camera objects (new)

Default profile `era_o2_g0`. Preflight `--only`: `disc1_preflight: PASS (deep, 41 c / 9 asm / 1 rodata)`.
Gate: see the ovl_0700 gate line in `build/lanes/ovl8/results.jsonl` (row func_80193478).
Obj layout `{int pad[2]; MATRIX mat; SVECTOR rot;}` (mat.t = +0x1C is func_8018F55C's VECTOR out,
rot = +0x28). **Lever — alias class of the pointer globals:** with six scalar `Obj *` globals the
Obj field stores (in-struct, varying address) cannot alias a fixed scalar global, so cc1 keeps
the pointer in a register (256 diff lines); retail reloads `D_801EA578` after every store, which
is what an in-struct global gives: the six pointers are one array `Obj *D_801EA578[6]`
(D_801EA578 … D_801EA58C). First draft with the array: 0 mismatches.

## ovl_0700 — func_80191854 (0x2864, 0x440): map-exit setup (new)

Default profile `era_o2_g0`. Preflight `--only`: `disc1_preflight: PASS (deep, 42 c / 8 asm / 1 rodata)`.
Gate: ovl_0700 gate line in results.jsonl (row func_80191854).
Exact on the first draft: the three map words are fields of one game-state struct at
D_800A77F4 (`cur` +0, `flags` +8, `dest` +0x124 = D_800A7918), read through the cse'd base
register as retail; 16 + 14 plain `if (x == K) …` statements (cc1's cse path limits give
retail's two mid-chain reloads).

## Parked this lane (best drafts in build/lanes/ovl8/drafts/park_*.c)

* **ovl_0700 func_8018F55C** (0x3D0, spline sample for the camera path; needs `MASPSX_EXPAND_DIV=1`
  for its three `%` by `n-2`): frame/saved-register set now right (VECTOR a..e declared before the
  SVECTOR copy temp; separate `ya0`/`ya` angle locals + barrier). Residual: retail reads the point
  count with `lhu` + `sll/sra 16` (every C spelling folds to `lh`; an asm barrier gets the lhu but
  shifts the schedule), `tab + idx*4` operand order, and the prologue param-copy order.
* **ovl_03C9 func_801EF560** (0x418) best 84 diff lines: mode-1 part exact (inner `switch(wait)`
  with `goto out` tails, `D_800942EC[]` as an array so the `o->vy` store orders before its load,
  `o->vy *= -1` keeps the `sll/sra` before `negu`, `o->step * 32` for `lh`). Residual (mode 2):
  retail keeps the `0x20`/`0x40` constant in `$v1` then `li $v1,4` for the CLUT compare; with two
  `register int four asm("$3")` sets the hard reg is set twice, so sched1 no longer boosts the
  `li 4` (it drifts to the block head); `four = 0x20; … four = 4;` gets the `$v1` reuse but not the
  D_800F3368 store / o->step / D_800F336C load order.
* **ovl_0700 func_80195F6C** (0x52C, POLY_G4/DR_TPAGE double-buffer init): retail addresses every
  field as `sym+off($at)` with the i*36 giv; cc1 cse-folds `&D[i]` (the SetPolyG4 argument) into a
  base register and uses `off(base)` (395 diff lines). Flags tried: -fno-cse-follow-jumps,
  -fno-cse-skip-blocks, -fno-rerun-cse-after-loop, -fno-expensive-optimizations,
  -fno-strength-reduce, -O1 — no change.
* **ovl_03C9 func_801EE594** (0x634) best 42 diff lines: mode 1 exact (`o->y += -5 - (rand & 7)`,
  `o->x += (rand & 7) - 3` for retail's reassociation; `if (++o->step < 0x12) break;`), mode-2
  case 2 stack stores via a 400-permutation search. Residual: mode-2 case 0 shares `$a1 = 1` for
  D_800F336A/D_800F336C early in retail (ours materialises the 1 late), and case 2's 0x20 stores sit
  after `li $s0,1` in ours.

## ovl_0457 — func_8012403C (0x333C, 0x55C): credits page state machine (new)

Local profile `ovl_0457_dispatch_8012359C_expand_div` = `-O2 -G0` + `MASPSX_THREE_WORD_SYMBOL_STORE=1`
+ `MASPSX_DISPATCH_FOLD=jtbl_8012359C` (inside the existing `[0x22A4, rodata]` pool; entries
0x80124244 … 0x80124550 = cases 0..7) + `MASPSX_EXPAND_DIV=1`.
Preflight `--only`: `disc1_preflight: PASS (deep, 26 c / 1 asm / 1 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_0457]=PASS plan=232d2c94d044… yaml=ed6ae36231a2… spans=[26 c, 1 asm, 1 rodata, 2 bin] sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6`
Ladder (word mismatches): first draft 168 (+9 words) → **divisor through a barrier**: cc1 turns
`x % 450` into the 0x91A2B3C5 magic multiply; retail is `li $v1,450` / `divu` / `addiu $v0,$v1,-93`,
reproduced by `v = D_80172CD4; n = 450; asm("" : "=r"(n) : "0"(n)); t = v % n; if (t == n - 93)`
(size exact, 35) → `d = h; d -= g; d <<= 11;` + `(d += 0x43000)` (fold-const otherwise pulls the
constant out of `base + (d + K)`) 29 → `register int d asm("$6")` for cases 0/1 and a separate
unpinned local for case 2 (retail keeps the case-2 chain in `$v1`) 5 → case 0 built in pinned
`$a1`/`$a3` locals: `a5 = 0x38800; k7 = 0x43000; …; a5 += d; a5 = D_80011610 + a5;`
`func_8010C89C(tab, a5, D_80011610 + (d += k7))` 0.

## ovl_03D2 — func_80190064 (0x1074, 0x5FC): title-menu input handler (new)

Default profile `era_o2_g0`. Preflight `--only`: `disc1_preflight: PASS (deep, 23 c / 4 asm / 0 rodata)`.
Gate: `EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=f3502b156e68… yaml=9e0c727b99e7… spans=[23 c, 4 asm, 0 rodata, 2 bin] sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32`
Task struct as in func_801931BC/func_80192F98. All twelve list walks are one
`static inline Task *find(int id)` (`for (p = D_801D1370; p; p = p->next) if (p->f2C == id) break;`);
cc1 reproduces both retail shapes (result copied to `$s0` in the `beq` slot when kept, plain `$v1`
use when dereferenced once). Ladder (diff lines): first draft 39 → `find(7)->f06 > t->f06`
(retail loads the found task's field first) 37 → down-move bound written as
`if (find(6)->f1C == 0x100 ? k >= 0x8D : k >= 0xA1)` (retail's beqz-to-body / `j`-to-skip) 0.
