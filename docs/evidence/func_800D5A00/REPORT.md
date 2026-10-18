# exe func_800D5A00 (NON_MATCHING)

SLUS file offset 0xC6200, 0x2E4 bytes, VMA 0x800D5A00. The span is `asm` in `configs/USA/disc1.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/exe/func_800D5A00.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_b3938761). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
func_800D5A00 -- EXE, VRAM 0x800D5A00, file offset 0xC6200, 0x2E4 bytes.
 * NON_MATCHING behaviour C (proven by the e21 differential; not a byte match).
 * Three-mode effect callback (a0 = mode, a1 = effect record):
 *   mode 0: seed the record (rand into +8, +0xC = 0, +0/+2/+4 = halfwords
 *           0x268/0x26A/0x26C of the object at D_800F32D0->+8), then
 *           func_800CE560(D_800F33E0->+8, 8, 0x18, func_800D5898); its result
 *           is the return value.
 *   mode 1: when D_800E27EC is 4..10 spawn up to three particles from
 *           func_800CE610(D_800F33E0->+8); return D_800E27EC >= 0x20.
 *   mode 2: D_800F3374 = 0x40; when D_800E27EC < 13 draw with
 *           f = sin((D_800E27EC << 10) / 12); set the camera matrix
 *           *D_800BCFA4 and func_800CF5B0(a1, 0). Returns 0.
 * Retail copies the 4 bytes at D_800C22C4 to a stack buffer (stack address s2)
 * and passes that buffer to func_800CF3AC / func_800D0728.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py exe func_800D5A00 --check-gte --uncovered --seeds 288
    e21_leaf: ok func_800D5A00 (exe) 550 seeds, retail coverage 185/185 words
    e21_leaf: uncovered none

## Uncovered words

Main run only: 800D5A5C (j .L800D5CC4) and 800D5A60 (its nop delay slot). This is the "mode < 2 but not 0 or 1" arm, so it needs a negative a0. The supplementary run with a0=0xFFFFFFFF,0x80000000 executes it with no mismatch. No word is left unexecuted overall.

## Notes from the decomp agent

Behaviour of func_800D5A00(mode, rec):
- mode 0: seeds rec->+8 from func_80071A54, sets +0xC to 0, and copies halfwords 0x268/0x26A/0x26C of D_800F32D0->+8 into rec +0/+2/+4. It then returns func_800CE560(D_800F33E0->+8, 8, 0x18, func_800D5898), so the return value is that call's result.
- mode 1: when D_800E27EC-4 is below 7 (unsigned), it runs 3 iterations of func_800CE610(D_800F33E0->+8). For each non-null particle it writes rec+0xC to +2, (rand&3)+0xC to +6 and rec+8 to +4, then does rec+8 += 0x8AA + (rand&0x1F). It returns D_800E27EC >= 0x20.
- mode 2: D_800F3374 = 0x40. When D_800E27EC < 13 (signed), s0 = func_80077CF4((D_800E27EC<<10)/12), rec+0xC = s0/8 (signed), func_800CF3AC(D_800E166C, &buf, D_800E27EC), then func_800D0728 with 11 arguments. It then sets D_800F3374 = 0x40 again, loads the GTE rotation (ctc2 0-4) and translation (ctc2 5-7) from *D_800BCFA4, calls func_800CF5B0(rec, 0) and returns 0.
- Any other mode returns 0.
Layout point: retail keeps the 3-short vector at sp+0x30 and the 4-byte buffer (a word copy of D_800C22C4) at sp+0x38, adjacent. func_800D0728's first argument points at the vector, and the differential reads 32 bytes through it. The first attempt used separate locals and mismatched because the buffer was not adjacent. The C now uses `struct { GteShortVector v; B4 buf; } L` so the layout is identical.
Retail stack behaviour: func_800D0728 receives &sp+0x30 and can read sp+0x36 (the pad halfword, never written) and sp+0x3C..0x4F (unwritten stack, the saved-s0 slot and beyond). That is retail behaviour. The differential only compares the bytes retail wrote (0-5 and 8-11).
Nothing else is hidden: there are no float or static constructs, and the only inline asm is the Psy-Q-style SetRot/SetTrans macros ($12-$14 clobbered). D_800BCFA4 is declared as an array of MATRIX pointers and element 0 is used, as in room_m0429i/func_801982A4.c. The first e21 run took over 120 s, mostly the first-time compile at JOBS=2, so allow for that. Only the deliverable file was created or edited. Scratch lives in /dev/shm/pe-native-34f506ce/work/exe_func_800D5A00. I did not call the advisor before finishing.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py exe func_800D5A00 --nonmatching C6200:2E4 --check-gte --uncovered --seeds 288 --inputs "a0=0,1,2,3;w(0x800E27EC)=0,4,10,11,12,13,0x1F,0x20,0xFFFFFFF9"`
  - e21_leaf: ok func_800D5A00 (exe) 262 seeds, retail coverage 183/185 words | e21_leaf: uncovered 800D5A5C 800D5A60
