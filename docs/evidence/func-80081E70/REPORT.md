# func_80081E70 — EXACT (lane exeD3, 2026-09-27)

- VRAM `0x80081E70`, file offset `0x72670`, size `0x2DC`. CD data-ready interrupt callback (`D_8009B6EC` record). Inside the 2.8.1 band.
- Profile `era_o2_g0_cc1_281`: `ERA_CC1_VER=2.8.1`, `-O2 -G0 -mno-split-addresses`.
- Yaml: `- [0x72670, c, func_80081E70]`. The next row is already `[0x7294C, c, func_8008214C]`.

## Verification (private config build/lanes/exeD3/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeD3/disc1.yaml --profiles build/lanes/exeD3/disc1_build_profiles.json --deep --only func_80081E70
  disc1_preflight: PASS (deep, 2081 c / 191 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeD3/disc1.yaml --profiles build/lanes/exeD3/disc1_build_profiles.json func_80081E70
  LEAF_STRONG=EXACT func_80081E70 span=0x2DC vram=0x80081E70 compiled=0x2E0 failures=[none]
```

## Levers (starting point: exeD's 1-word park)
- In exeD's draft, cc1 CSE turned the first `r->cb(1, …)` into `move a0,$s3`, reusing the `$19`-pinned `one`. Retail has `li a0,1`.
- **Hide `one`'s value from CSE with an asm barrier at its definition**: `asm("" : "=r"(one) : "0"(1)); if (iv == one)`. That fixed the call, but `li $19,1` then scheduled ahead of the `andi` (the sched1 swap noted in exeD's park).
- **Zero-extend `intr` into an `int iv` local before the barrier** (`iv = intr;`) and use `iv` for every later test and for the cb argument. This put `andi` first. Using `intr` directly instead of `iv` is still 1 word off.
- **`DsCb` first parameter typed `int`**, not `unsigned char`. With `unsigned char`, the `r->cb(iv, …)` call re-emits `andi a0,$16,0xff` where retail has `move a0,$16`.
- Kept from exeD: `func_8007EE84(9, y, 0, -1)` with `y` from `asm("" : "=r"(y) : "0"(0))`. Dropping it is 1 word off (`move a2,a1`).
- A barrier on `iv` itself is not needed (ablation X2).
