# room_m0429i func_801978EC (NON_MATCHING)

Blob offset 0x8904, 0x9B8 bytes, VMA 0x801978EC. The span is `asm` in `configs/USA/overlays/room_m0429i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0429i/func_801978EC.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_9660d38d). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0429i — func_801978EC, blob offset 0x8904, 0x9B8 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Effect callback, modes 0/1/2 on a record a1 (state +8, frame +0xA, scale +0xC),
 * read from the retail disassembly. Mode 0 clears state/frame/scale, arms the
 * D_800F32D0 object byte, plays two sounds via D_800B0E64, registers the
 * func_801972EC child through func_800CE560. Mode 1 steps the record: state 0
 * (16 frames) ramps +0xC by func_80077CF4, state 1 (32 frames) spawns
 * particles from the func_800CE610 pool, state 2 (24 frames) fades +0xC with
 * func_80077DC4 and, on frame 1, flips the D_800F32D0 object's flag bits.
 * Mode 2 loads the GTE rotation/translation from D_800BCFA4[0], sets the
 * D_800F3368 draw block and draws two func_800CEE20 sprites plus the
 * func_800D004C / func_800D0728 glows when the +0xC scale is non-zero.
 * Retail frame 0x30..0x57 (buf, then the four short vectors at 0x38/0x40/0x48/
 * 0x50) is reproduced as one struct so the stack-pointer arguments carry the
 * same neighbouring bytes.
 * inline GTE/asm: Psy-Q inline_c.h-shaped GTE (COP2) SetRot/SetTrans macros.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0429i func_801978EC --check-gte --uncovered --seeds 192
    e21_leaf: ok func_801978EC__13217426_port.c 1096 seeds, retail coverage 615/622 words
    e21_leaf: uncovered 801980A8 801980AC 801980B0 801980B4 801980B8 801980BC 801980C0

## Uncovered words

801980A8-801980C0 (7 words: lui/lw D_800F3428, beqz, j, and the addiu a1,a1,6 delay slot): the "+6" CLUT arm taken when D_800F336C == 4 && D_800F3428 != 0. Provably unreachable: mode 2 stores 3 to D_800F336C at 80197FB4 and reads it back (lhu at 80198070) with no call or other store to it in between, so the compare against 4 can never be true. Kept in the C for fidelity. Also an untaken edge, not an uncovered word: the second 'D_800B0E64 != 0' re-check at 80197A10 (mocks never write RAM, so it cannot go to zero); its target 80197A2C is covered by fall-through. Run A printed 191 seeds of 192: seed 129 was dropped by an oracle-side fault ('delay-slot op 000F0001', seen with E21_FAULTS=1), i.e. the retail oracle ran garbage, most likely because a random a1 pointer overlapped code and the halfword writes corrupted it. That is not a native divergence, and the same state/frame combinations are covered by other seeds.

## Independent verification: confirmed

- `Re-ran M0, A, B, C, D exactly as claimed (python3 tools/analysis/e21_leaf.py room_m0429i func_801978EC --nonmatching 8904:9B8 --check-gte --uncovered --seeds <32|192|32|12|2> --inputs <spec>)`
  - M0: e21_leaf: ok func_801978EC__13217426_port.c 32 seeds, retail coverage 100/622 words
- `N1: --seeds 25 --inputs "a0=1;h(a1+8)=0;h(a1+0xA)=0x7FFF,0xFFFF,0xF,0x10,0x200;r(0x80077CF4)=-1,-33,31,-32,0x7FFF;<standard w() setup>"`
  - A: e21_leaf: ok func_801978EC__13217426_port.c 191 seeds, retail coverage 255/622 words
- `N2: --seeds 60 --inputs "a0=1;h(a1+8)=2;h(a1+0xA)=0,0x17,0x18,0x7FFF,0xFFFF;r(0x80077DC4)=-1,-33,31,-32;w(0x80133000)=0x01000000,0x41000000,0x02000000;h(0x8013000C)=0x0100;<standard w() setup>"`
  - B: e21_leaf: ok func_801978EC__13217426_port.c 32 seeds, retail coverage 115/622 words
- `N3: --seeds 72 --inputs "a0=2;h(a1+0xC)=1,-1,0x7FFF,-0x8000,15,16;h(a1+0xA)=0,1;w(0x800E27EC)=0,1,2,3,-1,0x10000000"`
  - C: e21_leaf: ok func_801978EC__13217426_port.c 12 seeds, retail coverage 242/622 words
- `Read 3FB4.s lines 4928-5578 against func_801978EC.c; inspected 80197FA0-801980C8`
  - D: e21_leaf: ok func_801978EC__13217426_port.c 2 seeds, retail coverage 39/622 words

No MISMATCH, notest or skipped line in any of the 8 runs. The claimed numbers for all five original runs reproduce exactly (including A at 191 seeds, not 192).

New edge specs: N1 (state 0, frame wrap at 0x7FFF and 0xFFFF, boundary 0xF/0x10, negative and rounding values from func_80077CF4), N2 (state 2, frame 0x17/0x18/wrap, negative values from func_80077DC4, flag mask 0x3F000000 variants) and N3 (mode 2, scale and frame edges, D_800E27EC low-bit and large values). All three matched retail. They added no coverage beyond the claimed union.

I could not refute the uncovered-word justification for 801980A8-801980C0. The only way to reach them is the compare against 4 at 801980A0. D_800F336C is stored as 3 at 80197FB4/80197FB8 and reloaded at 80198070 (lhu). Between those, the stores in the block go to D_800F336E/3372/3370/3374 and to 0x40-0x46($sp), and the only labels are .L80198014 and .L80198044, both inside the block. A write to D_800F336C would need $sp to point at 0x800F332x, which is not a real state. Mode 2 has no other entry between those addresses. The arm is dead.

Minor notes. C follows the asm there: the `+= 6` arm is kept for fidelity and the lhu type of D_800F336C is honoured. The dropped seed 129 in run A is a harness-side oracle fault and not a native divergence. I did not re-verify seed 129 itself. The same state and frame combinations appear in other seeds, since N1 and N2 re-cover those combinations with a different seed set."

