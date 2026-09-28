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

Last updated: 2026-09-28. **Open divergences: 27.**

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
| 6 | MDEC drops the decoder's padding tail | `pe_mdec.c` | Open |
| 7 | CD audio mix volume assumed full (0x1B0/0x1B2 not modelled) — loud movie audio can clip | audio lane | Open |
| 8 | SPU main volume assumed 0x3FFF; SPU IRQ and CD-DA not modelled | `pe_spu.c` | Open |
| 9 | The field-VM host argument frame lives in guest RAM at 0x801FF400 (port-only scratch) | `func_80017018_port.c` | Audited: nothing else writes it (`PE_VMFRAME_AUDIT=1`, bad=0 over ~1.6M opcodes); retail uses the real stack instead, so it stays listed until the port runs the matched VM |
| 10 | Memory-card saves do not persist after quitting | — | Open |

## Game behaviour (open bugs, observed by the owner or tests)

| # | Divergence | Evidence | Status |
|---|---|---|---|
| 11 | Damage far too high in both directions: enemies die in one shot, and Eve one-shots Aya | owner reports on build r2 | Being diffed against the matched combat C (shared damage path suspected) |
| 12 | Particle/effect sprites render as long streaks (Eve's spells, rats) | owner report; retail video 598 s | Renderer root cause under investigation |
| 13 | Aya's gun is not drawn when she readies it before the rat fight | owner report | Open |
| 14 | Pause freezes the picture but audio (and possibly logic) keeps running | owner report | Open |
| 15 | Plaza: the date does not walk to the door with Aya (`persist[0]` bit 2 never set) | trace + retail video 316.5 s | Open |
| 16 | Unported room-effect callbacks are skipped with a `[DAY1_SHIM] … not ported` line, so those effects do not draw (M34 phase 2: 80191A98, 80191FE0, 80190E98 — no matched C yet — and 8019253C; M0359I 8018F238; rooms A8002148/A8001148) | port logs | Port from matched C where it exists |
| 17 | Eve's flash: when the caller-stack ring position is unresolved, the first frame uses the owner joint position | `m0023i_effect_port.c` | Open |
| 18 | Police-siren ambience after Carnegie Hall | owner report (unsure) | **Untested** — to be checked against the retail video first |
| 19 | Frame pacing paces one host frame per emulated VBlank at 59.94 Hz | `host_window.c` | Believed faithful (240 Hz sequencer tick measured retail-exact); window-side timing **untested** on the owner's PC beyond his playtest |
| 20 | Second Eve battle: the music's vocal/choir layer is missing (instrumental plays) | owner report on r2 | Under investigation (audio sweep): check against the retail video and the matched sound-driver C |
| 21 | On death there is no Game Over screen: the game resets to the intro logo, and the music is left in a bad state | owner report on r2 | Cause found: field op 0x31 runs a cut `func_80017BB4` that drops the Game Over/continue branch, and `func_8006A25C` keeps 3 of its stores (no audio stop/reset); full versions exist and are being wired in (day1) |
| 22 | Battle tick `func_800299CC` is not a port of the matched unit: hand cuts drop `func_8003495C` (flag 0x300), the `D_8009D23C` script commands (`func_8006F6D4` ×2) and `func_80033430` | shadow audit vs `src/func_800299CC.c` | Switching to the matched C (port3) |
| 23 | Battle mode 3 cut drops item-0x12 consume (`func_8005409C`), the rec+0x56/+0x66 countdowns and the per-enemy walk | shadow audit + day1 vs `src/func_800299CC.c:305-323` | Switching to the matched C |
| 24 | `func_80024A3C` (Aya's action step) phases 4/6 deferred: effect teardown/spawn and Aya's reposition/facing/SFX 0x4B6 missing | shadow audit | Switching to the matched C |
| 25 | Field op 0xED runs a cut of `func_80016910` that drops ~10 keys; key 0x8FC writes `D_8009CED4` directly instead of calling `func_800375D0` | shadow audit | Wiring the full version (day1) |
| 26 | The field-VM dispatcher `func_80017018` is itself a hand port; 47 opcodes are re-implemented inline (34 have generated matched TUs, 12 have faithful ports) and an unlisted opcode stops the port | shadow audit | Switch to matched C in batches |
| 27 | `func_8006FC18` (Pm_Stop) skips unknown destroy callbacks and always returns 0 instead of the callback's result | shadow audit | Open |

## Fixed (kept for the record)

- Wrong movie on New Game/opera (movie id slot overwritten by the overlay load) — fixed, owner-confirmed "videos playing perfectly".
- Missing room sound banks (room-loader sound tail cut) — fixed, owner-confirmed "sound is great"; footstep timing/loudness matched to the retail video.
- Scripted turns finishing in one tick (turn opcode returned 1 instead of retail's 0) — fixed against the matched C.
- Game ran at 2× speed (per-present pacing) — fixed.

Test-only route aids (HP lock, auto-fight) run only under `--route-pad` and are
not divergences of the game; the normal-start check (`day1_route_regress.sh`,
exit 3) enforces that they are off.
