# func_8007BDDC — not carved

VRAM `0x8007BDDC`, size `0x168` (90 words).

Best link: **25 words**, linked 368 bytes, `nonzero_pad=0`.
`ERA_CC1_VER=2.8.1`, `-O2 -G0 -mno-split-addresses`,
`MASPSX_BEQZ_SYM_STORE_DELAY=1`.

That knob moves the symbolic counter `sw` into the empty `beqz` delay
(`lui $at` / `beqz` / `sw %lo($at)`). With it, the counter block matches
except the `beqz` displacement (`0x1b` vs `0x1c`): the bad path is one
instruction longer than retail. The `func_80071A74` address math is still
a different schedule (`lbu` of `bytes[1]` and `D_8009AFD5` before
`bytes[0]`, and the format `addiu` sits before `jal` instead of in its
delay). The join uses `$v1` for the `-1` status; retail uses `$v0` and a
`j` over an explicit zero.

Not integrated. No exact rebuild was run.
