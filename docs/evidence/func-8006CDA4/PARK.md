# func_8006CDA4 — PARKED (lane exeC, 2026-09-27)

- VRAM `0x8006CDA4`, file offset `0x5D5A4`, size `0x2D4` (181 words), `asm/disc1/5D5A4.s`.

## This lane (exeC)

- Not re-attacked this lane (time went to the closer candidates; 10 of 29 closed). Best known state is the
  prior fan-out record below.

## Prior fan-out record (build/archive/fanout/*/results.jsonl)

- `decompB`: {"status": "parked", "profile": "era_o2_g8_dispatch_80011428 (new private: 2.7.2 -O2 -G8 + DISPATCH_FOLD=jtbl_80011428 + THREE_WORD)", "best_mismatches": 112, "draft": "drafts/func_8006CDA4.c", "note": "CD_ReadSectors state machine (states 0/7/8/9/10 via jtbl_80011428, gp-relative D_8009D170..7C, 6 args). Structure exact: jump table folds, case 8/9/10 and the mode dispatch (if-chain ==1/==0/==2/==3 with bodies in 0..3 order) match one word shifted. Levers: 6th stack arg copied to a local (retail lw s3,0x54(sp) at entry; also restores the idx*2 spill slot / 0x40 frame), rem-nonzero arm first in state 7, pins st/done/stop/block/dst. Residual (count inflated by the 1-word shift): prologue param-copy order (buf/mode), state 0 table addressing (retail hoists &D_8009317C and keeps lhu 6(base+idx*2); cc1 folds base+6 into the hoisted constant for every spelling tried), state 7 rem/chunk register roles."}

## Lane exeC3 (2026-09-27) — 115 raw → 13 raw words

Best draft `build/lanes/exeC3/drafts/func_8006CDA4.c` (profile `era_o2_g8_dispatch_80011428`).
Landed: `mode`/`dst` as plain params (only `block` pinned $19; prologue save order a0,a2,a3 as
retail); `unsigned short *p = (unsigned short *)&D_8009317C + idx; n = p[3] - D_8009317C.off[idx];`
(hoisted `s7 = &off[idx]` = Q + (sym+4), off[idx+1] via (sym + Q) + 6 with Q spilled — the prologue
now matches); case 7 `rem = D_8009D178` + block-local `register int ch asm("$7")` set through
`asm("" : "=r"(ch) : "0"(rem))`; case 10 `register int seven asm("$3") = 7`; case 9
`if (r == -1) { asm volatile(""); st[0xF0] = 0; goto busy; } st[0xF0] = 10; break;` (the barrier stops
the cross-jump into case 10's identical `sb zero; j busy` and gives retail's bne polarity).
Residual (case 0 only): retail reads `off[idx]` twice (lhu a0 for the sum, lhu v0 for the
difference, both before the stores) with n in a1 and `st[0x100]` loaded early into v1; cc1 CSEs the
two reads. Volatile-cast reads get both loads before the stores but permute a0/a1/v1 (A8/z2 forms,
12 aligned but +1 word); 128-form address-spelling sweep, inline fsize()/fpos() helpers, statement
orders: no better.
