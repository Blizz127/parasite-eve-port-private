# TOOLCHAIN_REBUILD — rootless mipsel toolchain and the exact packed SHA-1

Status (2026-09-11): **the exact packed rebuild runs in this host environment
without docker, and produces the retail SHA-1.**

```
orig SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
cand SHA-1: 452fb033f2eaa4b18aa20a5bca60b8125af3a37b
RESULT: EXACT MATCH
...
VERIFY_US=PASS
matching-C count: 570 (from YAML)
```

The exact run was made against a **frozen snapshot** of the working tree
(`configs` + `src` copied to a scratch dir) because sibling workers were
editing `configs/USA/disc1.yaml` and `src/` during every split. The snapshot
needed three leaf corrections that are **sibling-owned matching edits** (no
`src/`/`disc1.yaml` file was changed in this repo) — see "Leaf findings".
Apart from those three leaves, all 570 C spans and 288 asm spans round-trip
byte-exact through this toolchain.

This document owns the build-environment facts. Matching progress and the
handoff remain in `docs/ai_context/ACTIVE_HANDOFF.md`.

## Environment / why rootless

- Ubuntu 26.04 (resolute), `uid=1000`, `sudo` requires interactive auth.
- No `docker`, `podman`, or `distrobox` → the documented `pe-mipsel-img`
  container cannot be entered.
- Ubuntu 26.04 apt has **no** `gcc-mipsel-linux-gnu` (`Candidate: (none)`);
  only `binutils-mipsel-linux-gnu` exists. So a native distro install is not
  possible non-interactively.
- `dev/mipsel/Dockerfile` pins `debian:trixie` + `gcc-mipsel-linux-gnu`
  (= **GCC 14.2.0-13cross1**, binutils 2.44), which is the compiler the
  `modern` build profile was matched against.

The fix is to unpack the **same Debian trixie packages** the Dockerfile
installs into a git-ignored tree-local prefix, with no root and no container.

## Install (idempotent, pinned)

```
scripts/setup_mipsel_host.sh
```

- Location: `tools/mipsel-host/` (now git-ignored via `.gitignore`
  `tools/mipsel-host/`). Debs cached in `tools/mipsel-host/.debs/`.
- Entry points: `tools/mipsel-host/bin/mipsel-linux-gnu-{gcc,as,ld,objcopy,readelf}`
  (shims that set `LD_LIBRARY_PATH` and re-export `PATH`, then exec the real
  binary under `usr/bin/`). GCC 14 finds its own `cc1` because Debian's
  driver is relocatable relative to `<prefix>/usr/bin`.
- Installed versions (verified):

```
mipsel-linux-gnu-gcc-14 (Debian 14.2.0-13) 14.2.0
GNU assembler (GNU Binutils for Debian) 2.44
GNU ld (GNU Binutils for Debian) 2.44
```

Packages, URLs, and SHA-256 (Debian trixie, host `amd64`; base
`http://deb.debian.org/debian/`):

