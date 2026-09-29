# Native port: known divergences from retail

**Purpose: preservation** — the port preserves the game exactly as it shipped.

**Standard (owner, 2026-09-28): the port must behave 1:1 like retail —
gameplay, logic, visuals and audio. No approximations, invented behaviour or
silent cuts.** The only allowed differences are:

1. the platform layer (host rendering, audio output, input, disc access),
   which must reproduce PS1 hardware behaviour faithfully, and
2. opt-in mods/cheats, off by default and inert when off.

Everything else that differs from retail is a divergence and is listed here
with its reason until it is fixed. "Matches retail" is only claimed with
objective evidence: a byte-matched C path, a numeric comparison against the
matched code and disc data, or an emulator/video diff. Anything not checked is
marked **untested**.

Last updated: 2026-09-29. **Open divergences: 17.**

## Structural (whole classes of code)

| # | Divergence | Evidence / reason | Status |
|---|---|---|---|
| 1 | 1167 EXE functions run as **hand-translated** ports, not the matched decomp C (104,889 words, 72% of EXE code the port runs; only 946 functions / 15.3% come from decomp-derived C) | `tools/analysis/pc_port_coverage.py` | Being switched to matched C in route-tested batches; each hand port is unverified until switched or diffed |
| 2 | 40 stubbed and 88 absent EXE functions | same tool | Replace with matched C or faithful ports |
| 3 | 438 Sony Psy-Q library functions are linked (libcd/libds, libgpu, libgte, libetc, libpress, libspu, libpad, libcard) — via generated decomp and hand translations | `tools/analysis/psyq_port_guard.py` | To be replaced by PsyCross/own platform code (`docs/ai_context/PSYQ_PORT_REPLACEMENT.md`); also a licensing blocker for public release |
| 4 | Overlay (room/battle) code: coverage not yet measured the same way | — | **Untested**; the shadow audit will report it |

## Platform layer (hardware behaviour not yet faithful)

| # | Divergence | Reason | Status |
|---|---|---|---|
| 5 | Movies are fed by a host-side stream producer instead of the modelled CD drive's interrupt-driven delivery; ~2 s read-ahead cap; no seek/spin-up timing | `pc_port/platform/pe_str_feed.c` | Open |
| 8 | SPU main volume assumed 0x3FFF; SPU IRQ and CD-DA not modelled | `pe_spu.c` | Open |
| 9 | The field-VM host argument frame lives in guest RAM at 0x801FF400 (port-only scratch) | `func_80017018_port.c` | Also covers GA_POLY/GA_DESC/GA_EA_SLOT, moved out of the overlay window into the same audited block. Audited: nothing else writes it (`PE_VMFRAME_AUDIT=1`, bad=0 over ~1.6M opcodes); retail uses the real stack instead, so it stays listed until the port runs the matched VM |

## Game behaviour (open bugs, observed by the owner or tests)