- `python3 tools/analysis/e21_leaf.py exe func_800D5A00 --nonmatching C6200:2E4 --check-gte --uncovered --seeds 8 --inputs "a0=0xFFFFFFFF,0x80000000"`
  - e21_leaf: ok func_800D5A00 (exe) 8 seeds, retail coverage 33/185 words (uncovered list is everything else: 800D5A64..800D5CC0, as expected for a negative-a0-only run; 5A5C/5A60 are not listed so they ran)
- `python3 tools/analysis/e21_leaf.py exe func_800D5A00 --nonmatching C6200:2E4 --check-gte --uncovered --seeds 60 --inputs "a0=2;w(0x800E27EC)=0xFFFFFFFF,0x80000000,0x7FFFFFFF,0,1,11,12,13,14,0xFFFFFFF4,0x200000"`
  - e21_leaf: ok func_800D5A00 (exe) 60 seeds, retail coverage 107/185 words (mode 2 with D_800E27EC edge values incl. negative, INT_MIN, INT_MAX, 12/13 boundary)
- `python3 tools/analysis/e21_leaf.py exe func_800D5A00 --nonmatching C6200:2E4 --check-gte --uncovered --seeds 60 --inputs "a0=1;w(0x800E27EC)=3,4,10,11,0x7FFFFFFF,0x80000000,0xFFFFFFFF,0x1E,0x1F,0x20,0x21,0xFFFFFFFC"`
  - e21_leaf: ok func_800D5A00 (exe) 57 seeds, retail coverage 69/185 words (mode 1 with D_800E27EC edge values incl. 3/4/10/11 boundary, INT_MIN/MAX, -1, 0x1F/0x20 boundary)

No MISMATCH, notest or skipped line in any of the four runs. Side-by-side read of the asm and C found no discrepancy. Mode dispatch is signed (slti/beq), so a0 = 0x80000000 or 0xFFFFFFFF reaches 800D5A5C, which returns 0; the C does the same (mode < 2 and mode != 0 gives return 0). The sltiu range check (D_800E27EC - 4 < 7) matches the unsigned cast. The slti 0x20 return and slti 0xD branch are signed in both. The /12 division (mult by 0x2AAAAAAB, mfhi, sra 1, minus sign bit) matches the C signed division, including negative values. The s0/8 rounding (add 7 if negative, then sra 3) matches the C signed /8. In mode 2, D_800F3374 is written twice (0 from the delay slot, then 0x40) and the C only keeps the final 0x40 store, which is not observable through the final RAM. All GTE register writes (ctc2 0..7) are reproduced. Two things I could not check. The e21 harness reports only totals for a2/a3 and stack-argument comparison, so I cannot say whether func_800D0728's 11 arguments (stack args at sp+0x10..0x28) are compared individually. The 800D5A5C/800D5A60 coverage is shown only indirectly, through the second run's uncovered list. Neither is a mismatch. The stated uncovered justification (needs a negative or INT_MIN a0) is correct and reachable via the supplementary run, so all 185 words are executed across the runs.

