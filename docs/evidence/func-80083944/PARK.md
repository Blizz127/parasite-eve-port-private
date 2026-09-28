# func_80083944 — PARKED (lane exeD, 2026-09-27)

VRAM `0x80083944`, file `0x74144`, size `0x274`, `asm/disc1/74144.s`.

- Best: 29 diff lines (2.8.1 + MASPSX_RETURN_VIA_EPILOGUE_JUMP) under 2.8.1.
- Draft: `build/lanes/exeD/drafts/func_80083944.c` (if present).
- libpad payload state machine. Inverting the case 2/3 exit polarity (`if (idx >= n) {reset; return 1;} return 0;`) gives 37 -> 29. The case 3 5-byte record addressing (retail loads the 0x3C source pointer before the idx*5 table address) and the case 4 copy-loop registers are unresolved; a direct-indexing macro form is worse (54).
