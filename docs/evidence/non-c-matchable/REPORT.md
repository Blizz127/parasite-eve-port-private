# Non-C-matchable split-asm spans — classification

## Refresh 2026-09-29 (2243 C / 143 asm): classifier revision + manifest regeneration

Plan `42b65e87c391` (the `func_80035558` gate). Commands:

```
python3 tools/analysis/route_coverage.py --no-history --top 20   # before  -> build/lanes/m348/nm/route_before.txt
python3 tools/analysis/nonmatchable_spans.py --json > docs/evidence/non-c-matchable/MANIFEST.json
python3 tools/analysis/route_coverage.py --no-history --top 25   # after   -> build/lanes/m348/nm/route_after.txt
```

### Why the classifier changed

1. **COP2 ops are no longer non-C evidence.** The retired `handwritten-gte-wrapper`
   rule assumed cc1 cannot emit COP2 and that no matched leaf contains a COP op.
   Both are false now: Psy-Q `inline_c.h` GTE macros are inline `asm` inside C,
   95 matched EXE leaves carry them (`inline GTE/asm`), and `func_800E051C` /
   `func_80035558` were in that very bucket before they were matched
   (`LINK_EXACT`, full EXE gate). The disassembler's `/* handwritten instruction */`
   flag is emitted for the macros too. COP-bearing spans without integer-side
   evidence are now reported as informational **`gte-inline`** (liftable).
2. **New counted class `handwritten-integer`** with per-span `reasons` in the
   JSON: trapping `add`/`addi`/`sub`; `$ra` in a GPR or at a non-`$sp` address;
   BIOS vector call (`li $t2,0xA0/B0/C0` + `jr`/`jalr $t2`); COP0; ALU writes to
   `$sp`/`$gp`; saves below `$sp+0x10` in a call-bearing body; a branch into
   another span. cc1 emits none of these. (`$fp` writes are excluded: cc1 uses
   `$fp` as `$s8`.)

### Totals (222 split spans)

| class | spans | words |
|---|---:|---:|
| alignment-filler | 105 | 105 |
| gte-inline | 40 | 13188 |
| handwritten-integer | 22 | 731 |
| handwritten-jr-t2 | 53 | 163 |

`handwritten-cop` and `handwritten-syscall` are now empty (their members were
matched as inline-asm leaves). Counted non-C = `handwritten-integer` +
`handwritten-jr-t2`; `alignment-filler` is layout; `gte-inline` is liftable work.

### `handwritten-integer` members

| span | words | reasons |
|---|---:|---|
| `func_80070DD0` | 13 | trapping-arith, ra-outside-stack |
| `func_80072534` | 42 | trapping-arith, ra-outside-stack, sets-sp-gp |
| `func_800725DC` | 28 | save-in-arg-area |
| `func_8007264C` | 26 | save-in-arg-area |
| `func_80077F7C` | 32 | ra-outside-stack, cop0 |
| `func_80078004` | 33 | trapping-arith |
| `func_80078094` | 35 | trapping-arith |
| `func_80078120` | 5 | cross-span-branch |
| `func_80078134` | 24 | ra-outside-stack |
| `func_80078194` | 48 | trapping-arith |
| `func_80078254` | 58 | ra-outside-stack |
| `func_800787D4` | 88 | trapping-arith |
| `func_80078A94` | 41 | trapping-arith, ra-outside-stack |
| `func_80078B38` | 41 | trapping-arith, ra-outside-stack |
| `func_8007A134` | 43 | ra-outside-stack, bios-vector-call |
| `func_8007A1E0` | 12 | ra-outside-stack, cop0 |
| `func_8007E22C` | 28 | trapping-arith, ra-outside-stack, bios-vector-call |
| `func_8007E2A4` | 26 | trapping-arith, ra-outside-stack, bios-vector-call |
| `func_8007E334` | 15 | bios-vector-call |
| `func_8007E3DC` | 37 | ra-outside-stack, bios-vector-call |
| `func_8007E470` | 28 | ra-outside-stack, bios-vector-call |
| `func_8007E514` | 28 | ra-outside-stack, bios-vector-call |

