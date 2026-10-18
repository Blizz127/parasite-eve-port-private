# room_m0429i func_80192878 (NON_MATCHING)

Blob offset 0x3890, 0x298 bytes, VMA 0x80192878. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_80192878.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_80192878, blob offset 0x3890, 0x298 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Per-frame effect-object step. Copies the 10-word argument block
 * (a0+0x1C..0x40) into the D_8009D254 player record (+0x1E8..0x204, +0x28,
 * +0x30), then dispatches on the mode byte s0+0xE (s0 = *(a0+8)): 0xD and
 * 0x10 are the two timed modes, 0xC does nothing, any other mode (and a
 * dead/empty record *D_8009D254, lh +0xC <= 0) runs the teardown
 * func_80020CE4 + func_80192B10(s0, a0+0xC) + func_80192BFC(a0).
 * The shorts s0+0x16 / s0+0x1A are the current / previous frame counters,
 * compared with the limit s0+0xF - 1 and the thresholds 0x12 / 3 / 0xB.
 * Declared void: the retail v0 on exit is whatever the last call/load left
 * and no caller reads it.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_80192878 --check-gte --uncovered --seeds 1728
    e21_leaf: ok func_80192878__0db91557_port.c 1728 seeds, retail coverage 166/166 words
    e21_leaf: uncovered none

## Uncovered words

none

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0429i func_80192878 --nonmatching 3890:298 --check-gte --uncovered --seeds 1728 --inputs "<claimed spec>" (re-run exactly as claimed)`
  - e21_leaf: ok func_80192878__0db91557_port.c 1728 seeds, retail coverage 166/166 words ; e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80192878 --nonmatching 3890:298 --check-gte --uncovered --seeds 576 --inputs "a0=0x80120000; w(a0+8)=0x80121000; w(0x8009D254)=0x80122000; w(0x80122000)=0x80123000; h(0x8012300C)=1; w(a0+0x10)=0x80124000; w(0x80124000)=0; h(0x8012100E)=0x010D,0xFF0D,0x0110,0xFF10,0x010C,0x010E,0x0100,0x00FF; h(0x80121016)=0,2,3,0xB,0x11,0x12,0x7FFF,0x8000,0xFFFF; h(0x8012101A)=0,2,3,0xA,0xB,0x11,0x12,0x8000; w(0x80121000)=0x80125000; w(0x80125018)=0x80126000; h(0x80126000)=1"`
  - e21_leaf: ok func_80192878__0db91557_port.c 576 seeds, retail coverage 166/166 words ; e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py room_m0429i func_80192878 --nonmatching 3890:298 --check-gte --uncovered --seeds 864 --inputs "a0=0x80120000; w(a0+8)=0x80121000; w(0x8009D254)=0x80122000; w(0x80122000)=0,0x80123000,0x80127000; h(0x8012300C)=0,1; h(0x8012700C)=0xFFFF; w(a0+0x10)=0x80124000; w(0x80124000)=0; h(0x8012100E)=0x030D,0x0310,0x030C,0x0305; h(0x80121016)=2,3,0x12; h(0x8012101A)=0,3,0x11; w(0x80121000)=0,0x80125000; w(0x80125018)=0x80126000; h(0x80126000)=1,2"`
  - e21_leaf: ok func_80192878__0db91557_port.c 864 seeds, retail coverage 165/166 words ; e21_leaf: uncovered 80192A18

No MISMATCH, notest or skipped in any run. The one uncovered word in my run C (0x80192A18, the delay-slot sll reached when mode 0xD has the previous counter s0+0x1A >= 0x12) is a gap in my own spec, because I only used 0/3/0x11 for that field. It is reachable and the claimed run does cover it (166/166), so it is not a refutation. Side-by-side read of asm against C found no divergence. Compares use signed shorts: lhu followed by sll/sra 16 equals the C short compares. The mode byte is lbu and the limit is lbu minus 1 compared signed. The dead-record test (null, or lh +0xC <= 0) goes to teardown. Modes 0xC, 0xD, 0x10 and the default arm match, including the order of func_80020CE4, func_80192B10(s0, a0+0xC) and func_80192BFC(a0), the 0x4000 and 0x80000000 ORs, the byte stores of 2 and 4, the 0x280000 store to s0+0x14 and the word 2 store through *(a0+0x10). Return value is void, and no caller reads v0 per the C header comment. Run A pushed 0x7FFF, 0x8000 and 0xFFFF counters, limit bytes 0 and 0xFF, and modes 0 and 0xFF through all arms with no mismatch. Run B pushed null/negative player records and a null *s0 through all arms with no mismatch. The first launch timed out in the foreground and was rerun in the background with identical arguments. The unconstrained e21 tool does not check the func_80020CE4 return value, since the C is void and ignores it."]

