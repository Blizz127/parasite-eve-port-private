# Native audio for the Day-1 preview (audio lane, 2026-09-28)

Coordinator task 11b0bbc26eca: working sound and music in the native port,
played through PipeWire/PulseAudio. This makes the port audible. It does
**not** claim 1:1 accuracy. Approximations are listed in
`docs/ai_context/DAY1_FIDELITY_GAPS.md` §3.

## Design

| Piece | File | What it does |
| --- | --- | --- |
| SPU synth | `pc_port/platform/pe_spu.{c,h}` | 24 voices of 4-bit ADPCM (5 filters, loop flags), pitch with linear interpolation, PMON, NON noise, ADSR per the psx-spx rate formula, volume sweeps, the psx-spx reverb algorithm, CD/XA input, manual FIFO transfers. It observes every `PE_SpuRegister_StoreU16` and serves the live registers (ENDX, per-voice envelope `+0xC`, current main volume). It is inert until activated. |
| Driver clock | `pc_port/platform/pe_audio_driver.{c,h}` | Runs the **retail machine code** of `func_8008DB7C` (AKAO tick) out of guest RAM in an R3000A integer interpreter. SPU I/O goes to `pe_spu`, RAM goes through `pe_guest_ram`, and the stack is private. There are 4 ticks per emulated VBlank, from RCNT2 `0x44E8`×8 = 141120 cycles against 564480 cycles per VBlank, which is 240 Hz. 183.75 output samples are rendered after each tick. |
| Output | `pc_port/platform/host_audio.{c,h}` | `dlopen("libpulse-simple.so.0")` with no link dependency. A writer thread drains a ring of about 40 ms plus a device target of about 40 ms, so `Submit` never blocks. Also writes a WAV dump. |
| XA tap | `pc_port/platform/pe_cdreg.c` → `PE_Spu_CdSector` | Handles form-2 audio sectors read with drive mode 0x40 that pass the Setfilter check. It decodes them, resamples to 44.1 kHz, applies the drive's volume matrix and queues them as CD input. Sector delivery to the game is unchanged. |

Hooks in shared files (minimal):
- `pe_spu_dma.c`: register store/load forwarding, `PE_SpuRam_Data()`, and `PE_Spu_Reset` on DMA reset.
- `pe_libetc.c`: `PE_Event_AudioTimerArmed()`, and `PE_Event_ServiceAudioCommands` becomes a no-op while the driver runs (the interpreted `8DB7C` calls `8CA84` itself).
- `pe_gpu.c`: `PE_GPU_VBlankStep` → `PE_AudioDriver_VBlank`.
- `port_main.c`: enable, sink and exit summary.
- `CMakeLists.txt`: new sources, plus `Threads` for the executable only.

Why interpret instead of port: `func_8008DB7C` is matched C, but the generator skips it ("pointer-global store"). Its callees `func_80089328`, `func_80087AA8` and all the `func_8008F2xx`–`func_800907xx` opcode handlers are skipped too (pointer-to-pointer parameters). The sequencer step `func_8008E8D0` (0x800 bytes) is not matched at all (`docs/evidence/func-8008E8D0/PARK.md`). Interpreting the retail bytes gives the original sequencer behavior now, with no invented C.

## Controls

`PE_AUDIO=0` gives the old silent behavior. `PE_AUDIO_WAV=path` writes a WAV,
and the header is refreshed every second.

`PE_AUDIO_DEVICE=1` forces the device when headless; normally it opens only
with a window. `PE_AUDIO_LATENCY_MS` defaults to 80.

`PE_AUDIO_KON_LOG=path` writes `vblank mask` lines. `PE_AUDIO_DEBUG=1` prints
a status line every 10 s of emulated time.

## Verification (all commands from the repo root, `TMPDIR=$PWD/build/tmp`)

Build: `cmake -S pc_port -B build/pcbuild-audio && cmake --build build/pcbuild-audio`.

Native suite: `./build/pcbuild-audio/pe-native-tests` returned
`Results: 1579 run, 1579 passed, 0 failed, 0 skipped`. This includes the new
`AUDIO_adpcm_decode`, `AUDIO_adsr`, `AUDIO_voice_render` and
`AUDIO_driver_interpreter` tests (`pc_port/tests/test_spu_synth.h`).
`AUDIO_xa_decode` was added later and passes (`PE_TEST_FILTER=AUDIO_`: 5/5). A later full run on a tree with another lane's concurrent movie edits gave 1579/1580; the one failure, `OPEN1_909B4_enters_fmv_without_skip_movie`, had passed in the earlier run and is outside the audio hooks, which are inert in tests. The
vectors are hand-computed from the psx-spx formulas:
- filter-1 recursion 16384→15360→14400
- filter-2 clamp to 32767
- shift 13 treated as 9
- ADSR linear attack 14336/sample, exponential decay −16384, the rate-13 4-cycle step, and exponential attack ×4 above 0x6000
- a KON clears ENDX, a loop end sets ENDX, and an end without repeat mutes
- interpreter delay slot, SPU store/load round trip, and a bad fetch treated as a fault

