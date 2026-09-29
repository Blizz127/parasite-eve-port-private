# Handoff to Codex — Parasite Eve decomp + native port

Written 2026-09-29 by the Claude lead session, when the owner moved the project
to Codex. Everything here is checkable from the repo; re-derive numbers rather
than trusting prose (see "Gate commands").

## 1. Goal and standing rules

- **Goal:** the whole game playable end to end in the native port, both discs
  (Days 1–6, the disc swap, the post-game content as retail has it), plus
  continued matching decomp. Ship a playable build to the owner at each
  milestone (each newly completable day). Day 1 is complete today.
- **1:1 preservation.** The port must behave like retail in gameplay, logic,
  visuals and audio. No approximations, invented behaviour or silent cuts.
  Game code runs the byte-matched decomp C wherever it exists. Hand ports are
  temporary and listed. Every difference goes in
  `docs/port/KNOWN_DIVERGENCES.md`, with its reason. "Matches retail" needs
  objective evidence: matched C, the emulator oracle (`build/oracle/`,
  PCSX-Redux + OpenBIOS), a retail-EXE interpreter oracle, or the retail
  video (`build/reference/retail-video/`, cited as "part NN @ mm:ss").
  Untested means say untested.
- **Allowed differences:** a faithful platform layer, and opt-in cheats (F1–F8)
  that are off by default and inert when off. The route aids under
  `--route-pad` are test harness only; `normal_start_shim_off` must stay OK.
- **No game data** in git: no disc images, extracted files or assets, and no
  retail byte runs in tracked files (`tools/analysis/retail_data_guard.py`).
  **No Sony Psy-Q code** beyond what KD #3 already lists
  (`tools/analysis/psyq_port_guard.py` must PASS).
- **PE stays private** (owner: "this is for me and to test on my Legion Go").
  No public release. The Windows build is on hold (feasibility notes in §8).
- `CLAUDE.md` (repo root) has the matching-decomp rules and levers.
  `docs/ai_context/ACTIVE_HANDOFF.md` has the long history.

## 2. Current state

- **main = `ca2bd4f2`** (clean, nothing uncommitted), on top of:
  - `16a267b1` — host signal handling (SIGTERM/INT/HUP → clean host-quit with
    summaries; SIGPIPE ignored; SEGV/BUS/ILL/FPE → fatal line plus backtrace),
    and the pad hotplug test.
  - `bcb49b8b` — r5 fixes: real-time movies (XRender server-side scaling,
    Release build by default), the warp guard, the Psy-Q guard clean-up,
    switch-over batches K–O and A1.
  - `ca2bd4f2` — test-only: save-overwrite regression tests plus the
    callback-whitelist audit (`docs/evidence/port-memcard-overwrite/`).
- **Decomp:**
  - EXE: 2252 matched C leaves. Check with
    `python3 tools/build/disc1_plan.py --check` →
    `2394 spans (2252 c, 140 asm, 2 rodata)`. The retail SHA-1 `452fb033…`
    is reproduced by `scripts/exact_rebuild.sh`.
  - Real remaining asm on the Day 1 route: 7 functions / about 2.5k words
    (`python3 tools/analysis/route_coverage.py --no-history`).
    - `func_800D9A8C` and `func_800DA1FC` are parked at 8 words (a register
      choice). See `docs/evidence/func-800D9A8C/PARK.md`.
    - Not yet attempted: `func_800C71E4`, `func_8001AE40`, `func_80068014`.
  - Overlays: `cat configs/USA/overlays/*.yaml | grep -cE ',[[:space:]]*c,'`.
    About 135 asm rows remain; the pick order is in
    `build/lanes/m348/`, the decomp agent's notes.
- **Port coverage:** `python3 tools/analysis/pc_port_coverage.py`. At r5 that
  gave about 1009 EXE functions from the decomp and about 1111 hand ports.
- **Open divergences: 21** (`docs/port/KNOWN_DIVERGENCES.md`; the header
  count must equal the number of open rows). Row numbers in use: up to #36.
  #33 and #34 are reserved by pending patches (§4).

## 3. Releases

| Build | Commit | Status |
|---|---|---|
| r3 | b45aee3e | superseded. **Defect:** built in a worktree without `build/extracted`, so every overlay fingerprint is 0 |
| r4 | 0999c817 | superseded (same defect) |
| r4b | 0999c817 | owner played it on the Legion Go. Reported slow movies, the warp freeze and the save-overwrite close; all fixed in r5 |
| **r5** | **16a267b1** | **`day1-r5-16a267b1`, handed to the launcher agent for the private owner channel** |