| package | pool path | SHA-256 |
|---|---|---|
| `gcc-14-mipsel-linux-gnu` | `pool/main/g/gcc-14-cross-mipsen/gcc-14-mipsel-linux-gnu_14.2.0-13cross1_amd64.deb` | `04a58165ab8f9a1c692d5244b5608251e9b18649e8832a8226cd103de2fa878a` |
| `gcc-14-mipsel-linux-gnu-base` | `pool/main/g/gcc-14-cross-mipsen/gcc-14-mipsel-linux-gnu-base_14.2.0-13cross1_amd64.deb` | `3983042af2d6b7b365a588b394d538559a59a3d0d2cc2a0efac2bb3a23c99501` |
| `gcc-14-cross-base-mipsen` | `pool/main/g/gcc-14-cross-mipsen/gcc-14-cross-base-mipsen_14.2.0-13cross1_all.deb` | `5823a47dba0c833ce0381cd8e5cf21c9120df92a54a603685dedad8dbf6ea74a` |
| `cpp-14-mipsel-linux-gnu` | `pool/main/g/gcc-14-cross-mipsen/cpp-14-mipsel-linux-gnu_14.2.0-13cross1_amd64.deb` | `40fec73bd218c5766da2981103b8e8f66f7bc60de748d0673f1f10e657d8be43` |
| `libgcc-14-dev-mipsel-cross` | `pool/main/g/gcc-14-cross-mipsen/libgcc-14-dev-mipsel-cross_14.2.0-13cross1_all.deb` | `4b5fa7f019ebf939eeeeead8551b99cf2f777479b84af1d920193849fe969174` |
| `binutils-mipsel-linux-gnu` | `pool/main/b/binutils-mipsen/binutils-mipsel-linux-gnu_2.44-3cross1+nmu1+b1_amd64.deb` | `2514dd910f344c923531f959ea4ffd542e51640aac0caa1bc51ee293851788ab` |
| `binutils-common` | `pool/main/b/binutils/binutils-common_2.44-3_amd64.deb` | `002da5d23f8757dee97a2c0a40e0e1d4d85a43da094488ee2ee7068d4d3691f9` |
| `libsframe1` | `pool/main/b/binutils/libsframe1_2.44-3_amd64.deb` | `38f625dfdc582717029ac3a3e97c51d994ec2e7a0e9b230c6b44e40d1276311f` |
| `libmpc3` | `pool/main/m/mpclib3/libmpc3_1.3.1-1+b3_amd64.deb` | `2af0a5c128e03694a41c0b011bd8a958b7297436cdb3a15ddad7866dae8c300b` |
| `libisl23` | `pool/main/i/isl/libisl23_0.27-1_amd64.deb` | `ac8518042e81c00de1effb72bba7e88ac4ecd488f7ea8b9e3ebc63159cb53b35` |

No libc headers / target libc are needed: every `src/*.c` is freestanding
(zero `#include <...>`), the build compiles with `-c`, and links with
`-nostdlib`. `libmpc3` / `libisl23` are the only host runtime libraries Ubuntu
26.04 lacks for `cc1`; the shims provide them.

## Container-vs-native decision path

`tools/build/disc1_build.py::find_toolchain()` now resolves, in order:

1. the five `mipsel-linux-gnu-*` tools already on `PATH` (`note="host PATH"`);
2. the repo-local shim dir `tools/mipsel-host/bin` (prepended to `PATH`,
   `note="repo-local tools/mipsel-host/bin"`);
3. `distrobox enter pe-mipsel` if distrobox is installed;
4. else a hard error that now points at `scripts/setup_mipsel_host.sh`.

So the documented docker flow is unchanged (step 1 wins inside the image), and
the native path is usable on a host with no container runtime. The error text
was extended accordingly.

## Exact run (commands)

In the frozen snapshot (or in the repo once the three leaf fixes below land):

```
export PATH="$PWD/tools/mipsel-host/bin:$PATH"
scripts/split_us.sh              # splat; ~52 s
scripts/build_us.sh              # ~125 s; ends RESULT: EXACT MATCH
scripts/verify_us.sh             # 7/7 gates; ends VERIFY_US=PASS
```

Snapshot used for the recorded proof:

- `configs/USA/disc1.yaml` SHA-256 `62bbb0c86390d57a5cd3af119e7e569092b7d1fab3c5eac547413164c4692f5f`,
  570 C spans / 288 asm / 2 rodata.
- plan SHA-256 `b3f26413a65ddc886afdfda54fd2e464ce19a51e3303a05f99427cb97cf785a2`.

