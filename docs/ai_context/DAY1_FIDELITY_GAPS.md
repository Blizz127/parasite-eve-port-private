# Day-1 fidelity gaps (cold boot → `persist[74]=0x80`)

**Status:** Day 1 is **not** complete. This is a prioritized blocker list for
100% 1:1 retail accuracy through end of Day 1 (sounds, movies, all systems).
Day 1 ends when `M0036I @ 0x801B82FC` writes `persist[74]=0x80`, after which
`M0351I` starts Day 2 (`DAY1_DAY2_TRANSITIONS.md`).

**Live counts (re-derive):** `python3 tools/build/disc1_plan.py --check` →
`1145 spans (796 c, 347 asm, 2 rodata)`, plan `b50a30290d4d…`.
Route coverage: `371/979` matched C on Tier A∪B; `532` real remaining asm
funcs / `65774` words (`tools/analysis/route_coverage.py --no-history`).

---

## Ranked Day-1 blockers

### 1. Opening FMV — plays on New Game without `--skip-movie` (fmv lane, 2026-09-28)

Evidence: `docs/evidence/fmv-playback/REPORT.md`. Headless, real Disc 1, no
skip flags: boot → logos → `FMV1/FMV001.STR;1` (LBA 189742) all **2077**
frames in **8318** VBlanks (15 fps; retail `func_80070E54` `VSync(4)`), with
**2597** XA-ADPCM sectors mixed through the audio lane's SPU CD input → title
"Press Start Button" → Start → "New Game / Tutorial" → New Game → field
opcode `0x35` plays `FMV000.STR` (SQUARE logo, 150 frames) → opening
Carnegie Hall scene. Start during FMV001 skips it (retail 60-frame wait).
`--skip-movie` stays for the `--route-pad` autopilot.

| Item | Status |
| --- | --- |
| Stream delivery | **HOST_ADAPTED producer** `pc_port/platform/pe_str_feed.{c,h}`: production runs never enable the modeled CD register device, so the retail IRQ-driven stream assembly (7C13C→7C564→DMA3→7C214) is replaced by a host feeder that reads raw sectors at the 2× rate (2.5 sectors / VBlank, credited from **all** VBlanks), writes STR chunk headers/data into the `StSetRing` records exactly where `func_8007C484`/`func_8007C394` read them, and advances VBlanks only while the guest waits on the stream. Everything downstream (91B64/924F8/92934, DecDCTvlc, MDEC, 91DC8 slices, display) is the guest player |
| Frame pacing / present | While a stream is open the feeder pays one host present per VBlank (the window pacer is one present = 1/60 s), so picture and XA audio run at real time. Approximation: the drive read-ahead is capped at ~2 s; ring-full stalls the drive instead of dropping sectors |
| ReadS / Pause with device off | `func_80081314` no longer queues the four SetMode/Setloc/ReadS packets that nothing drains (they blocked every later `func_8006E6D4` read); `func_80080DC4` Pause/Stop closes the feeder and completes without controller response bytes |
| XA audio | Audio sectors go to `PE_Spu_CdSector` at their drive-time position. Drive ATV volume matrix is not modeled with the device off → straight stereo 0x80 (retail power-on/CdMix default); SPU CD volume 0x1B0/0x1B2 unwritten (see §3) |
| MDEC | DecDCTvlc pads past the last macroblock; a new decode command now drops the stale tail (hardware leaves it in the FIFO) |
| 24-bit display | `HostFB_PresentDispEnv` decodes `DISPENV.isrgb24` (packed RGB, disp.w in VRAM units) |
| Title | `func_801909B4` full retail loop: attract timeout (1000 frames → replay FMV001), menu dispatch (0xA0 NEW GAME → 1; card screen via `func_8005C498`; 0xB4 w/o card / ≥0xB5 → 3), teardown + env restore. Title objects: `func_8018F468` retranslated (dirty-rect union, TIM background restore, all node callbacks), `func_80192FE8` (y/alpha were swapped), new `title_menu_callbacks_port.c` (8018F958/8018FD04/8018FE1C/80192F98/80193084/801930D8/8019316C/801931BC/80193200). Host pad fill before the title/movie `func_8003EB04` reads (retail libpad fills at VSync) |
| Port bugs fixed from overlay bytes | `func_8010C89C` long-code `at` reload (0x8010CAD4 delay slot); `func_80191B64` end-frame latch (DBD also when `w >= record+8`); `func_801924F8` give-up wait compares `$s2=1`; `D_800ACDDC` → `D_8009CDDC` (924F8/92934/91DC8/909B4); `func_80121C04` "\\FMV1" prefix is an address |
| Title audio | **Verified, no gap:** retail title overlay 0x03D2 has no AKAO/SEQ start. Its only sound calls are the menu SFX helpers `func_800525EC` (0x44C) / `func_8005267C` (0x44E) → `func_8006DF50`, plus `func_800870F0` (movie CD volume); the caller `func_8001220C` → `func_8006E834` → `func_801909B4` starts none. A retail-byte scan (overlay `jal` list; the three `jalr` sites are the title-node +0xC/+0x10/+0x14 slots, all overlay-internal; 141 EXE callees walked 3 levels from the disc EXE) finds no call to the music-start entries 0x8006CDA4/0x8006D2B8/0x8006D60C/0x80086FF8/0x80015DAC. So the title after FMV001 is silent apart from menu SFX in retail too, and those SFX play in the port (WAV `build/lanes/fmv/skip/after.wav`, 27.6–28.1 s) |
| Not claimed | No hardware pixel/audio golden; drive seek/spin-up time and retry timing are not modeled; CD-DA not decoded |

