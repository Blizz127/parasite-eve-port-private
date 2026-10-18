# exe func_8004A9A0 (NON_MATCHING): the shop input callback

SLUS file offset 0x3B1A0, 0x3FC bytes, VMA 0x8004A9A0. The span is a matched
`c` leaf (`src/func_8004A9A0.c`, LINK_EXACT, `docs/evidence/func-8004A9A0/`).
The port runs `src/nonmatching/exe/func_8004A9A0.c` instead. That file is the
matched leaf with one change, recorded in `configs/USA/overlays/nonmatching.yaml`
with its directed inputs.

## Why it was missing

`func_8004A570` (hand port, `absent_lo_window_port.c`) opens a task-9 window
with update callback 0x8004A9A0 and focuses it. `func_80049354`, the equip-slot
list, calls it on Confirm. No `pc_port` file defines `func_8004A9A0`, and the
guest-code registry had no row for 0x8004A9A0. So the first pad event in that
window reached `menu_callback`'s default arm
(`game/boot/func_80063E0C_port.c`), which raises the `PE_MenuInputCallback`
boundary and stops. `PV_4A9A0_slot_detail_input_runs_native` showed it before
the fix:

    [MENU] Unported input callback 8004A9A0
    [STUB:BOOTSTRAP_RET] PE_MenuInputCallback (first invocation)
    TEST PV_4A9A0_slot_detail_input_runs_native... FAIL: slot-detail input reached an unresolved input callback

The generator could not lower the matched leaf: `e21_leaf: skipped call
arity func_8005BBE4 1!=2`.

## The one change

The matched leaf declares `func_8005BBE4(int)`, but `func_8005BBE4(int a0,
int key)` reads a second argument, so retail passes whatever `$a1` holds.
The retail code shows that is `D_8009CFDC` at both calls:

    8004ab98: lw   a1,620(gp)        # D_8009CFDC (gp 0x8009CD70 + 0x26C)
    8004abb8: jal  0x8005bbe4        # a0 = D_8009CFD0 (lw a0,608(gp) at 8004ab94)
    8004abc0: lw   a1,620(gp)        # reloaded; unchanged up to the next call
    8004abf4: lw   a0,608(gp)
    8004abf8: jal  0x8005bbe4

In the matched C this is the pinned `register int c asm("$5") = D_8009CFDC`.
The behaviour C passes it explicitly: `func_8005BBE4(D_8009CFD0, D_8009CFDC)`.

## Proof (e21 differential)

    python3 tools/analysis/e21_leaf.py exe func_8004A9A0 --seeds 180 --uncovered
    e21_leaf: ok func_8004A9A0 (exe) 785 seeds, retail coverage 249/255 words
    e21_leaf: uncovered 8004AD14 8004AD18 8004AD1C 8004AD20 8004AD24 8004AD28

The directed inputs are in nonmatching.yaml:
- sell (`a1=0x1000`) over cases 5, 6 and 0-2, with `D_800A18D8[5|6]`
  below and above `D_8009CFDC`, reaching both `func_8005BBE4` calls;
- buy (`a1=0x4000`), with `out=8005BA78:2:4`, because `func_8005BA78`
  always writes `*outRem` and the mock must fill it in both runs;
- confirm, cancel and no-key;
- confirm with `r(0x80052F0C)=0`, which reaches the item-record compares.

The differential compares every argument register of every call.
A mutation run shows that it catches a wrong second argument. With the
argument set to 0:

    e21_leaf: MISMATCH func_8004A9A0 (exe) seed 0: call 8005BBE4 a1 00000BB8 vs 00000000

0xBB8 is the spec's `D_8009CFDC = 3000`. The file was restored afterwards.

## Uncovered words

The 6 words at 8004AD14..8004AD28 are the `else if (rec ==
func_8005332C(D_800C0E22[0]))` arm of the confirm path. The harness's mock
returns depend only on the callee and the seed, so every `func_8005332C` call
in a run returns the same value. The first compare, against
`func_8005332C(D_800C0E20[0])`, therefore always succeeds when it is reached.
