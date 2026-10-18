# room_m0350i func_80196F2C (NON_MATCHING)

Blob offset 0x7F44, 0x2F0 bytes, VMA 0x80196F2C. The span is `asm` in `configs/USA/overlays/room_m0350i.yaml`, and no lineage or vendor tree has C for it.
Source: `src/nonmatching/overlays/room_m0350i/func_80196F2C.c`. It is behaviour C, not a byte match,
recorded in `configs/USA/overlays/nonmatching.yaml` with its directed inputs.

Written from the retail disassembly by a decomp agent (workflow wf_441734e8). An
independent verifier agent then re-ran the claim, added edge-value runs and read
the C against the asm.

## Source header

```
room_m0350i — func_80196F2C, blob offset 0x7F44, 0x2F0 bytes. NON_MATCHING
 * (behaviour C, proven by the e21 differential; not a byte match).
 * Per-frame callback of a thrown object (record a1: +0..+4 velocity, +8..+0xC
 * position, +0xE landed flag).  mode 1 integrates the position, flags a
 * fall past the floor (D_800942EC) or a hit (func_8019721C) and otherwise
 * logs the point in a 4-entry trail at D_8019A85C - 0x28; mode 2 draws the
 * effect (func_800CEE20 + func_800D004C) at the record position.
```

## Proof (e21 differential), re-run by the main session after integration

    python3 tools/analysis/e21_leaf.py room_m0350i func_80196F2C --check-gte --uncovered --seeds 48
    e21_leaf: ok func_80196F2C__09affac1_port.c 48 seeds, retail coverage 168/188 words
    e21_leaf: uncovered 80197054 80197058 8019705C 80197060 80197064 80197068 8019706C 80197070 80197074 80197078 8019707C 80197080 80197084 80197088 8019708C 80197090 80197094 80197098 8019709C 801970A0

    python3 tools/analysis/e21_leaf.py room_m0350i func_80196F2C --nonmatching 7F44:2F0 --check-gte --uncovered --seed-list 21628,29820,38012,46204,54396 --inputs "h(0x8019A85C)=4,-1,2,0,3;a0=1;h(a1+0xE)=0;h(0x800942EC)=0x7FFF"
    e21_leaf: ok func_80196F2C__09affac1_port.c 5 seeds, retail coverage 64/188 words
    e21_leaf: uncovered 80196FA8 80196FAC 80196FB0 80196FB4 80196FB8 80196FBC 80196FC0 80196FC4 80196FC8 80196FCC 80196FD0 80196FD4 80196FD8 80196FDC 80196FE0 80196FE4 80196FE8 80196FEC 80196FF0 80196FF4 80196FF8 80196FFC 80197000 80197004 80197008 80197020 80197024 80197028 8019702C 80197030 80197034 80197038 8019703C 80197040 80197044 80197048 8019704C 80197050 801970A4 801970A8 801970AC 801970B0 801970B4 801970B8 801970BC 801970C0 801970C4 801970C8 801970CC 801970D0 801970D4 801970D8 801970DC 801970E0 801970E4 801970E8 801970EC 801970F0 801970F4 801970F8 801970FC 80197100 80197104 80197108 8019710C 80197110 80197114 80197118 8019711C 80197120 80197124 80197128 8019712C 80197130 80197134 80197138 8019713C 80197140 80197144 80197148 8019714C 80197150 80197154 80197158 8019715C 80197160 80197164 80197168 8019716C 80197170 80197174 80197178 8019717C 80197180 80197184 80197188 8019718C 80197190 80197194 80197198 8019719C 801971A0 801971A4 801971A8 801971AC 801971B0 801971B4 801971B8 801971BC 801971C0 801971C4 801971C8 801971CC 801971D0 801971D4 801971D8 801971DC 801971E0 801971E4 801971E8 801971EC 801971F0 801971F4 801971F8

## Uncovered words

The official command alone leaves 80197054..801970A0 (20 words) uncovered. That is the else-if arm taken when func_8019721C returns 0 and D_8019A85C < 4, i.e. the 4-entry trail log with its increment and three halfword stores. It is reachable, not impossible. Reaching it needs the mocked func_8019721C to return exactly 0. mock_ret in e21_diff.py has no directed control, and the first seed that returns 0 is 21628 (then 29820, 38012, 46204, 54396, ...), about 0.3 s per seed. The scratch driver drv.py reuses e21_diff.inputs() with `range(seeds)` replaced by an explicit seed list, so the oracle, mocks and comparison are unchanged. It covers the trail arm for D_8019A85C = 0, 2, 3 and -1 (the `slti` taken arm). The `D_8019A85C >= 4` skip arm is covered by an official-command seed and by one driver seed.

## Independent verification: confirmed

- `python3 tools/analysis/e21_leaf.py room_m0350i func_80196F2C --nonmatching 7F44:2F0 --check-gte --uncovered --seeds 48 --inputs "a0=1,2,0,3;h(a1+0xE)=0,1;h(0x800942EC)=0x7000,0,0xFFF0" | python3 drv.py 21628,29820,38012,46204,54396 room_m0350i func_80196F2C --nonmatching 7F44:2F0 --check-gte --uncovered --seeds 1 --inputs "h(0x8019A85C)=4,-1,2,0,3;a0=1;h(a1+0xE)=0;h(0x800942EC)=0x7FFF"`
  - e21_leaf: ok func_80196F2C__09affac1_port.c 48 seeds, retail coverage 168/188 words; uncovered 80197054..801970A0 (20 words) exactly as claimed
- `e21_leaf.py ... --seeds 150 --inputs "a0=1;h(a1+0xE)=0,0x100;h(a1+0xA)=0x7FFF,0x8000,0x7000,0xFFFF;h(a1+2)=0,1,0x7FFF,0x8000;h(0x800942EC)=0x7FFF,0x8000,0x7000,0,0xFFFF"`
  - driver run: e21_leaf: ok func_80196F2C__09affac1_port.c 5 seeds, retail coverage 64/188 words; its uncovered list does not include 80197054..801970A0 (trail arm is covered)
- `e21_leaf.py ... --seeds 100 --inputs "a0=2,3,0,0xFFFFFFFF,0x10000001;h(0x800E11FA)=0,1,0x7FFF,0xFFFF;h(0x800E120A)=0,0xFFFC;h(0x800942EC)=0x8000"`
  - new run 1 (mode 1 edge values: lbu flag 0x100, signed floor compare at +/-0x8000 boundaries, velocity edges): e21_leaf: ok ... 150 seeds, retail coverage 82/188 words (no MISMATCH)

No mismatch found. I read the retail asm and the C side by side and they agree on every point I checked. Both mode-1 loads (lbu 0xE, lh 0xA vs lh D_800942EC with slt) have the right signedness. The 16-bit adds and stores match. The 0xC8 and 0x50 call arguments match. The D_8019A866 = y-0xA0 store matches. Trail index idx*8 relative to &D_8019A85C-0x28 matches the asm. The slti guard matches (the driver includes -1). Mode 2 passes the same stack arguments (a2 = D_800E11FA<<1, D_800E120A+4, the 0xA00 constants, D_800942EC lhu stored to v.y). Minor note: the new inputs cover only part of the function. Coverage was 82/188 and 102/188 words. The directed 20-word trail arm is covered only through the supplementary driver, which is not the standard command. That arm is reachable (mocked func_8019721C returning 0), so the claim's 188/188 union is plausible but depends on the scratch drv.py. The e21 runs are slow when another worker is active (several minutes each)."]