### 2. In-route movies (FMV000/FMV003 etc.) — field opcode `0x35` plays

`func_80017018` opcode `0x35` (fn `0x80014E30`) now runs the matched-decomp
`func_80014E30` (movie overlay load + `1216C4`/`121C04`/`1223A8`) when not
skipping; the stream is served by the same feeder. Verified on New Game
(`FMV000.STR`, 150 frames, `docs/evidence/fmv-playback/REPORT.md`).
`--route-pad` still forces `--skip-movie` (its pad schedule is keyed to
present counts; movies would shift it), so FMV003 on `m0372i` is exercised only
in a player run. Remaining: FMV003 not yet driven headless end-to-end (needs a
player-route pad script past the sewers).

### 3. Sound / music / SPU / XA on Day-1 path

**Audible since 2026-09-28 (audio lane).** Evidence:
`docs/evidence/pe-native-audio/REPORT.md`. The music/SFX you hear comes from
the retail sound driver itself: `pc_port/platform/pe_audio_driver.c` runs the
retail machine code of `func_8008DB7C` (AKAO tick: sequencer, voice updates,
command queue `func_8008CA84`, SPU flush) out of guest RAM in a small R3000A
integer interpreter, 4 ticks per emulated VBlank (retail RCNT2
`SetRCnt(0xF2000002, 0x44E8, 0x1000)` = 240 Hz). `pe_spu.c` synthesizes the
SPU; `host_audio.c` outputs through a `dlopen`ed `libpulse-simple.so.0`.
Headless route runs: 0 interpreter faults over 128k ticks, ~2 M ADPCM blocks
decoded with 0 invalid headers, output byte-identical between runs, route log
identical with `PE_AUDIO=0`.

