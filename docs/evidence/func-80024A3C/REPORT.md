# func_80024A3C — LINK_EXACT (lane exeA, 2026-09-27)

- VRAM `0x80024A3C`, file offset `0x1523C`, size `0xA80` (ends at `0x15CBC`, the next row `func_800254BC`)
- Profile `era_o2_g8_dispatch_80010824_force_d8009d254_d8009d278_d8009d20c_d8009d2e8_d8009d1a0_d801f1f38_absolute`:
  `-O2 -G8`, `MASPSX_THREE_WORD_SYMBOL_STORE=1`, `MASPSX_DISPATCH_FOLD=jtbl_80010824`,
  `MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_8009D254,D_8009D278,D_8009D20C,D_8009D2E8,D_8009D1A0,D_801F1F38`
- YAML: `- [0x1523C, c, func_80024A3C]` replaces `- [0x1523C, asm]` (span is the whole former asm row).

## Verification (private lane config)

```
python3 tools/build/disc1_preflight.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json --deep --only func_80024A3C
  disc1_preflight: PASS (deep, 2042 c / 217 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeA/disc1.yaml --profiles build/lanes/exeA/disc1_build_profiles.json func_80024A3C
  LEAF_STRONG=EXACT func_80024A3C span=0xA80 vram=0x80024A3C compiled=0xA80 failures=[none]
```

`profile_necessity.py` has no `--config/--profiles` option (hard-wired to the main
config) — skipped. Manual necessity: default `-O2 -G0` → 662 word mismatches;
profile without `MASPSX_THREE_WORD_SYMBOL_STORE` → 2 mismatches; full profile → `LINK_EXACT`.

## Levers

Started from the parked decompA draft (6 words off, all in case 8 — retail
`lhu 0x250 / lb 0xD8($gp) / ori 0x20 / sll / addiu / andi`, cc1 scheduled the
`lb` argument chain first and the `ori` last). The closing lever is **named
value and argument locals computed before the store**:

```c
unsigned short v = UH(p, 0x250) | 0x20;
unsigned short a = D_8009CE48 * 2 + 8;
UH(p, 0x250) = v;
func_8001A680(p, a);
```

Ladder: `UH(p,0x250) |= 0x20; func(p, D_8009CE48*2+8)` → 6; `int a` loaded after
the RMW → 6; `unsigned short a` computed before the RMW → 6; value+arg locals
both before the store (above) → 0. All other levers are the decompA draft's
(struct-member field macros, `unsigned short[3]` for D_8009CE58/5A/5C, bitfields
in case 9, if/else in case 16).
