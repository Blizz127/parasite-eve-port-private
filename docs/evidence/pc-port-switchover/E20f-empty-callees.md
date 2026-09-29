# E20f: literal arguments to proven-empty callees

Generator rule in `tools/analysis/gen_decomp_ports.py` (`EMPTY_CALLEES`,
`empty_callee_droppable`, `empty_callee_args_drop`).

A matched leaf may call a callee whose retail body is empty with arguments.
Retail loads `$a0` before the `jal`, but the canonical pc_port prototype takes
no parameters. The call is kept and its arguments are dropped, under all of
these conditions:

- the callee is listed below with its disassembly;
- its canonical prototype has arity 0;
- every argument is an integer literal, so dropping it removes no side effect.

Any other call-arity mismatch is still rejected.

## func_800527C0

Command:
`mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL --adjust-vma=0x8000F800 build/extracted/disc1/SLUS_006.62 --start-address=0x800527C0 --stop-address=0x800527C8`

    800527c0:  03e00008  jr   ra
    800527c4:  00000000  nop

The body reads no register, stack slot or memory, and writes nothing. That
includes `$v0`, which callers such as func_80062A7C rely on (see
`O-func_80044E14-v0.md`). `src/func_800527C0.c` is `void func_800527C0(void) {}`.
Retail call sites pass literal codes 1, 2, 3, 4, 0xA, 0xB and 0x1F.

## Inertness

The generator was run before and after the rule (build/wt-mc3, batch O). All
1011 emitted TUs in `pc_port/game/decomp/` are byte-identical
(`diff -rq` is empty), so the rule changes no current output. The two leaves
it was written for are still blocked by other rules:

- func_80062A7C: `guest-address result of func_8005DC4C bound to host pointer str`.
- func_8005F354: `pointer-global store D_8009D12C`.

Unit tests: `E20fTests` in `tools/analysis/test_gen_decomp_ports.py`, with
positive, non-literal, unlisted/non-empty and declaration cases.