Route runs:
`PE_AUDIO_WAV=… PE_AUDIO_KON_LOG=… PE_AUDIO_DEBUG=1 ./build/pcbuild-audio/parasite-eve-port --headless --route-pad --max-frames N --disc-image "rom/image/Parasite Eve (USA) (Disc 1)/Parasite Eve (USA) (Disc 1).bin"`

| Run | Frames | Emulated audio | Driver | Voices / data |
| --- | --- | --- | --- | --- |
| run1 | 3000 | 100 s, peak 21620, RMS 2934, 2 of 20 five-second windows silent | 23992 ticks, 0 faults | 971 KON |
| run2 | 16000 | 533 s, RMS 2447 | 127992 ticks, 0 faults, 107 M instructions | 3372 KON / 4785 KOFF |
| run6 | 12000 | 400 s | 95992 ticks, 0 faults | 3256 KON, 1,976,252 ADPCM blocks decoded, **0 with an invalid header**, reverb base 0xF204, EON active |

- **Determinism:** the first 70 MB of run2 and run6 are byte-identical. Only the WAV header size differs.
- **No gameplay change:** the run1 route log (`[ROUTE]` lines, milestones, stub summary) is identical with and without `PE_AUDIO=0`.
- **Output path:** a real-time-paced stand-in `libpulse-simple.so.0` (`build/lanes/audio/fakepulse/`) was loaded via `LD_LIBRARY_PATH`. It received `rate=44100 ch=2 fmt=S16LE`, and 12.5 s of audio came out of a 14 s headless wall-clock run. The game loop was not slowed; excess headless samples are dropped by design.
- On a machine without the library the port prints `[AUDIO] libpulse-simple.so.0 unavailable …; audio output silent` and continues.

Timeline (run2 / run6, 10 s windows; t = VBlank/60; route frame ≈ t×30):

| t (s) | Content | Driver state |
| --- | --- | --- |
| 0–30 | music, RMS 2.5–2.9k, centroid 1.5–2.1 kHz | seq0 = 0x1FFFFF (21 channels) |
| 30–40 | silence | seq0 = 0 (between tracks) |
| 40–70 | SFX / ambience | sfx mask 0xC00000 |
| 70–165 | music, RMS 2–4.9k | seq0 = 0x3FFFFF, then 0x007FFF |
| 170–300 | silence; black screen at frame 6000 | seq0 = 0, sfx = 0, no KON (see note) |
| 300–415 | music plus dense SFX (up to 46 KON/s at 360–380 s), peaks clip at 32766 | many voices, reverb |
| 420–510 | silence | no KON |

Note on the silences: the driver runs every tick with the gate armed and no
fault. The game has no sequence or SFX active there. The 170–300 s stretch is
a black screen under `--route-pad`, which forces skip-movie. On retail, an FMV
with XA audio would play at that point. XA decoding is implemented, but the
route never reaches it (`XA-sectors=0`). This is a hypothesis, not proven
scene by scene.

Evidence files (git-ignored, local): `build/lanes/audio/`:
- WAVs: `run1.wav`, `run2.wav`, `excerpt_opening_0-30s.wav`, `excerpt_70-150s.wav`, `excerpt_battle_340-400s.wav`
- analyses: `run2_analysis.txt`, `run6_analysis.txt`
- key-on timelines: `run2_kon.txt`, `run6_kon.txt`, `run6_timeline.txt`
- logs: `run*.log`
- tools: `analyze.py` (RMS, peak, zero-crossings, FFT centroid) and `excerpt.py`

## Not verified

- **Listening:** nobody has listened yet. Plausibility rests on the metrics above.
- **Live PipeWire output:** not tested on Bazzite; this box has no libpulse.
- **XA on a real FMV:** not tested.
- **Plain headless launch:** `--headless` without `--route-pad` never reaches VBlanks within 60 s, even with `PE_AUDIO=0`. This predates the audio work, so that path could not be tested headless.

## audio2 lane (2026-09-28): static fix + fast-forward

Owner report on Bazzite (PipeWire 1.6.9, pulse compat, 48 kHz float sink):
audio "a little staticky" during the opening movie, and fast-forward needs a
controller binding with sane audio. Evidence files and tools are in
`build/lanes/audio2/` (git-ignored): `wavstat.c` (clipping, DC, exact-zero
gaps, jumps), `specstat.c` (band energy), `sinecheck.c` (sine-continuity
clicks), `sim.c` (drives `host_audio.c` like the 59.94 Hz window pacer), and
`fakepulse/` (a real-time stand-in `libpulse-simple.so.0` modelling the
device buffer, underruns and `pa_simple_get_latency`). The "before" binary was
relinked from the pre-change `pe_spu.c`, so everything else is identical.

### Causes found (all fixed)