- **r5 package:**
  `build/release-final/parasite-eve-day1-day1-r5-16a267b1-linux-x64.tar.gz`
  - sha256 `32259e41359b23bf4fef8768361ce0730a7fd7176b9a848d7562ab70ddf2647d`,
    854913 bytes;
  - binary sha256 `b42d1eec…` (the stripped gated Release binary);
  - fingerprints 221/221; ldd shows libc only.
- **r5 gate:** suite 1616/1616 (0 skipped), Psy-Q guard PASS, REGRESS
  day1_end_80 PASS at frame 67186, MOVIES_DAY2 PASS, and a real-disc
  fullscreen boot to the title (`scaler=xrender`).
- The owner-facing notes for each build are in `docs/port/release-notes/`.

## 4. r6 queue: all as patch files, none committed

Every item below is saved as a patch on the WIP branch
**`wip/codex-handoff`**, under `wip-patches/`, with base commit and status
listed in `wip-patches/README.md`. Each was gated by its lane on its own base.
None has had the **combined** r6 gate. Apply them in the order in that README
and run the full gate once on the combined tree.

1. **Event log** (`eventlog-r5.patch`, base bcb49b8b; applies cleanly on
   16a267b1 at the text level).
   - Default-on `[EVENT]` lines in the run log, with ISO time and VBlank:
     title, new_game or continue, room transfers (token, story, prev),
     save and card_read, and every cheat toggle and warp.
   - Rate-limited (10/s, 1000 lines per run). Off with `PE_EVENT_LOG=0`.
     Log-only.
   - Lane gate: suite 1618/1618, normal_start OK, route PASS at 67186.
2. **Field-menu stops** (`r5-stops.patch`, base ca2bd4f2). The audit found
   two hand-port branches that stop the port:
   - item "Use" in the field for subtypes 2, 4–6 and 12–14
     (`func_80057834`);
   - `func_8004DCA4` with a non-zero argument.

   Both are fixed line by line from the matched C. The evidence is the
   retail-EXE interpreter oracles (INV10 at 213 cases, NAM9 at 151),
   compared by memory hash, plus a negative control. There's no live
   menu-navigation run; the evidence file says "believed correct; live-menu
   path untested". Lane gate: suite 1618/1618, route 67186, both guards PASS.
3. **A2** (`A2.patch`, base ca2bd4f2; stacks on r5-stops).
   - The new generator rule E20g (V0_PASSTHROUGH): `func_80086464` and
     `func_80087090` now come from the generator.
   - `func_8004D690` returns retail's `$v0`.
   - `func_80086608` is deliberately kept as a hand port, because it calls
     the `PE_M34StackInvalidate` hook.
   - Lane gate: suite 1622/1622, route 67186, guards PASS.
4. **CD load timing, step A plus the harness ammo aid**
   (`stepA-bcb49b8b.patch`, `route-pilot.patch`, base bcb49b8b).
   - Disc reads take emulated time (sector rate, seek), and retail per-sector
     bookkeeping is followed.
   - The global `VSync(-1)` returns the retail VBlank count `D_800956AC`.
   - The #33 room-load loop caps are removed. They were a test-fixture
     artefact, and retail's loops are unbounded.
   - Evidence: the port's field-tick count moves 47% of the way toward the
     emulator oracle (+275 → +146 ticks; see
     `docs/evidence/cd-timed-reads-stepA/REPORT.md`).
   - **This shifts the Day 1 route end frame to 66356.** Timing changes the
     Eve 1 battle seed, so `route-pilot.patch` adds a route-only ammo floor
     before Eve 1 (harness aid, logged `[ROUTE_SHIM]`).
   - Lane gate: suite 1617/1617, route PASS at 66356, MOVIES_DAY2 PASS.
   - Not yet modelled: the read-end CD Pause, which needs the host libcd
     command queue ("step A2"), and the seek-time curve ("step B",
     calibrate on several retail-video load points).
