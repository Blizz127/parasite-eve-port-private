# func_8008CF70 — matching C leaf

- VRAM `0x8008CF70`, ROM offset `0X7D770`, span `0x1CC`
- Source: `src/func_8008CF70.c`; YAML row `[0X7D770, c, func_8008CF70]`
- Build profile: `era_o2_g0_cc1_281`

## Levers
PsyQ SpuSetReverbModeType shape (clear-work-area flag 0x100, 68-byte preset copied bytewise from D_8009C8C0 + mode*68, delay/feedback by mode 7/8, reverb-enable bit parked around func_8008D140). Band compiler 2.8.1 -mno-split-addresses (needed: the reverb-attr struct D_8009B39C {mask, mode, depth_l, depth_r, delay, feedback} stores go through a CSE'd `la` base with 0/-4 and 0/2 offsets; 2.7.2 emits per-store `lui $at`). Other levers: SPU register base `volatile unsigned short *D_8009B3FC` (the RMW reloads), `on = (D_8009B3FC[0xD5] & 0x80) != 0` (a shift-and-mask local makes the andi write $s1 directly), the do/while byte copy with `--n != -1`, and a `register long off asm("$2") = a0 << 2` pin for the startaddr table pointer (unpinned: $v0/$v1 swap, 4 words).

## Verification (decompB private config, 2026-09-23)
```
python3 tools/build/disc1_preflight.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json --deep --only func_8008CF70
  -> disc1_preflight: PASS (deep)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/decompB/disc1.yaml --profiles build/fanout/decompB/disc1_build_profiles.json func_8008CF70
  -> LEAF_STRONG=EXACT func_8008CF70 span=0x1CC failures=[none]
```