### Manifest diff (old = 2026-09-28 file, new = this regeneration)

Full list: `build/lanes/m348/nm/manifest_diff.txt` (lane-local).

| change | entries |
|---|---:|
| removed: span is now matched C in `disc1.yaml` | 87 (61 gte-wrapper, 22 handwritten-cop, 2 handwritten-syscall, 2 gte-inline) |
| removed: split artefact (`func_800D004C` 0x6DC, now a 1-word `[0xC084C, asm]` row + `func_800D0050`) | 1 |
| path renames only (same class; asm files re-split since the old manifest) | 36 |
| reclassified gte-wrapper → gte-inline (inline_c.h C bodies) | 39 |
| reclassified gte-wrapper → handwritten-integer | 5 |
| reclassified gte-inline → handwritten-integer (`func_80078004`, `func_80078094`, `func_80077F7C`) | 3 |
| added: alignment-filler (new 1-nop spans from carving) | 72 |
| added: handwritten-integer (previously unclassified) | 14 |
| added: gte-inline | 1 |

### Route effect

```
before: plan=42b65e87c39126eefde7d9da38d81bf8f4260ef15175aec8f207224db3a6fff2 disc=1 funcs=1499/1573 c_words=118064 nonc_funcs=64 nonc_words=4475 asm_funcs=10 asm_words=308 asm_words_unknown=0 native_c=760/1573 native_c_words=1453 nonmatchable=present tierA=1331 tierB=248 tierB_unresolved=0
after:  plan=42b65e87c39126eefde7d9da38d81bf8f4260ef15175aec8f207224db3a6fff2 disc=1 funcs=1499/1573 c_words=118064 nonc_funcs=60 nonc_words=592 asm_funcs=14 asm_words=4191 asm_words_unknown=0 native_c=760/1573 native_c_words=1453 nonmatchable=present tierA=1331 tierB=248 tierB_unresolved=0
```

