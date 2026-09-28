# func_80080404 — LINK_EXACT (lane exeE, 2026-09-27)

- VRAM `0x80080404`, file offset `0x70C04`, size `0x288` (162 words), inside the cc1 2.8.1 band.
- Shape: libds-style link state machine over the `D_8009B578` state block (words at
  -4/0/+4/+0x14/+0x18/+0x28, flag byte at -0xC, `D_8009B554` at -0x24) and three
  optional callbacks `D_800A36A4/A8/AC`.
- Profile: `era_o2_g0_cc1_281_load_delay_nop_before_label`
  (`-O2 -G0 -mno-split-addresses`, `ERA_CC1_VER=2.8.1`, `MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1`).
- YAML: `- [0x70C04, c, func_80080404]` (the following row `- [0x70E8C, c, func_8008068C]` already exists).

## Verify (private lane config build/lanes/exeE/)
```
python3 tools/build/disc1_preflight.py --config build/lanes/exeE/disc1.yaml --profiles build/lanes/exeE/disc1_build_profiles.json --deep --only func_80080404
  disc1_preflight: PASS (deep, 2088 c / 190 asm / 2 rodata)
python3 tools/analysis/leaf_strong_check.py --config build/lanes/exeE/disc1.yaml --profiles build/lanes/exeE/disc1_build_profiles.json func_80080404
  LEAF_STRONG=EXACT func_80080404 span=0x288 vram=0x80080404 compiled=0x290 failures=[none]
(ERA_CC1_VER=2.8.1 MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1 tools/analysis/era_link_check.py src/func_80080404.c 0x80080404 0x288 -O2 -G0 -mno-split-addresses)
  word mismatches=0
```
`profile_necessity.py` has no `--config` option (it rejects a leaf present only in the
private yaml), so necessity is shown by the ladder below; the lead's gate run covers it.

## Levers / mismatch ladder
| variant | mismatches |
|---|---|
| archived draft (if-chain, non-volatile fn pointers, wrong `st[0]=13` in else arm), 2.7.2 or 2.8.1 | 112 |
| nested `switch` statements (cc1 emits `slti` trees; retail is pure `bne` chains), 2.8.1 | 143 |
| if/else-if ladders + `extern void (*volatile D_800A36Ax)(int,int)`, default 2.7.2 | 146 |
| same, 2.8.1 `-mno-split-addresses` | 2 (two `j` to the shared `jalr` one word early) |
| + `MASPSX_LOAD_DELAY_NOP_BEFORE_LABEL=1` | **0** |

1. **Volatile callback pointers**: retail re-loads each `D_800A36Ax` between the null
   test and the `jalr` (test load in `$v1`, call load in `$v0`); `*volatile` reproduces both loads.
2. **if/else-if, not switch**: both the `st[0]` (13/14/16..17) and `st[1]` (21..24) dispatches are
   equality chains in retail; a C `switch` gives a balanced `slti` tree.
3. The else arm stores `12` (retail `sw $v1` with `$v1 = 12` from the entry compare), not 13.
4. cc1 2.8.1 cross-jumps the three callback calls onto one `jalr`; the knob puts the `lw`
   load-delay nop before the shared label, so the two `j` land on the `jalr` itself.
