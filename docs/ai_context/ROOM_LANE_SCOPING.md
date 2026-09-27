# Room tier scoping — PE.IMG room package chunk 2

Status: scoped 2026-09-22 (overlay lane). One proof-of-concept target,
`room_m0022i`, is configured and gated; the rest of this file is the census
that decides how the room tier should be run. The subsystem-overlay lane
reference is `OVERLAY_LANE.md`.

## 1. Census (reproduce: `python3 tools/analysis/room_census.py`)

Read-only: the script reads the EXE room table (`D_80093378`, 438 slots) and
every chunk 2 directly from the extracted PE.IMG; nothing is written.

```
slots 438 with chunk2 414
code rooms 185 framed prologues 4018
functions 9117 words 701609                 # jr $ra-delimited bodies
unique unmasked 1820 words 241290
unique jal/lui-masked 1500 words 192397
unique masked 623 words 106405              # + %lo of lui-based ld/st/addiu masked
masked fan-in {'1': 341, '2-9': 201, '>=10': 81} words {'>=10': 10124, '2-9': 32169, '1': 64112}
shared masked bodies 282 at one offset in every room 75
shared masked bodies byte-identical everywhere 148
code rooms with a whole-chunk twin 0 groups 0
code rooms sharing an identical text window 156 distinct texts 66
rooms in table order that introduce new bodies 48
rooms with most >=10-fan-in bodies [('m0393i', 58), ('m0396i', 58), ('m0401i', 58), ('m0392i', 56), ('m0186i', 46)]
text groups (size:first room): [(30, 'm0022i', '0x1e4', '0x3500'), (9, 'm0014i', …), (9, 'm0167i', …), (8, 'm0203i', …), (7, 'm0017i', …), (7, 'm0162i', …), …]
distinct text words 284027
groups with differing text_start 0
```

### Structure of a code chunk 2

`[header + rodata][.text][data]`, loaded at `0x8018EFE8`:

* header: `u32` (≈ chunk size, e.g. `0x1A438` of `0x1A800`), `u32`, `u32 0x10`,
  then the `.rodata` switch jump tables as **absolute** `0x8018xxxx` pointers
  into the text (`m0022i`: `0xC..0x1E4`; `m0418i`: small ints `0x1C..0x7C`,
  then tables `0x80..0x258`). A few rooms carry no pointers here (e.g.
  `m0005i`: small ints, text from `0x30`).
* `.text`: contiguous and position-dependent. The most common start is `0x1E4`
  (71 rooms).
* data after the last `jr $ra`: behaviour/handler pointer tables
  (`0x8018F1CC …`, again absolute), then room data.

The chunk has **no relocations**. Every code and data reference is an
absolute address at `0x8018EFE8`, so each room is one fixed-address splat
`code` segment, exactly like a subsystem overlay.

### Survey refresh (memory figures vs. now)

| figure | survey | this census | note |
| --- | --- | --- | --- |
| slots with chunk 2 | 414 | 414 | same |
| code rooms | 185 | 185 | same |
| framed functions | 4,015 | 4,018 framed prologues | ≈ same |
| unique bodies | 1,202 / 205K words (jal/lui immediates masked) | 1,500 / 192K words (jal/lui masked), 623 / 106K words (also %lo masked), 1,820 / 241K words byte-identical | this census splits at every `jr $ra` rather than at prologues, so the counts are body *fragments*; the order of magnitude agrees |
| fan-in | 45 in ≥10 rooms, 656 in 2–9, 672 bespoke | 81 / 201 / 341 masked bodies | different masking and splitting; same shape |
| rooms introducing all code | 66 | **66 distinct text windows**; 48 table-order rooms introduce every masked body | the 66 is exactly the distinct-text count |

## 2. What this means for targets

* **No whole chunk is duplicated** (data differs in every room), but **156 of
  the 185 code rooms share a byte-identical text window** with another room.
  There are only **66 distinct texts** (284K words), and in every group the
  window sits at the **same offset**. A leaf matched in one room of a group is
  byte-identical in every room of that group with no relink. The largest
  group, `m0022i`'s, covers 30 rooms.
* **Across different texts, shared bodies mostly move.** Only 75 of the 282
  masked bodies found in ≥ 2 rooms sit at one offset everywhere, and only 148
  are byte-identical everywhere. Every other shared body differs in its
  `jal`/`lui`/`%lo` addresses, so one C file must be compiled once and linked
  once per room at that room's addresses. This is the
  `src/overlays/room_common/` relink proof that `OVERLAY_LANE.md` §3 plans.
  The byte-identical ones need no relink.
* Recommended run order:
  1. one target per **distinct text**, 66 targets, keyed by the first room of
     each group (`room_m0022i` first: 30 rooms). The remaining rooms of a group
     are covered by a cheap check: text bytes identical at the same offset;
  2. match the `≥10`-fan-in bodies in the high-count texts (`m0393i`,
     `m0396i`, `m0401i`, `m0392i` each hold 56–58 of them). Once the
     `room_common` relink proof exists, propagate each matched body to every
     other text that contains it;
  3. the bespoke bodies last.
