# Overlay lane — PE.IMG overlays as exact-rebuild targets

Status: infrastructure landed 2026-09-22 (session "overlay lane"). This
document is the working reference for adding overlays; the survey that
specified it is summarised in §1 and the commands in §4 are the evidence.

## 1. What an overlay is here

* The only overlay-bearing disc file is `PE.IMG` (206,213,120 B = 100,690
  sectors, SHA-1 `146c0ce7308bf9fdc2ba5a84230e198db0663f3b`, byte-identical
  on both discs, LBA 1013). `scripts/extract_us.sh` does not extract it;
  `scripts/extract_overlays.sh` does (from an already extracted copy, or from
  the disc image under `rom/image/` via `tools/extract/psxiso.py`).
* The EXE describes the layout in three tables, all read by
  `tools/extract/peimg.py tables`:
  * `D_80093150` — 21 halfwords, PE.IMG sector boundaries of the image-load
    ranges (`0x328 … 0x8B0`);
  * `D_8001160C` — 4 words, the fixed load addresses
    `0x8010BCF8, 0x80120D00, 0x8018EFF0, 0x801ED7F8`;
  * `D_80093378` — 438 × `{u32 rel, u32 packed}` room packages,
    `packed = sec0 | sec1<<8 | sec2<<20`, base LBA `D_800B0DD8 = 1013`,
    three contiguous chunks; chunk 2 (the MIPS-bearing chunk) loads at
    `0x8018EFE8`.
* Six image-load ranges carry MIPS code (the "subsystem overlays"): they are
  position-dependent binaries with **no PS-X header and no relocations**, so
  each is exactly one splat `code` segment at a fixed `vram`. They use **zero
  `$gp`-relative accesses** (EXE: 3,128) and the same Psy-Q GCC 2.7.x
  fingerprint (`addu rd,rs,$zero` moves), so the era default profile
  `era_o2_g0` is the right first rung and the `-G8` machinery is unneeded.
* **One arena, many overlays.** `0x8018EFF0` hosts `ovl_03D2` (title),
  `ovl_0700` (map-exit chooser) *and*, at `0x8018EFE8`, every room chunk 2;
  `0x80120D00` hosts both `ovl_03C5` and `ovl_0457`. A `func_801xxxxx` name is
  therefore only unique **within one overlay**.

## 2. What exists

| Piece | Path | Role |
| --- | --- | --- |
| Manifest | `configs/USA/overlays/manifest.yaml` | committed authority: sector ranges, VRAM, sizes, SHA-1s, text windows — hashes/geometry only, never bytes |
| Extractor | `tools/extract/peimg.py`, `scripts/extract_overlays.sh` | `tables` / `extract` / `check` / `room`; idempotent; every blob is SHA-1-checked against the manifest; output under git-ignored `build/extracted/disc1/peimg/` with `hashes.txt` |
| Target descriptors | `tools/build/overlay_targets.py` | `Target` dataclass, `MAIN_TARGET` (the EXE, historical geometry), stdlib YAML-subset reader for the manifest, `load_target(id)` |
| Plan / build / verify / preflight | `tools/build/disc1_{plan,build,verify,preflight}.py --target <id>` | parametrized over a `Target` instead of forked; default = EXE, unchanged |
| Strong leaf check | `tools/analysis/leaf_strong_check.py` | uses the plan unit's `start` as the retail file offset (EXE: identical to the old `vram-0x80010000+0x800`) |
| Weak leaf check | `tools/analysis/era_link_check.py --target <id> <src> <vram> <size> [flags]` | compares against the overlay blob |
| Split | `scripts/split_overlay.sh <id> [--check]` | same guards as `split_us.sh`; output `asm/overlays/<id>/`, `assets/overlays/<id>/`, `linkers/overlays/<id>.ld`; the overlay's `undefined_*_auto.txt` and `.splache` live under `asm/overlays/<id>/` so an overlay split never overwrites the EXE's |
| Gate | `scripts/exact_rebuild_overlay.sh <id>` | preflight → split → build → verify; prints `EXACT_REBUILD_GATE[<id>]=PASS …` |
| First config | `configs/USA/overlays/ovl_0700.yaml` + `ovl_0700_build_profiles.json` | `[0x0,bin) [0x6C,asm) … [0xCFD4,bin) [0x5B800]`; profiles inherit the EXE definitions via `profiles_from` |
| Tests | `tools/build/test_overlay_plan.py` | manifest reader, overlay plan geometry/paths/profiles, preflight per target, EXE invariance (no game data needed) |

### 2.1 Target descriptor

`Target(id, config, profiles, retail, file_start, file_end, vram, has_header,
src_dir, asm_dir, asset_dir, build_dir, candidate, generated_dir,
expected_sha1, derive_symbol_prefixes)`.

* `MAIN_TARGET`: `configs/USA/disc1.yaml`, `0x800 … 0x1EE800`, VRAM
  `0x80010000`, PS-X header, `src/`, `asm/disc1/`, `build/disc1.candidate.exe`.
  The plan hash of a given YAML is **unchanged**: the descriptor is attached
  to the plan (`plan["target"]`) *after* `plan_sha256` is computed, and
  `render_linker_script` / `verification_manifest` / `matching_status` render
  byte-identical output (proven against the HEAD module, §4).
* Overlay: `file_start = 0`, `file_end = size`, no header, `src/overlays/<id>/`,
  `asm/overlays/<id>/`, `assets/overlays/<id>/`, `build/overlays/<id>/…`,
  `derive_symbol_prefixes = (D_, jtbl_, func_)` — the probe link binds the
  overlay's calls into the EXE (`func_8001xxxx`) by their address-named form,
  exactly as the EXE lane binds `D_` externs (the EXE keeps its `D_`-only rule).

