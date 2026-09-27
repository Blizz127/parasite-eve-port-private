# func_8008DB7C — matching C leaf

- VRAM `0x8008DB7C`, ROM offset `0X7E37C`, span `0x6C0`
- Source: `src/func_8008DB7C.c`; YAML row `[0X7E37C, c, func_8008DB7C]`
- Build profile: `era_o2_g0`

## Levers
Akao_Tick: three tempo/voice sweeps. Tempo read through the 0x22 halfword (unsigned short at +0x22 of the 0x68-byte control block); channel-1 tempo read before D_8009D2C8 = c + 1; fresh Ctl*/value locals per block (d for the 0x60 counter chain, e/w for the channel-1 accumulator); h58-slide write through char-offset lvalues (*(T *)((char *)c + off)) keeps the scalar D_8009D2C4 RMW after the field stores; D_800BA560 - 24 for the memcpy source (retail addiu -0x1AA0); third sweep statement order h56--, f50++, h58-- (5 other orders miss by 55-58)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008DB7C
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008DB7C
  -> LEAF_STRONG=EXACT func_8008DB7C span=0x6C0 failures=[none]
```
2.8.1 band but 2.7.2 exact