1. **Output path starved and overfilled (the main static).** The game renders
   735 frames per emulated VBlank on a 60 Hz cycle budget, but the window
   paces at 59.94 Hz, so the stream is 0.1 % short. Slow frames lose time
   because the pacer discards its backlog. The device ran with an 80 ms
   budget. The writer inserted 256-frame zero blocks when starved, and the
   ring dropped single samples above 40 ms. Test: `sim` + fakepulse, 440 Hz
   sine, 59.94 Hz deadline pacer:

   | scenario | before: clicks / device underruns | after |
   | --- | --- | --- |
   | steady frames, 10 s | 11 / 6 | 1 (start-up fade) / 0 |
   | 40 ms stall every 2 s, 10 s | 46 / 37 | 0 / 0 |
   | 40 ms stall every 10 s, 20 s | 48 / 33 | 0 / 0 |
   | fast-forward 4–7 s (uncapped) | n/a | 1 (the fade) / 0 |

   Fix (`host_audio.c`): the producer side has a cubic-Hermite adaptive
   resampler, at most ±0.5 % (8.6 cents), steering the queue to target.
   Default latency is 100 ms: a 60 ms device `tlength` plus a 40 ms queue.
   The writer uses `pa_simple_get_latency` (optional) and waits while the
   device still holds audio. A real starve ramps down from the last frame,
   re-primes to half the target, then fades in over 64 frames. Overflow drops
   the oldest audio with a 64-frame crossfade. `HostAudio_SetFastForward`
   mutes the device (a fade, then silence) and resumes with a fade-in. The
   WAV dump stays the unresampled emulated mix.
2. **XA (movie) queue underruns.** The movie feed delivers XA sectors as
   4 singles plus 1 pair per 16 VBlanks, not at the drive's steady rate. The
   CD queue ran to 0 and underran **5×** in the opening movie. Over the
   stream, supply and consumption balance (1,481,760 vs 1,481,025 frames);
   the worst in-stream deficit is 2,646 frames. Fix (`pe_spu.c`): a
   133 ms (5,880-frame) CD jitter buffer that plays out after 100 ms without
   sectors, with 64-frame fade-in and fade-out. Opening movie (no-skip boot,
   2500 frames): CD underruns 5 → **0**, exact-zero gaps of 64+ frames
   inside audio 5 → **0**, minimum queue at a push 3,234 frames.
3. **XA resampler imaging.** The linear 37.8→44.1 kHz resampler left energy
   at 18.9–22 kHz of −29.6 dB relative to the total; the source has nothing
   there. It is replaced by the CD decoder's documented 7-phase zigzag FIR
   (psx-spx "CDROM Format", tables transcribed verbatim to
   `pe_spu_zigzag.inc`, per-term `/8000h`): **−33.3 dB**, which psx-spx calls
   inaudible ~22 kHz zigzag noise, at hardware DC gain (−0.85 dB). A 60 dB
   Kaiser polyphase FIR measured −86.6 dB. It was not kept because the goal
   is retail behaviour. The XA decoder itself was checked against psx-spx
   `decode_28_nibbles`: header offset `4+blk*2+nibble`, data
   `16+blk+j*4`, `shift = 12-n` with 13–15→9, and K0/K1. On the disc's 375
   opening-movie sectors, 0 groups have a header-duplicate mismatch and all
   filters are 0–3 (coding 0x01: stereo, 37.8 kHz, 4-bit).
4. **Reverb imaging.** Reverb output was held for 2 samples. Input and output
   now go through the psx-spx 39-tap half-band FIR (in: decimate; out:
   zero-stuffed ×2). Route run (12000 frames, music stretch 0–200 s):
   18.9–22 kHz −36.2 → **−54.9 dB**, 15–18.9 kHz −42.0 → **−53.5 dB**.
5. **Voice interpolation.** Voices were linear; they now use the psx-spx
   4-point gaussian table (`pe_spu_gauss.inc`, verbatim; the build-time check
   confirms every 4-tap sum is 0x7F7F..0x7F81). Index = pitch counter bits
   4..11.

Not changed: the battle stretch (300–400 s) keeps about 290 near-full-scale
samples (|v| ≥ 32760; before 288, after 298). This is mix-bus saturation:
voices, CD and reverb are clamped before main volume, which is the usual
emulator order. psx-spx does not document the order. It is the game's own
hot mix under the assumed 0x3FFF main volume, not a synthesis bug. The
sequencer is unchanged: the route run gives KON 3256 / KOFF 4627 / 0 faults,
identical before and after.

### Fast-forward

F6 still toggles. On a controller, **L3** (left stick click) toggles and
**R3** (right stick click) fast-forwards while held (`host_pad.c`
`HOST_PAD_HOTKEY_*`; stick clicks never reach the PSX pad word). Whenever the
effective state changes, `host_window.c` resets the pacer, retitles the window
and calls `HostAudio_SetFastForward`. The Windows window is unchanged: it has
no device audio.

### Verification

- `pe-host-pad-tests`: OK (new `test_hotkeys`).
- `PE_TEST_FILTER=AUDIO_ pe-native-tests`: 6/6. The new
  `AUDIO_gauss_and_cd_jitter` covers the gaussian 255/256 gain (16310..16322
  for a constant 16384), CD priming, fade-in, and a click-free starve
  (max step < 200).
- Full native suite: see the lane report.
- Runs: `PE_AUDIO_WAV=… parasite-eve-port --headless --max-frames 2500`
  (opening movie) and `… --route-pad --max-frames 12000`.

