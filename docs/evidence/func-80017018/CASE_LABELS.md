# func_80017018: operand-kind case labels corrected to the retail jump table

## Defect

`src/func_80017018.c` (field-VM task interpreter) byte-matched with three of
its five operand-kind `case` labels on the wrong bodies:

| kind | old src body | retail body (jtbl_80010690) |
|---|---|---|
| 0 | `&ops[i]` (immediate) | `&ops[i]` |
| 1 | `&D_8009DF70[ops[i]]` | `&D_8009D2F0->vars[ops[i]]` (actor + 0xAC) |
| 2 | `&D_8009D2F0->vars[ops[i]]` | `&D_800A77F0[ops[i]]` |
| 3 | `&D_800A77F0[ops[i]]` | `&D_8009DF70[ops[i]]` |
| 4 | `&D_800B6A80[ops[i]]` | `&D_800B6A80[ops[i]]` |

It matched because its profile sets `MASPSX_DISPATCH_FOLD=jtbl_80010690`:
maspsx retargets cc1's table reference to the retail table and drops cc1's own
table, so the byte gate never compares label -> body.  Any consumer of the C
(a generated port TU, a reader) would decode script operands wrongly.

## Retail evidence

EXE `build/extracted/disc1/SLUS_006.62` (SHA-1 452fb033...).  Dispatch at
0x80017130 (`sltiu v0,v1,5` / `sll` / `addu v0,v0,s1` / `lw` / `jr v0`) with
`$s1 = 0x80010690` (0x80017020 `lui s1,0x8001` / `addiu s1,s1,0x690`).
Table words at 0x80010690:

```
kind 0 -> 0x80017154   &ops[i]
kind 1 -> 0x8001718C   lw D_8009D2F0; +0xAC            (actor vars)
kind 2 -> 0x800171BC   0x800A77F0
kind 3 -> 0x8001716C   0x8009DF70  (lui 0x800a / addiu -0x2090)
kind 4 -> 0x800171DC   0x800B6A80
```

Bodies appear in code in the order 154, 16C, 18C, 1BC, 1DC, i.e. the src
source order; the fix relabels the cases and keeps the body order, so the code
is byte-identical and cc1's own table (unfolded) now equals retail's.

## Verification

- `tools/analysis/check_leaf.sh func_80017018 0x80017018 0x27C -O2 -G8` with the
  profile environment: `word mismatches=0`, `LINK_EXACT`, deep preflight PASS.
- `tools/analysis/dispatch_fold_audit.py --only func_80017018`:
  original src -> `MISMATCH ... case1: C->+0x154 retail->+0x174; case2: ...;
  case3: ...`; fixed src -> `OK exe:func_80017018 jtbl_80010690 (5 entries)`.
- Full audit of every MASPSX_DISPATCH_FOLD leaf (EXE + all overlay local
  profiles): `dispatch_fold_audit: 227 ok, 0 mismatch, 0 skipped` --
  func_80017018 was the only mislabel.  The 4 MASPSX_RODATA_FOLD-only leaves
  fold `$LC` literals only (byte-proved by the build); their switch tables are
  cc1's own and covered by the byte gate.
- Full matching gate: see the EXACT_REBUILD_GATE / VERIFY_SWEEP lines recorded
  with the commit.

The native port (pc_port/game/boot/func_80017018_port.c) already used the
retail mapping; its loop now also follows the corrected src line by line.

Gate (2026-09-29, `python3 tools/build/disc1_plan.py --write-status` then
`TMPDIR=$PWD/build/tmp bash scripts/exact_rebuild.sh`):

```
ok VERIFY_SWEEP=PASS leaves=2241 plan=8b67747d3c23c4af925d1108b284ab1c6506999d4e52f96fe3a5ec872316f0f2 yaml=38267c98259cd816ca5addf9daf435540b4460432eda2949e4740fcd2cb5ebd4
EXACT_REBUILD_GATE=PASS plan=8b67747d3c23... spans=[2241 c, 144 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```
