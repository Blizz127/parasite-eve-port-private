# func_8008E8D0 — EXACT (lane exeI, 2026-09-28)

VRAM `0x8008E8D0`, file `0x7F0D0`, size `0x800` (AKAO sequencer step: opcode loop + note dispatch).
Profile `era_o2_g0_expand_div` (stock cc1 2.7.2 `-O2 -G0`, `MASPSX_EXPAND_DIV=1`). No new maspsx knob.
YAML: `- [0x7F0D0, asm]` → `- [0x7F0D0, c, func_8008E8D0]` (next row `0x7F8D0` is `func_8008F0D0`, so no resume row).
Source `src/func_8008E8D0.c` = lane exeH's best draft (3 raw words; see PARK.md history, items 1–17) plus the fix below.

## Closing lever (3 → 0)
Retail at 0x8008EF54: `mflo $7 / srl $3,$7,7 / lhu $2,144($16) / sh $3,146($16)`.
The exeH draft computed the product into `m` in each arm and shifted at the join (`m >>= 7; U16(0x92) = m;`).
The shift belongs **inside each arm**:

    if (!(xq & 0x8000)) {
        m = (d * ((k * 15) >> 8)) >> 7;
    } else {
        m = (d * k) >> 7;
    }
    U16(0x92) = m;

Each arm's product is now a block-local LO pseudo whose only use is the `srl` right after it. Its reload
(`mflo` into the first unused call-used reg, `$7`) stays inside the arm and does not evict anything
function-wide. The shifted `m` is a GR pseudo (`$3`) that lives across the join. Post-reload cross-jumping
merges the identical `mflo $7 / srl $3,$7,7` arm tails. At the join, sched1 puts the `lhu 144` before the
`sh 146`, which is retail's order.
Ladder: exeH `m >>= 7` at the join: 3. Per-arm `U16(0x92) = (d*k) >> 7` stores: 3 (gets `mflo $7`, but `srl` goes to `$2` and `sh` lands before `lhu`).
Per-arm `m = … >> 7` with the store at the join: **0**.
The `int pad[2]` stays (removing it gives a 56-byte frame against retail's 64).

## Verification (private config build/lanes/exeI/)
- `python3 tools/build/disc1_preflight.py --config build/lanes/exeI/disc1.yaml --profiles build/lanes/exeI/disc1_build_profiles.json --deep --only func_8008E8D0` → `disc1_preflight: PASS (deep, 2142 c / 149 asm / 2 rodata)`
- `python3 tools/analysis/leaf_strong_check.py --config … --profiles … func_8008E8D0` → `LEAF_STRONG=EXACT func_8008E8D0 span=0x800 vram=0x8008E8D0 compiled=0x800 failures=[none]`
- profile necessity (profile_necessity.py has no `--config`, so checked by hand):
  `era_link_check.py src/func_8008E8D0.c 0x8008E8D0 0x800 -O2 -G0` (default) → `linked .text 2016 bytes … word mismatches=33`;
  with `MASPSX_EXPAND_DIV=1` → `word mismatches=0 … LINK_EXACT`. The profile is load-bearing.
