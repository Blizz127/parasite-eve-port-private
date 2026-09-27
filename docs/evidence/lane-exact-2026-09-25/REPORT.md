# Four lane leaves, 2026-09-25

Each retail span below was re-verified by `integrate_leaves.py` (`LINK_EXACT`) and then `leaf_strong_check.py` (`LEAF_STRONG=EXACT`) plus `disc1_preflight.py --deep --only`. Zero tail words past the declared span are padding; the compared span itself matches.

| Function | VRAM | Size | Profile |
|---|---|---|---|
| `func_8005FCAC` | `0x8005FCAC` | `0x144` | `era_o2_g8_expand_div` |
| `func_800777C0` | `0x800777C0` | `0x144` | `era_o2_g0_cc1_281` (`-mno-split-addresses`) |
| `func_800CE610` | `0x800CE610` | `0x78` | default `-O2 -G0` |
| `func_800875FC` | `0x800875FC` | `0x110` | `era_o2_g0_no_schedule_insns` |

`func_800875FC` needs `-fno-schedule-insns`: with sched1 on, the opening `$s5`/`$s0` save-and-move pair swaps (4 words).
