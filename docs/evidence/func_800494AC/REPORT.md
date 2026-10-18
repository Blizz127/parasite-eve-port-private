# exe func_800494AC (E21 lowering of the matched leaf): the item-list update callback

VMA 0x800494AC, 0xC1C bytes (775 words), matched leaf `src/func_800494AC.c`
(LEAF_STRONG=EXACT, `docs/evidence/func-800494AC/REPORT.md`). The port runs
the generated E21 TU `pc_port/game/decomp/func_800494AC_port.c`, lowered from
that leaf by `gen_decomp_ports.py --e21`.

## Why it was missing

`func_80048918` (item/equipment window constructor, hand port
`decomp_hand/port6_port.c`) gives the 0x0D item list window the update callback
0x800494AC. The matched sources call that constructor from these 12 functions:
- `func_80043DA4`, `func_800466C0`, `func_80047678`, `func_80047A30`,
  `func_80047D74`, `func_80047FE0`, `func_80049354`;
- `func_8004DCA4`, the field-VM menu op;
- `func_8004ECB4`, `func_8004EF58`, `func_80057834`, `func_8005D2B4`.

`menu_callback` resolves an unlisted callback only through the generated
registry, and nothing defined `func_800494AC`. The generator's verbatim path
refused it (`address of function`: the body stores `&func_X` callbacks), and
that reason was not one the E21 lowering accepted.

A Confirm (type-4 event, bit 32 -> 0x10000) dispatched by `func_8005E30C`
passes the focused list node on to the window, because the node's
`func_80063E0C` returns 0 for 0x10000 while +132 is 0.
`PV_494AC_item_list_confirm_runs_native` showed it before the fix:

    [MENU] Unported input callback 800494AC
    [STUB:BOOTSTRAP_RET] PE_MenuInputCallback (first invocation)
    TEST PV_494AC_item_list_confirm_runs_native... FAIL: Confirm on the item list reached an unresolved input callback

## Fix

`guest_lower.LOWER_REASONS` gains "address of function". A `&func_X` is the
same 32-bit code address as a bare `func_X` value, which the lowering already
handles.
- `gen_decomp_ports.py --check --e21` before the change listed 33 E21 leaves.
  After it lists 34, and `func_800494AC` is the only new name.
- `gen_overlay_ports.py --verify` reports 0 drifted rooms and overlays under
  the new reason.

## Proof (e21 differential)

    python3 tools/analysis/e21_leaf.py exe func_800494AC --seeds 128 --uncovered
    e21_leaf: ok func_800494AC (exe) 446 seeds, retail coverage 731/775 words

The directed inputs in nonmatching.yaml drive these branches:
- Confirm with `D_8009CFC4 >= 0` and `D_8009CFC8` 0/1/2/3;
- Confirm with each mode word (`D_8009CF1C`, `D_8009CF30`, `D_8009CF34`,
  `D_8009CF38`, `D_8009CF0C`);
- 0x1000 and cancel (0x40);
- a variant with `r(0x80062A34)=0`.

The 44 uncovered words are short arms that depend on mocked window fields or
mock return values.