### 2.2 New span kind: `bin`

A `bin` row is a raw byte span (splat `bin` subsegment →
`assets/overlays/<id>/<start hex>.bin`). The build wraps it in a generated
`.incbin` unit (`build/asm/overlays/<id>/<start hex>.bin.s` → `.o`, section
`.data`), so container headers and data tails round-trip verbatim with no
symbolisation noise. The EXE YAML uses no `bin` rows; `counts.bin` is only
added to a plan when non-zero, so the EXE plan's key set is untouched.

### 2.3 Build profiles

An overlay's `*_build_profiles.json` may carry
`"profiles_from": "configs/USA/disc1_build_profiles.json"` and then owns only
`assignments` (and optionally `default_profile`). The profile *definitions*
(era flags, maspsx knobs) live once, in the EXE manifest; assignments must
name C spans of that overlay's YAML (the stale-override rule is per target).

## 3. Naming and namespace rule

* Every overlay function is `func_<VRAM>` and lives in
  `src/overlays/<id>/func_<VRAM>.c`. The directory is the namespace.
* Two overlays are never linked together, and the preflight `geometry` rule
  ("a C symbol's name encodes its own mapped VRAM") holds **per target**
  (`vram = target.vram + offset - target.file_start`).
* `src/overlays/room_common/` is the planned home for room bodies shared by
  ≥ 2 rooms (`func_<VRAM-in-first-room>.c`); it is deliberately **not
  populated** yet. A shared body may only be re-linked into another room's
  plan once its masked-body identity is proven (compile once, link per room,
  byte-exact in each) — a `shared:` YAML entry and that proof are the next
  infrastructure step for tier 4 below.
* The EXE lane's residual/disposition ledgers (`MATCHING_RESIDUAL_POLICY.md`,
  `disc1_nonmatching_sources.json`) are EXE-only; for an overlay, `verify`
  instead requires that every tracked `src/overlays/<id>/func_*.c` is a YAML
  C span.

## 4. Evidence (2026-09-22, this worktree)

Main-EXE invariance (same YAML/profiles/src as main, `src/func_8008068C.c`
md5 `8f72602034625991dff65790e837258d`):

```
$ python3 tools/build/disc1_plan.py --check          # before and after
disc1 plan: 1783 spans (1396 c, 385 asm, 2 rodata), geometry=0x1EE000, plan=b11ae663c06b
$ python3 tools/build/disc1_preflight.py --deep      # before: exit 0, after: exit 0
disc1_preflight: PASS (deep, 1396 c / 385 asm / 2 rodata)
HEAD-module comparison: plan hash equal, linker script identical, verify
manifest identical, status identical, plan dict identical except the
appended `target` key.
```

Overlay extraction and round trip:

```
$ scripts/extract_overlays.sh
wrote build/extracted/disc1/peimg/ovl_0700.bin sectors [0x700,0x7B7) size 0x5B800 vram 0x8018EFF0 sha1 9f8786a9d6f8b11548a14680d34fc540d8e909de
(… ovl_039F e7331867…, ovl_03C5 f8f6ea38…, ovl_03C9 1d0e972c…, ovl_03D2 a0b604ea…, ovl_0457 08b5c119…)
$ scripts/split_overlay.sh ovl_0700 && scripts/exact_rebuild_overlay.sh ovl_0700      # all-asm config
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=a2cc32d19c7f9087e86bfdf506ee0844b6fcfd14bd558f2ad0cff48335da96ef yaml=5f9f90324c64be0b63c1fa2a7ca2b2a0ab62aa2d564d82c81311af8a3b2f070b spans=[0 c, 1 asm, 0 rodata] sha1_orig=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1_cand=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```

First overlay leaf, `func_80191740` (blob `0x2750`, `0x14`, `D_8019CC50 = 200`
as a 16-bit store), registered in `ovl_0700.yaml`:

```
$ LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu \
  python3 tools/analysis/era_link_check.py --target ovl_0700 src/overlays/ovl_0700/func_80191740.c 0x80191740 0x14 -O2 -G0
linked .text 32 bytes, target 0x14, word mismatches=0, nonzero_pad=0
LINK_EXACT
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 1 c / 2 asm / 0 rodata)      # strong check at the overlay VMA
$ python3 tools/build/disc1_build.py --target ovl_0700     # RESULT: EXACT MATCH, 9f8786a9…
```

`disc1_verify.py --target ovl_0700` additionally requires the leaf's source
to be git-tracked (same rule as the EXE lane); until it is added, the gate
reports the "staging gap" classification after an EXACT MATCH build.

### 4.1 ovl_0700 matching session (2026-09-22, overlay lane agent)

Gate re-run on the committed infrastructure (after `scripts/extract_overlays.sh`,
all six blob SHA-1s agree with the manifest). The only initial failure was the
verify "staging gap"; sources are staged with `git add -N` (intent-to-add:
visible to `git ls-files`, carries no content into a plain `git commit`).

```
$ scripts/exact_rebuild_overlay.sh ovl_0700        # 1 C leaf
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=f2bd5e8a… spans=[1 c, 2 asm, 0 rodata, 2 bin] sha1=9f8786a9…
$ scripts/exact_rebuild_overlay.sh ovl_0700        # after batch 4 (31 C leaves)
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=0cc0cb09326956db6875ee195201e2085877a1f155f944f84be235dbf5557686 yaml=2101eda0… spans=[31 c, 10 asm, 0 rodata, 2 bin] sha1_orig=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1_cand=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```

Each leaf: `era_link_check.py --target ovl_0700 <src> <vram> <size> -O2 -G0`
→ `LINK_EXACT`, then `disc1_preflight.py --deep --target ovl_0700` PASS, then
the gate. Leaves (VRAM, blob offset, size), all default `era_o2_g0` unless noted:

