# func_8019253C (room_m0203i, 0x2E0; twin m0231i func_80192C8C) — PARKED (lane ovl5, 2026-09-27)

Best 14 words (`build/lanes/ovl5/drafts/park_func_8019253C.c`); all residual in the case-1 prologue schedule:
retail loads h12 first and the hC read after the h12 store; cc1 hoists hC. Fixed on the way: `/24` (not /12),
`SH(o,0xA) *= -1` (retail's sign-extended negate; `-x` and `0 - x` omit the sext), `if (h12 >= 0x18) return 1;
break;` for the branchy return. Tried: all 24 orders of the 4 adds, single-move climb over the 7 case-1 lines,
64 in-struct/non-struct mixes (all 14), -fno-schedule-insns / -O1 / 2.8.1 (worse).
