# room_m0318i func_80196820 (NON_MATCHING)

Blob offset 0x7838, 0x428 bytes, VMA 0x80196820. The span is `asm` in `configs/USA/overlays/room_m0318i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0318i/func_80196820.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0318i — func_80196820, blob offset 0x7838, 0x428 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback with three modes (a0):
 *   0: set up the effect record (func_800CE870 / func_800CE9D4), clear the
 *      halfword at +8 and register func_8019646C (0x14, 0x28) via
 *      func_800CE560;
 *   1: gate on the halfword at D_800E2368->0x16 (0 -> return 1); with
 *      D_800E27EC == 2 spawn one record copied from the argument block plus
 *      four randomised ones (x/+0 jitter, +0xA += 0x400, +8 += rand); with
 *      D_800E27EC == 8 the same with the roles of +0/+2 and +8/+0xA swapped;
 *      returns 2 when D_800E27EC >= 8, otherwise 0;
 *   2: load the rotation/translation of *D_800BCFA4 into the GTE, copy the
 *      three position halfwords of the argument block to D_80199930.. and set
 *      the D_800F336C draw block.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

The first re-run used 40 seeds and the one spec line the yaml had then. The
mode-1 gate (the halfword at D_800E2368->0x16) is read through a pointer, and too
few seeds reached its open arm, so 70 mode-1 words were left uncovered. The yaml
then got two more lines, the verifier's edge-value specs. The first of them also
points D_800E2368 at a known word and sets both gate values
(`w(0x800E2368)=0x80150400;h(0x80150416)=0,1`). The 200-seed re-run over all three
lines covers every word.

    python3 tools/analysis/e21_leaf.py room_m0318i func_80196820 --check-gte --uncovered --seeds 40
    e21_leaf: ok func_80196820__d2c8742e_port.c 30 seeds, retail coverage 196/266 words
    e21_leaf: uncovered 8019690C 80196910 80196914 80196918 8019691C 80196920 80196924 80196928 8019692C 80196930 80196934 80196938 8019693C 80196940 80196944 80196948 8019694C 80196950 80196954 80196958 8019695C 80196960 80196964 80196968 8019696C 80196970 80196974 80196978 8019697C 80196980 80196984 80196988 8019698C 80196990 80196994 80196998 8019699C 801969A0 801969A4 801969A8 801969AC 801969B0 801969B4 801969B8 801969BC 801969C0 801969C4 801969C8 801969CC 801969D0 801969D4 801969D8 801969DC 801969E0 801969E4 801969E8 801969EC 801969F0 801969F4 801969F8 801969FC 80196A00 80196A04 80196A08 80196A0C 80196A10 80196A14 80196A18 80196A1C 80196A20

    python3 tools/analysis/e21_leaf.py room_m0318i func_80196820 --check-gte --uncovered --seeds 200
    e21_leaf: ok func_80196820__d2c8742e_port.c 499 seeds, retail coverage 266/266 words
    e21_leaf: uncovered none

## Uncovered words

none

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0318i func_80196820 --nonmatching 7838:428 --check-gte --uncovered --seeds 200 --inputs "a0=0,1,2,3,-1;w(0x800E27EC)=2,8,5,9"  (rerun of the claim)`
  - e21_leaf: ok func_80196820__d2c8742e_port.c 152 seeds, retail coverage 266/266 words | e21_leaf: uncovered none
- `python3 tools/analysis/e21_leaf.py room_m0318i func_80196820 --nonmatching 7838:428 --check-gte --uncovered --seeds 72 --inputs "a0=1;w(0x800E27EC)=2,8,7,-1,0x80000000,0x7FFFFFFF;h(a1+8)=0,0x7FFF,0x8000,0xFFFF;h(a1+0xA)=0,0x7C00,0xFFFF"`
  - e21_leaf: ok func_80196820__d2c8742e_port.c 48 seeds, retail coverage 185/266 words | e21_leaf: uncovered 0x80196840-0x801968C0 (mode 0 and mode>=3 arms) and 0x80196B6C-0x80196C28 (mode 2) -- only because the spec pins a0=1; no MISMATCH
- `python3 tools/analysis/e21_leaf.py room_m0318i func_80196820 --nonmatching 7838:428 --check-gte --uncovered --seeds 180 --inputs "a0=2,0,-1,3,0x10000;h(0x800E11EA)=0,1,5,0xFFFF;h(a1+0)=0,0x8000,0xFFFF;h(a1+4)=0,0x7FFF,0x8000"`
  - e21_leaf: ok func_80196820__d2c8742e_port.c 150 seeds, retail coverage 96/266 words | e21_leaf: uncovered 0x801968C4... (mode-1 body) -- only because the spec has no a0=1; no MISMATCH

No MISMATCH, notest or skipped in any run. The claimed command was re-run and reproduced exactly (266/266, uncovered none).

The two new specs were deliberately narrow, so their own coverage is partial (185/266 and 96/266). That is a property of my specs, not a defect in the claim, and the claimed run already covers all 266 words.

Side-by-side read of the C (src/nonmatching/overlays/room_m0318i/func_80196820.c) against asm/overlays/room_m0318i/517C.s lines 2686-2965 found nothing that differs:
- Mode dispatch: a0=1, a0=2, a0=0, and any other value (negative, 3, 0x10000) return 0 with no side effects.
- Mode 0: call arguments match, including the 0x14/0x28/func_8019646C registration, the a1+8 pointer and the zero halfword store at a1+8.
- Mode 1: the D_800E2368->0x16 gate returns 1 when it is zero. The code reloads D_800E27EC after the ==2 block, tests ==8 on the reloaded value, and the final slti-8 check reloads it again, so it returns 2 for values >= 8 (signed). The two randomised-record loops are identical, including the order of the three func_80071A54 calls and the mask/offset constants (0x7F-0x40, 0x1F+0x10). The +0xA += 0x400 and +8 += rand additions are 16-bit, matching the unsigned-halfword wraparound.
- Mode 2: the GTE rotation/translation loads match, the three position halfwords are copied, and the D_800F336C block gets 3/0/0/0x20 with the halfword from D_800E2850[D_800E11EA]. All of these loads and stores are unsigned halfword (lhu/sh).

Weakness in the claim's evidence, not a refutation: the claimed inputs vary only a0 and D_800E27EC, so the a1 field values, D_800E11EA and the D_800E2368->0x16 gate are exercised only through the harness's random seeding. My two extra specs, which varied those fields, also found no mismatch."