| batch | leaves |
| --- | --- |
| 0 | `func_80191740` (0x2750, 0x14) |
| 1 | `func_80191834` (0x2844, 0x20), `func_80191FE8` (0x2FF8, 0x24), `func_8019200C` (0x301C, 0x24), `func_80190D08` (0x1D18, 0x34), `func_8019BF8C` (0xCF9C, 0x38), `func_8019BF50` (0xCF60, 0x3C), `func_80191DE8` (0x2DF8, 0x48) |
| 2 | `func_80191580` (0x2590, 0x5C), `func_80191754` (0x2764, 0x68), `func_801917BC` (0x27CC, 0x78), `func_80190AEC` (0x1AFC, 0x8C), `func_801915DC` (0x25EC, 0x9C), `func_80190B78` (0x1B88, 0xA4), `func_80193AB0` (0x4AC0, 0xAC), `func_80191678` (0x2688, 0xC8), `func_80194108` (0x5118, 0x9C) |
| 3 | `func_80192740` (0x3750, 0xC0), `func_801958D4` (0x68E4, 0xC0), `func_80190D3C` (0x1D4C, 0xC8), `func_801938E8` (0x48F8, 0xC8), `func_80191E30` (0x2E40, 0xCC), `func_80190C1C` (0x1C2C, 0xEC), `func_80191EFC` (0x2F0C, 0xEC), `func_801939B0` (0x49C0, 0x100) |
| 4 | `func_80195D3C` (0x6D4C, 0x110), `func_80195E4C` (0x6E5C, 0x120), `func_8018FFF4` (0x1004, 0x130), `func_80190124` (0x1134, 0x130), `func_80190998` (0x19A8, 0x154), `func_80191C94` (0x2CA4, 0x154) |
| 5 | `func_801941A4` (0x51B4, 0x158), `func_8019BD78` (0xCD88, 0x1D8), `func_80190254` (0x1264, 0x25C, **`era_o2_g0_expand_div`** — load-bearing: 40 word mismatches without `MASPSX_EXPAND_DIV=1`) |

Final ovl_0700 state (34 C leaves; per-leaf evidence in
`docs/evidence/ovl_0700-func_*/REPORT.md`, each re-verified `LINK_EXACT`):

```
$ python3 tools/build/disc1_preflight.py --deep --target ovl_0700
disc1_preflight: PASS (deep, 34 c / 10 asm / 0 rodata)
$ scripts/exact_rebuild_overlay.sh ovl_0700
EXACT_REBUILD_GATE[ovl_0700]=PASS plan=7e5e52a5d4efd6ed60f525ccbdc13384817347c132ec7d0fcbad6425cc437cae yaml=dcb82646ae75fa17112a20779195a4cba8a7834003ff8f21ef95d71538c6e1f4 spans=[34 c, 10 asm, 0 rodata, 2 bin] sha1_orig=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1_cand=9f8786a9d6f8b11548a14680d34fc540d8e909de sha1=9f8786a9d6f8b11548a14680d34fc540d8e909de
```

Overlay `*_build_profiles.json` assignments use the EXE format,
`{"<profile>": ["func_…", …]}` (profile → list), not func → profile.

Data model recovered along the way (for the remaining leaves): a 0x6C-byte
node pool `D_801E4E00[200]` (id, MATRIX at +8, SVECTOR at +0x28, depth +0x5C,
parent +0x60, next/prev +0x64/+0x68), a `{short prev, next}` slot table
`D_801E4A88` with free list `D_8019CBC0`/`D_8019C830`, three depth sentinels
`D_8019CC58[3]` ↔ `D_801EA5E8[3]`, a Psy-Q double buffer `DB D_8019C1F8[2]`
(`primptr`, `ot`, `DRAWENV`, `DISPENV`; 0x78 B) with `D_8019C9C0` = current,
and a two-channel path walker `Walk D_8019C05C[2]` + `Step D_8019C050[2]`.

Levers that carried over from the EXE lane / were new here:
* declaration order of two locals decides their hard registers
  (`int b, a;` in `func_8018FFF4`/`80190124`), and zero-init statement order
  decides the `addu` order (`func_80190254`);
* ternary / else-branch polarity (`func_80192740`, `func_80194108`);
* a pointer local for a global pair keeps the base callee-saved and fixes the
  frame size (`&D_800BCFA4` in `func_80191E30`/`80191EFC`; `D_800B0CD8` base
  in `func_80191C94`);
* `unsigned` loop counter → `sltu` pointer bound (`func_80193AB0`);
* a `$2`-pinned temporary lets reorg duplicate an idempotent `sll` into a
  branch delay slot where an in-place `sll $5,$5,3` cannot be
  (`func_80190254`).

PARKED (no C in tree):
* `func_8019959C` (0xA5AC, 0x20) — 2 words: retail computes
  `table + i*4` into `$v0`, cc1 reuses dead `$a1`; ~15 spellings incl. `$2`
  pins tried.
* `func_80195BC8` (0x6BD8, 0x174) and `func_80195994` (0x69A4, 0x234) — the
  same `Walk`/`Step` initialiser pair. Best 46/93 and 19/141 words; all 120
  block orders, axis-major orders, param types and `-fno-schedule-insns`
  tried. Residual: retail masks the shift in place (`andi s2,s2,0xFF` then
  `sllv` by the masked value) and interleaves `v.x`/`C810.x` loads; cc1 drops
  the mask for `sllv`.
* `func_8018F344` (0x354) contains inline GTE (`ctc2`/`lwc2`/`mvmva`, flagged
  handwritten) — `handwritten-gte-wrapper` class, not C.

