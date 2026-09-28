# Psy-Q out of the port: classification, guard, replacement plan

Owner directives, 2026-09-28, via the Devbox Coordinator:

* The native port (`pc_port/`) must not compile or link any Sony Psy-Q SDK
  library code. That covers original objects and decompiled, matched or
  translated Psy-Q C. It also covers Psy-Q-derived data tables.
* The matching build keeps its Psy-Q TUs for byte-exactness. The port
  excludes them, and a check must fail if any of them is linked.
* The port ships only our code plus compatible open-source dependencies.
  Everything from the game is read from the user's disc at runtime.
* PsyCross (MIT) is the replacement layer. It is fetched, never committed.
* The layering follows `docs/ARCHITECTURE-PORT.md`: game C calls only
  `pe_plat_*`.

The full lane report with per-library reasoning, effort and risks is in
`build/lanes/psyq/PLAN.md`, which is local and git-ignored. This file is the
durable summary.

## Artifacts (tracked)

| path | what |
|---|---|
| `tools/analysis/psyq_classify.py` | Classifies every EXE function plus the ovl_039F libpress fragment. Writes `configs/USA/psyq_classification.tsv`. |
| `configs/USA/psyq_classification.tsv` | `addr name library confidence evidence sdk_name`. `confidence` is one of `signature`, `bios-stub`, `layout`, `band`, `overlay`, or `-` for game. |
| `configs/USA/psyq_data_ranges.tsv` | Psy-Q table **address ranges only**: the libgte rsin/SQRT/rsqrt/rcossin/ratan tables, and the libpress quant/scale blocks. The libcd 0/1 command tables are too low-entropy to scan and are tracked manually. |
| `tools/analysis/psyq_port_guard.py` | The guard, described below. |
| `tools/analysis/test_psyq_port_guard.py` | Self-test. Synthetic data only. |
| `pc_port/psyq_guard_allowlist.txt` | Today's offenders. It can only shrink; empty is the goal. |
| `pc_port/cmake/psyq_guard.cmake` | ctest `psyq-port-guard` plus `psyq-port-guard-selftest`. Needs `include(cmake/psyq_guard.cmake)` in `pc_port/CMakeLists.txt`, which is not added yet (the file is shared). |

Regenerate the classification:

```sh
python3 tools/analysis/psyq_classify.py --out configs/USA/psyq_classification.tsv
```

Its inputs are the oracle's `psyq_provenance.json` SDK signature matches,
its splat layout and symbols, `build/generated/disc1_plan.json` plus
`asm/disc1`, and the user's EXE for the BIOS-stub scan.

Run the guard:

```sh
python3 tools/analysis/psyq_port_guard.py --binary <bdir>/parasite-eve-port --binary <bdir>/pe-native-tests           # vs allowlist
python3 tools/analysis/psyq_port_guard.py --strict --binary <bdir>/parasite-eve-port --binary <bdir>/pe-native-tests  # goal state
cd tools/analysis && python3 -m unittest test_psyq_port_guard
```

## Classification result

* 2443 EXE functions in total. **625 are Psy-Q**:
  * 507 by SDK signature
  * 52 BIOS A0/B0/C0 stubs
  * 12 by oracle layout
  * 54 in the SDK link band 0x800718D0–0x80084FE4
* **1818 are game code**, including the AKAO driver.
* **24 are libpress functions** in ovl_039F.

Per library:

| library | functions |
|---|---|
| libgpu | 103 |
| libcd | 95 |
| libgte | 80 |
| libapi | 79 |
| libds | 68 |
| libspu | 59 |
| libpad | 56 |
| libetc | 31 |
| libcard | 25 |
| libpress | 24 |
| libmath | 18 |
| libc | 7 |
| libsn | 3 |
| libsnd | 1 (SsInit, 0x80036DC8, the only one outside the band) |

The gcc-2.8.1 band **is not all SDK**. The AKAO driver (Square) is
interleaved with libspu from 0x80084FE4 upward, so band membership is only
used below that address. Above it, only signature- or layout-proven objects
count as Psy-Q.

## Audit result (build/pcbuild-port, 2026-09-28)

* **Shipped `parasite-eve-port` links 438 Psy-Q functions.** By library:
  libgpu 85, libcd 85, libds 60, libpad 48, libgte 35, libspu 34, libapi 32,
  libetc 28, libcard 17, libpress 8, libc 3, libsnd 1, libsn 1, libmath 1.
