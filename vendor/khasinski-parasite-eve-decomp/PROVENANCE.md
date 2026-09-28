# Provenance and notice — khasinski/parasite-eve-decomp (vendored)

This directory is a verbatim, unmodified copy of the upstream repository tree
(the `.git` directory is the only thing not copied). No file in it has been
edited or stripped. This PROVENANCE.md is the only file added here.

| Field | Value |
|---|---|
| Source repository | https://github.com/khasinski/parasite-eve-decomp |
| Author / copyright holder | khasinski (Chris Hasinski) and that project's contributors |
| Upstream commit | `3067919e7b82f0334b873117eef07e7157f4ed69` |
| Upstream commit date | 2026-09-28 06:43:25 +0200 |
| Vendored on | 2026-09-28 |
| Target | Parasite Eve (USA) `SLUS_006.62`, SHA-1 `452fb033f2eaa4b18aa20a5bca60b8125af3a37b` (same binary as this repo) |
| Upstream license | none chosen: see `LICENSE.md` in this directory, kept verbatim and reproduced below |

## Upstream LICENSE.md (verbatim)

```
# License

No open-source license has been selected for this release candidate.

The repository is prepared as a source-only public-review tree. It excludes game
data, generated assets, proprietary SDK files, compiler binaries, and generated
raw split output. Until a project license is chosen, all project-authored source
remains under the default copyright of its authors.

Separately licensed adapted source is identified in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md); its original license is retained.

Parasite Eve and related trademarks/copyrights belong to Square Enix. Sony PSYQ
SDK materials belong to their respective owners and are not distributed here.
```

Upstream `THIRD_PARTY_NOTICES.md` (the MIT notice for psy-q-decomp-derived
source) is also kept verbatim in this directory.

## Owner authorization

On 2026-09-28 the owner of this repository (Blizz127) directed, through the
Devbox Coordinator, that existing Parasite Eve decompilations be reused:
copying is explicitly allowed, including from this unlicensed repository, on
the conditions that (1) provenance is tagged `vendor-derived` and attribution
is kept, and (2) every imported function still passes this repository's
byte-exact gates. After being told that the upstream has no license and that
this repository is public, the owner confirmed: "push it publicly anyway".

## How imported code is used

* Imported functions live in this repo's normal layout
  (`src/func_X.c`, `src/overlays/<id>/func_X.c`). Each starts with the header
  `/* vendor-derived: khasinski/parasite-eve-decomp@<commit> <path> — attribution khasinski (Chris Hasinski) */`.
* An imported file is the upstream translation unit run through the era
  `cpp` with the upstream CPP flags (`-undef -P -I include -I tools/m2c`), with
  the upstream symbol names mapped to this repo's address names from the
  upstream `configs/USA/sym.*.txt` tables. No function body is rewritten.
* Every imported function is proven by this repo's own gates
  (`era_link_check.py` LINK_EXACT, `disc1_preflight.py --deep --only`, and
  `scripts/exact_rebuild_overlay.sh <id>` = PASS for overlays). The per-function
  record is in `build/lanes/vendor/results.jsonl` (lane-local), and the committed
  per-batch evidence is `docs/evidence/vendor-khasinski-2026-09-28/REPORT.md`.
* Parasite Eve is (c) Square Enix. No game data, SDK files or compiler binaries
  are in this directory (upstream is source-only; audited 2026-09-28: 0 binary files).