### audio2 follow-up: battle music "super fast and staticky"

**Cause: wall-clock pacing, not the sequencer.**
- The AKAO clock is exact in emulated time. A headless route of 12000 presents ran 95,992 ticks over 23,998 VBlanks: 4.000 per VBlank, 240 Hz, matching retail RCNT2 0x44E8.
- The game presents once every 2 emulated VBlanks. `PE_AUDIO_PACE_LOG` shows **30 presents per 60 VBlanks in all 399 one-second windows**, field and battle alike.
- The window paced one 59.94 Hz frame per *present*. Emulated time therefore ran at up to 119.9 VBlanks/s: 2× gameplay, 2× music tempo (≈480 ticks/s), and 88.2 k samples/s into a 44.1 k device.
- The surplus samples overflowed. The old output path clicked **564 times in 10 s** (simulator, 440 Hz sine, 2 VBlanks per 59.94 Hz present). The new output path dropped half the audio in 80 crossfaded cuts.

**Fix.** `PE_AudioDriver_VBlank` calls a registered `PeVBlankPacer` after each emulated VBlank, including when audio is disabled. `host_window.c` registers `pace_vblank`, which waits one 59.94 Hz frame (skipped in fast-forward) and polls input. `HostWindow_Pace` (per present) only polls. It falls back to pacing presents only when no VBlank has paced for 100 ms of wall time, so pe_str_feed's catch-up presents cost nothing. Headless runs register no pacer and are unchanged.

**Evidence.** The real binary ran in window mode under a stand-in `libX11.so.6` (`build/lanes/audio2/fakex/`) and the real-time stand-in libpulse, for 338 s of the `--route-pad` route. Emulated VBlanks per wall second, per 60-VBlank window:
- Before: up to **116.3/s** (≈2×); most windows 60–110/s.
- After: capped at **59.9/s**, i.e. at most 239.6 AKAO ticks/s against retail's 240.

The dev box was CPU-starved (load average 50 on 12 cores), so both runs fell below real time at times. On the owner's PC, "before" sits at 2× and "after" at 59.94/s.

Simulator, owner case modelled: 2 VBlanks' worth of samples per 59.94 Hz present gave 564 clicks (old path). Pacing 1 VBlank per 59.94 Hz frame gives 0 overflows and 0 clicks.

**Movie skip.** When XA stops, `PE_Spu_CdInputStop` (added by the fmv lane in `pe_spu.c`, semantics agreed) flushes the XA jitter buffer with the 64-frame fade. host_audio needs no call: it holds at most about 100 ms and fades a starve.

### audio2: New Game movie bug (owner, build c744fab6)

Symptom: on New Game, a loud noise burst, then a cut straight to the Aya-and-date car arrival, with the movie's music playing on.

**Cause.** Every field movie resolved to id 0. The script VM's host stand-in frame `GA_VM_FRAME` sat at 0x80120F80 in `func_80017018_port.c`, inside the 0x80120D00 movie-overlay window. Opcode 0x35 (`func_80014E30`) loads the overlay and only then reads `**args`. So New Game played FMV000 (the SQUARE logo, 150 frames) instead of FMV002 (the Carnegie Hall opening, 1021 frames).

That explains all three symptoms:
- **Noise burst:** FMV000's XA is correctly decoded but very loud on the disc (shift 2–3): RMS 13.6k, 673 clipped samples over 2 s.
- **Cut:** the logo lasts 5 s, then the scene starts.
- **Music played on:** FMV002's partly AKAO-sequenced score, started for the movie, continued into the scene.

The frame was moved to 0x801FF400 (the list to 0x801FF460) in the shared tree by another lane.

**Evidence** (fixed tree, scripted skip-FMV001 → New Game; `PE_FMV_LOG`, `PE_AUDIO_PACE_LOG`):
- `movie id=2 name=\FMV1\FMV002.STR;1`. The stream runs from LBA 103206 and closes at 113427 (file end 113438): 1021/1021 frames, 0 dropped, 1277 XA sectors, 0 CD underruns.
- Windowed run (stand-in X11 + real-time fake Pulse): the same 1021/1021 frames to the file end.
  - Wall time was 263.5 s because this box managed 16.1 VBlanks/s (max 23.6). The pacer never waited. On a real PC it should be 68.1 s.
- XA stops at the close. The CD/XA frames mixed per 60 VBlanks are 44100 during the movie, 40489 in the window with the close (≈18 ms past the close VBlank, 64-frame fade included), and 0 afterwards.
  - After the close only the SPU voices' release/reverb tail remains (peak 786 → 351 at +1 s → 2 at +3 s), with no key-ons.
- After the movie the screen shows the correct Carnegie Hall scene.
- The shipped build with the same script opens LBA 101705 (FMV000): the bug.
- The pacer, the CD-input stop and the jitter buffer were not involved.

### audio2: field SFX (no footsteps, no grunt, loud bangs)

