# func_800775E8 — MATCHED (59 words, LINK_EXACT)

VRAM `0x800775E8`, file `0x67DE8`, span `0xEC`. era cc1 2.8.1,
`-O2 -G0 -mno-split-addresses`. The profile already existed; no new
assignment name.

`leaf_strong_check.py func_800775E8` →
`LEAF_STRONG=EXACT func_800775E8 span=0xEC vram=0x800775E8 compiled=0xF0 failures=[none]`.
`disc1_preflight.py --deep --only func_800775E8` → PASS (1960 c).

The body names the GPU-stat wait (`D_80095860 & 0x01000000`,
`D_80095854 & 0x04000000`), calls `func_80074E28(&D_800119BC, a0)`,
then `D_80095744[8](a0, a1)`.
