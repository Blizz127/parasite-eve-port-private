# exe func_800DD9E4 (NON_MATCHING): the rising-sparks particle callback

SLUS file offset 0xCE1E4, 0x38C bytes (227 words), VMA 0x800DD9E4. The span is a
matched `c` leaf (`src/func_800DD9E4.c`, `docs/evidence/func-800DD9E4/`). The
port runs `src/nonmatching/exe/func_800DD9E4.c`: the matched leaf with its
block-scope `typedef struct { unsigned char r, g, b, cd; } CV;` moved to file
scope. Nothing else changed. `CV` names nothing else in the file.

## Why it was missing

`func_800DDD70` (hand port, `decomp_hand/absent_ovl2_port.c:531`) handles the
rising sparks at the player's stats-block anchor. Its mode 0 makes a
`func_800CE560` pool whose per-particle callback is 0x800DD9E4. No `pc_port`
file defined it, and the registry had no row. The E21 lowering refused the
matched leaf with "local typedef".

The pool tick (`pe_particles` -> `PE_EffectCallback`,
`game/boot/func_800D413C_port.c`) calls an unresolved callback through
`PE_GuestCall` under the port binary's stop policy, which is the default
(`port_main.c`: only `PE_BOUNDARY_POLICY=record` selects the diagnostic
`[DAY1_SHIM]` skip). So the first tick of the sparks stopped the game.

## Proof (e21 differential)

    python3 tools/analysis/e21_leaf.py exe func_800DD9E4 --check-gte --seeds 72 --uncovered
    e21_leaf: ok func_800DD9E4 (exe) 216 seeds, retail coverage 227/227 words
    e21_leaf: uncovered none

The directed inputs in nonmatching.yaml:
- `cmd` 1 (tick) over states 0-3 and -1;
- `cmd` 2 (draw) over states 0/1/2, timers around the 0x1C and 8
  thresholds, `D_800F336C` 4/0 and `D_800F3428` 0/1, with
  `out=800CF3AC:1:4` (`func_800CF3AC` fills the stack colour);
- other `cmd` values.

The harness got one extent pair from the retail callee, `func_800D1DEC`:
- `pos` (a0) is read only by `lwc2 $0,0` / `lwc2 $1,4`, and VZ0 keeps the
  low halfword, so 6 bytes are compared;
- `col` (a1, saved in s2) is read only as `lbu 0/1/2`, so 3 bytes are
  compared.

Without the pair, the default 16-byte window compared the caller's frame
layout past those bytes. Retail keeps `pos` at col+8 and native at col+4.
