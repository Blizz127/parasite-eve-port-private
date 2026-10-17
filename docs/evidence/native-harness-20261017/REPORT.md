# Native oracle harnesses: font, animation, field VM (2026-10-17)

Command: `scripts/run_native_oracle_harnesses.sh` (retail EXE
`build/extracted/disc1/SLUS_006.62`, SHA-1 `452fb033…`). The run finished at
2026-10-17T21:41:58Z (mtime of `vm-results.json`). `summary.json` records the
counts, the SHA-256 of every input file, and the SHA-256 of each full result
file. The full result files stayed in tmpfs and are not committed.

| Harness | Native side | Oracle side | Result |
|---|---|---|---|
| font | `font_page_init_port.c` `func_80038D74`, five callees mocked | retail body (SHA-256 `450c3907…`) in the e21 MIPS oracle | 54 cases PASS, 0 skipped; all 252 normal words covered, 8 trap-guard words excluded |
| animation | `animation_decode_port.c` (80039B74/D24/ED4) plus the port's real `func_800794C4` rotation; nothing mocked | the composed retail four-function graph, pinned by `tools/analysis/animation_native/retail-pins.json` | 117 cases PASS, 0 skipped |
| field VM | E21-generated `func_800133E8` / `func_80019810` (`gen_decomp_ports.py --e21`); three math callees mocked | retail handler bodies | 693 cases PASS, 0 skipped; all 112 reachable words covered, 8 impossible division-guard words excluded |

What is compared:
- the return value;
- the ordered, typed call list;
- all main RAM except the exact incoming-register spill bytes each validator names;
- every scratchpad byte.

Every case also runs negative controls (return, call list, RAM, scratchpad), and each one must fail the comparison.

Deviations from the prepared 2026-10-15 recipes:
- `-Wno-comment`, because `pe_port_compat.h` contains `game/boot/*.c` inside a comment.
- The VM harness compiles the generated TUs, not the never-applied hand file `field_vm_remaining_port.c`. It adds `-Wno-unused-variable`, because the matched leaf declares an unused `scaled`, and `tools/analysis/field_vm_native/trap_shim.c`, because the generated division helper references `PE_Decomp_GuestTrapDivZero`, which cannot be reached.
- Resource limits are `ulimit -v` 2 GiB, `nice 19` and a 900 s timeout per step.

Not covered:
- real IO;
- room or campaign reachability;
- VM opcode dispatch through an actual task script;
- visual, audio, timing and save parity;
- the owner device gate.

These harnesses check function behavior only. They are not evidence of a playable slice.
