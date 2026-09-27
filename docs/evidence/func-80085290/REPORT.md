# func_80085290 — matching C leaf

- VRAM `0x80085290`, ROM offset `0X75A90`, span `0x3B4`
- Source: `src/func_80085290.c`; YAML row `[0X75A90, c, func_80085290]`
- Build profile: `era_o2_g0`

## Levers
arg-pointer local q=&D_800C0D90 (retail stores through $a0); loop pointer p carried across loops 1-2 with p++ in the for header; control-block stores via *(int *)((char *)c + off) (a non-MEM_IN_STRUCT access keeps the scalar D_800BCD6x/D_8009D2C4 accesses in order); D_8009D2C0 = 1 as the second statement (sched tie-break puts li v1,1 first)

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_80085290
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_80085290
  -> LEAF_STRONG=EXACT func_80085290 span=0x3B4 failures=[none]
```
Spu_InitVoiceState; 2.8.1 band but 2.7.2 exact (2.8.1 best 15)