| # | Divergence | Evidence | Status |
|---|---|---|---|
| 13 | Aya's gun is not drawn when she readies it before the rat fight | owner report | Open |
| 15 | Plaza: the date's walk to the door with Aya after the dialogue (retail part 01 @ 5:16) not yet observed in the port | owner report; emulator oracle | **Untested past the dialogue**: every date state observed matches retail (arrival anims 0B→00→01→00 confirmed after the turn fix; plaza anim 04 standing identical to the oracle through VSync 16500, `persist[0]` bit 2 clear in both). The walk needs the player input the video used; parked (cosmetic) |
| 16 | Unported room-effect callbacks are skipped with a `[DAY1_SHIM] … not ported` line, so those effects do not draw (M34 phase 2: 80191A98, 80191FE0, 80190E98 — no matched C yet — and 8019253C; M0359I 8018F238; rooms A8002148/A8001148) | port logs | Port from matched C where it exists |
| 17 | Eve's flash: when the caller-stack ring position is unresolved, the first frame uses the owner joint position | `m0023i_effect_port.c` | Open |
| 18 | Police-siren ambience after Carnegie Hall | owner report (unsure) | **Untested** — to be checked against the retail video first |
| 19 | Frame pacing paces one host frame per emulated VBlank at 59.94 Hz | `host_window.c` | Believed faithful (240 Hz sequencer tick measured retail-exact); window-side timing **untested** on the owner's PC beyond his playtest |
| 20 | Second Eve battle: the music's vocal/choir layer is missing (instrumental plays) | owner report on r2; reproduced on the r2 binary (vocal-window band r 0.30 vs retail) | **Not reproduced on the current tree** (full-fight 600–1500 Hz band correlation vs retail part 02 @ 1:23–2:20: median r 0.58, vs 0.21 at a wrong offset). r2 lacked them (reproduced). Not caused by EA100/101 (a controlled run with them suppressed keeps the vocals) or func_8006F044 (not on the Day-1 path). The r2→current change that restored them is unidentified. Regression check: `pc_port/tools/audio/eve2_vocal_check.sh` (current 0.59 PASS, r2 0.30 FAIL) |
| 26 | The field-VM dispatcher `func_80017018` is a hand port; its inline opcode re-implementations are being switched to the generated matched handlers (batch 1: 42 opcodes done, suite + route green; unlisted opcodes now use the guest-code registry instead of stopping) | shadow audit | In progress (port3); note the matched `src/func_80017018.c` itself has mislabelled case labels (see ACTIVE_HANDOFF) |
| 28 | Disc swap: `func_80069B08` (insert-disc prompts, lid open/close poll, re-validate) is a stub, so past the disc-1/disc-2 story boundary (story ≥ 0x258) the port skips the prompt and keeps reading disc 1. **Not on the Day 1 path** (0 hits on the route) | gfx2 audit item 26 | Open: needs a disc-swap model in the CD/disc layer (lid status, switching to the Disc 2 image) |
| 29 | Dithering of flat textured semi-transparent polygons (e.g. floor-shadow edges): the port dithers them, the PCSX-Redux oracle does not; psx-spx does not specify it (DuckStation dithers) | gfx2 first-room oracle diff (98.90%) | **Untested** against real hardware; kept as is |

## Fixed (kept for the record)

