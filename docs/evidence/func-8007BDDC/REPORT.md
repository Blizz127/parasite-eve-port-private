# func_8007BDDC

VRAM `0x8007BDDC`, size `0x168`, file `asm/disc1/6C5DC.s` before the carve.
Profile `era_o2_g0_cc1_281_beqz_sym_store_delay_fill_call_delay_slot_index_pair_jal_schedule`:
`-O2 -G0 -mno-split-addresses`, `ERA_CC1_VER=2.8.1`,
`MASPSX_BEQZ_SYM_STORE_DELAY=1`, `MASPSX_FILL_CALL_DELAY_SLOT=1`,
`MASPSX_INDEX_PAIR_JAL_SCHEDULE=1`.

The leaf polls `func_80073A44(-1)` until `D_800A3478` or the `0x3C0000`
counter cap is passed, or until bit 24 of `*D_8009B2C0` clears. The
failure path prints two indexed table words through `func_80071A74` and
returns `-1`. The success path returns `0`, or `1` when the flag argument
is set and the bit is still set.

cc1 2.8.1 shifts the first index byte before the second, leaves a
load-delay `nop` where retail adds the shifted byte-0 base, and puts the
stack argument in the `jal` delay. The status `-1` falls through in `$v1`.
`MASPSX_INDEX_PAIR_JAL_SCHEDULE` reorders that one window: byte 1 is
shifted first, the byte-0 add replaces the `nop`, the stack word is stored
before the two result loads, the format address low half fills the `jal`
delay, and the failure path jumps over `$v0 = 0` to the status test.
The compare-store knob puts the counter increment in the `beqz` delay.
The call-delay knob puts `D_80011B18` in the `func_80073C5C` delay.

```
TMPDIR=build/tmp ERA_CC1_VER=2.8.1 \
  MASPSX_BEQZ_SYM_STORE_DELAY=1 \
  MASPSX_FILL_CALL_DELAY_SLOT=1 \
  MASPSX_INDEX_PAIR_JAL_SCHEDULE=1 \
  python3 tools/analysis/era_link_check.py \
  src/func_8007BDDC.c 0x8007BDDC 0x168 -O2 -G0 -mno-split-addresses
```

`LINK_EXACT`, word mismatches 0, `nonzero_pad=0`. The linked section is
368 bytes; the extra 8 are a zero alignment pad. `leaf_strong_check.py
func_8007BDDC` printed `LEAF_STRONG=EXACT` (`compiled=0x170`).
`disc1_preflight.py --deep --only func_8007BDDC` passed
(`2036 c / 219 asm / 2 rodata`). `profile_necessity.py --only
func_8007BDDC` printed `PROFILE_NECESSITY=PASS`. The disc-1 exact rebuild
then passed: `spans=[2036 c, 219 asm, 2 rodata]`, both SHA-1s
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`, `VERIFY_SWEEP=PASS leaves=2036`.
This leaf is off the gameplay route, so those route totals stay
`funcs=1261/1453`, `c_words=68845`.
