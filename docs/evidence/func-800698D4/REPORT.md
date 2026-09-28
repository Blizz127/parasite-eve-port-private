# func_800698D4 — EXACT (lane exeF, 2026-09-28)

- VRAM `0x800698D4`, file offset `0x5A0D4`, size `0x234` (141 words), `asm/disc1/5A0D4.s`.
- Profile: default `era_o2_g0` (`-O2 -G0`, stock cc1 2.7.2); no assignment needed.
- YAML row: `- [0x5A0D4, c, func_800698D4]` replaces `- [0x5A0D4, asm]` (next row `0x5A308` is
  already `func_80069B08`).
- **Registry:** `src/func_800698D4.c` must be REMOVED from `configs/USA/disc1_nonmatching_sources.json`
  (it was `accepted-residual`; it is now exact).

## Verification
```
python3 tools/analysis/era_link_check.py src/func_800698D4.c 0x800698D4 0x234 -O2 -G0
  linked .text 576 bytes, target 0x234, word mismatches=0, nonzero_pad=0
  LINK_EXACT
python3 tools/build/disc1_preflight.py --config build/lanes/exeF/disc1.yaml --profiles build/lanes/exeF/disc1_build_profiles.json --deep --only func_800698D4
  disc1_preflight: PASS (deep, 2131 c / 154 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeF/disc1.yaml --profiles build/lanes/exeF/disc1_build_profiles.json func_800698D4
  LEAF_STRONG=EXACT func_800698D4 span=0x234 vram=0x800698D4 compiled=0x240 failures=[none]
```

## Lever
Residual was 1 word (560 vs 564 bytes): retail leaves the `beqz v1,.L80069ADC` delay slots of
searches #3/#4 as `nop`, cc1 stole `li v0,-1` from the fall-through as it (and retail) does for
#1/#2. gcc-2.7.2 `reorg.c`: the beqz is predicted not-taken (EQ), so fill_eager_delay_slots calls
`fill_slots_from_thread` on the fall-through first; its scan stops at `stop_search_p`, which is
true for an asm insn. An empty `asm("");` placed between the `if (v1 != 0)` and `if (v1 != -1)`
tests of #3 and #4 stops that scan before `li v0,-1`; the target-thread fallback finds only the
2-word `lb v0,D_800B0DCD`, so the slot stays `nop` — retail's shape. The asm emits no code and
changes no scheduling or allocation here. `asm volatile("")`, `asm("" : : "r"(v1))` also exact.
(Prior PARK notes blamed the asm for bloat because of where it was placed.)
Ladder: base 51 (1 word short, shifted) → asm at #3 only would fix one slot → asm at #3 and #4: 0.
Also retained from exeC5: `extern signed char D_800B0DCD` (`lb`) and the `== 0` tail (`sltiu`).