`scripts/split_us.sh` can still exit 1 in the live repo with
`ERROR: the split created files git does not ignore: src/func_*.c` — this is a
**race with concurrent sibling edits** (a `src/func_*.c` appearing between the
script's before/after `git status` snapshots), not a real ignore violation;
splat itself completed and the artifacts are valid.

## Assembler baseline: ASPSX 2.30 (2026-09-22)

The era pipeline (`cpp` -> `cc1` -> `maspsx` -> `as`) now runs maspsx with
`--aspsx-version=2.30` by default (`tools/analysis/era_link_check.py`,
`tools/build/disc1_build.py`, `tools/build/disc1_preflight.py`,
`tools/analysis/era_leaf_match.sh`; `ERA_ASPSX_VER` in a profile's
`environment` still overrides per leaf), and every driver now forwards cc1's
`-G<n>` to maspsx so its small-data classification matches what GNU as will
do. Evidence (assembler-version study, whole loadable image of
`SLUS_006.62` decoded minus the two declared rodata pools): the version gate
in maspsx flips exactly three switches below 2.30 (`nop_at_expansion`,
`nop_mflo_mfhi`, `addiu_at`). Above it, the only field that changes between
2.30 and 2.50 is `expand_li` (True -> False), and this build line always
passes `--dont-expand-li`, which forces `expand_li=False` at every version --
so **2.30 and 2.56 are the identical configuration for this project** (which
is why another decomp's 2.56 default agrees with the >= 2.30 evidence here);
2.60 flips `sltu_at`, 2.70 `gp_allow_offset`, 2.80 `gp_allow_la`, all inert
for a `-G0` corpus and only `gp_allow_offset` observable for `-G8` leaves
(one band's 91 leaves match identically at 2.30, 2.56, 2.60 and 2.70). The
default stays at 2.30 because that is the boundary the evidence pins. All
three retail signatures are unanimous for
>= 2.30 -- **34** `mflo`/`mfhi` -> `nop`/`nop` -> `mult` hazard pairs and **0**
`mult`/`div` within two instructions of a move-from; **138** `load` ->
`lui $at` -> store macros with no interposed nop and **0** `load`/`nop`/`lui
$at` anywhere; **1074** three-word `lui $at`/`addu $at,$at,rb`/`op
rt,%lo($at)` indexed expansions and **0** four-word `addiu $at` forms -- 1246
positive sites for 2.30, zero for 2.21, uniform across every 64K bucket that
holds code. The 2x2 over the study's 1045 era leaves was 1023 both / 3
2.21-only / 19 2.30-only / 0 neither; the three "2.21-only" leaves
(`func_800339A0`, `func_80046DFC`, `func_80012700`) were a maspsx defect, not
evidence: each is `load` / `nop` / single-instruction gp-relative store, and
maspsx skipped `.extern SYM, size` lines so `_uses_gp` never saw the symbol
as gp-relative and the mandatory MIPS-I load-delay nop survived only through
the version-gated 2.21 clause. Vendored maspsx local patch 7 records sized
externs that fit `-G<n>` and feeds them to `_uses_gp` (always on; durable
test `tools/era/maspsx/tests/test_extern_gp_load_delay.py`, in
`scripts/setup_era.sh` `MASPSX_TRACKED` and the `.gitignore` negations);
with it and `-G` forwarding the matrix is 1026 / 0 / 19 / 0. Patches 2 and 5
(`MASPSX_THREE_WORD_SYMBOL_STORE`, `MASPSX_SYMBOL_AT_TEMP`) emulate the 2.30
`addiu_at` expansion width under 2.21 and are retained as knobs, but their
per-leaf assignments were dropped wherever the leaf links exact without them
(65 leaves, plus the 3 `MASPSX_FILL_INDEXED_STORE_DELAY_SLOT` leaves that
matched without that knob at both versions; the 7 leaves that keep
`MASPSX_THREE_WORD_SYMBOL_STORE` are the `MASPSX_DISPATCH_FOLD` profiles,
where the flag is the gate the fold code requires); the 51 `*_aspsx_230`
assignments folded into their version-less profiles, and 9 more leaves whose
profile only existed to dodge 2.21's spurious pre-`$at` nop fell to the
default. Net: 128 leaves re-homed, 72 -> 56 profiles, no profile carries
`ERA_ASPSX_VER`. The vendored maspsx log now reads patches 1-9: 6 branch
fill, **7** the sized-`.extern` load-delay fix above, **8** the call/jump
delay-slot fill (`MASPSX_FILL_CALL_DELAY_SLOT`, ROM: 90 sites / 45 functions,
`tests/test_fill_call_delay_slot.py`), **9** the explicit `%lo`-store
passthrough that the per-leaf `ERA_CC1_VER=2.8.1` compiler needs
(`tests/test_lo_store_passthrough.py`); a corpus flag-off identity run
(every `src/*.c` leaf x `-G0`/`-G8` x 2.21/2.30 x 7 knob variants, 1605
leaves / 44940 pairs, merged maspsx vs the reconstructed patches-1-6 tree)
shows the additions byte-identical together: 0 diffs with every knob off, the
only knob-on diffs being the 40 leaves whose `MASPSX_SYMBOL_LOAD_DEST_TEMP`
output changes by design (the `dest == index` fallback to `$at`) and patch
7's mandatory nop under `-G8` (285 sites). Proof on the live plan (1650 c
spans, plan `d93bd0c660ba`, `disc1.yaml` sha256 `aef392ff…a0e1`):
`python3 tools/analysis/profile_necessity.py --refresh` ->
`1486/1486 era leaves clean; 0 hard defect(s); 0 redundant`,
`PROFILE_NECESSITY=PASS`; `python3 tools/build/disc1_preflight.py --deep` ->
`disc1_preflight: PASS (deep, 1650 c / 345 asm / 2 rodata)`, exit 0.
Maspsx suite: 241 tests OK.

## Leaf findings (sibling-owned; NOT edited in this repo)

Running the full link for the first time exposed three integrated C leaves
whose object-level match does not survive relocation/scheduling resolution.
All three were fixed in the scratch snapshot only.

1. `func_80086FF8` / `func_80087024` (`src/func_80086FF8.c`,
   `src/func_80087024.c`). Source declares `extern int D_800CCD80;`, which the
   build resolves from the symbol name to `0x800CCD80`. Retail's address is
   `0x800BCD80`: `lui at,0x800C` + `sw v0,-0x3280(at)` (`0x800C0000 - 0x3280`).
   Fix: name the global by its real address, `extern int D_800BCD80;`, in both
   files (the repo convention is address-named symbols and the linker's
   name-derived fallback then yields the right value). Rename-only was
   verified to rebuild exact.
2. `func_8006F2C4` (`src/func_8006F2C4.c`). The `unsigned int *flagsPtr =
   &D_800B0CD8;` is declared **before** the `D_800E10A0` clear loop, so cc1
   hoists the address materialization and allocates it to a caller-saved
   register; retail materializes `&D_800B0CD8` in `$v1` **after** the loop.
   Fix: declare `unsigned int *flagsPtr;` (or nothing) before the loop and
   assign `flagsPtr = &D_800B0CD8;` after it — cc1 then emits the retail
   `li $4,-131072` / `la $3,D_800B0CD8` / `lw $2,0($3)` / `ori $4,$4,0xffff` /
   `and` / `sw` sequence.

With those three files corrected (and nothing else), `build_us.sh` and
`verify_us.sh` both succeed and the packed SHA-1 equals retail.

## Per-leaf era cc1 version (`ERA_CC1_VER`)

`scripts/setup_era.sh` installs two stock decompals/old-gcc (release 0.17)
compiler pairs side by side: `tools/era/gcc-2.7.2-psx/` (the default, Psy-Q
`ccpsx` fingerprint) and `tools/era/gcc-2.8.1-psx/` (which ships its own `cpp`
and `cc1`; its `cpp` output is identical to 2.7.2's, the `cc1` differs). The
knob `ERA_CC1_VER` (`2.7.2` default, `2.8.1`) selects the pair for one leaf
exactly the way `ERA_ASPSX_VER` selects the maspsx dialect: put it in the
leaf's build-profile `environment` block and `tools/build/disc1_build.py`
(the SHA-1 gate), `tools/build/disc1_preflight.py --deep`,
`tools/analysis/leaf_strong_check.py` and `tools/analysis/profile_necessity.py`
all compile that leaf with `tools/era/gcc-<ver>-psx/{cpp,cc1}`; the per-leaf
helpers `tools/analysis/era_link_check.py` and `era_leaf_match.sh` read the
same variable from the shell (`ERA_CC1_VER=2.8.1 python3
tools/analysis/era_link_check.py src/func_X.c 0x... 0x... -O2 -G0`). The
default path is byte-identical to the previous hard-coded 2.7.2 path. The 2.8.1
`cc1` accepts the same flag set the profiles use (`-quiet`, `-O1`/`-O2`,
`-G0`/`-G8`, `-fno-delayed-branch`, `-mips1`, `-mcpu=3000`, `-funsigned-char`)
with no extra flags; it emits absolute stores as explicit `lui $r,%hi` /
`sw ..,%lo($r)` pairs (in a free temporary, not `$at`) and formats `li` as
`li $r,<decimal>`, both of which maspsx/GNU as consume unchanged. Why it exists:
a family of near-miss leaves is exactly "retail fills a `jr`/`jal`/`beqz`
delay slot that 2.7.2 leaves to a nop"; 2.8.1's scheduler fills those. A leaf
that uses it must carry an explicit `assignments` entry on a profile whose
`environment` sets `ERA_CC1_VER`, and `profile_necessity.py` proves the
assignment is load-bearing.

## Band-compiler search: negative result (2026-09-24)

Question: is the 0x80071A84-0x8008D610 band compiled by a cc1 that is neither
our 2.7.2 nor our 2.8.1 build? Parked band drafts show "mixed traits"
(2.8.1-style reorg slot steals, 2.7.2-style epilogue scheduling).
Every cc1 decompals/old-gcc publishes (latest release 0.17; no SN/Psy-Q
builds exist there) was installed side by side under
`tools/era/gcc-<ver>-psx/` (`ERA_CC1_VER=<ver>`; NOT fetched by
`scripts/setup_era.sh`, no build code changed) and each best draft was
link-checked at `-O2 -G0` (+ `-mno-split-addresses` where accepted, best of
the two, marked `m`), with each leaf's own maspsx knobs, ASPSX 2.30.

| leaf (draft) | 2.6.3 | 2.7.2 | 2.7.2-cdk | 2.7.2.3 | 2.8.0 | 2.8.0 plain | 2.8.1 | 2.91.66 | 2.95.2 |
|---|---|---|---|---|---|---|---|---|---|
| func_8007D1D4 (Q6, 2.7.2-tuned) | 15 | 15 | 116 | 15 | 62m | 62 | 62m | 151m | 136m |
| func_8007D1D4 (M5) | 22 | 22 | 115 | 22 | 62m | 62 | 62m | 151m | 136m |
| func_8007E964 (G1, 2.8.1-tuned) | 31 | 31 | 128 | 31 | 9m | 9 | 9m | 129 | 125 |
| func_8007E964 (E2, structural) | 66 | 66 | 128 | 66 | 86m | 86 | 86m | 129 | 124 |
| func_8007D778 (U1) | 142 | 142 | 139 | 142 | 43m | 43 | 43m | 130m | 83m |
| func_8007D074 (lane-5 draft) | 41 | 41 | 39 | 41 | 16m | 16 | 16m | 16m | 50m |
| func_800844E4 (lane-5 draft) | 53 | 53 | 64 | 53 | 35m | 35 | 35m | 79m | 78m |

Findings: 2.6.3 / 2.7.2 / 2.7.2.3 are code-identical on these leaves, and so
are 2.8.0 / 2.8.0-plain / 2.8.1; the 2.7.2 CDK build, 2.91.66 and 2.95.2 are
worse everywhere (2.91.66 ties only on func_8007D074). No version wins across
the board: func_8007D1D4 is best on the 2.7 family, the other four on the
2.8 family. So there is no better single band compiler among the published
builds; keep `ERA_CC1_VER` per leaf (2.7.2 or 2.8.1) and try both on every
band leaf. Also verified the same day: ASPSX 2.30 and 2.56 produce
byte-identical maspsx output in our pipeline (the only switch between them,
`expand_li`, is already forced off by `--dont-expand-li`), so khasinski's 2.56
default is not a separate dialect for us. Drafts and the matrix script:
`build/fanout/tooling2/` (drafts/, w/vmatrix.py; git-ignored).

## Reproduce from scratch

```
scripts/setup_era.sh            # era gcc-2.7.2-psx + gcc-2.8.1-psx + vendored maspsx
scripts/setup_mipsel_host.sh    # modern GCC 14.2.0 + binutils 2.44 (this doc)
export PATH="$PWD/tools/mipsel-host/bin:$PATH"
scripts/split_us.sh && scripts/build_us.sh && scripts/verify_us.sh
```