5. **Warp door value** (`warp-door-only.patch`, base 95c67a74/bcb49b8b; only
   `plat_cheats.c`).
   - Each warp target sets the entry-door word `0x800A77F4` (persist[1]),
     taken from a VM trace of the retail exit scripts.
   - **The "M0027I defect" is not a port defect.** M0027I and M0028I are
     walk-in trigger rooms, so they wait until Aya moves into a zone. An idle
     pad after a warp shows them dark and parked; holding Up starts the
     fight. Only the door value was genuinely missing.
   - The new-game sweep was interrupted by this handoff. So far: a warp
     during FMV002 waits and fires after the hand-over (target 0 LIVE).
   - The "snapshot B" state-restore design is **not needed** unless a target
     fails for a state reason the door value doesn't explain. It would need
     disc-derived RAM, so it isn't built.
6. **day1 lane patches** (base 95c67a74, rebased r95 versions). Unit-tested
   only unless noted; none has had a full route gate.
   - `gte-lowering-batch1.r95.patch`, which contains:
     - the COP2 engine `pe_gte_cop2.c`, which passes a real-console GTE log
       1100/1100 (JaCzekanski ps1-tests `gte-fuzz`, vendored MIT);
     - a one-line `pe_gte.c` MVMVA fix, which changes gameplay rendering, so
       it needs a first-room oracle pixel diff;
     - the instruction-level inline-GTE lowering tooling;
     - batch 1 (`func_8003EAC8`);
     - KD #34.
   - `ram-mirror.patch`: the 2 MB RAM mirrors across the first 8 MB of each
     segment, per psx-spx, hardware-exact. It touches every guest access, so
     it needs suite plus route.
   - `gm-core.patch`: the guest-model backend, which turns pointers into
     32-bit guest addresses, with pycparser vendored and pinned. It uses a
     two-phase manifest, and its allowlist is empty, so it is inert until
     batches are enabled.
     - Dry run: 1954 of the 2443 pointer-blocked bodies would translate.
     - Guest frame window `0x801F8000–0x801FBFFF`. It needs the
       `PE_GFRAME_AUDIT` write-watch over the route plus movies before use.
   - `boundary-policy.patch`: one boundary policy. In play a boundary is
     logged and play continues. Gates run `--strict-stubs`, and arity-0
     boundaries are now caught.
