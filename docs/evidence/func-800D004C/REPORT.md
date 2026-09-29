# func_800D004C — EXE, inline GTE

- VRAM 0x800D004C, file offset 0xC084C, size 0x6DC. On the Day 1 route (`route_coverage.py` fan-in 29).
- Role: shaded fan. It draws n POLY_G3 triangles around the centre with alternating radii, each with an optional DR_MODE for semi-transparency. The pc_port shape is `PE_EffectFanD004C`.
- Source: `src/func_800D004C.c`, hand-written in this repo (khasinski's vendored tree has it as asm only).
  It carries the `inline GTE/asm` tag: Psy-Q `inline_c.h`-shaped macros. Count it separately
  from plain C.
- Profile: `era_o2_g0_expand_div`. `MASPSX_EXPAND_DIV=1` is load-bearing.
- **Split-artefact merge**: the yaml rows `[0xC084C, asm]` (a single `addiu $sp,$sp,-0xF0`) and `[0xC0850, asm]` (which the splitter named `func_800D0050`) were one function. They are now the single row `[0xC084C, c, func_800D004C]`. With the old rows, route_coverage counted `func_800D004C` as 1 word.

## Verification

```
$ python3 tools/analysis/era_link_check.py src/func_800D004C.c 0x800D004C 0x6DC (MASPSX_EXPAND_DIV=1)
word mismatches=0, LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --only func_800D004C
disc1_preflight: PASS (deep, 2246 c / 141 asm / 2 rodata)
$ python3 tools/analysis/leaf_strong_check.py func_800D004C        -> LEAF_STRONG=EXACT
$ python3 tools/analysis/profile_necessity.py --only func_800D004C -> PROFILE_NECESSITY=PASS
$ python3 tools/build/disc1_plan.py --write-status && scripts/exact_rebuild.sh
VERIFY_SWEEP=PASS leaves=2246 plan=7dc9bcb473ab3fe1fce4396ecc712e7c1dd9d48af35f5e2e443d7bd4317765f9 yaml=19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d
EXACT_REBUILD_GATE=PASS plan=7dc9bcb473ab3fe1fce4396ecc712e7c1dd9d48af35f5e2e443d7bd4317765f9 yaml=19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d spans=[2246 c, 141 asm, 2 rodata] sha1_orig=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1_cand=452fb033f2eaa4b18aa20a5bca60b8125af3a37b sha1=452fb033f2eaa4b18aa20a5bca60b8125af3a37b
```

That gate (batch 1 of the EXE route bodies) covers func_800CEE20, func_800D0728 and
func_800D004C together. `sha256(configs/USA/disc1.yaml)` = `19d9c88d72a7cb6c2760be85838259435e1bda58f1efb9289c0abb53b7ba6a1d`.

## Levers

- Adapted from func_800D0728 (the same prologue and colour/matrix set-up; ScaleMatrix comes before the optional 786E4 camera rotation).
- `radii[2]` on the stack, and the single chain `v[0].vx = v[0].vy = v[0].vz = v[1].vz = v[2].vz = 0` (5 → 0).

## Prior note (kept for history, now resolved by the row merge above)

### func_800D004C

VRAM `0x800D004C`, declared size `0x4` (one word), `asm/disc1/C084C.s`.

The span is the single instruction `addiu $sp, $sp, -0xF0`. The next YAML asm span, at `0xC0850`, is labeled `func_800D0050` (`0x6D8`, `asm/disc1/C0850.s`) and starts with `sw $s3, 0xD4($sp)`. That save is inside the `0xF0` frame. Its epilogue is `addiu $sp, $sp, 0xF0` / `jr $ra`. Callers `jal func_800D004C` (`asm/disc1/CE8A8.s`, `asm/disc1/C5A90.s`). `func_800D0050` has no reference outside its own file.

The two labels are one function that splat split after the stack adjustment. A C body for the declared `0x4` span cannot match: cc1 will not emit a lone `addiu` with no `jr`, and a full function's linked `.text` is not a 4-byte zero tail. It stays asm until the two spans are joined. Not integrated.