Remaining ovl_0700 asm (21 functions, all >= 196 words): `func_80190E04`,
`80192030`, `8018F55C`, `8019234C`, `80191854`, `80191114`, `80193478`,
`801904B0`, `80195F6C`, `80193B5C`, `8018F92C`, `8019B1D0`, `80192800`,
`801995BC`, `8019A318`, `801942FC`, `80196498`, `80197BA0`, plus the three
parks and `func_8018F344` (GTE) above.

### 4.2 The other five subsystem overlays (same session)

All configured from the ovl_0700 template (`[0x0,bin) [text_start,asm)
[text_end,bin) [size]`; manifest entries gained `text_start`/`text_end`/
`config`/`build_profiles`), all-asm gate PASS first, then leaves:

| overlay | text window | notes | C leaves |
| --- | --- | --- | --- |
| ovl_03C5 | 0x300–0x16F4 | FMV name header before; code starts with a `lui` ahead of the 0x304 prologue | 2: `func_80122354`, `func_801223A8` |
| ovl_03C9 | 0x60–0x43A0 | handler descriptors/data after; no internal calls | 7: `func_801F1B00`/`B10`/`B20`/`B30`/`B40`/`B50` (six identical accessors), `func_801F1B60` |
| ovl_0457 | 0x28DC–0x4E48 | header, credits text and switch jump tables before the text | 7: `func_80124034`, `80125444`, `80125894`, `80125AF0`, `801255A4`, `80124598`, `801258EC` |
| ovl_03D2 | 0x304–0x4264 | FMV header before; data/title assets after | 10: `func_8019319C`, `8019316C`, `801931BC`, `80192C9C`, `80192F98`, `80192C48`, `80193084`, `80193200`, `801930D8`, `80192FE8` |
| ovl_039F | 0x54–0xF04 | Psy-Q libpress (hand-scheduled `addi`/explicit `$at` from 0x7FC); the survey's "last `jr $ra`" at 0x127F0 is a table word | 0 (SDK — classified, not matched) |

Gate lines (re-run after the ovl_0700 batch 5; sha1_orig == sha1_cand == manifest):

```
EXACT_REBUILD_GATE[ovl_03C5]=PASS plan=503c6567… spans=[2 c, 1 asm, 0 rodata, 2 bin] sha1=f8f6ea3892df4a7b1c8b58c7fb76222e4bb52736
EXACT_REBUILD_GATE[ovl_03C9]=PASS plan=040852fb… spans=[7 c, 1 asm, 0 rodata, 2 bin] sha1=1d0e972cf9446dc700c82a8597292ced45958ef4
EXACT_REBUILD_GATE[ovl_0457]=PASS plan=d3b9d0b5… spans=[7 c, 6 asm, 0 rodata, 2 bin] sha1=08b5c11984153ba1fd6ec23d4a2b79a0c9acada6
EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=33c55039… spans=[10 c, 2 asm, 0 rodata, 2 bin] sha1=a0b604ead810592dd9a8d74f7c74fe94151f4d32
EXACT_REBUILD_GATE[ovl_039F]=PASS plan=c831e57c… spans=[0 c, 1 asm, 0 rodata, 2 bin] sha1=e7331867b4d1464da4d5f386d9137eaa1c98422b
```

Levers: a named `int id` local in a list walk fixes a delay slot
(`func_801931BC`, `func_80192F98`); `volatile` counter/mirror globals reproduce
retail reloads (`func_80125AF0`, `func_801258EC`); a `goto` to one shared store
fixes branch order (`func_801223A8`, and its byte-identical 03D2 twin).

### 4.3 Continuation (2026-09-22/23, overlay lane agent)

`TMPDIR=build/tmp` for every command; each overlay gate was run serially (one
gate at a time). Every leaf is `LINK_EXACT` at its overlay VMA, passes
`disc1_preflight.py --deep --target <id>`, and has
`docs/evidence/<ovl>-func_*/REPORT.md` (re-verified by re-running the link
check when the report was generated).

| overlay | new leaves | C total | last gate |
| --- | --- | --- | --- |
| ovl_0457 | `func_801247F4`, `func_80123F8C`, `func_80125A40`, `func_80125980`, `func_80124704`, `func_8012562C`, `func_80125464`, `func_80125744`, `func_80124F40`, `func_80124AA4`, `func_80123E08`, `func_80123950` | 19 | `EXACT_REBUILD_GATE[ovl_0457]=PASS plan=bad90d75793b08b338441e79d57bb56b8f076d9396432d8053a09caf9a531c93 spans=[19 c, 7 asm, …]` |
| ovl_03D2 | `func_8018F7F0` | 11 | `EXACT_REBUILD_GATE[ovl_03D2]=PASS plan=7faee51d4307a4fe597eace0c84ce64b0d5e45b0b4ff45a79c17afaa3be49058` |
| ovl_03C9 | `func_801F0BA0` | 8 | `EXACT_REBUILD_GATE[ovl_03C9]=PASS plan=e5994b0efd644bd2a65465ee9e4f86cf84bb98f718ec51f3c99cd80c9963b6b5` |

Overlay leaf total: ovl_0700 34 + 03C5 2 + 03C9 8 + 03D2 11 + 0457 19 +
room_m0022i 3 = **77**.

