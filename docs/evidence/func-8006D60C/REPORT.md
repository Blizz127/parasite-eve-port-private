# func_8006D60C — matching C leaf

- VRAM `0x8006D60C`, ROM offset `0X5DE0C`, span `0x53C`
- Source: `src/func_8006D60C.c`; YAML row `[0X5DE0C, c, func_8006D60C]`
- Build profile: `era_o2_g8_dispatch_80011508_abs`

## Levers
PRIVATE profile era_o2_g8_dispatch_80011508_abs = -O2 -G8 + MASPSX_DISPATCH_FOLD=jtbl_80011508 + MASPSX_THREE_WORD_SYMBOL_STORE=1 + MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800B0E08,D_8009CDA4 (gp slots D_8009D188/18C/190 stay gp-relative; the two absolute words are within the -G8 window but retail addresses them absolute). goto re-dispatch state machine (0x41-entry jump table); volatile D_800B0E08 (retail loads it for the test and again for the argument); case 0x2E flags read once into fl with nested ifs (a && form folds to (fl & 0x44) == 0x44)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8006D60C
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8006D60C
  -> LEAF_STRONG=EXACT func_8006D60C span=0x53C failures=[none]
```
CD_StepReadState; 2.7.2 exact

## Main-tree integration (2026-09-27)

Integrated into `configs/USA/disc1.yaml` / `disc1_build_profiles.json` and re-verified against the
main config (not the lane copy): `disc1_preflight.py --deep --only func_8006D60C` PASS,
`leaf_strong_check.py func_8006D60C` → `LEAF_STRONG=EXACT`, `profile_necessity.py --only func_8006D60C` PASS.
Full gate: `EXACT_REBUILD_GATE=PASS plan=cda0547790149322… spans=[2041 c, 218 asm, 2 rodata]`,
sha1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