**Cause.** The port's room loader (`func_8006B4F8_port.c`, a partial cut marked "6CDA4 tail is not this cut") never ran the sound tail of retail `func_8006B4F8` (`src/func_8006B4F8.c`). That tail reads three tables from the room file header:
- hdr+0x24: the room SFX banks `st+0xEA`/`+0xEB` and the music id `st+0xE8`, loaded with `func_8006CDA4(2|1, …, blocking)`.
- hdr+0x2C: Aya's footstep sound ids into `D_8009448C[row][col]`.
- hdr+0x30: `st+0xE0`/`+0xE1`/`+0x124`.

What followed:
- No room SFX bank was ever uploaded. `PE_AUDIO_DMA_LOG` showed 51 SPU DMAs, all at boot, none after New Game; SPU 0x50540–0x75220 was never written.
- Room SFX keyed on boot samples. For example, the lobby footsteps played SPU 0x06CB0: the "loud bangs".
- Aya's footstep records kept sound id 0: no footsteps.

**Fix.** `pe_6b4f8_sound_tail()`, ported from the matched C, runs at the end of `func_8006B4F8_dest_load_cut`. Diagnostic: `PE_AUDIO_ROOMSND_LOG`.

**Evidence** (`--route-pad`, headless; retail reference is the owner's Day-1 video):
- Banks load per room, e.g. 0x84 → SPU 0x68000 and 0x83 → 0x6A000; 12 room loads over the route.
- Aya's records get 3FE/3F6 and 3FF/3F7.
- Plaza walk: before, 0 SFX; after, footsteps every 0.63–0.70 s. Retail 316.02–317.35 s shows them.
- Loudness, calibrated on FMV002 XA (port RMS 4687, video 1926, ×0.41):
  - plaza footstep high-pass energy 144–197, against retail's 145–208;
  - lobby RMS 180–217, against retail's 143–265.
- Route regression (12000 frames): same end state, 20 battle edges, 0 faults; music and battle levels unchanged; KON 3255 → 3801.
- Native suite: `Results: 1590 run, 1590 passed, 0 failed, 0 skipped`.

**Remaining (animation, not audio).** The date's step triggers (EA 406: actor 02/00, anim 0x01) never match, because the port's date walks with anim 0x0B (arrival) and 0x04 (plaza). His retail footsteps at 260.06/260.66 s and the interleaved 0.33 s plaza cadence are therefore still missing.

### audio2: pause does not stop audio

**Retail behaviour** (`src/func_8003F3C4.c`, `src/func_8006A0E8.c`):
- Start in the field tick toggles `D_8009D1A0` bit 0 and sets bit 0x10 (pause entered) or 0x20 (pause left) for one frame. While paused the tick only calls `VSync(2)`.
- Every tick then calls `func_8006A0E8`. On 0x10 it draws the pause overlay through the draw env and sends AKAO command **0x99** (`func_800867E4(0)`); on 0x20 it sends **0x98** (`func_8008682C(0)`).
- Battles run in the same field tick.

**Cause.** The port's `func_8006A0E8` (in `func_8003F3C4_port.c`) was a stub ("PutDrawEnv / SetDrawArea body is not this cut"), so neither command was sent. The AKAO tick runs from the emulated root counter whatever the game's pause state (as on retail), and it kept sequencing.
- Evidence: tracing `func_8008CBA8` showed no command at the pause.
- A `HostFB_VSync` backtrace while paused showed `func_8003F3C4 → VSync(2)`.

**Fix.** The full retail body is ported (overlay draw plus 0x99 / 0x98).

**Evidence.** Windowed real-time run: stand-in X11 injecting Start (`build/lanes/audio2/fakex`), real-time stand-in Pulse, `--route-pad --hand-over-at 10400`, paused mid-battle (first Eve fight).
- Key-ons per 60 VBlanks: 20–41 before, **0** during the pause (VBlanks 21240–21480), again from 21540.
- Mix RMS: 4–5k before; after the last key-on (352.31 s) it decays through each voice's ADSR release and the reverb tail: 1.4k at +0.2 s, 500 at +0.6 s, 100 at +1.1 s, near 0 by +2 s. This is the retail driver's 0x99 handling (retail driver bytes in the interpreter).
- On unpause (0x98) the music resumes where it left off: RMS 2656 in the first 50 ms, 6248 after 1 s.
- Route regression (12000 frames, no pause): same end state, 20 battle edges, 0 faults.
- The port has no host-level pause on focus loss (`FocusOut` only releases held keys). A host-side freeze with no emulated VBlanks drains host_audio's ~100 ms queue with a 64-frame fade.

### audio2: sound audit items 9–14, sirens, Eve 2 vocals, Day-1 sweep (in progress)

**Item 9 — FIXED.** `func_80015DAC_default_cut` silently dropped 23 retail EA keys: 100, 101, 208–216, 302, 304, 306–313, 408, 410. All are now ported 1:1 from `src/func_80015DAC.c`; 408 shares case 200's music-start body.
- Seen on the Day-1 route (fixed build, `PE_AUDIO_EA_LOG`): **EA408 at route frame 15300, room A80004C8, loads bank 66 and starts music 57.** It was dropped before, so that room's music never started.
- Also seen: EA100/101 (AKAO pause/resume, variant 1) just before the Eve 2 battle.

**Item 10 — done** (pause; see above).

**Item 11 — FIXED.** `func_8006F044` was a `psx_compat.h` boundary stub. It is now `game/boot/func_8006F044_port.c`, a port of the matched C:
- resets the six `B0DB2..7` bytes and clears the `B0CD8` 0xF0 bits;
- sends AKAO 0xF0 (stop all);
- reloads the two resident blocks (`D_8009315E`/`D_80093166` → `D_8001160C`/`D_80011610`).

**Item 12 — low impact, open.** SpuSetCommonAttr runs (main 0x3FFF, CD 0x7FFF). Still collapsed:
- `func_800862F4` per voice (zero attributes, the power-on state);
- `func_8008CB54(4)` + `func_80085A64(1)` (boot reverb type 4 plus enable).
Each song's reverb set-up by the interpreted driver overrides this.

**Items 13/14 — native tick only.** `func_8008A92C`/`8A750` and the `func_8008CA84` callback copies are reached only from the native AKAO opcode handlers (`decomp_hand/seq_ops_hi_port.c`). The shipping path interprets retail bytes.

**Sirens — no gap.** Retail has none audible:
- part 1 @ 12:20–13:20 (ambulance/cops outside Carnegie Hall): mostly digital silence, with short event sounds only;
- part 2 @ 14:35–15:45 (reporter, "Sorry I'm late, Aya"): RMS 17–140 with fully silent stretches, and no sustained or sweeping tone;
- part 2 @ 15:50+ (the car ride): a steady low (<300 Hz) engine drone.

**Opera aria — no gap.** The in-engine opera performance (part 1 @ ~5:30–7:00) is FMV003's XA. With movies playing (`PE_ROUTE_PLAY_MOVIES=1`), FMV003 streams with the game's ATV (0x98 → 0x88) and 2358 XA sectors by route frame 12500. `--route-pad`'s skip-movie is why route runs are silent there.

**Eve 2 vocals — open.**
- Same song for Eve 1 and Eve 2 (song 0x25, `AKAO 25 00`, 22 channels).
- Eve 2's type-3 bank 130 (disc LBA 8287, 23 sectors) adds distinct looping samples at SPU 0x54070/0x571C0 (descriptors 2/3). Bank 129 maps descriptors 2..5 to one sample, 0x50C80, which never keys.
- The port keys the bank-130 choir samples **once** per fight (5–10 s in, ~5 s sustain), both in a short fight and in a 100 s fight.
- Verified identical to retail: the song bytes, the bank upload/relocation (`func_80087428`), the SFX/command producers, and the driver (retail bytes).
- Needs a retail timestamp where the choir is heard, or an AKAO channel decode of song 0x25.

### audio3: Eve 2 vocals over the whole fight (divergence #20)

**Verdict: no gap.** The port plays the same vocal/choir events as retail for the
whole retail fight, part 02 @ 1:23–2:20 (57 s of battle music).

Run: `PE_CHEAT_KEYS=42800:F2` (god mode, opt-in cheat), `--route-pad --hand-over-at 42930
--max-frames 45700` with `PE_AUDIO_WAV`. The Eve 2 battle starts at route frame 42924. No death edge
follows before the frame limit, so the fight runs about 92 s with no player input.

Alignment: the full-band spectrogram cross-correlation peaks with retail part 02 @ 1:28.0 at port
capture 1447.0 s (r = 0.59; neighbouring offsets 0.48–0.50).

Test: correlation of the 600–1500 Hz log spectrogram per 3 s window, best lag within ±0.3 s
(`build/lanes/audio3/tools/bandcorr.c`). The baseline is the same comparison with the port shifted
by +5 s.
- Aligned: r = 0.26–0.75 (median 0.58) across windows 0–54 s.
- Baseline: r = 0.03–0.34 (median 0.21).
- The best lag drifts only −0.02 → +0.07 s over 57 s: the port follows the same song timeline.
- The low aligned windows (6, 15, 24, 51 s) are the ones with fight SFX. The player's actions
  differ between the video and the port.
- Retail's fight ends at 57 s: retail band energy falls 10 dB, r = −0.05.
- The second song section is where retail's strongest ~1 kHz vibrato vocals are (part 02 @ 2:03–2:12).
  Its windows score r = 0.58–0.75.
- An aligned 400–2000 Hz spectrogram pair (`build/lanes/audio3/eve2_ret_vs_port.png`, retail on top)
  shows every vibrato vocal blob at the same music time in both.

Port level in this band is +7…+8 dB above the video. That is the known capture calibration (FMV002
XA: port/video ×2.4), not a missing layer.

The bank-130 samples 0x54070/0x571C0 key once, at 10.6 s, as before (`voice.txt`). The recurring
vocals are the song's own instruments.

This build is later than r2: it has audio2's per-VBlank pacing, which fixed the 2× tempo and the
overflow that dropped half the samples. The owner's r2 report is therefore not reproduced on the
current tree. Whether r2's overflow was the cause is not proven.

### audio3: audit item 12 (boot reverb) — fixed

`src/func_80085290.c` ends with `func_800862F4(i,0,0,0,0)` in both 24-voice loops, then
`func_8008CB54(4)` and `func_80085A64(1)`.

**Change.** `pe_stream.c` `PE_Stream_StateInit` collapsed all of these. It now runs them, using the
same condition as the existing `func_80085F74` call: SpuInit's register base is 0x1F801C00. The
reverb-RAM clear inside `func_8008CB54` waits on the SPU DMA completion event, so the event must also
be live. Unit fixtures have neither; the booted port has both.

**Evidence.** Headless, `--max-frames 30`, VBlank 33, before any song:

| | before | after |
|---|---|---|
| SPU 1A2 | unwritten | F204 (the type-4 work area) |
| SPUCNT | 0001 | 0081 (reverb enable) |
| SPU register writes | 16 | 138 |

- Boot plus 1500 route frames: stop_reason frame-limit, 0 faults.
- `pe-native-tests`: 1603/1603.

**Also fixed.** The reverb-mode attribute temp in `stream_mode_configure` stands in for retail
`func_8008CF70`'s stack record. It lived at guest 0x80122300, which is ovl_03C5 (movie controller)
.text +0x1600 (ovl_03C5 is at 0x80120D00, size 0x2000). Every song start that changed the reverb
mode wrote 0x44 bytes over it. It is now at 0x801FFF40–0x801FFF83, unused port scratch next to the
0x801FFF00 DISPENV and 0x801FFF20 movie words. No visible effect was found, because the port runs
that overlay natively.

### audio3: Eve 2 vocals on r2, and a control run

**r2 reproduces the owner's report.** The r2 preview binary (`rel-ship2`, 9f74e882 plus uncommitted fixes) ran the same `--route-pad` route to frame 44400. Its autopilot fights Eve 2 from frame 42925 to 44054, about 38 s.
- Alignment matches the current tree: retail part 02 @ 1:28 ↔ capture 1447.0 s, r = 0.61.
- In the 600–1500 Hz band the instruments still match. The vibrato vocal blobs at 18–21 s and 25–29 s of music (retail part 02 @ 1:44–1:53) are absent. See `build/lanes/audio3/eve2_ret_cur_r2.png` (retail / current / r2).
- Mean band r over the three vocal windows (18, 21, 27 s):

| Build | Mean band r |
|---|---|
| current tree | 0.593 |
| r2 | 0.297 |

**EA100/101 is not the cause.** r2's EA log marks EA100/101 (AKAO pause/resume) as dropped four times just before the fight (VBlank 84868–85190). The only other drop on the route is EA408, at VBlank 30564. Control: the current tree, with only EA100/101 suppressed (a private, env-gated binary; the shared source is unchanged), still has every vocal blob, mean r = 0.483 (`eve2_ret_dropea100.png`).

**`func_8006F044` is not the cause.** It was also ported after r2, but its only caller is `func_8001220C`'s 0xAA108448 branch, which is off the Day-1 route.

The r2→current change that restored the vocals is unidentified.

**Current-tree lead.** The vocal blobs coincide with slow-attack sustained voices (ADSR B4FF/5FE8) on resident samples SPU 0x0F540/0x0FE70.

**Regression check.** `pc_port/tools/audio/eve2_vocal_check.sh <capture.wav> <approx music start s>` needs the git-ignored owner video. It aligns to retail over 100–4000 Hz, then fails if the vocal-window mean r is below 0.42:

| Capture | Mean r | Result |
|---|---|---|
| current tree | 0.593 | PASS |
| r2 | 0.297 | FAIL (exit 1) |
| EA100/101 suppressed | 0.483 | PASS |

### audio3: Day-1 audio sweep with movies (vs the owner's retail video)

**Runs.** Movies on (`PE_ROUTE_PLAY_MOVIES=1 --route-pad`), current tree, with `PE_AUDIO_WAV`, `PE_AUDIO_EA_LOG`, `PE_AUDIO_VOICE_LOG` and `PE_AUDIO_ROOMSND_LOG`.
- Run 1 went to 72000 presents: route frame ~60000, the sewers.
- Run 2 went to 95000 presents. It reached A8000048 story 0x80 (the reporter and the car ride) at route frame 66610, then stopped (see below). Capture length: 2471 s.
- Engine: 0 faults, 0 CD underruns. Every movie played all its frames: FMV002/003/004/005/006 = 1021/1151/561/231/744, dropped = 0.

**Method.** `build/lanes/audio3/tools/align.c` matches each 10 s port segment against the whole retail part 01 + part 02 audio, using 24 log bands from 80 Hz to 5 kHz at 10 fps and Pearson r. Flagged segments were then checked by hand with finer band correlation (`bandcorr`), 0.1 s RMS timelines, the EA and voice logs, and video frames.

**Result: no missing or wrong sound found from New Game to the car ride.**
- Run 1: 158 of 220 segments match at r ≥ 0.75. The level offset port − video is +7.3 dB (sd 2.6), which is the capture calibration.
- Run 2 tail: the sewers, FMV006, the alligator fight, the reporter and the car ride all match.

**Car-ride engine drone (part 02 @ 15:50+).**
- Port 2440–2460 s aligns with part 02 @ 16:05–16:35 at r = 0.95.
- The level is steady at 62.1–62.3 dB, which is 54.9 dB after calibration; retail is 54.8.
- Per-band shares are within 0.3 dB of retail in every band below 9.6 kHz (port −4.4/−4.7/−8.7/−9.5/−13.6/−20.9/−30.4 vs retail −4.5/−4.7/−8.4/−9.6/−13.6/−20.9/−30.6).

**Flags that were checked and are not defects.**
- **Digital silences.** They occur in the same places as retail: after FMV002, and after the Eve 2 fight (retail part 02 @ 2:25–2:34 and 3:11–3:17). The port's silences are longer because the autopilot waits on dialogue. The footstep segment in between has retail's structure: isolated steps with silence between them.
- **Player timing.**
  - Song 0x21 (EA200 at port 1473 s) plays about 120 s in A80614C8; the video player left after about 20 s.
  - Several quiet rooms sit near the video's noise floor (~41 dB).
- **FMV003.** The alignment shifts once by 1.38 s at port ~254 s (retail part 01 @ ~7:31), and the lag is stable on either side. The port played the complete stream (1151/1151 frames, 1440 XA sectors, 76.9 s). Its long-term spectrum is within 1 dB per band of retail below 9.6 kHz. No duplicated audio was found in the video around the jump, so the cause is unexplained; it is not treated as a port defect.
- **FMV006.**
  - The field script starts song 0x29 during the movie (EA200 at VBlank 136371; the movie closes at 137513) and again at the movie's end. `func_80121C04` (src/overlays/ovl_03C5) only starts the movie and returns, so in retail too the VM runs while a movie plays.
  - The movie's two halves sit at the same per-band offset from retail (+10.7…+11.1 dB, except the top band at +13.0/+11.8), so the port adds no layer retail lacks.
  - The song's post-movie restart matches retail part 02 @ 11:55.2 (r = 0.76).

**Not audio: stop after story 0x80.** Run 2 stopped with `stop_reason=unresolved-boundary` at 2471 s, right after the car ride. The log line is `[STUB:BOOTSTRAP_RET] func_80077404`. The StoreImage attribution first given here was wrong; see the next section for the cause and the fix.

### audio3: Day 1 → Day 2 stop (divergence #27) — fixed

**Symptom.** With movies on, the port stopped right after the car ride (A8000048, story 0x80, 2471 s), logging `[STUB:BOOTSTRAP_RET] func_80077404`. The no-movie route never gets that far. Three port sites stop through that name: `func_800768A0` (StoreImage wait), `func_80076664` (LoadImage wait) and `func_80076C34` (command-ring enqueue).

**Not StoreImage.** A diagnostic on StoreImage's fatal timeout path never printed in a repro run that stopped at the same point (`build/lanes/audio3/si/run.log`).

**Cause: `func_80076C34`'s full-ring cut.**
- Retail (`src/func_80076C34.c`): `while (((D_80095874+1)&0x3F) == D_80095878) { if (func_80077404() != 0) return -1; func_80076EE4(); }`.
- The port called the stub on the first full ring and stopped ("Neither continuation is translated"). The Day 1 → Day 2 transition queues more than 63 GPU commands.

**Fix, in `pc_port/game/boot/func_80076C34_port.c`.** The full-ring path now loops as retail does. Each pass:
1. Runs `func_80077404`'s normal half: `VSync(-1) > D_80095888`, then `D_8009588C++ > 0xF0000`. The increment happens only when the deadline has not passed, as in retail.
2. Lets the in-flight DMA2 finish through `PE_Port_ServiceDmaIrqCheckpoint()`. Its IRQ runs the pump callback, which advances the consumer; on hardware this happens on its own. It is the same pattern as `pe_libgpu.c`'s DrawSync wait.
3. Calls the pump (0x80076C78) and re-tests.

The timeout-recovery half (ring reset, GP1/DPCR writes) stays a named boundary.

**Test.** `B53C_full_detection_and_wrap` pinned the old stop. Its full-ring cases now assert that the worker is not issued directly and that the wait was entered (in that fixture the pump reaches an unpopulated slot). `pe-native-tests`: 1606/1606.

**Evidence** (movies on, `build/lanes/audio3/si2/`):
- The run continues past story 0x80 through A80650C8, A8004148 (the Day 2 title, then the precinct, story 0x88) and A80040C8 (story 0x90), to route frame 77000 with no boundary. Screenshots are `si2/f0*.ppm`; there the route's pad script has ended and Aya stands in the chief's office.
- Audio after the car ride:
  - Day 1 closing narration: port 2470–2500 s ↔ part 02 @ 18:17–18:42, r = 0.74–0.96, level offset +7.6…+10.0 dB.
  - Day 2 title: port 2510 s ↔ part 03 @ 0:06, r = 0.82.
  - The precinct and chief's office are digital silence with short footstep bursts, as in retail part 03 @ 0:12–2:06.
  - Retail's precinct music (part 03 @ 2:11+) starts only after the player leaves the office, which the route never does.
- Part 02 @ 19:06+ is the YouTube channel's outro, not game audio.
- Part 03 was fetched into `build/reference/retail-video/` (git-ignored; README updated).
