# ovl12 lane — subsystem overlays (2026-09-28)

Scope: ovl_039F, ovl_03C5, ovl_03C9, ovl_03D2, ovl_0457, ovl_0700 (rooms belong to ovl11).
Records: `build/lanes/ovl12/results.jsonl`.

## ovl_03D2 func_801924F8 (0x801924F8, blob 0x3508, 0x43C) — era_o2_g0 (default) — ovl6/ovl8/ovl10 PARK resolved

Movie-open routine (id bound check, D_801D0E00 entry select, path build, CD open/seek retry,
Mov state init, first-frame wait loop). Started from ovl10's 62-word park draft.

Mismatch ladder (link-level word mismatches, `tools/analysis/room_lane/ldiff.py`):
* 62 (ovl10 park) → 61 (hill-climb of the Mov init-block statement order).
* `D_801D1494` store through a **non-volatile** declaration: removes the `li $s0,1` misplacement
  (retail fills the `jal func_8010C0D8` delay slot with it; with the volatile store sched put it
  before `jal func_8010BE3C`). The body became one word shorter (a retail load-delay `nop` is then
  the only length residual) — word count jumps because of the shift, diff lines 37 → 55 (shift).
* **int parameter + pinned `$v1` copy for the byte store**: `int func_801924F8(int id)`,
  `register int cp asm("$3"); cp = id;` before the bound check, `D_800B0DBF = cp;`,
  index `&D_801D0E00[(short)id]`. sched1 already put the `sb` at the block head; sched2 (after
  reload) moved it down because the copy lived in `$a1` with no hard-reg dependence. With the copy
  in `$v1` the `sll/sra` chain writing `$v1` holds the `sb` above it, and `andi`/`sll` read `$a0`
  directly like retail (a `short` param keeps a HImode pseudo that `andi` reads) → 180w / 47 lines
  (only the init block left).
* Init block re-hill-climbed from the new base: 8 → 3 → **0**. Final order: `rc.x = e->wA;
  rc.y = e->wC; ent[0].y = rc.y + 0xF0; tabA[0]; curA = 0; tabA[1]; tabB[0]; tabB[1]; curB = 0;
  ent[0].x; idx; ent[1].y; ent[1].x`.

Diagnosis aid: `cc1 -dS` (sched1 dump) prints per-block ready lists / priorities (`7f000001` =
birthing boost) and LOG_LINKS; it showed sched1 already had the retail `sb` position, so the
residual was sched2 / hard-register allocation.

    disc1_preflight: PASS (deep, 26 c / 2 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=ffbb87472d69f0187f1b6d1f5175102e9bad9c5c13fd0cbbb9f0efd5b0e6541b yaml=7cf49dbec1fe52a188ca115111de4712ec83101d2af53f06c244cc96c87ec39c spans=[26 c, 2 asm, 0 rodata, 2 bin] sha1_orig=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1_cand=a0b604ead810592dd9a8d74f7c74fe94151f4d32 sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32

Commands: `python3 tools/build/disc1_preflight.py --target ovl_03D2 --deep --only func_801924F8`;
`flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh ovl_03D2`.

## ovl_03C5 func_80121C04 (0x80121C04, blob 0xF04, 0x43C) — era_o2_g0 — port

Twin of ovl_03D2 func_801924F8: the two retail listings (285 lines each) are identical after
masking symbols and labels. Symbol map derived line-by-line from the two disassemblies
(D_801D0DC4→D_801223FC, D_801D1464→D_801228CC, D_801D11AC→D_801227E4, D_801D0E00→D_80122438,
D_8018F2E4/2EC→D_80120FF4/FFC, func_801918F8/91B64/91DC8→func_80121004/21270/214D4, …); EXE
symbols unchanged. First compile: 0 word mismatches.

    disc1_preflight: PASS (deep, 9 c / 1 asm / 0 rodata)
    EXACT_REBUILD_GATE[ovl_03C5]=PASS plan=1af819adc1473a2b2ef2a3484366b783599fcec515fea2f9c06d28624ac0f195 yaml=5d944dde8826714093b01d25af601f91f6a8ab63bfc9a5f94848ab3708d5170d spans=[9 c, 1 asm, 0 rodata, 2 bin] sha1_orig=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1_cand=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736 sha1=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736
