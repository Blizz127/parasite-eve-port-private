# func_8006062C — LINK_EXACT (lane exeC, 2026-09-27)

- VRAM `0x8006062C`, file offset `0x50E2C`, size `0xA18` (646 words); yaml `- [0x50E2C, c, func_8006062C]` replaces
  `- [0x50E2C, asm]` (that span ended at `0x51844`, where `[0x51844, c, func_80061044]` from this lane begins).
  **Integrate together with func_80061044's row.**
- Profile `era_o2_g8` (`-O2 -G8`). Fresh draft (never drafted before). `-O2 -G0` → 634 words + size change.
- Segmented gauge: sprite record `s = func_8005DADC(0x48)`, pen pushed on the `D_8009D12C` stack (the push/pop
  bodies of `func_800634D4`), label via `func_8005FA3C(a0)`, colours `D_8009D110/114`, pop, `func_8005EB64(0x49)`,
  up to six POLY_FT4 segments (`code 0x2C`, len 9, tpage 7, uv/clut from `s`) chosen by `n`, pop, DR_MODE with the
  record's tpage bits `((s[6] & 3) << 7) | 7`.

## Verification (private lane config `build/lanes/exeC/`)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json --deep --only func_8006062C
  disc1_preflight: PASS (deep, 2054 c / 208 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeC/disc1.yaml --profiles build/lanes/exeC/disc1_build_profiles.json func_8006062C
  LEAF_STRONG=EXACT func_8006062C span=0xA18 vram=0x8006062C compiled=0xA20 failures=[none]
```

## Levers (ladder; aligned-instruction diffs from build/lanes/exeC/adiff.py)

1. First draft from the retail control flow: `func_800634D4` push/pop inlines, `func_8005EED4` FT4 alloc/colour and
   chained-store idiom, `func_80061044` bitfield `Tag` link, one `SEG(x0, w, du)` macro per segment → 189.
2. Second push writes the stack slot before bumping `D_8009D12C` (`r[0] = t0; r[1] = t1; D_8009D12C = r + 2;`,
   separate `push2()` inline); the first push keeps the pointer-first order → 54.
3. Store pointer: retail stores through `$a3`, a copy of the pool pointer `g` (`$s0`) made after the init block
   (`move $a3,$s0` in the `beqz` slot and at the end of the init). A block-local copy is coalesced away (or lands in
   `$v1` behind a barrier, or `$a3` pinned but then the `& 0xFFFFFF` tag mask ties into `$a3`); **one function-scope
   `PolyFT4 *p` shared by all six segments** makes it a global pseudo that global-alloc places in `$a3` → 6.
4. Right-cap x0 spelled **`D_8009D124 - (n - K)`** (K = 0x30/0x31/0x32): cc1 then emits retail's
   `addiu K; subu n`; `D + K - n` is canonicalized to `D - (n - K)` with a separate `addiu -K`, and `D - n + K` does
   the subtraction first → 0.
