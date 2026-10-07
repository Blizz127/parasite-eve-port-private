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

## Round 3: whole-tick oracle for func_8008DB7C (same day)

Removing the interpreter left the native AKAO tick (`akao_tick_port.c`,
`func_8008DB7C` and everything it reaches) unchecked against anything.
`pe-akao-tick-oracle-tests` now checks the whole tick against the matched C,
still with no disc and no retail bytes:

```
python3 pc_port/tools/build_leaf_oracle.py --out build/leaf-oracle-akao \
    --range 0x80085000 0x80092000 func_8008DB7C     # 241 functions, 43616 bytes
build/pcbuild/pe-akao-tick-oracle-tests             # PE_TICK_ORACLE_CASES / _SEED
```

`build_leaf_oracle.py` now takes `--range` (every `src/func_X.c` in a range
plus the callee closure). It links read-only data (switch tables) into the
image and drops the `MASPSX_*_FOLD` knobs, which only rename cc1's own table
onto retail table symbols. Leaves with no matched source, and anything that
calls them, are dropped and listed.

For each case the test:
- randomises the driver globals (0x8009B000..0x800C2000), the control
  records, a 128 KiB sequence-byte sandbox and the 256 SPU registers;
- applies only the invariants the driver relies on:
  - voice masks use bits 0..23, or 12..23 for the SFX bank (otherwise the
    bit-walk loops never end);
  - each voice's script cursor, wave and key-split pointers (+0x00, +0x14,
    +0x1C, +0x20, +0x24) point into the sandbox;
  - the SPU channel (+0xF0) is below 24;
  - the instrument row (+0x5A) is below 32;
  - `D_8009B3FC` = 0x1F801C00.
- biases the tempo accumulators and note timers so the tick actually fetches
  and dispatches opcodes;
- fills the opcode tables `D_8009C8F0`/`D_8009CCF0` with random handlers from
  the 91 image functions in 0x8008F1B0..0x80091100 that the native registry
  dispatches, and places a `j` trampoline at each retail address;
- runs the matched tick in the test-only interpreter, restores, runs the
  native tick, and compares guest RAM (minus the 0x801FF000 host scratch
  page, as `PE_AKAO_VERIFY` did) plus the SPU registers.

Cases where the interpreter faults are skipped, not judged. Typical causes:
random jump targets, a portamento divide-by-zero trap, or loop opcodes that
never end.

Result: 4 seeds x 3000 cases = 8785 judged, **0 mismatched**. Native coverage
over the judged cases: about 45k `func_8008E8D0` opcode fetches, 540k
`func_80087AA8`, 1.7k `func_80088980`. Negative controls:
- `func_8008E8D0` `+0x58 -= 2` changed to `-= 3`: 131/219 mismatched.
- `func_80087AA8` volume-slide mask `0xFFE00000` changed to `0xFFF00000`:
  17/219 mismatched.

ctest `akao-tick-oracle` runs 300 cases and skips (77) when there is no image.

Also: `test_pa_handhi.h` `PA_handhi_seq_flow` still expected the old
`func_8008A92C` boundary from `func_80090C88` (stale since round 2). It now
asserts that no boundary is recorded.
