# func_8006D2B8 — matching C leaf

- VRAM `0x8006D2B8`, ROM offset `0X5DAB8`, span `0x354`
- Source: `src/func_8006D2B8.c`; YAML row `[0X5DAB8, c, func_8006D2B8]`
- Build profile: `era_o2_g8_abs_800b0e64`

## Levers
PRIVATE profile era_o2_g8_abs_800b0e64 = -O2 -G8 + MASPSX_FORCE_ABSOLUTE_SYMBOLS=D_800B0E64 (D_8009D180/D_8009D184 gp-relative; the D_800B0E64 pointer absolute). Levers: Span bitfield {off:22,n:10} compared directly in the loop condition (blez/slt); unsigned-char read of the signed bDA slot then (signed char) cast (lbu+sll/sra) with D_8009D180 = 0 stored between; the flags&0x80 -> -2 path is a goto into the flags&0x40 -2 store (retail cross-jumps the second into the first); done = (arg5 ^ 1) & 1 (xori/andi); register int done asm("$17") pin fixes the s1/s2/s3 rotation (accepted pin device); ((unsigned char *)s + i)[0xDA] and s->bDC[*out << 1] give retail's base-first addu

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8006D2B8
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8006D2B8
  -> LEAF_STRONG=EXACT func_8006D2B8 span=0x354 failures=[none]
```
CD_SeekToTrack; 2.7.2 exact