7. **In progress when stopped** (see `wip-patches/README.md`):
   - port3: making `func_8003AF14` (Render_DrawEntity, KD #35) match the
     matched C statement by statement.
     - The "lighting cut" in the KD row was a misnomer, and the flag dispatch
       is equivalent.
     - The two guards are to be dropped. `dest==0` is provably unreachable.
       The RAM guard itself diverges from retail: on actor-model pool
       exhaustion retail writes through KUSEG 0x50.
     - The oracle pixel diffs and the Day 1 peak pool measurement are still
       to do.
   - audio3: clean-room replacement of the linked Psy-Q `SpuSetVoiceVolumeAttr`
     call in `pe_stream.c`. Its bin allowlist rows are "pending replacement".

## 5. Gate commands (run on the exact tree to be committed)

Build in a clean worktree. Link `build/extracted` and the disc folders into it,
because `build/` and `rom/image/` are git-ignored. `gen_overlay_ports.py` now
falls back to the main checkout or fails loudly.

```sh
R=$PWD   # main checkout
git worktree add --detach build/wt-gate <commit>
cd build/wt-gate
mkdir -p build && ln -s $R/build/extracted build/extracted
for d in $R/rom/image/*/; do ln -s "${d%/}" "rom/image/$(basename "${d%/}")"; done
export TMPDIR=$R/build/tmp          # keep off the /tmp tmpfs
JOBS=3 scripts/build_pc_port.sh --no-tests --build-dir $R/build/pcbuild-gate   # Release by default
grep -c ', 0x00000000u, pe_gct' pc_port/game/decomp_ovl/_dispatch/zz_dispatch.c  # must equal the main checkout's count at the same commit (EXE-only; 2122 at r5)
cd $R && build/pcbuild-gate/pe-native-tests          # from the repo root (or set PE_DISC1_BIN), otherwise disc tests SKIP; need "0 skipped"
cd build/wt-gate && python3 tools/analysis/psyq_port_guard.py --tree pc_port \
    --binary $R/build/pcbuild-gate/parasite-eve-port --binary $R/build/pcbuild-gate/pe-native-tests   # PASS
DISC="$R/rom/image/Parasite Eve (USA) (Disc 1)/Parasite Eve (USA) (Disc 1).bin" \
  pc_port/tools/day1_route_regress.sh --bin $R/build/pcbuild-gate/parasite-eve-port --until day1_end_80
#   expect "REGRESS ok normal_start_shim_off" and "REGRESS PASS until=day1_end_80 frame=67186"
#   (66356 once CD step A lands)
cd $R && bash pc_port/tools/movies_day2_check.sh $R/build/pcbuild-gate/parasite-eve-port build/tmp/movies   # MOVIES_DAY2=PASS
python3 tools/analysis/retail_data_guard.py --staged     # before committing
```

Decomp gates: `scripts/exact_rebuild.sh` (EXACT_REBUILD_GATE plus
VERIFY_SWEEP) and, per overlay,
`flock build/lanes/ovl.lock scripts/exact_rebuild_overlay.sh <id>`. Run
`disc1_plan.py --write-status` first, keep at least 3 GB free, and run one
gate at a time.

Pitfalls learned today:
- Run the route from a worktree only with `DISC` set to an absolute path.
- Never `pkill -f` or `pgrep -f` a pattern that appears in your own command
  line.
- Cap any per-frame capture (`PE_SHOT_VSYNC_MAX`, `PE_CD_TRACE_MAX`); a
  runaway once filled 23 GB.

## 6. Packaging for the launcher (private owner channel)

1. Build and gate the commit as in §5 (Release).
2. From a clean worktree at that commit, run:
   ```sh
   PE_BINARY=$R/build/pcbuild-gate/parasite-eve-port PE_SOURCE_COMMIT=$(git rev-parse HEAD) \
   PE_RELEASE_VERSION=day1-rN-$(git rev-parse --short=8 HEAD) PE_RELEASE_LIMITS=<notes.txt> \
   bash pc_port/package_release.sh
   ```
   The output lands in that worktree's `build/release/`; move it to
   `build/release-final/`.
   - `version.txt` must be new: the launcher refuses an unchanged id.
   - `package_release.sh` refuses unoptimised binaries (see
     `PE_ALLOW_UNOPTIMISED`) and runs the retail-data guard and the ldd check.
3. Verify the package:
   - `python3 pc_port/tools/check_release_fingerprints.py <extracted>/bin/parasite-eve-port`
     → N/N (221/221);
   - the packaged binary equals `strip --strip-unneeded` of the gated binary;
   - boot the extracted `./parasite-eve <disc.bin>` under
     `xvfb-run -s "-screen 0 1920x1200x24"` to the title, and check that
     `scaler=xrender` is logged.
4. Hand the path, sha256, MANIFEST and notes to the coordinator. The
   launcher agent publishes to the private `parasite-eve/` bucket. The lead
   never publishes.

**Owner machines:**
- **Legion Go:** Bazzite, gamescope, 1920x1200 fullscreen, FPS limit 60.
- **Alienware:** KDE Wayland/Xwayland, 2560x1600.

The logs are in `~/.local/state/parasite-eve-port/logs/` on each machine. The
coordinator can have banshee-realm-client fetch them read-only into
`build/lanes/legion/` or `build/lanes/alienware/`.

## 7. Next steps (suggested order)

1. Run the combined r6 gate: §4 items 1–5, then the day1 patches one at a
   time, with the extra checks their notes list. Package r6.
2. Finish `func_8003AF14` (KD #35) and the SPU voice-volume replacement.
3. Extend the route past Day 1. Add a route regression for each day, starting
   with Day 2 (the movies-on run already reaches story 0x90).
4. Model the disc swap (KD #28, `func_80069B08`) before the disc-1/disc-2
   boundary at story ≥ 0x258.
5. Continue the decomp: the 3 big route bodies, then the overlays by twin
   count. gm backend batches unlock most of the remaining hand ports.

## 8. Windows build (on hold)

Feasible, but not trivial.
- **What exists:** CMake already branches for WIN32/MSVC, there is a GDI
  window backend (`host_window_win32.c`), and it uses the same scaler.
- **What's missing:** a Windows audio backend (WASAPI or winmm), controllers
  (XInput; `host_pad.c` is Linux `/dev/input/js*` only), %APPDATA% paths, and
  a Windows launcher.
- **Toolchain:** MinGW-w64 cross-compiling needs `apt install mingw-w64`,
  which needs owner approval.
- **Estimate:** half a day to a first build, 1–2 days to a complete one.