| Area | Implemented | Approximation / gap |
| --- | --- | --- |
| AKAO sequencer + SFX | Native C `func_8008DB7C` (`akao_tick_port.c`, from matched C incl. `func_8008E8D0`) at 240 Hz; gated by the retail event registration + critical sections (`PE_Event_AudioTimerArmed`). Native only since 2026-10-07: the old R3000A interpreter is a test-only oracle (`pc_port/tests/pe_akao_interp_oracle.c`) | Ticks run in bursts at VBlank boundaries (sample output between ticks keeps even 240 Hz spacing in emulated time). `func_8008E23C`'s CPU-load bookkeeping (`D_8009CDE4`) is not updated. The native tick has not yet had a real-disc `PE_AKAO_VERIFY=1` differential against retail (needs a binary that links the oracle); disc-free, it matches the matched C on 8785 random whole-tick cases (`pe-akao-tick-oracle-tests`) |
| ADPCM voices | 24 voices, 5 filters, shift 13–15→9, loop start/end/repeat, ENDX, KON/KOFF, repeat-address ignore-after-KON rule, noise (NON), pitch modulation (PMON) | 4-point gaussian interpolation from the psx-spx table (audio2 lane; was linear) |
| Envelopes / volume | psx-spx ADSR rate formula (exp ×4 above 0x6000, exp decrease scaling), voice/main volume sweeps via the same stepper | Sweep "phase" bit treated as sign only |
| Main volume / SPUCNT | Honors written values | The port collapses SpuInit (`func_8007D1D4`), so main volume 0x180/0x182 is never written → treated as **0x3FFF** (libsnd `SsSetMVol(127,127)`); SPUCNT mute bit only honored once bit 15 has been written |
| Reverb | psx-spx algorithm at 22.05 kHz, work area in SPU RAM, EON input, 0x184/0x186 output | Input/output resampled through the psx-spx 39-tap half-band FIR (audio2; was a 2-sample average + hold). Exact per-stage rounding/saturation positions unknown |
| CD / XA input | XA-ADPCM tap on every drive sector (`pe_cdreg.c` → `PE_Spu_CdSector`): mode 0x40, form-2 audio, Setfilter file/channel, 37.8/18.9 kHz, mono/stereo, 4/8-bit, drive volume matrix | psx-spx 7-phase zigzag FIR resampler (audio2; was linear); 18.9 kHz fed twice into the 37.8 kHz path (approximation). 133 ms jitter buffer with fades because the port's feed delivers XA in bursts (real drive: steady). CD volume 0x1B0/0x1B2 unwritten (collapsed SpuInit/SsSetSerialVol) → full volume. **Exercised on FMV playback** (fmv lane): 2597 XA sectors over FMV001 via `pe_str_feed`; `--route-pad` still forces skip-movie, so 0 XA sectors are played there; Day-1 BGM is SPU-sequenced (AKAO), XA is FMV audio. CD-DA `Play` not decoded (not seen on the Day-1 path) |
| SPU extras | Manual FIFO writes (0x1A8) | SPU IRQ (0x1A4), capture buffers, SPU→RAM DMA reads not modelled |
| Wall-clock pacing | Per emulated VBlank at 59.94 Hz via `PE_AudioDriver_SetVBlankPacer` (audio2, 2026-09-28). Before, the window paced per present; the game presents every 2 VBlanks, so gameplay, AKAO tempo (≈480 ticks/s) and sample production (2x → overflow static) ran at 2x | Slow host frames discard backlog (music slows, never speeds); a scene presenting without VBlanks falls back to per-present pacing after 100 ms |
| Host output | PulseAudio/PipeWire via `dlopen` (no link dep), writer thread, 100 ms default (`PE_AUDIO_LATENCY_MS`), never blocks; adaptive ±0.5 % resampler on the device feed; fast-forward mutes with fades (audio2); WAV dump (unresampled, never muted) | Device opens only with a window (or `PE_AUDIO_DEVICE=1`). Faster-than-real-time headless overflows are dropped with a crossfade; genuine starves fade out/in. Windows build: WAV only |
| Spatial sound `func_8006DE80` | Native port helpers | Still **asm** matching leaf (fan-in 15) |

--- | --- | --- | --- |
| Event BGM handles `0xC8`/`0xC9` (seq 26/27) | Command IDs proven on Day-1 rooms | Full audible score sequencing | `PE_DAY1_ACCEPTANCE_CONTRACT.md:223-238` |
| Audio command queue / EA dispatch | Control/data graphs | Not audible synthesis | `DAY2_AUDIO_DISPATCH.md`; `pc_port/README.md:127-128` |
| SPU mode / DMA / registers | Register + DMA event model | Synthesis collapsed | `DAY2_SPU_MODE_*.md`; `pc_port/platform/pe_stream.c:12-36` |
| Ambient reload after sewer battle | Loader transitions | Provider internals / audible | `DAY1_SEWER_FADES.md:53-72` |
| XA decode + mix | FMV XA through `pe_str_feed` → `PE_Spu_CdSector` (fmv lane) | CD-DA undecoded | `fmv-playback/REPORT.md` |
| Spatial sound `func_8006DE80` | Native port helpers | Still **asm** matching leaf (fan-in 15) | `route_coverage.py` top list |

---

### 4. Connected route — does **not** reach `M0036I` / `persist[74]=0x80`

| Claim | Verdict |
| --- | --- |
| Static Day-1 exit | **Proven** — `0x78→M0036I`, `M0036I@0x801B82FC→0x80`, `0x80→M0351I` (`DAY1_DAY2_TRANSITIONS.md`) |
| Live harness reaches `0x80` | **No** |

**Current frontier (connected cold boot + reward pilot):**

- Live stop is **m0034i** / `persist[74]=0x6C` (M34 alligator). Prior fixed-pad
  52 000-frame / m0027i notes are historical.
