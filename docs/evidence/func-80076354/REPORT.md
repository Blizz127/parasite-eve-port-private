# func_80076354 — DMA ch2 OT kick + CHCR busy poll (0x80076354, 0xE0)

Status: **matched** (tooling2 lane, 2026-09-23), `LINK_EXACT` + `LEAF_STRONG=EXACT`.

- YAML: `- [0x66B54, c, func_80076354]` then `- [0x66C34, asm]`
- Profile: `era_o2_g0_fill_call_volatile_store_fill_epilogue_delay_slot`
  = 2.7.2 `-O2 -G0` + `MASPSX_FILL_CALL_VOLATILE_STORE=1` (new maspsx patch 18)
  + `MASPSX_FILL_EPILOGUE_DELAY_SLOT=1` (patch 3).
- Source: `src/func_80076354.c` (from the lane-4 draft, unchanged body).

## Why the new patch

Retail 0x800763C4: `jal func_800773D0` / `sw $v1,0($v0)` — the volatile CHCR
store sits in the call's delay slot. cc1 (both 2.7.2 and 2.8.1) emits the
store before the `jal` because reorg never schedules a volatile insn; ASPSX's
reorder pass moved it. Patch 18 models that move (see the patch log in
`tools/era/maspsx/maspsx/__init__.py`).

## Load-bearing ladder (era_link_check.py, 2.7.2 -O2 -G0)

| knobs | word mismatches |
|---|---|
| none | 27 |
| patch 3 only | 28 |
| patch 18 only | 2 (epilogue `addiu $sp` before `jr`) |
| patch 18 + patch 3 | **0 (LINK_EXACT)** |
| 2.8.1 -mno-split-addresses + patch 18 | 20 (s0/s1 swap: loop mask CSE'd from the first test) |

## Verification (private config build/fanout/tooling2/)

```
python3 tools/build/disc1_preflight.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json --deep --only func_80076354
  -> disc1_preflight: PASS (deep, 1836 c / 303 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/fanout/tooling2/disc1.yaml --profiles build/fanout/tooling2/profiles.json func_80076354
  -> LEAF_STRONG=EXACT func_80076354 span=0xE0 vram=0x80076354 compiled=0xE0 failures=[none]
```