- Two game globals were split between a host copy and the guest word, so hand-written writers and generated (matched-C) readers disagreed: `D_8009D018` (the generated `func_80051E58`, live on the field-message path, always read 0) and the story word `D_800A7918` (the main loop's volume gate in `func_8001220C` always saw story < 0x258). Both now live only in guest memory; proof tests `pc_port/tests/test_split_brain.h` fail without the fix. A sweep found no other split globals (found and fixed 2026-09-29, never separately listed).
- Pause screen not drawn: while paused, the port kept the last presented frame instead of retail's dimmed pause screen. The port now rescans the GP1(05h) display area from VRAM on every VBlank without a PutDispEnv, as the GPU does (display only; no present, no route-clock effect). Paused frame vs the emulator oracle: 99.2% identical on the unchanged background; luminance port 38.46→20.37→38.46 vs oracle 35.85→17.79→36.31; real-time windowed check dims and resumes. Harness: `PE_PAD_SCRIPT_CLOCK=vblank` lets scripted input reach a paused game (currently keyed on the VSync call count; to be switched to the emulated VBlank count) (former #30).
- Damage "far too high in both directions" (owner report on r2): **no divergence on the current tree**. The combat path equals the matched C; Aya's persistent stats, inventory and equipment at the first field room are byte-identical between a normal movies-on New Game (real title, opening movie, name entry) and route mode (only name-entry UI residue differs; build/lanes/day1/ayarec/mem_diff.txt). Hits-to-kill match or are harder than retail (first rat: port 2 commands, retail part 01 @ 15:53–16:03 one); Eve hits Aya for 5–11. r2 still ran the hand-cut battle tick, since replaced by the matched C (66d5f407), so the owner's r2 observation is not re-checked on r2 itself (former #11).
- With movies on, the port stopped at the end of Day 1 (story 0x80, after the car ride). Cause: `func_80076C34` (libgpu command-ring enqueue) cut retail's full-ring wait (`while (full) { if (func_80077404()) return -1; func_80076EE4(); }`) and stopped on the first full 64-entry ring (first attributed to StoreImage `func_800768A0`; a fatal-path diagnostic there never fired). Fixed 2026-09-29: the full-ring path now waits as retail does (`func_80077404` normal half, DMA/IRQ checkpoint, pump); the movies-on run continues into Day 2 (story 0x90) with no boundary, and the Day 1 closing narration and Day 2 opening audio match retail part 02 @ 18:17–18:42 and part 03 @ 0:06 (former #27).
- Retail's per-VBlank game callback `func_8003E91C` (RNG advance + the four timer ramps incl. play time) now runs every VBlank in normal play, as verbatim retail TUs; rates identical to the emulator oracle (frame counter +1/VSync, play time +600 per 600 VSyncs). Saves now record real play time (former #29).
- `func_8006FC18` (Pm_Stop) now follows the matched C: calls the destroy callback, returns its result, frees the slot; 0 destroy-callback skips over the whole Day-1 route (former #27).
- Game Over timing: no divergence. Mode-3 phases 1–5 match retail per phase to the VSync (emulator oracle vs port: 48/142/62/162/120/2, 536 VSyncs mode 3 → restart in both). The earlier +304-frame figure included the port-side forced-death detection lag (former #28; re-check after #29).
- Memory-card saves persist: host 128 KiB .mcd behind our own BIOS card-file functions (clean-room from psx-spx); save → relaunch → load → resume verified in-game (former #10). Play time saves as 00:00:00 until #29 is fixed.
- Particle/effect sprites as streaks (Eve's spells, rats): `func_8006914C` skipped retail's effect-module inits, so the effect texpage table was never filled; now runs them (matches retail part 01 @ 9:45–9:58). (former #12)
- Pause kept audio running: `func_8006A0E8` was a stub; now sends the retail AKAO pause/resume (verified in a real-time window). (former #14)
- Game Over: field side (ops 0x31/0xED, `func_8006A25C`, BIOS event calls) and battle side (`func_8002B29C`, `func_8006CC68` tail) now the matched C; fade → green caption → restart → FMV001 → title, same order as the emulator oracle. Timing matches retail per phase (see below). (former #21)
- Battle tick `func_800299CC` now a complete translation of the matched C (was hand cuts). (former #22)
- Battle mode-3 death path now the matched C (item 0x12 consume, countdowns, enemy walk). (former #23)
- `func_80024A3C` now all 17 phases from the matched C. (former #24)
- Field op 0xED now the full `func_80016910`. (former #25)
- MDEC: output of a previous decode command is now drained in FIFO order per psx-spx (was dropped); unit test added; not reached on Day 1 (former #6).
- CD/XA audio level: the drive volume (CdMix/ATV) follows the game's own writes, and the SPU main/CD volume comes from the retail SpuSetCommonAttr call; level gap logo−FMV002 −7.61 dB vs retail video −7.94 dB, FMV002 fade-out within ~1 dB (former #7).
- Wrong movie on New Game/opera (movie id slot overwritten by the overlay load) — fixed, owner-confirmed "videos playing perfectly".
- Missing room sound banks (room-loader sound tail cut) — fixed, owner-confirmed "sound is great"; footstep timing/loudness matched to the retail video.
- Scripted turns finishing in one tick (turn opcode returned 1 instead of retail's 0) — fixed against the matched C.
- Game ran at 2× speed (per-present pacing) — fixed.

Test-only route aids (HP lock, auto-fight) run only under `--route-pad` and are
not divergences of the game; the normal-start check (`day1_route_regress.sh`,
exit 3) enforces that they are off.