- **F434 cleared** on heal-crit; Aya dies at **f=61469** (close-range PE menu).
- Supply7/11/12/13 item7 Use failed in m32 battle menu; supply14/15 idled at
  m31 door after premature heal arm. Supply16 wedged at door spawn on
  HEAL_WAIT (fixed: center X then `(0,1000)`). Supply17 reached clear + HEAL
  arm but mode=9 Items dismiss ate the menu (no HEAL_MENU). Fix: hold dismiss
  while heal/equip armed; Triangle re-pulse. Supply18: focus stayed 0 after
  HEAL arm (dismiss-hold insufficient). Supply19: heal owns pad through
  `d1a0&4` + Circle/Triangle open pulses; heal WORKS (DONE@58044). Supply20:
  post-DONE Down+Cross on id0 opened id=8; Cross never exits. Supply21:
  call-site/`DFFF` exit → **`A8003148`**@58560; then m32 PE-heal
  (item7=0) starved by cancel-only-in-mask → `A8001048` mode=-1. Fix:
  cancel-only only at HEAL_DONE call site; restore mask PE nav.
  Supply22: exit + **`A8003248` M34**; PE heal 8→38; died re-hug
  (no 2nd heal) mode=-1@64000. Supply23/24: two PE heals then west-pin
  soft-fail (`supply24` x≈-2700). Supply25: east bias held x≈-2316 but
  ATB Up-freeze mode=1 FFEF→frame budget. Supply26: pure-east into boss
  → soft-fail@64000. Supply27 live (lateral-then-east + emergency heal).
  Oracle Use on **m31** `A80030C8`: Down id1→row2 → Cross → id2@row0 → Cross.
- **Never observed live:** `0x78`, `0x80`, `M0036I`, `M0351I`.

Still ahead on Day 1 only: win M34 → `M0359I` → `M0036I` → `persist[74]=0x80`
then Day 2 selector (`DAY1_ROUTE_AUDIT.md` / `BOOT_TO_DAY2_COVERAGE.md`).

HOST_ADAPTED skips remain on every successful connected run (`SetSkipMovie(1)` /
`--skip-opening-menu`).

---

### 5. Matching-C leaves still `asm` that gate Day-1 movie / audio / boot

Live top remaining-asm by on-path fan-in (`route_coverage.py --top`):

| Rank | Function | Fan-in | Words | Day-1 relevance |
| ---: | --- | ---: | ---: | --- |
| 1 | `func_80079FB4` | 19 | 93 | Top remaining; PARKED (`docs/evidence/func-80079FB4/PARK.md`) |
| 2 | `func_80073A44` | 17 | 94 | VSync; movie/boot waits; PARKED |
| 3 | `func_8003708C` | 16 | 7 | Wide fan-in mul residual; PARKED |
| 4 | `func_8006DE80` | 15 | 21 | Spatial sound; PARKED |
| 5 | `func_8008CBA8` | 12 | 242 | Streaming / SPU command producer |

Also still **unnamed `asm`** and load-bearing for movies despite lower/merged fan-in reporting: `func_80014E30` (opcode 35), `func_80017018` (VM), `func_8007C564` / `func_80081314` / `func_8007C214` (stream/CdReadS/DMA3), `func_8001220C` (crt0 main), `func_800698D4` / `func_80069B08` / `func_8006AD40` / `func_8003F3C4` (boot spine). Overlay title/FMV bodies (`func_801909B4`, `func_80192CE8`) are overlay/`pc_port` translations, not disc1 `c` leaves.

---

## What must not be claimed

- Day 1 complete / `persist[74]=0x80` reached on a connected run.
- Opening or field FMV “working” because `--skip-movie` advances story.
- XA audio or hardware-exact pixels from autonomous/complete-frame tests.
- Matching progress from `pc_port/` translations alone.

## Key paths

| Concern | Path |
| --- | --- |
| Skip API | `pc_port/bootstrap/game_port.c` (`PE_Port_SetSkipMovie`) |
| Opening skip | `pc_port/game/boot/func_801909B4_port.c` |
| Field movie skip | `pc_port/game/boot/func_80017018_port.c` (`0x80014E30`) |
| Movie player / ticks | `pc_port/game/boot/movie_overlay_port.c`, `pc_port/platform/host_framebuffer.c` (`HostFB_StreamTick`) |
| Route harness | `pc_port/tests/test_route_boot_day2.c` |
| Day-1 exit proof | `docs/ai_context/DAY1_DAY2_TRANSITIONS.md` |