* **Where they come from:**
  * 215 of the 946 generated `pc_port/game/decomp` TUs are Psy-Q leaves.
  * 67 hand-port files (17.1k lines) are Psy-Q translations or mixed.
    Worst: `cd_stream_port.c`, `pe_libcd.c`, `pe_libgpu.c`,
    `absent_cd_port.c`, `pe_gte.c`, `pe_libetc.c`, `pe_irq_delivery.c`, the
    libgpu `func_8007xxxx_port.c` set and `func_8010C89C_port.c`
    (DecDCTvlc).
* **Psy-Q tables:**
  * None in the shipped binary.
  * All of them in `pe-native-tests`, seeded in 36 test headers.
  * **PsyCross itself ships the libgte SQRT, rcossin and ratan tables,
    byte-identical to retail.** The PsyCross integration must exclude those
    headers and generate the tables in our code, with a disc-time equality
    test.
* **Retail bytes:**
  * The shipped binary holds only one small table: `func_80029810`'s rows
    from EXE 0x800106A4.
  * Tests are heavier. 76 of 451 test and data files (about 57 MB) embed
    retail data, code or SDK tables. The generators are the
    `pc_port/tools/pe_*_oracle.py --write-header` scripts.
* **Preview tarball** (`package_preview.sh`): clean. It holds four files and
  has its own guard. `package_runtime.sh` copies full disc images, so it is
  private only.
* **Disc validation today is geometry only.** There is no hash, serial,
  region or disc-number check.

## Guard status

* `--strict`: **FAIL**, with 1447 findings (the goal state is not met yet):
  * 458 Psy-Q definitions in 283 source files
  * 438 Psy-Q symbols linked into each binary
  * 50 game-TU backend includes
  * table hits in about 30 test headers and in `pe-native-tests`
* Against `pc_port/psyq_guard_allowlist.txt`: **PASS**.
* Self-test: **PASS**.

Checks:

1. `func_<psyq addr>` defined in `pc_port/`, or a `decomp-source:` header
   naming one.
2. Such a symbol linked in `--binary` (checked with `nm`).
3. Psy-Q table bytes, read from the user's disc, found in binaries or as
   literals in sources.
4. Game TUs (`pc_port/game/**`, `pc_port/bootstrap/**`) including PsyCross,
   Psy-Q, host/OS or in-house backend headers instead of `pe_plat_*`.

New findings fail, and so do stale allowlist entries.

## Plan summary (order, per step: route regression PASS, then allowlist shrinks)

| step | work |
|---|---|
| P0 | Guard ctest, `pe_plat/*.h` interface headers, `THIRD_PARTY.md`. |
| P1 | `gen_decomp_ports.py` excludes Psy-Q leaves by classification and emits a `psyq_entry_map.h` (`#define func_<psyq> pe_plat_*`). Pure entry points (BIOS libc, rand LCG, primitive setters, soft-float, rsin/rcos) become our own code. |
| P2 | Timing/events on `host_frame_pacer`/`pe_rcnt2`/`pe_vsync`. Add a trap so the AKAO interpreter never runs Psy-Q addresses. |
| P3 | Audio: `pe_plat_audio` on the in-house `pe_spu`. AKAO stays game C plus the interpreter. |
| P4 | Input (`host_pad`) and storage (`bu00:`/`_card_*` save files). |
| P5 | Disc: our own libds/St* queue model on `pe_disc`, plus the first-run disc check: known-disc table by serial and boot-EXE SHA-1 (USA D1 `c339455d…`/D2 `6dc5b537…`, EXE `452fb033…`), clear errors, multi-disc swap. |
| P6 | FMV: our own VLC decoder (from the MDEC bitstream spec) plus `pe_mdec`/`pe_str_feed`. |
| P7 | GTE: PsyCross software GTE plus our generated tables. |
| P8 | Renderer: PsyCross GPU (OpenGL) as the windowed backend. `pe_gpu` stays for headless, regression and tests. |
| P9 | Tests move to a disc fixture (`$PE_TEST_DISC`; skip with 77 when absent). `--strict` enabled in ctest and `package_preview.sh`. |

PsyCross fetch follows the Brave Fencer pattern:

* Pinned clone of `OpenDriver2/PsyCross` (candidate
  `e56e4cde1c2b8a15e0d4e38b26cdd9202e0d17e6`) into git-ignored
  `pc_port/third_party/psycross`.
* Our own CMake glob. The checkout's own CMake has a case-sensitive `*.C`
  glob bug and wrong OpenAL casing.
* Build-directory patches, never edits to the checkout.
* The MIT license is recorded in `pc_port/THIRD_PARTY.md` and shipped as
  `LICENSES/PsyCross.txt`.

PsyCross gaps that we must write ourselves: no libds, no St* ring streaming,
no libpress, no libsnd.
