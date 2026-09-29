# func_800D0728 — EXE, inline GTE

- VRAM 0x800D0728, file offset 0xC0F28, size 0x760. On the Day 1 route (`route_coverage.py` fan-in 30).
- Role: shaded ring. It draws n POLY_G4 annulus segments (inner/outer radius, rcos/rsin via func_80077DC4/func_80077CF4), each with an optional DR_MODE for semi-transparency. The pc_port shape is `PE_EffectRingD0728`.
- Source: `src/func_800D0728.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0_expand_div`. `MASPSX_EXPAND_DIV=1` is load-bearing (the `(i << 12) / n` angle divides).

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D0728.c 0x800D0728 0x760 (MASPSX_EXPAND_DIV=1)
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D0728
disc1_preflight: PASS (deep, 2246 c / 141 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D0728        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D0728 -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2246 plan=7dc9bcb473ab3fe1fce4396ecc712e7c1dd9d48af35f5e2e443d7bd4317765f9 yaml=19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d
EXACT_REBUILD_GATE=PASS plan=7dc9bcb473ab3fe1fce4396ecc712e7c1dd9d48af35f5e2e443d7bd4317765f9 yaml=19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d spans=[2246 c, 141 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 1 of the EXE route bodies) covers func_800CEE20, func_800D0728 and
func_800D004C together. `sha256(configs/USA/disc1.yaml)` = `19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d`.

## Levers

- `D_800B0E38` as the `RenderBufferPrefix` struct: 268 → 1. This fixes the frame size as well as the ordering/packets bases.
- `ot = (unsigned int *)((otz << 2) + (int)ordering[bank])` gives retail's `addu $s0,$v0(shift),$v1(base)` operand order: 1 → 0.