---

## Route shims (autopilot only — "Day 1 playable ASAP", 2026-09-28, lane day1)

These are **driver** shims in the `--route-pad` autopilot, not changes to game
code. A human player with a real pad is unaffected.

### Route shim: M34 HP lock + simple tail controller

| Field | Value |
| --- | --- |
| What | While token `A8003248` (M0034I), `RouteM34Shim()` writes Aya's current HP (`*(0x8009D278)+12`) := max HP (`+28`) every pad poll, and replaces the 52-run kiting pilot with: learned pad→world map, walk to ≤707u of the type-4 tail, Cross (attack/reload) when `t2≤900k`, mode 1 selects the tail then Cross. |
| Why | 52 supply runs (09-15..09-18) never broke the tail: every run died to the head while positioning. Survival heuristics were the blocker, not port code. The shim lets the route continue so later Day-1 rooms can be tested. |
| What still runs unmodified | Damage (`func_80028574`), hit gate (`func_80021278`), ATB/targeting, the room script's `HP ≤ 1,000,000` poll, outro and the M0359I transfer. |
| Files | `pc_port/tests/route_reward_sewer_pilot.h` (`g_route_m34_shim`, `RouteM34Shim`), `pc_port/src/port_main.c` (enables it for `--route-pad`). |
| How to restore | Run with `PE_ROUTE_M34_NO_SHIM=1`: the old supply pilot then owns M34, with no RAM writes. The test harness never sets the flag. |

### Route shim: M0023I battle aid

| Field | Value |
| --- | --- |
| What | `RouteBattleShim()`: during the M0023I rehearsal battle (token `A80021C8`, `D1A0&2`), Aya's HP is locked to max, the aid keeps 300..600u from the nearest enemy, presses Cross every 8f and, in mode 1, Crosses the default target. |
| Why | The replayed pads for this fight drifted with port changes: the 09-22 binary wins it, while the 09-28 02:18 build resets to New Game. |
| Scope | Rooms `A80021C8` (M0023I) and `A8064448` (M34 phase 2, sewer tunnel): HP lock + auto-fight. Sewer fights `A80023C8`/`A8002448`/`A8003148`: HP lock only (the sewer pilot plays; added 09-28 after the retail battle tick made the autopilot lose M0032I). |
| Restore | `PE_ROUTE_BATTLE_NO_SHIM=1` (replayed pads). The route counts as green only once a run in which Eve's flash (`PE_M0023I_Flash`) fires passes. |

---

## Port shims (game code, lane port, 2026-09-28)

These change what the native port does for **every** player, not just the
autopilot. Each keeps a strict switch so tests still see the retail boundary.

### Port shim: M0023I F710 flash ring position

| Field | Value |
| --- | --- |
| What | Eve's flash effect callback `0x8018F710` (`PE_M0023I_Flash`, `pc_port/game/boot/m0023i_effect_port.c`) draws its state-0 ring at six bytes it borrows from the original caller's stack. The port models those bytes only on the known effect-stack paths (`func_800D413C_port.c` `effect_stack`). On any other path the port used to record a boundary and **stop the game** (`stop_reason=unresolved-boundary` at frame ~43860 in the first Eve fight). Now it uses the owner's current joint position (`PE_ActorJointPositionCE8F0`, the same value F710 writes back into that slot at the end of every mode-2 call) and logs `[DAY1_SHIM] PE_M0023I_Flash ...` once. |
| Accuracy cost | Only the ring's position on the first flash frame can differ from retail, which is whatever stale stack bytes the caller left there. Gameplay state is unaffected (the ring is draw-only). |
| Restore | `g_pe_strict_effect_stack = 1` restores the explicit boundary + stop. `test_DAY1_retail_m0023i_flash` checks both modes. |

### Not a port bug: first Eve fight route reset (09-22 pass → 09-28 fail)

Bisect (`build/lanes/port/NOTES.md`, "port3"): b04c438e passes `--until eve1_victory`; 9dd1bd1c and later fail. The cause is the retail-matching generated TU for `func_800144FC` (field-VM battle-entry state machine). It replaced the approximate hand port (`func_800144FC_state*_cut` in `func_80029810_port.c`). Every battle now starts 2–3 frames earlier (d1a0 `|=0x80`/`|=2` and mode 7→0: M0002I 10252→10250, M0011I 18903→18900, M0023I 42934→42931). Room entry frames and field RNG call counts are unchanged. The replayed exact pads `[42713,45041)` were recorded on the old timing. Shifting them by −3 frames does not reproduce the fight because Eve's AI state also differs. The fix therefore belongs in the route (battle aid above), not in reverting the retail TU.

