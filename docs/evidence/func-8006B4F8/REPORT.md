# func_8006B4F8 — EXACT (lane exeC5, 2026-09-28)

- VRAM `0x8006B4F8`, file offset `0x5BCF8`, size `0x870` (540 words), frame `0x80`, `asm/disc1/5BCF8.s`.
- Source `src/func_8006B4F8.c` (drafted from retail control flow; no prior draft).
- Profile `era_o2_g8_force_d800b0dd8_d800942e0_absolute` (new):
  `{"toolchain":"era","flags":["-O2","-G8"],"environment":{"MASPSX_FORCE_ABSOLUTE_SYMBOLS":"D_800B0DD8,D_800942E0"}}`
  (`-G8` for the `0x58($gp)` read of `D_8009CDC8`).
- YAML: `- [0x5BCF8, c, func_8006B4F8]` replaces `- [0x5BCF8, asm]` (next row is already
  `- [0x5C568, c, func_8006BD68]`, 0x5BCF8 + 0x870 = 0x5C568).

## What it does
Room load: `func_8006E2D0(buf, arg)` / `func_8006E454(buf)` resolve a room-file index into the
8-byte table `D_80093378` (`lba`, packed lengths `n0:8 | n1:12 | n2:12`); three
`func_8006E6A8` reads (into `st+0x194`, `+0x168`, `+0x18C`) each polled with `func_8006E7E8`
(-1 = retry the read), relocating the previous block (`func_8006E1C0` over the 0x14-byte
entries at header `+0x28`) while the next read is in flight; then fixes up the tables at
`st+0x198`, `st+0x1C0[b][k]`, `0x944..0x94C` (`func_80012574`), `0x950`, the handler slots
(`D_800942E0[k]` for 8..0x54, `D_800E1044[k-0x55]` above), the `D_8009448C` short tables,
the `0xE0/0xE1/0x124` record, and the `0xE8/0xEA/0xEB` state bytes with three
`func_8006CDA4(…, 0x21, 1)` kicks. Returns 0.

## Verification (private lane config build/lanes/exeC5/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC5/disc1.yaml --profiles build/lanes/exeC5/disc1_build_profiles.json --deep --only func_8006B4F8
  disc1_preflight: PASS (deep, 2102 c / 178 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC5/disc1.yaml --profiles build/lanes/exeC5/disc1_build_profiles.json func_8006B4F8
  LEAF_STRONG=EXACT func_8006B4F8 span=0x870 vram=0x8006B4F8 compiled=0x870 failures=[none]
MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800B0DD8,D_800942E0 python3 tools/analysis/era_link_check.py src/func_8006B4F8.c 0x8006B4F8 0x870 -O2 -G8
  word mismatches=0 / LINK_EXACT
```
Profile necessity (by hand; `profile_necessity.py` has no `--config` and reads the main YAML):
default `-O2 -G0` → 535 word mismatches; `-O2 -G8` alone → LD FAILED; forcing only
`D_800B0DD8` → `R_MIPS_GPREL16` truncated against `D_800942E0`; only `D_800942E0` → truncated
against `D_800B0DD8`. Both symbols and `-G8` are load-bearing.

## Levers (aligned-diff ladder)
1. First draft (int arrays `D_80093378[idx*2]`): 203 → struct array + byte/word union for the
   packed lengths (index stays in the memory operand, no hoisted address giv): 122.
2. `if (W & 0xFFC00000)` outer tests for 0x2C/0x30/0x24 (retail tests the mask, then the loop
   re-tests `>>22`), fresh-byte `st[0xEA] = e[4]` stores, `j = k - 0x55` (stops the
   `D_800E1044-0x154` fold): 92.
3. **One pointer variable per block group** (`tbl`: 0xC/0x10/0x20/0x4, `ent`: 0x2C/0x30/0x24,
   `q`: the 0x28 relocations) — one pseudo per variable gets one hard reg, reproducing retail's
   `v1`+copy vs direct-`a0` split: 80.
4. `r` / `i` register swap (s1/s2): a `$17` pin gives 23 but makes cse substitute `s1` for the
   known-zero `r` in later `= 0` stores (`move s3,s1`); instead **`asm("" : : "r"(r), "r"(r))`
   after each poll call** (one insn, two refs: priority up without lengthening `done`'s range;
   two separate asm insns flipped `done`/`p`, 0.780 vs 0.786): 35.
5. **Frame**: each `for` whose entry test cc1 folds leaves a dead ST_REGS pseudo with an 8-byte
   stack slot (`cc1 -dl -dg`: "used 2 times across 1 insns ... ST_REGS or none", no
   disposition). Retail has six; writing the 0xC loop as `i = 0; if (n) do {} while (++i < n)`
   drops ours from seven to six (frame 0x88 → 0x80): 13.
6. `unsigned int t = H(e, 0xA); unsigned short v = t; if (t & 1)` (retail's `move v1,v0` copy;
   `int v` gives `andi 0xffff`): 12.
7. Handler slot: `t = e[7]; k = t; if (t - 8 < 0x4D) { t <<= 2; slot = t + (unsigned
   int)D_800942E0; } else if (k >= 0x55) …` (then-arm reuses the load register, else arm the
   copy; operand order `t + base`): 10 → 3 → 2.
8. 0x10 row table: retail keeps `+0x1C0` off the store (`addu v0,v0,s6; addiu v0,v0,0x1C0;
   … sw v1,0(a0)`); every spelling folds it into `sw 448(a0)`. An `asm("" : "=r"(row) :
   "0"(row))` barrier fixes the association but moves the giv increment out of the delay
   slot (2); **`register unsigned char **row asm("$2")`** stops combine without touching the
   schedule: 0 (LINK_EXACT).
