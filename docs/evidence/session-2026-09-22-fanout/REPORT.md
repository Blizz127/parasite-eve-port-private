# Session 2026-09-22 — parallel fan-out (EXE lanes, tooling, overlays, port)

## Result

| Target | Before | After | Proof |
| --- | --- | --- | --- |
| Disc 1 EXE `c` spans | 1655 (working tree; HEAD 1650) | **1780** | build `RESULT=EXACT_MATCH` orig=cand=`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`; `VERIFY_US=PASS`; `VERIFY_SWEEP=PASS leaves=1780 plan=40f8548371965848… yaml=51492d17bb684d13…`; `disc1_preflight --deep` PASS (1780 c / 310 asm / 2 rodata) |
| Overlay `c` leaves | 1 (ovl_0700) | **68** across 7 targets | `EXACT_REBUILD_GATE[<id>]=PASS` for ovl_0700 (34), ovl_03C5 (2), ovl_03C9 (7), ovl_0457 (12), ovl_03D2 (10), ovl_039F (0; Psy-Q libpress, classified), room_m0022i (3; room proof of concept) |
| pc_port suite | 1405/1405 | **1419/1419** | `scripts/build_pc_port.sh --build-dir build/pcbuild-lead` |
| decomp-derived port leaves | 368 | **477** (14.4% of matched words) | `tools/analysis/pc_port_coverage.py` |

Commands (from the repo root, `export TMPDIR=$PWD/build/tmp`):

```
python3 tools/build/disc1_plan.py --write-status
scripts/exact_rebuild.sh --preflight-fast      # build EXACT_MATCH (verify then tripped on the stale status doc)
scripts/verify_us.sh                            # VERIFY_US=PASS after --write-status
python3 tools/analysis/verify_matched_leaves.py # VERIFY_SWEEP=PASS leaves=1780
python3 tools/build/disc1_preflight.py --deep   # PASS
scripts/exact_rebuild_overlay.sh <id>           # per overlay, PASS
scripts/build_pc_port.sh --build-dir build/pcbuild-lead   # 1419 run, 1419 passed
```

## How it ran

Six contiguous address lanes over the 519 non-handwritten remaining asm
functions, each verifying against a **private copy** of the YAML/profiles
(`disc1_preflight.py --config/--profiles --deep --only` + `leaf_strong_check.py
--config/--profiles`), merged by the orchestrator with a 3-way `git merge-file`
against a fixed base; a tooling lane (maspsx + build), an overlay lane, two port
lanes. Per-leaf evidence: `docs/evidence/func-<X>/REPORT.md`; per-lane
attempt logs (matched / parked / skipped, with residuals):
`lane_results/lane<N>.jsonl`; parked best drafts (NOT matching C):
`park_drafts/*.c.draft`. All agents stopped at an account usage limit, so the
lanes are partially worked — see each jsonl for what was not attempted.

## Tooling added

* **Multi-table dispatch fold** — `MASPSX_DISPATCH_FOLD=jtbl_A,jtbl_B`: the
  i-th cc1 switch table label in `.rdata` maps to the i-th symbol;
  `strip_dispatch_rodata` sums the pool blocks and accepts only a single zero
  `.align 3` word between tables (read from the object's relocations).
  Tests: `tools/build/test_dispatch_strip.py`,
  `tools/era/maspsx/tests/test_dispatch_fold_multi.py`.
* **maspsx patch 11** `MASPSX_FILL_JUMP_STACK_STORE=1` — moves a volatile
  `sw/sh/sb $r,N($sp)` into a following plain `j <label>` slot (cc1's reorg never
  slots volatile insns; ASPSX does). 4 retail `sw $zero` sites.
* **patch 12** `MASPSX_RETURN_VIA_EPILOGUE_JUMP=1` — frameless functions: an
  early `j $31` becomes a jump to the final `j $31` (retail memmove 0x80072368).
* **patch 13** `MASPSX_LOAD_DELAY_CALL_SLOT_STORE=1` — keeps the load-delay nop
  before a `jal` whose slot store reads the loaded register (2.8.1 band).
* Flag-off byte identity for 11–13: 1614/1614 era leaves identical to the
  pre-patch maspsx. maspsx suite: 300 tests OK.
* `era_link_check.py` puts the repo mipsel-host lib dir on `LD_LIBRARY_PATH`.
* `exact_rebuild.sh` keeps its log directory on FAIL (it used to delete it,
  which hid two split failures caused by concurrent gate runs).
* `pc_port_coverage.py` judges stub-ness per definition body, not per file
  (stub 256 → ~20; `tools/analysis/test_pc_port_coverage.py`).
* `gen_decomp_ports.py`: derived host signatures + `T **args` guest-pointer
  vectors (E7c); stopped emitting 6 silently-wrong TUs (arity / host-pointer).

## Open tooling gaps (from the lanes)

* Load-delay nop placed after a loop label instead of before (maspsx ~l.1692):
  `func_80083578`, `func_8007BF44` (1 word each).
* ASPSX moving a plain register-base `sb` into a `jal` slot after the load-delay
  nop: `func_8007A2A4` (6 words; patch 13 may cover it — untested).
* Store-then-reload through a global only reproduces with an
  `asm volatile("":::"memory")` barrier — ruled NOT acceptable:
  `func_80039898`, `func_80039310`, `func_80039184`.
* Recurring 2.8.1 residual: retail rematerialises constant 1 where cc1 CSEs it
  (`func_80081D74`, `func_80083D04`).

## Incidents

* The `/tmp` tmpfs has a per-user quota; ~7 GB of unrelated projects' build
  trees plus the agents' temp use exhausted it and killed every agent shell once.
  Temp use was moved to `build/tmp` (git-ignored).
* `func_80085F74` (then uncommitted) broke the full build (two switch tables vs
  a one-symbol fold); demoted, then restored once the multi-table fold landed.
* 19 unregistered drafts in `src/` (crashed-session leftovers) were moved out of
  `src/`; lane leftovers likewise.
