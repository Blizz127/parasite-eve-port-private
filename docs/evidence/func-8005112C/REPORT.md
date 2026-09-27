# func_8005112C — LINK_EXACT

- VRAM `0x8005112C`, file `0x4192C`, size `0x118`
- Profile: **new** `era_o2_g8_dispatch_80011158` (`-O2 -G8`,
  `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_80011158`).
  The fold only applies when the three-word gate is also set.
- Verification (lane-3 private config):
  - `disc1_preflight.py … --deep --only func_8005112C` → `dispatch dedup: stripped 24-byte duplicate table …` / `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005112C` → `LEAF_STRONG=EXACT func_8005112C span=0x118 vram=0x8005112C compiled=0x120 failures=[none]`
- Levers: an explicit empty `case 1: break;` makes cc1 emit the 5-entry table
  that retail uses (`jtbl_80011158` = {0→117C, 1→end, 2→11C0, 3→11DC, 4→1204});
  without it, cc1 emits a compare tree. `p = --D_8009D014;` is needed, because a
  separate `p = D - 1; D = p;` reorders the load (63 mismatches).
  `Rec { int w[9]; }` has a stride of 0x24.
- Shape: pop one undo record and replay it.
