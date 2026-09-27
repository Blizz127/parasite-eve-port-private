# func_80089D10 — matching C leaf

- VRAM `0x80089D10`, ROM offset `0X7A510`, span `0x1A8`
- Source: `src/func_80089D10.c`; YAML row `[0X7A510, c, func_80089D10]`
- Build profile: `era_o2_g0`

## Levers
Akao voice-register flush triplet (func_80089980 / func_80089B48 / func_80089D10 differ only in the per-track mask field 0x9C/0xA0/0xA4, the secondary mask 0x34/0x38/0x3C, the OR-in global D_800BCD6C/70/74 and the output D_800C0DD4/D0/D8). Plain 2.7.2 even inside the 2.8.1 band (2.8.1: 103 words). Load-bearing: D_8009D2C8 typed as a pointer to a real struct (Trk) so the record-field loads are MEM_IN_STRUCT and schedule above the D_8009D2C8 pointer-global store (byte-pointer casts keep source order: 10 words); the global is temporarily advanced by 0x68 around each func_80089724 call as retail does.

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80089D10
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80089D10
  -> LEAF_STRONG=EXACT func_80089D10 span=0x1A8 failures=[none]
```

