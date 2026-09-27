# func_8005BCBC — LINK_EXACT

- VRAM `0x8005BCBC`, file `0x4C4BC`, size `0x54`
- Profile: `era_o2_g8` (`-O2 -G8`)
- Verification (lane-3 private config):
  - `disc1_preflight.py --config $S/lanes/lane3/disc1.yaml --profiles $S/lanes/lane3/disc1_build_profiles.json --deep --only func_8005BCBC` → `disc1_preflight: PASS (deep, …)`
  - `leaf_strong_check.py … func_8005BCBC` → `LEAF_STRONG=EXACT func_8005BCBC span=0x54 vram=0x8005BCBC compiled=0x60 failures=[none]`
- Levers: `-G8` for `D_8009D0C0/C4/C8`, `D_8009D218`; a pointer local `p`
  with a conditional `p += 0x10` (both arms) gives retail's `j`/delay-slot
  `addiu $a1,$a1,0x10`.
- Shape: select text window (`D_800C20A4` or `D_800C0DE0`, +0x10 variant), size 8.
