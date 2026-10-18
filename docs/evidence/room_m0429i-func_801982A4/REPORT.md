# room_m0429i func_801982A4 (NON_MATCHING)

Blob offset 0x92BC, 0x670 bytes, VMA 0x801982A4. It is inside the room's asm
span `3FB4.s`, and no lineage or vendor tree has C for it. Source:
`src/nonmatching/overlays/room_m0429i/func_801982A4.c`. That is behaviour C,
not a byte match, recorded in `configs/USA/overlays/nonmatching.yaml` with
its directed inputs.

The function was written from the retail disassembly, starting from the near
twin room_m0269i func_8019070C (instruction-class similarity 0.92). The
differences are listed in the source header.

## Proof (e21 differential)

    python3 tools/analysis/e21_leaf.py room_m0429i func_801982A4 --check-gte --coverage --seeds 96
    e21_leaf: ok func_801982A4__1e52d605_port.c 91 seeds, retail coverage 401/412 words

These inputs come from `nonmatching.yaml`:

    a0=1,2; h(a1+0x10)=0,1,2; h(a1+0x12)=0,11,12,24; h(0x800F336C)=4,0; w(0x800F3428)=1,0; out=800783E4:4

The 11 uncovered words fall in two groups:
- 801982FC/80198300 (mode not 1 or 2), 80198324-8019833C (cmd 1 state below
  0 or above 2) and 8019848C-801984A0 (cmd 2 state below 0 or above 2). Each
  is a `return 0` arm.
- 8019864C, the `bgez`/`addiu 0xF` rounding of a negative `f * 1204`.

A supplementary run executes those arms:

    python3 tools/analysis/e21_leaf.py room_m0429i func_801982A4 --nonmatching 92BC:670 --check-gte --coverage --seeds 18 \
        --inputs "a0=0,1,2; h(a1+0x10)=0xFFFF,3,1; h(a1+0x12)=0xFFF0,5; out=800783E4:4"
    e21_leaf: ok func_801982A4__1e52d605_port.c 18 seeds, retail coverage 173/412 words

The first directed run found a real lowering bug. A direct call through the
leaf's own `short` prototype (`func_800CFB7C(void *, short, void *)`) passed
0x0007A24A where retail's `sll 4; sra 16` passes 0xFFFFA24A. guest_lower
now converts narrow prototype arguments on direct calls too.
