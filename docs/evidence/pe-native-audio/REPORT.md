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
