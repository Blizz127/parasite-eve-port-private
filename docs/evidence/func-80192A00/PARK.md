# func_80192A00 (room_m0174i, 0x1B0; twin m0348i func_801918E8) — PARKED (lane ovl5, 2026-09-27)

Best 40 words. Two 0x18-byte sprite records at D_80197490/D_801974A8 initialised then passed to func_800C4E50.
Retail anchors a callee-saved base on `&D_80197490[0].hC` (0x8019749C) and addresses only `[0].hC`, `[0].p`,
`[1].p` and both call args through it; all other fields stay absolute `lui $at`. Separate externs: 88;
`SPR D_80197490[2]`: 61 (anchor right, but [1] fields go reg-relative); pointer-local variants 40–70.
