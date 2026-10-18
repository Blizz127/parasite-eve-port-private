# exe func_800DB6BC (NON_MATCHING): field effect type 16 main

SLUS file offset 0xCBEBC, 0x3E0 bytes (248 words), VMA 0x800DB6BC. The span is
a matched `c` leaf (`src/func_800DB6BC.c`, vendor-derived from khasinski's
`FieldEng_DiamondEmitter.c`). The port runs
`src/nonmatching/exe/func_800DB6BC.c`, which differs from the matched leaf in
two ways, both semantics-preserving:
- The two `struct FieldActorNode *` members, `FieldActor.task_node_lists[3]`
  and `FieldActorNode.next`, are typed `void *`. Both remain 4-byte guest
  pointers at the same offsets, and the function reads neither.
- The two `static __inline__` helpers are plain `static`.

The E21 lowering failed to parse the matched file at the first helper:
`parse: func_800DB6BC:1108:1: before: {`. It accepts file-local static helpers
but not `__inline__`.

## Why it was missing

The retail EXE holds a table of field-effect nodes at `D_800E1044`
(0x800E1040 holds 0xFFFFFFFF, then 23 node pointers). Each node holds:
- word 0: the effect's main function;
- word 1: a second callback;
- word 13 (`+0x34`): a pointer to a sub-table;
- the last words: type ids.

Node 16 is 0x800E1CF8: main 0x800DB6BC, type word 0x00020011. A field command
record with code 0x55 binds a node through `func_800CE49C` (from
`func_8006F39C`), and the effect dispatcher calls the main function through
`PE_EffectCallback`. 0x800DB6BC was the only function-start word in the EXE
image with no registry row and no case label (the dangling-callback census in
NATIVE_COMPLETION.md). Under the port binary's default stop policy,
`PE_EffectCallback` calls an unresolved function through `PE_GuestCall`, so a
type-16 effect stopped the game. The `[DAY1_SHIM]` skip applies only with
`PE_BOUNDARY_POLICY=record`. Every callee of the function (`func_800DB5F4`, `func_800DB25C`, `func_800CF3AC`,
`func_800CE560`, `func_800CE610`) already had native code.

## Proof (e21 differential)

    python3 tools/analysis/e21_leaf.py exe func_800DB6BC --check-gte --seeds 48 --uncovered
    e21_leaf: ok func_800DB6BC (exe) 177 seeds, retail coverage 248/248 words
    e21_leaf: uncovered none

The directed inputs in nonmatching.yaml:
- mode 0 (spawn);
- mode 1 over frame counters `D_800E27EC` 0..80, across the 25 and 72
  thresholds;
- mode 2 (draw) across the 24..40 glow window, with `out=800CF3AC:1:4`;
- other modes.

The GTE state is compared (`--check-gte`), which covers the inline `ctc2`
matrix loads.