### Port shim: unported room effect init / callback (func_8006F39C, PE_EffectCallback)

| Field | Value |
| --- | --- |
| What | The effect constructor `func_8006F39C` (`pc_port/game/boot/func_8006F39C_port.c`) and the effect callback dispatcher `PE_EffectCallback` (`func_800D413C_port.c`) used to stop the game (`stop_reason=unresolved-boundary`) on any overlay init/callback address with no native port. That happened first at M34 phase 2 (room m0348i, token `A8064448`, frame ~64603). Both now try the generated guest-code registry (`PE_GuestCode_Resolve`/`PE_GuestCall`) first. If that fails they skip the call and log `[DAY1_SHIM] func_8006F39C: init fn=... not ported` / `[DAY1_SHIM] PE_EffectCallback: room effect ... not ported` once per address. A skipped update returns "finished" (1), so the effect pool retires the record. |
| Also | `PE_WeaponCallback` (`func_800C2414_port.c`, effect-VM callbacks) and the destroy callback in `func_80020F18_port.c` (`func_8006FC18`) take the same registry-then-skip path; a skipped VM callback retires its record (`rec+1 = 2`), a skipped destroy callback still frees the slot. First hit: m0348i `0x8018F240` (particle/actor-attached effect chain, ~30 unported overlay leaves). |
| Ported instead of shimmed | m0348i `func_8018F06C` (init), `func_8018F0C4` (callback), `func_8018F128` (update) and `func_8018F1B4` (cleanup), from the matched `src/overlays/room_m0348i/*.c`, in `m34_boss_effect_port.c` (`PE_M0348iEffect*`, gated on the resident m0348i words). |
| Accuracy cost | A skipped effect is not drawn (visual only). Its effect-bytecode side effects (sounds, child effects) do not run. |
| Restore | `g_pe_strict_effect_stack = 1` restores the explicit boundary + stop. `SEW12_m0013i_command_callback` checks strict mode. |

## Player cheats (opt-in, lane port, 2026-09-28)

An opt-in testing/cheat console on the mods layer (`pc_port/include/pe_plat/cheats.h`,
`pc_port/platform/plat_cheats.c`; it runs from the mods `FRAME_TICK` event).
Everything is **off by default**. With all cheats off it writes no guest state and
the frame is presented unmodified (`test_CHEATS_all_off_identity`). `PE_CHEATS=0`
hard-disables it. The screen shows "CHEATS" whenever any cheat is active.

| Key | Cheat | Mechanism |
| --- | --- | --- |
| F1 | help overlay | host-side text only |
| F2 | god mode | Aya HP (`*0x8009D278` +0x0C) held at max (+0x1C) every frame, the same mechanism as the harness HP lock but gated by this toggle |
| F3 | infinite ammo + full Parasite Energy | loaded rounds (weapon record +0x0C low 10 bits) and reserves (`0x800A1E6E+k*32`) restored to their highest seen value; PE (`**0x8009D254` +0x08) := max (+0x28) |
| F4 | instant win | every live enemy's HP (`*(actor)+0x10`) := 0 during a battle; the game's own death/victory/reward path then runs |
| F5 | area kit | scans the loaded room scripts (0x80180000..0x801F0000) for field op 0xA7 (give item) with a key-item id 0x100..0x17F and adds each id not owned with the game's own `func_80053D2C`. No retail tables are committed. Items given by other areas' scripts, and door checks that test story flags rather than items, are not covered |
| F6 | fast-forward toggle (unchanged) | |
| F7 | fast-forward speed | cycles max / 2x / 4x / 8x (a fixed N-x speed paces one wall frame per N emulated VBlanks) |
| F8 | warp menu | Up/Down + Enter: sets `persist[74]` to the checkpoint's story value and transfers through the field VM's own `room_transfer` (`func_80017BB4`, op 0x31), only while not in a battle or transfer. Only the story word is set; other per-room flags and inventory are whatever the save/run already has |

Scripted keys for headless tests: `PE_CHEAT_KEYS="frame:KEY,..."`.
