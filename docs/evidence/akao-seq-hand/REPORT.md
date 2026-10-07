# func_8008A750 / func_8008AC40 hand adapters (2026-10-07)

**Why:** with the native-only AKAO tick (no interpreter), the generated
`func_8008A92C` (Seq_StartNestedStreams) and `func_8008AE94`
(Seq_SelectPlaybackBank) reached these two matched leaves as decomp boundaries
(the generator skips them: "pointer cast of non-D_ operand"). Under the
native-only STOP policy the first nested-stream start or bank restore in the
music would have ended the run.

**What:** `pc_port/game/decomp_hand/akao_seq_port.c`, line-for-line from
`src/func_8008A750.c` and `src/func_8008AC40.c` (guest addresses, PE_Load/
PE_Store at the leaf's offsets and widths, leaf order).
`hand_akao_seq_protos.h` makes the generator emit direct calls; regenerated
`func_8008A92C_port.c` / `func_8008AE94_port.c` no longer carry the boundaries.

**Evidence (no disc, no retail bytes in git):**
`pc_port/tools/build_leaf_oracle.py` compiles the matched leaves and their
callee closure (func_80089F58, func_8008F178, func_8008F0D0, func_80089FE0,
func_80089960, func_80089B28, func_80089CF0, func_8008D820) with the era
toolchain (each leaf's profile; func_8008D820's "modern" profile substituted by
era -O2 -G0 because the box has no MIPS gcc; semantics only) into
`build/leaf-oracle/image.bin` (git-ignored), relocated to 0x801C0000.
`pe-leaf-oracle-tests` runs each leaf in the test-only R3000A oracle and the
native C from the same randomised AKAO state (0x8009C000..0x800BE000 plus a
control-record window) and compares all 2 MiB of guest RAM.

```
python3 pc_port/tools/build_leaf_oracle.py func_8008A750 func_8008AC40 func_80089F58 \
  func_8008F178 func_8008F0D0 func_80089FE0 func_80089960 func_80089B28 func_80089CF0 func_8008D820
build/pcbuild/pe-leaf-oracle-tests
LEAF_ORACLE func_8008A750: 400 cases, 0 mismatched, 0 oracle faults -> PASS
LEAF_ORACLE func_8008AC40: 400 cases, 0 mismatched, 0 oracle faults -> PASS
```

Negative control: changing one constant in each adapter (`+0x30` -> `+0x31`,
`v+0x58 = 1` -> `3`) gives 100/100 and 86/100 mismatched cases.

## Round 2: func_8008A92C callers (same day)

The AKAO opcode hand ports `func_80090C88` (seq_ops_hi_port.c),
`func_8008B168` (batch5_hi_port.c), `func_8008B084`, `func_8008B124`,
`func_8008B0C8` (batch4_hi_port.c) still called `func_8008A92C` through
`PE_D_COMP_BOUNDARY3` from the days it had no native callee. They now call the
generated TU directly (arguments as in their src/ leaves). Oracle, 400 random
cases each, all guest RAM compared:

```
python3 pc_port/tools/build_leaf_oracle.py      # DEFAULT_LEAVES (20 leaves)
build/pcbuild/pe-leaf-oracle-tests
LEAF_ORACLE func_8008A750 / 8008AC40 / 8008A92C / 8008B084 / 8008B124 /
            8008B0C8 / 8008B168 / 80090C88: 400 cases, 0 mismatched -> PASS
```