New levers, all overlay code:
* **`volatile` on shared stream-state globals** decides store order and
  delay-slot fill (`func_8012562C`: exactly CE8 via a pointer-to-volatile,
  CEC/CF0/CF4/CF8/CFA volatile, D00 plain — found by a 64-way mask search;
  `func_80125980`: volatile ring pointer keeps a store out of `j`'s slot;
  `func_80123F8C`: both window pointers volatile for retail's reload).
* **`goto` retry loops** keep `li s0,1` per retry instead of hoisted
  (`func_8012562C`, `func_80125744`), and a `goto` loop keeps an invariant
  stack address from being hoisted into a callee-saved register.
* **`noreturn` on `func_800719E4`** (BIOS `exit`) again removes a redundant
  re-materialisation (`func_80125980`).
* **Switch lowering reproduces a compare tree**: retail's `== 0x20`,
  signed `slti 0x21`, `== 9`, `== 0x5C` sequence is GCC's switch decision
  tree; case-body layout follows source case order (`func_80123950`).
* **`* 32` vs `<< 5`**: `r.w = o->step * 32` keeps retail's sign-extending
  `lh`; the shift spelling lets combine narrow it to `lhu`
  (`func_801F0BA0`). An `int` CLUT row avoids a stray `andi 0xFFFF`.
* **`switch` + `break` to a shared `return 0`** stops GCC turning
  `if (++n < K) return 0; return 1;` into a store-flag `slt`
  (`func_801F0BA0`, `func_801F1070`).
* **Hand-unrolled macro with a ternary** reproduces a register copy that
  appears only in the first three of four unrolled copies
  (`func_8018F7F0`: `p = dst` after `src++`, `*p = (v >= d) ? v : d`).
* **Declaration order of locals** again decides register pairs
  (`func_80125464` `r, b, g`), and `$13`/`$14` pins fix a base-pointer
  pair.

Parked (no C in tree; best mismatched-word counts):
* `func_8012462C` (0457, 19 w): SPRT field-store scheduling; pins fix the
  prologue, but no source order or `-fno-schedule-insns[2]` reproduces
  retail's `w,h` stores ahead of the byte stores.
* `func_8018FD04` / `func_8018FBC0` (03D2 task allocators, 33 / 74 w): retail's
  8-byte leaf frame is a reload spill-slot artefact ("vars= 8") and a
  dead `f2C = 0` store keeps its place; not reproduced.
* `func_8018F2F4` (03D2, 19 w): the flag load goes to `$v1` in retail.
* `func_801F1070` / `func_801F15DC` (03C9, 13 w / 1-word layout shift):
  register choice for the `/12` quotient and the `D_800F336C` read.
* `func_801214D4` and its 03D2 twin `func_80191DC8`: retail
  re-materialises `&D_801228F4` after the RECT struct copy where GCC CSEs it.
* `func_80123CAC`, `func_80123B1C` (0457): switch jump tables live in the
  pre-text `bin` region (`jtbl_80122FBC`, `jtbl_80123134`, `jtbl_801232AC`,
  `jtbl_80123424`); these need a `rodata` span + the dispatch-fold knob
  before they can be carved.
* In progress when the shell went down: `func_8012489C` (0457,
  87 w; needs the `&D_80093168` pointer local plus store-order work).

**Blocker (recurring):** the per-user `/tmp` quota fills up while other lanes
run, and then every shell command fails. `TMPDIR=build/tmp` covers the
tools, but the harness itself writes to `/tmp`.

**Tooling fix (overlay-only):** `tools/build/overlay_targets.py` overlay
`derive_symbol_prefixes` now also includes `.L`. Splat named a byte-table base
in ovl_0457 as a code label (`.L80125A9C`, referenced from `func_80123CAC`);
once a carve split the reference and the label into different asm files the
link failed. The label now binds by address. `tools/build/test_overlay_plan.py`: 16 OK.

**Hazards seen:** the shared `/tmp` tmpfs quota filled up twice (other lanes'
multi-GB `war-*` dirs), which stops every shell command until space frees up.
`split_overlay.sh` sometimes fails its git-status ignore check when another lane
writes `docs/evidence/…` at the same moment; a retry passes.

### 4.4 Continuation (2026-09-23/24, overlays2): subsystem leaves + the room tier at scale

**Lane metric (use this, never "2469 leaves"): 216 hand-matched leaves (139 new) + 2253
masked-body ports = 187 distinct bodies across 72 gated targets.** (2469 C spans / 1649 distinct
names; overlays2's own masked-hash count is 186, the lead's 187 is authoritative.)

`TMPDIR=build/tmp` for every command. Every gate ran serially, one at a time.

**Subsystem overlays (new leaves).** ovl_0457: `func_8012489C` (was 87 words off; lever:
`$2`-pinned `off`, a separate `lo = base + off` with `off += 0x54000` in place, volatile
`D_80172CC4`; the XA-start argument is written as `(char *)D_80011610 + (off + 0x43000)`),
`func_80123B1C`, `func_80123CAC` (jump tables carved as data),
`func_801235DC` (credits main loop, 221 words), `func_80125094` (credits page step, 236
words). ovl_03C5: `func_80121A00`, `func_80121004` (`signed char` params; plain `char` is
unsigned for this cc1). Evidence under `docs/evidence/ovl_0457-func_*/` and `ovl_03C5-func_*/`.

**Jump tables in pre-text data.** ovl_0457's switch tables sit in the header region, which is now
`[0x22A4, rodata]` (splat emits `asm/overlays/ovl_0457/data/22A4.rodata.s` with `dlabel jtbl_*`).
A leaf's own cc1 table is stripped against that pool by `strip_dispatch_rodata`, as in the EXE
lane. Room headers get the same treatment: `[<first jtbl>, rodata]`. Three overlay-only tooling
changes made this work (the EXE plan hash, preflight and build are unchanged; the EXE has no
`local_profiles` and no pool block ends in a zero literal):
* `local_profiles` in an overlay `*_build_profiles.json` (`disc1_plan.py`, `disc1_preflight.py`).
  These are target-specific profiles next to `profiles_from`, e.g. a `MASPSX_DISPATCH_FOLD` naming
  this overlay's own `jtbl_`. They may not shadow an inherited name. Tests:
  `tools/build/test_overlay_plan.py` (`test_local_profiles_*`, 18 OK).
* `strip_dispatch_rodata` accepts splat's `.word .L<vram>` pool entries (targets in the same
  segment), and it drops a trailing `.word 0x00000000` from a pool block. That word is the
  `.align 3` pad splat folds into the preceding dlabel; a jump-table entry is never zero
  (`disc1_build.py`).