All ten former route "remaining" functions are now counted `handwritten-integer`
(`func_80078004`, `func_80078134`, `func_800725DC`, `func_8007A134`,
`func_8007E3DC`, `func_80077F7C`, `func_8007E470`, `func_8007264C`,
`func_8007E334`, `func_80072534`). "Real remaining asm" does **not** fall to 0:
it is now **14 functions / 4191 words**, because 13 route bodies that the old
rule wrongly counted as non-C (`handwritten-gte-wrapper`) are ordinary C with
inline GTE macros and are back in the matching queue (`func_800CEE20`,
`func_800D0728`, `func_800D27FC`, `func_800D1DEC`, `func_800D0E88`,
`func_800D2370`, `func_800DD9E4`, `func_800DA1FC`, `func_800D9A8C`,
`func_800C71E4`, `func_8006DFA8`, `func_8001AE40`, `func_80068014`), plus the
1-word split artefact `func_800D004C` (the `[0xC084C, asm]` / `[0xC0850, asm]`
rows split one function's prologue from its body; merging the rows fixes it).

## Refresh 2026-09-25 (2018 C / 230 asm)

Command: `python3 tools/analysis/nonmatchable_spans.py` on the split after the
2018 gate (`plan=2eaed0f5a667`). 421 split spans:

| class | spans | words |
|---|---:|---:|
| alignment-filler | 81 | 81 |
| gte-inline (informational; COP2 transfer, no C operator) | 5 | 192 |
| handwritten-cop | 22 | 123 |
| handwritten-gte-wrapper | 107 | 19957 |
| handwritten-jr-t2 | 53 | 163 |
| handwritten-syscall | 2 | 8 |

151 spans are unclassified. After dropping `jr`/`jalr $t2`, `syscall`, `break`,
`mfc0`, and handwritten `addi`, 127 ordinary-looking functions remain. The
five `gte-inline` functions (`func_8003EAC8`, `func_800130B4`, `func_80077F7C`,
`func_80078004`, `func_80078094`) still contain `mtc2`/`mfc2`/`ctc2`/`swc2`
tagged handwritten; cc1 has no operator for them. They stay asm. See
`docs/evidence/gte-inline-block-2026-09-25/REPORT.md`. Route on this image:
`asm_funcs=126 asm_words=36634`.

## Refresh 2026-09-25 (2007 C / 237 asm)

Command: `python3 tools/analysis/nonmatchable_spans.py` and `--json`.
Split-asm glabels the tool classified:

| class | spans | words |
|---|---:|---:|
| alignment-filler | 81 | 81 |
| gte-inline (informational, not non-C) | 5 | 192 |
| handwritten-cop | 22 | 123 |
| handwritten-gte-wrapper | 107 | 19957 |
| handwritten-jr-t2 | 53 | 163 |
| handwritten-syscall | 2 | 8 |

Those glabels were then placed inside the 237 YAML `asm` rows of the 2007 image (plan `fe9249539949`). A row is one class when every glabel that starts inside it shares that class. The terminal row at file `0xD0D1C` is 0 bytes.

| YAML asm row | spans |
|---|---:|
| alignment-filler only | 72 |
| handwritten non-C only (gte-wrapper, jr-t2, cop, syscall) | 61 |
| gte-inline only | 2 |
| mixed filler + gte-inline + gte-wrapper (`0x80077F68`, `0xD2C`) | 1 |
| no classified glabel (still C-expressible) | 100 |
| terminal 0-byte row at file `0xD0D1C` | 1 |

The C-expressible rows are the matching work. Smallest single-function files among them, from each file's `nonmatching func_*, 0xSIZE` line: `func_8005382C` 79 words (1 `jal`), `func_80046574` 83 (15 `jal`), `func_800534E4` 89 (10 `jal`), `func_80080F98` 94 (7 `jal`), `func_8007CEAC` 106 (1 `jal`). `gte-inline` stays in scope and is not counted as matched C.

## Prior classification (2026-09-24)

This report separates the split asm under `asm/disc1/` into provably
non-C-matchable spans and real remaining asm, so the "remaining asm" count stops
overstating the work. Nothing is reclassified without positive per-span
instruction evidence, and nothing here changes the matching-C count or the
exact build: these spans stay in the YAML as `asm`.

Tool: `tools/analysis/nonmatchable_spans.py` (idempotent; prints counts,
`--list`, `--json`). Machine-readable inventory:
`docs/evidence/non-c-matchable/MANIFEST.json`.

## Current counts (2026-09-24, plan `1b3aad012eeac56180c070676e72dc45d01fc92a2b5c4eded713fc12a9806d06`, 1959 c spans)

Refreshed by tooling2 after the batch-10 gate split (`asm/disc1/*.s` of that
gate). Commands (read-only): `python3 tools/analysis/nonmatchable_spans.py
[--json]`; `python3 tools/analysis/route_coverage.py --no-history --quiet`;
per-span word counts are glabel..endlabel instruction words of the split asm.

EXE, whole image:

| bucket | functions | words |
|---|---:|---:|
| matched C (YAML `c` spans) | **1959** | 92999 |
| `handwritten-gte-wrapper` (non-C) | 107 | 19957 |
| `handwritten-jr-t2` (non-C) | 53 | 163 |
| `handwritten-cop` (non-C) | 22 | 123 |
| `handwritten-syscall` (non-C) | 2 | 8 |
| **non-C total** | **184** | **20251** |
| alignment filler (not functions) | 79 spans | 79 |
| `gte-inline` (liftable; informational) | 5 | 192 |
| **real remaining asm** | **209** | **47261** |
| real remaining incl. `gte-inline` | 214 | 47453 |

Real-remaining size profile: 50 functions <= 64 words, 111 <= 256, 48 <= 1024,
5 > 1024 (largest `func_8001D340` 2149w, `func_8002BC90` 1368w,
`func_8002DC58` 1302w, `func_800299CC` 1075w, `func_80037870` 1064w).

Boot -> end-of-Day-2 route (`route_coverage.py`): 1397 functions on route,
1161 matched C (59449 words), 77 non-C (5417 words), **159 asm remaining
(40622 words)**; tierA 1152, tierB 248, tierB unresolved 0.

Overlays (PE.IMG, 72 targets; `nonmatchable_spans.py` has no `--target`, so
their asm is unclassified): 6 subsystem overlays hold 82 c / 4374 words and
22 asm spans / 21872 words; 66 room targets hold 2402 c / 69579 words and
543 asm spans / 214396 words (room asm spans are whole-blob ranges that still
include data, so the word figure overstates code). All 72 overlay targets
pass their exact-rebuild gate (tooling2 Phase B, 2026-09-24).

## Resulting counts (historical, session cont. 19)

| bucket | functions | words |
|---|---:|---:|
| matched C (YAML `c` spans) | **695** | 9091 (route 332 / 5842) |
| non-C-matchable, whole image | **184** | **20252** |
| — of which on the boot→Day-2 route | 76 | 5414 |
| — of which off-path (attic/overlays) | 108 | 14838 |
| alignment filler (not functions) | 33 spans | 33 |
| `gte-inline` (informational; still liftable) | 5 spans | 192 |
| **real remaining asm (route)** | **571** | **66437** |

Route totals from `tools/analysis/route_coverage.py` (plan
`8a90f1e110e7f9390d1ccc2d82712c4ab07c0848e6359166068e164b77649c46`): 979
functions on route, 332 matched C / 571 asm; 5842 matched C words / 66437 asm
words.

Class breakdown of the 184: `handwritten-gte-wrapper` 107 / 19958w,
`handwritten-jr-t2` 53 / 163w, `handwritten-cop` 22 / 123w,
`handwritten-syscall` 2 / 8w. The 33 alignment spans include the 4-byte pad at
`0x685C0` that the deep-size repair freed from the old `func_80077D30` span
(session cont. 19).

## Class 1 — `handwritten-jr-t2` (53 functions, 163 words)

Entire body is one or more repeated triples:

```
addiu $t2, $zero, 0xA0      ; 0xA0 = BIOS A0, 0xB0 = BIOS B0
jr    $t2
addiu $t1, $zero, 0x2F      ; delay slot carries the function index
```

Representative spans: `func_800719E4` (two triples, `0xB0`/`0x38` then
`0xA0`/`0x15`), `func_80071A04` (`0xA0`/`0x18`), `func_80071A54` (`0xA0`/`0x2F`),
`func_80073C5C` (`0xB0`/`0x3F`). All live in `asm/disc1/621E4.s`, `64B54.s`,
`6E538.s`, `6E6C0.s`, `75F44.s`.

**Why no C form reproduces it.** The natural C spelling is an indirect call:

```c
typedef void (*fn_t)(int);
void func_80071A54(void) { ((fn_t)0xA0)(0x2F); }
```

era `cc1 -O2 -G0` emits:

```
subu  $sp,$sp,24
sw    $31,16($sp)
li    $1,160
jal   $31,$1          ; jal through $at, callee in $1
li    $4,0x2f         ; argument in $a0
lw    $31,16($sp)
addu  $sp,$sp,24
j     $31
```

Retail has **no frame, no `$ra` save, no `lw`/`addu` epilogue**, uses `jr $t2`
(not `jal $31,$at`), keeps the function index in `$t2` and the argument in
`$t1`. This is an SDK/handwritten BIOS dispatcher; the `addiu $t1` delay-slot
argument and the frameless `jr` are not reachable from any C call shape. The
only way to emit it from C is inline `__asm__`, which the residual policy
forbids.

## Class 2 — `handwritten-syscall` (2 functions, 8 words)

`func_80072714` (`li $a0,1` / `syscall 0` / `jr $ra` / `nop`) and
`func_80072724` (`li $a0,2` / `syscall 0` / `jr $ra` / `nop`).

**Why no C form reproduces it.** C has no `syscall` operator. The only source
spelling is inline `__asm__`. Even ignoring the policy ban, era `cc1` renders
the return via `j $31` rather than the retail `jr $ra` + `nop`, so it cannot be
byte-identical.

## Class 3 — `alignment-filler` (31 spans, 31 words)

A lone `nop` between functions (the disassembler prints the preceding
`endlabel` then the `nop`). These are not functions at all — they are layout
padding required by the next function's alignment. They are counted separately
and are **not** part of the non-C-matchable function total.

## Class 4 — `handwritten-cop` (22 functions, 123 words)

Pure COP2/GTE primitives whose body is only COP ops plus `jr $ra`/`nop` (e.g.
`func_80078EB4`, `func_80078EE4`, `func_80078F4C`, `func_80079228`,
`func_80079024`). They are the classic Psy-Q `gte`/libgte internal helpers:

```
func_80078EB4:  lwc2 $0,0x0($a0) ; lwc2 $1,0x4($a0)          ; jr $ra ; nop
func_80078EE4:  lwc2 $0..$5 from $a0/$a1/$a2                 ; jr $ra ; nop
func_80078F4C:  mtc2 $a0,$16 ; $a1,$17 ; $a2,$18 ; $a3,$19   ; jr $ra ; nop
func_80079228:  mtc2 $a0,$30 ; nop ; nop ; mfc2 $v0,$31      ; jr $ra ; nop
func_80079024:  ctc2 $a0,$26                                 ; jr $ra ; nop
```

All are flagged `/* handwritten instruction */` by the disassembler itself
where a REGIMM/COP transfer is involved.

**Why no C form reproduces it.** C cannot name or type a COP2 register; there is
no GTE intrinsic in the era frontend and no `__asm__`-free spelling. Register
numbers ($16…$31, $26) and the exact COP op per argument are only expressible in
assembly.

## Class 5 — `handwritten-gte-wrapper` (107 functions, 19958 words)

The largest bucket: a span that either contains a **GTE command op**
(`rtps`/`rtpt`/`mvmva`/`nclip`/`avsz3`/`sqr`/`op`/`gpf`/…), or a contiguous run
(>=3) of raw GTE ops, or has no conditional branch and no call while still
containing a COP op. Members include:

- the whole `asm/disc1/68664.s` GTE library (`func_80078344` … `func_80079414`),
- the `asm/disc1/561C8.s` GTE wrappers (`func_80065E48`, `func_800661A4`,
  `func_800661CC`, `func_800665A0`, `func_8006698C`, …),
- the `asm/disc1/2A19C.s` / `2E8FC.s` model-transform families
  (`func_8003A088`, `func_8003A6A8`, `func_8003B144`, `func_8003B97C`,
  `func_8003AC90`, `func_8003C0B4`, `func_8003C2E0`, `func_8003E188`),
- the `asm/disc1/BF0F0.s` / `C5060.s` overlay GTE users that expand an `rtps`
  (four such spans were previously mis-bucketed as `gte-inline` — a single
  `rtps` is a literal Psy-Q `gte_rtps` macro, not an isolated register
  transfer),
- the `asm/disc1/B3390.s` overlay GTE users
  (`func_800C3324`, `func_800C42A4`, `func_800DE0A8`, …).

**Why no C form reproduces it.** A C front end cannot emit a COP2 op:
- the COP2 register transfers are all flagged `/* handwritten instruction */`
  by the disassembler — 1810 of the 2419 transfer ops in this bucket carry the
  flag (the remainder are the implied-register `swc2`/`lwc2` forms the
  disassembler does not annotate);
- the GTE command ops (`mvmva`, `rtps`, `rtpt`, `nclip`, `avsz3/4`, `sqr`, `op`,
  `gpf`/`gpl`, `intpl`, …) are Psy-Q/libgte macro spellings with no C operator;
- **0 of the 695 matched C leaves contains a single COP op**. Every genuinely
  C-derived leaf in this repo is COP-free, which is the empirical proof that
  `cc1` never synthesises one.

The surrounding integer code may still be liftable, so the honest end-state for
these spans is a C body plus an asm-side GTE primitive — or an ACCEPTED-RESIDUAL
disposition. Until then they are counted as non-C-matchable **as a span** rather
than inflating the "real remaining asm" column.

## `gte-inline` (5 spans, 192 words) — informational, NOT counted

A span containing only an isolated COP2 **register-transfer** pair (`mtc2` /
`mfc2` / `swc2` / `lwc2`, max contiguous run < 3, no GTE command op) inside
otherwise clean, branch/call-bearing integer code — i.e. the span is dominated
by ordinary liftable C and the transfer is a localised island:

`func_8003EAC8` (15w — `sw $a0,($sp)` / `mtc2 $a0,$30` / compare / `swc2 $31,($sp)`
/ reload / index a `D_800A76F0` table with `$a1`),
`func_800130B4` (77w),
`func_80077F7C` (32w — `D_800960AC` `$ra` save plus a COP0 `mfc0/mtc0 $12` +
five `ctc2` constant seeds),
`func_80078004` (33w — `mtc2 $a0,$30` / `mfc2 $v0,$31` then pure integer
table interpolation),
`func_80078094` (35w — same shape with two outputs).

These are excluded from the non-C total and reported for visibility. They are
the genuine GTE-adjacent frontier; `func_80077F7C`/`func_80078004`/
`func_80078094` additionally carry `/* handwritten instruction */` on their
integer ops (`sub`/`addi`), so the residual is the flag-plus-COP2 pattern being a
hand-written macro region rather than clean liftable C. `func_8003EAC8` is the
smallest and most clearly liftable.

## Deliberately not classified

- **`func_8001F814`** — already `NONMATCHING_C` (jump table in the `0x800`
  rodata pool, `configs/USA/disc1_nonmatching_sources.json` disposition
  `nonmatching-native-cut`). Distinct taxonomy; left alone.
- **The 24 ACCEPTED-RESIDUAL leaves** — compiler-decision residuals, not
  handwritten code. Distinct taxonomy; left alone.
- Large functions that merely *contain* a BIOS call (`func_8003BCE0`,
  `func_80067E1C`, `func_8007E334`, …) are **not** classified: their bodies are
  ordinary C-matchable code with a thunk call at the end.

## Manifest contract

`docs/evidence/non-c-matchable/MANIFEST.json` keys:

- `spans` — consumer (non-C) contract. Every key here except `alignment-filler`
  is a non-C-matchable **function**; `route_coverage.py` reads these names
  read-only and subtracts them from the asm column.
- `matchable_cop_inline` — retained for backwards compatibility; now carries the
  5 `gte-inline` spans (`class: "gte-inline"`). Consumers must keep treating it
  as informational, not as non-C.
- `counts` / `words` — per-class reporting totals.

The classification rule for a COP-bearing span is now: **any** GTE command op
(`rtps`/`mvmva`/`nclip`/…), **or** a contiguous COP run >= 3, **or** a body with
no conditional branch and no call, is `handwritten-gte-wrapper` (non-C). Only an
isolated COP2 register-transfer pair inside branch/call-bearing integer code is
`gte-inline` (informational).

## Reproduction

```
python3 tools/analysis/nonmatchable_spans.py            # counts
python3 tools/analysis/nonmatchable_spans.py --list     # per-span evidence
python3 tools/analysis/nonmatchable_spans.py --json
python3 tools/analysis/route_coverage.py --no-history    # three-bucket totals
```