* Room ids stay `room_m####i`; the directory `src/overlays/room_<id>/` is the
  namespace (the `0x8018EFE8` arena is shared by every room, ovl_03D2 and
  ovl_0700).

## 3. Proof of concept: `room_m0022i`

`python3 tools/extract/peimg.py room m0022i` → `room_m0022i_c2.bin`, slot 21,
PE.IMG sectors `[0x3163,0x3198)`, size `0x1A800`, vram `0x8018EFE8`, SHA-1
`3231b287bdde6fb55bcc3734f5be6ff4906ab0c3`. Config
`configs/USA/overlays/room_m0022i.yaml` (+ `_build_profiles.json`, manifest
entry `room_m0022i`, `kind: room`):

```
- [0x0, bin]      # header + jump tables
- [0x1E4, asm]    # .text: 53 jr $ra, 18 framed prologues, last jr $ra 0x34F8
- [0x3500, bin]   # behaviour tables + room data
- [0x1A800]
```

```
$ scripts/exact_rebuild_overlay.sh room_m0022i          # all-asm
EXACT_REBUILD_GATE[room_m0022i]=PASS plan=cde05ce3f7637235371210a809cb5bd9494c8166d5dd6a53d4d4197b4d78ea89 yaml=772d0ed9… spans=[0 c, 1 asm, 0 rodata, 2 bin] sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3
$ scripts/exact_rebuild_overlay.sh room_m0022i          # 3 C leaves
EXACT_REBUILD_GATE[room_m0022i]=PASS plan=80412351f8f0aa82d5e007f591e0eae2febfa639b949667bcf00d7b37404c706 yaml=48e54b2123f171899c7dba43ed58c4e99e1d70d23353d56585a364761691ce3d spans=[3 c, 3 asm, 0 rodata, 2 bin] sha1_orig=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1_cand=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3
```

Leaves (`era_o2_g0`, evidence under `docs/evidence/room_m0022i-func_*/`):
`func_8018F1CC` (0x1E4, 0x8, `return 0` stub), `func_801910F8` (0x2110, 0x8,
empty stub), `func_801924C4` (0x34DC, 0x1C, state byte = 4 + clear linked
word). This text has **15 more two-word `return 0` / empty stubs**; the same
stub bodies recur across rooms.

Hazards:
* `scripts/extract_overlays.sh --check` (no `--only`) now also expects
  `room_m0022i_c2.bin`. Extract it with `--room m0022i` or
  `peimg.py room m0022i`; plain `extract` skips `kind: room` entries by design.
* `split_overlay.sh` sometimes fails its ignore check when another lane writes
  `docs/evidence/` at the same moment; retry (1 of 3 runs hit it here).

## 4. Status (2026-09-24, overlays2)

All 66 distinct room texts are now configured targets (`room_<first room of the group>`, plus the
8 zero-port texts). Each one's all-asm window was checked by the gate, and header windows were
repaired where needed (OVERLAY_LANE.md §4.4). The tier was run with `tools/analysis/room_lane/`:
hand-written leaves in a few high-fan-in texts (room_m0022i, m0167i, m0174i, m0137i, m0141i, m0421i,
m0145i, …), then three port passes that re-target every masked twin into every other text. All 66
room gates PASS. Leaf counts per target are in the OVERLAY_LANE.md §4.4 table. The
biggest unmatched shared bodies are the eight inline-GTE behaviours (39 texts each).

### Classified non-C: inline-GTE room bodies (ruling 2026-09-24)

These room bodies contain Psy-Q inline GTE macros (`ctc2` rotation-matrix load, `lwc2` vector load,
`mvmva`, `swc2` result store; flagged handwritten by the disassembler). Under the current rules they
are the EXE's `handwritten-gte-wrapper` class. They stay `asm` and are **not** lifted. Names use
room_m0022i numbering; each occurs in 39 of the 66 distinct texts:
`func_8018F630`, `func_80190370`, `func_80190D6C`, `func_80191100`, `func_80191884`,
`func_80191D08`, `func_8018FC58`, plus `func_80192344`. The m0391i body `func_8018F574` (6 texts)
is in the same class.

### Why every port is link-checked before carving (2026-09-24)

The masked-body hash can call two bodies twins when they are not the same C. The mask clears
`lui` immediates and every `%lo`-style immediate whose base register came from a `lui`. That also
covers a later `addiu $v0,$v0,-K` on a register that was loaded as `lui/addiu D+field`. Example: the
dry run after the port.py field-offset fix proposed two ports of room_m0273i
`func_8019A70C`, a field store at `+0x2E` that returns `&D` via `addiu $v0,$v0,-46`, into room_m0350i
(`func_80199E3C`, `func_8019A120`). There the field is at `+0x2A` (`addiu $v0,$v0,-42`). The hashes are
equal, but the re-targeted C links one word off. `doroom.sh` therefore requires `era_link_check` `LINK_EXACT`
at the target VMA for every port. A non-exact port goes to the drafts directory and is never carved,
which is what happened here. Never carve a port on hash identity alone.