**Room tier.** Tooling `tools/analysis/room_lane/` (work dir `$ROOM_LANE_WORK`, default
`build/room_lane`):
* `roomscan.py groups` lists the 66 distinct room texts (the census rule).
  `roomscan.py match <room>` lists the bodies whose masked hash equals a C leaf of another target.
* `port.py <room>` writes the C of each masked twin, re-targeted by symbol address. It renames the
  function, maps every `func_`/`D_` token by comparing the jal/j targets and lui/`%lo` addresses of
  the two bodies word by word, and refuses unmapped room-range tokens. Dispatch leaves get a remapped
  local profile plus a header rodata span.
* `doroom.sh <room>`: config (`newroom.py`), split, `fixwindow.py` (moves the text start past header
  words that `as` rejects, and prunes stale assignments), port, per-leaf `era_link_check` (a port that
  is not `LINK_EXACT` goes to drafts and is never carved), `carve.py`, `disc1_preflight --deep`, gate.
* `addleaf.sh <room> <draft.c> "<desc>"` installs a hand-written, link-checked leaf.

Room targets now configured: 66, one per distinct text (plus `room_m0022i`'s 30-room group, etc.).
A leaf in a text group is byte-identical in every room of that group (§2 of ROOM_LANE_SCOPING.md).
Hand-written room leaves: `docs/evidence/room_m0022i-{stubs,batch}-2026-09-23/`,
`docs/evidence/room-lane-2026-09-23/` (levers + parks). Port evidence per room:
`docs/evidence/room_<r>-ports-2026-09-23/REPORT.md` (source leaf and profile for every port, plus the
gate line).

Text-window fixes found by the gate: census windows that began in header data. `as` rejected a
`tge`-decoded word (m0152i, m0203i, m0137i, m0141i, m0231i, m0419i, m0421i; fixed automatically),
m0318i starts with printf strings (text start moved 0x1E4 → 0x264), and m0399i has two header words
before the first function (0x1EC → 0x1F4).

New levers (room code): see `docs/evidence/room-lane-2026-09-23/REPORT.md`. The most general:
a copy-pointer local with an initializer (`T *u = t;`) reproduces a mid-function `move` of an argument
register. A `short t` clamp local produces the unused 8-byte leaf frame. The GCC `==1`/`<2`/`!=0`
tree needs `if (r != 1) { if (r < 2) ... } else call`. A `(char *)` cast on a base keeps
`base + (off + K)` from being reassociated.

Not C under current rules: the eight 39-text room bodies with inline Psy-Q GTE macros
(`ctc2`/`lwc2`/`mvmva`/`swc2`) and `func_80192344`. They are the `handwritten-gte-wrapper` class.
If inline-asm GTE macros were ever accepted, these alone would add about 8 × 39 leaves.

Parked this session (drafts under the work dir; residuals in the room-lane report):
ovl_0457 `func_80124C04` (62 words, register allocation only), ovl_03C5 `func_80121270` (35
words), ovl_03D2 `func_8018FD04` (68 words, re-attempted), plus the room parks listed in the report.

Per-target state after the final gates (C spans / hand-written / ported; gate from the last serial run;
full `EXACT_REBUILD_GATE` lines under `build/fanout/overlays2/logs/gate_<id>.log` and in each room's port report):

| target | C | hand | ported | gate | plan | sha1 |
| --- | --- | --- | --- | --- | --- | --- |
| ovl_039F | 0 | 0 | 0 | PASS | c831e57c330d… | e7331867b4d1… |
| ovl_03C5 | 4 | 4 | 0 | PASS | 66c9f23417d3… | f8f6ea3892df… |
| ovl_03C9 | 8 | 8 | 0 | PASS | e5994b0efd64… | 1d0e972cf944… |
| ovl_03D2 | 11 | 11 | 0 | PASS | 7faee51d4307… | a0b604ead810… |
| ovl_0457 | 24 | 24 | 0 | PASS | 642d987efd66… | 08b5c1198415… |
| ovl_0700 | 34 | 34 | 0 | PASS | 7e5e52a5d4ef… | 9f8786a9d6f8… |
| room_m0005i | 1 | 1 | 0 | PASS | 8ee7d278f02f… | 13b5953b0d09… |
| room_m0012i | 1 | 0 | 1 | PASS | 151ad32dfb9e… | 7b38ab836272… |
| room_m0014i | 45 | 1 | 44 | PASS | 766afed9b504… | af4054c825b8… |
| room_m0017i | 2 | 0 | 2 | PASS | 46cb5465e044… | b40116e92e76… |
| room_m0022i | 45 | 45 | 0 | PASS | 79fe3ed8a901… | 3231b287bdde… |
| room_m0023i | 1 | 0 | 1 | PASS | 6663266fb0a1… | 3dfb340a8c3f… |
| room_m0034i | 14 | 0 | 14 | PASS | c7d26bde5466… | 2d09f16a1668… |
| room_m0049i | 9 | 2 | 7 | PASS | 6ba2132e7212… | 320861e23df3… |
| room_m0063i | 8 | 0 | 8 | PASS | 8af715cb8982… | 243ffc3252ab… |
| room_m0065i | 10 | 0 | 10 | PASS | e4260d845a06… | fa62e6cdf74b… |
| room_m0075i | 58 | 6 | 52 | PASS | 6b6d532d6a06… | ca34d1321e15… |
| room_m0086i | 0 | 0 | 0 | PASS | 37d8690cc8d6… | 1ebeb226defa… |
| room_m0087i | 55 | 0 | 55 | PASS | 208c0b2bd314… | 4c6107aa1de0… |
| room_m0089i | 44 | 0 | 44 | PASS | 8444c8cffb04… | 06bee6ec7422… |
| room_m0104i | 1 | 1 | 0 | PASS | 024e5c52d168… | 6ce13ec8a95c… |
| room_m0107i | 65 | 0 | 65 | PASS | fd2150ffe2b3… | dcf6cf737f82… |
| room_m0111i | 20 | 0 | 20 | PASS | 0073b6ec5f6a… | a360b6aa7991… |
| room_m0113i | 2 | 0 | 2 | PASS | 7d55e5013d21… | f55e2fa3c9c1… |
| room_m0116i | 45 | 0 | 45 | PASS | 659220c0135d… | cfc9e33ffd9b… |
| room_m0122i | 21 | 0 | 21 | PASS | f301c3dee22f… | 1aae0fbca4b2… |
| room_m0123i | 46 | 0 | 46 | PASS | d7e44921bf1e… | 7dbc62ff9946… |
| room_m0126i | 1 | 1 | 0 | PASS | 742d4e7ab3c6… | 4d06f0af6935… |
| room_m0137i | 18 | 10 | 8 | PASS | 5e4ac5f37530… | 6e146c37de27… |
| room_m0141i | 20 | 12 | 8 | PASS | 10366a0162e8… | 63c6c6ceb406… |
| room_m0145i | 51 | 3 | 48 | PASS | cd0cf7846b6c… | 8bb899c982ce… |
| room_m0146i | 33 | 0 | 33 | PASS | ffd5bce7a03f… | 72a6cf583e29… |
| room_m0152i | 52 | 0 | 52 | PASS | 413420993c58… | 3f71f680b594… |
| room_m0153i | 71 | 0 | 71 | PASS | 89b9b1d0b412… | 342c8ca376fc… |
| room_m0156i | 2 | 0 | 2 | PASS | c803208316a0… | 2d094b8b74b9… |
| room_m0162i | 45 | 0 | 45 | PASS | adc68acc6a7c… | 35983d5c944d… |
| room_m0167i | 55 | 7 | 48 | PASS | 5e4d61cbdcd1… | 8414175491c6… |
| room_m0174i | 89 | 21 | 68 | PASS | 5d594d10554e… | b2f0329a9b0c… |
| room_m0186i | 66 | 5 | 61 | PASS | 32e4d71acf8a… | 55c712d21f33… |
| room_m0187i | 10 | 0 | 10 | PASS | 1108f8b035da… | 24ba474b8589… |
| room_m0188i | 46 | 0 | 46 | PASS | f06a3b2908af… | 80f620421f13… |
| room_m0203i | 45 | 1 | 44 | PASS | 1bb4929e8572… | 6f9743ef89e5… |
| room_m0205i | 45 | 0 | 45 | PASS | fc0a4627b650… | 0c281041b45c… |
| room_m0231i | 46 | 0 | 46 | PASS | 7d542ad0ef28… | 94400edc11b5… |
| room_m0245i | 46 | 1 | 45 | PASS | a7ef4a79e935… | 512b5a7c4818… |
| room_m0256i | 49 | 0 | 49 | PASS | 3728a6e91639… | 618fb891329d… |
| room_m0263i | 46 | 0 | 46 | PASS | f69cee22f357… | 02f1b9cdb8eb… |
| room_m0269i | 7 | 1 | 6 | PASS | 57829ef8620d… | 06c90576dba2… |
| room_m0272i | 4 | 0 | 4 | PASS | e448ec60bfcb… | 9684da6b0a4c… |
| room_m0273i | 53 | 1 | 52 | PASS | 863dd71157c6… | 692fdf1477e2… |
| room_m0318i | 47 | 0 | 47 | PASS | 8eb19e041394… | 3deebe65a4bb… |
| room_m0348i | 27 | 0 | 27 | PASS | 1563f9aa0cdf… | 49478531f4b7… |
| room_m0349i | 0 | 0 | 0 | PASS | d47f03c48263… | 09410ce75c6f… |
| room_m0350i | 44 | 0 | 44 | PASS | 8876d9913210… | e9597b000d28… |
| room_m0358i | 53 | 5 | 48 | PASS | a07b3a75a55e… | 3fbd46633b22… |
| room_m0391i | 55 | 2 | 53 | PASS | 9878b316014e… | 327d5766f293… |
| room_m0392i | 59 | 0 | 59 | PASS | b2fda5b9853d… | b51734dcce48… |
| room_m0393i | 69 | 0 | 69 | PASS | 5dfa06f8677e… | cef7a5f2ad37… |
| room_m0399i | 62 | 1 | 61 | PASS | 728f1762a7ce… | d05fc0065434… |
| room_m0401i | 75 | 0 | 75 | PASS | f40a701a3be6… | 19d2caf765e8… |
| room_m0404i | 45 | 0 | 45 | PASS | 10e6b1ecd52b… | c611bd599af3… |
| room_m0412i | 65 | 0 | 65 | PASS | d703cfb14ed2… | 673097c6bd36… |
| room_m0413i | 45 | 0 | 45 | PASS | aeed7daec4c3… | 6083b41a6287… |
| room_m0414i | 20 | 0 | 20 | PASS | 62c4f1350326… | c1b8f25dd4ad… |
| room_m0418i | 81 | 0 | 81 | PASS | dda6001b0e04… | 03c8f1c6d2d4… |
| room_m0419i | 58 | 0 | 58 | PASS | a9c0c2c58448… | 3583c4ecd060… |
| room_m0421i | 22 | 8 | 14 | PASS | ac21acd794d5… | 983c83c8d658… |
| room_m0424i | 21 | 0 | 21 | PASS | 4996508888d5… | 5488eb751ec4… |
| room_m0428i | 45 | 0 | 45 | PASS | 6152cca2bc0c… | 7a8a6b604e21… |
| room_m0429i | 55 | 0 | 55 | PASS | 5a656d1a3709… | 1487c56a4d42… |
| room_m0430i | 0 | 0 | 0 | PASS | d0ed09f037db… | 475d8d7b8261… |
| room_m0432i | 47 | 0 | 47 | PASS | 16c0e606c4cc… | e13a8b541164… |

Overlay C spans: 2469 (see the lane metric at the top of §4.4) (216 hand-written in-target, 2253 masked-body ports). Previous state: 77.

## 5. How to add the next overlay

1. In `configs/USA/overlays/manifest.yaml` the entry already exists for all
   six subsystem overlays (`scan_first_prologue` / `scan_last_jr_ra` are raw
   scan facts). Audit its `.text` window: first framed prologue, last `jr $ra`
   plus delay slot, what follows (pad/data), and record `text_start` /
   `text_end`. For `ovl_0700` this was `0x6C … 0xCFD4` (44 prologues, 157/186
   in-blob `jal` targets on a prologue at this base).
2. Write `configs/USA/overlays/<id>.yaml` from the `ovl_0700` template: same
   `sha1` as the manifest, `base_path: ../../..`, `target_path` = the blob,
   `asm_path/src_path/asset_path/ld_script_path` under `overlays/<id>`,
   `cache_path` / `undefined_*_auto_path` under `asm/overlays/<id>/`
   (`split_overlay.sh` refuses a config without them),
   `find_file_boundaries: no`, one `code` segment at the manifest `vram` with
   `[0x0,bin)`, `[text_start,asm)`, `[text_end,bin)`, `[size]`.
3. Write `configs/USA/overlays/<id>_build_profiles.json` with
   `profiles_from` and empty `assignments`; add `config` + `build_profiles`
   to the manifest entry.
4. `scripts/extract_overlays.sh --only <id>` (or all), then
   `scripts/exact_rebuild_overlay.sh <id>` must print
   `EXACT_REBUILD_GATE[<id>]=PASS` on the all-`asm` config before any C is
   carved. If the reassembly is not byte-exact, the text window is wrong
   (data disassembled as code, or code left in a `bin`); fix the geometry, not
   the tooling.
5. Carve leaves exactly like the EXE lane: `src/overlays/<id>/func_<VRAM>.c`,
   a `- [offset, c, func_<VRAM>]` row, `era_link_check.py --target <id>` for
   the quick check, `disc1_preflight.py --deep --target <id>` for the strong
   check (size, link at the overlay VMA, terminator, no interior `jr $ra`),
   `git add` the source, then the gate.
6. Rooms: `tools/extract/peimg.py room m0418i` writes
   `build/extracted/disc1/peimg/room_m0418i_c2.bin` (VRAM `0x8018EFE8`) and
   prints its SHA-1; a room config is the same shape with the per-room text
   window (`m0418i`: first prologue `0x260`, 91 prologues, 150 `jr $ra`).
   Room ids are `room_m####i`; add them to `overlays:` with `kind: room` and a
   `blob` path when a config exists (the manifest reader already accepts
   `kind: room`; a room `sha1` is a recorded fact from the extractor).

## 6. Ordering (from the survey)

1. **Infrastructure** — this document (done: `ovl_0700` round trip + first leaf).
2. **Six subsystem overlays** (~170 functions, 24.5K words): `ovl_0700`
   (map exit, 44 f / 12.6K w), `ovl_03D2` (title, 21 f / 3.8K w), `ovl_03C9`
   (handlers, 14 f / 4.3K w), `ovl_0457` (credits, 23 f / 2.1K w),
   `ovl_03C5` (movie controller, 8 f / 1.3K w), `ovl_039F` (libpress/MDEC,
   14 f — likely SDK, classify with `nonmatchable_spans.py` before counting).
3. **The EXE room-runtime island** `0x800C20C8–0x800E21A4` (350 `jr $ra`,
   29.4K words, already inside `disc1.yaml` and the EXE gate): every code room
   calls it (`m0418i`: 235 of 375 EXE `jal`s), so it is the highest-leverage
   EXE region for this lane.
4. **45 shared room bodies** (present in ≥ 10 rooms, 6.9K words) via one
   high-fan-in room (`m0418i` or `m0273i`), landing in `room_common/` once the
   relink proof exists.
5. **656 behaviours shared by 2–9 rooms** (118.6K words).
6. **672 bespoke bodies** (123.4K words); 66 rooms in table order already
   introduce all unique code, the other 119 code rooms are relinks.

## 7. Notes and hazards

* `era_link_check.py` calls the binutils under `tools/mipsel-host/usr/bin`
  directly; on a tree-local install set
  `LD_LIBRARY_PATH=tools/mipsel-host/usr/lib/x86_64-linux-gnu` (the build's
  `find_toolchain` uses the shims in `tools/mipsel-host/bin`, which set it).
* An overlay split rewrites `include/*.inc` with the same splat-generated
  content as the EXE split; both are git-ignored.
* `disc1_plan.py --write-status` / `--check-status` are EXE-only (the
  generated `DISC1_MATCHING_STATUS.md` counts EXE leaves); overlay counts come
  from `disc1_plan.py --check --target <id>`.
* `profile_necessity.py`, `verify_matched_leaves.py`, `check_leaf.sh` and
  `route_coverage.py` are still EXE-only; give them `--target` when the first
  overlay profile assignment appears.
