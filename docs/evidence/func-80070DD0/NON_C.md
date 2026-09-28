# func_80070DD0 — provably non-C (lane exeC, 2026-09-27)

VRAM `0x80070DD0`, file `0x615D0`, size `0x34` (13 words), `asm/disc1/615D0.s`.

```
80070DD0  or    $v1, $zero, $ra        ; return address parked in a caller-saved GPR, no stack frame
80070DD4  jal   func_80070D6C
80070DD8   sub  $a1, $a1, $a0          ; trapping SUB (disassembler: handwritten instruction)
80070DDC  andi  $v0, $v0, 0xFFFF
80070DE0  mult  $v0, $a1
80070DE4  or    $ra, $zero, $v1        ; manual $ra restore
80070DE8  mflo  $v0
80070DEC  mfhi  $t3
80070DF0  srl   $v0, $v0, 16
80070DF4  sll   $t3, $t3, 16
80070DF8  or    $v0, $v0, $t3
80070DFC  jr    $ra
80070E00   add  $v0, $v0, $a0          ; trapping ADD (handwritten instruction)
```

Three independent non-C markers: (1) a call made without a frame, with `$ra` saved/restored through `$v1` via
`or` (cc1 always spills `$ra` to the stack in a non-leaf and uses `addu`/`move` for register copies);
(2) trapping `sub`/`add` (cc1 2.7.2/2.8.1 emit only `subu`/`addu` for C `int` arithmetic); (3) it relies on
`func_80070D6C` preserving `$v1`/`$a0`/`$a1` — a hand-written calling contract. It is a hand-scheduled fixed-point
interpolation helper (`a0 + ((f(...) & 0xFFFF) * (a1 - a0)) >> 16`). Prior fan-out (goalb2) best C attempt: 13 words.
