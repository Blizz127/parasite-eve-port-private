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

Last updated: 2026-09-28. **Open divergences: 19.**

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
| 10 | Memory-card saves do not persist after quitting | — | Open |

## Game behaviour (open bugs, observed by the owner or tests)

| # | Divergence | Evidence | Status |
|---|---|---|---|
| 11 | Damage far too high in both directions: enemies die in one shot, and Eve one-shots Aya | owner reports on build r2 | Being diffed against the matched combat C (shared damage path suspected) |
| 13 | Aya's gun is not drawn when she readies it before the rat fight | owner report | Open |
| 15 | Plaza: the date's walk to the door with Aya after the dialogue (retail part 01 @ 5:16) not yet observed in the port | owner report; emulator oracle | **Untested past the dialogue**: every date state observed matches retail (arrival anims 0B→00→01→00 confirmed after the turn fix; plaza anim 04 standing identical to the oracle through VSync 16500, `persist[0]` bit 2 clear in both). The walk needs the player input the video used; parked (cosmetic) |
| 16 | Unported room-effect callbacks are skipped with a `[DAY1_SHIM] … not ported` line, so those effects do not draw (M34 phase 2: 80191A98, 80191FE0, 80190E98 — no matched C yet — and 8019253C; M0359I 8018F238; rooms A8002148/A8001148) | port logs | Port from matched C where it exists |
| 17 | Eve's flash: when the caller-stack ring position is unresolved, the first frame uses the owner joint position | `m0023i_effect_port.c` | Open |
| 18 | Police-siren ambience after Carnegie Hall | owner report (unsure) | **Untested** — to be checked against the retail video first |
| 19 | Frame pacing paces one host frame per emulated VBlank at 59.94 Hz | `host_window.c` | Believed faithful (240 Hz sequencer tick measured retail-exact); window-side timing **untested** on the owner's PC beyond his playtest |
| 20 | Second Eve battle: the music's vocal/choir layer is missing (instrumental plays) | owner report on r2 | **Not reproduced** in the first ~30 s of the fight: vocal-band (600–1500 Hz vibrato) event clusters match retail part 02 @ 1:23–1:53 at similar density; song, sample bank and driver inputs match retail. **Untested** beyond 30 s (needs a port fight kept alive 60+ s) |
| 26 | The field-VM dispatcher `func_80017018` is itself a hand port; 47 opcodes are re-implemented inline (34 have generated matched TUs, 12 have faithful ports) and an unlisted opcode stops the port | shadow audit | Switch to matched C in batches |
| 27 | `func_8006FC18` (Pm_Stop) skips unknown destroy callbacks and always returns 0 instead of the callback's result | shadow audit | Open |
| 28 | Game Over hand-off timing: death → restart at +304 port frames vs retail ~+268 (emulator oracle) | oracle vs port | Open |

## Fixed (kept for the record)

- Particle/effect sprites as streaks (Eve's spells, rats): `func_8006914C` skipped retail's effect-module inits, so the effect texpage table was never filled; now runs them (matches retail part 01 @ 9:45–9:58). (former #12)
- Pause kept audio running: `func_8006A0E8` was a stub; now sends the retail AKAO pause/resume (verified in a real-time window). (former #14)
- Game Over: field side (ops 0x31/0xED, `func_8006A25C`, BIOS event calls) and battle side (`func_8002B29C`, `func_8006CC68` tail) now the matched C; fade → green caption → restart → FMV001 → title, same order as the emulator oracle. Residual: port reaches the restart at +304 frames vs retail ~+268 (tracked as #28). (former #21)
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
