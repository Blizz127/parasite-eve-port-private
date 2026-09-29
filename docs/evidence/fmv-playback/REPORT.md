# FMV / STR playback on the native port (fmv lane, 2026-09-28)

Task 11b0bbc26eca: FMV/STR movies and their XA audio play in the Day-1
preview, and the default run no longer needs `--skip-movie`. That flag stays
for the `--route-pad` autopilot. Playable beats 1:1; approximations are
listed in `docs/ai_context/DAY1_FIDELITY_GAPS.md` §1–§3.

## Result (headless, real Disc 1, no skip flags)

| Step | Observation | Evidence |
| --- | --- | --- |
| FMV001 (`\FMV1\FMV001.STR;1`, LBA 189742) | 2077/2077 frames, stream closed at the file end (LBA 210535), 0 dropped frames | `[FMV] stream close lba=210535 frames=2077 last=2077 video=18194 audio=2597 vblanks=8318 dropped=0` |
| Rate | 8318 VBlanks for 2077 frames (= 4.005/frame, 15 fps; retail `func_80070E54` `VSync(4)`) | same line |
| XA audio | 2597 XA sectors decoded (`XA-sectors=` in the audio summary). A 2500-present WAV is 41.75 s long, non-silent from 8 s (movie start ≈ present 470) to 40 s, peaks up to 30278 | `build/lanes/fmv/run3/fmv001.wav`, `run3/full.wav` |
| Frames | Statue of Liberty and sewer shots decode with correct 24-bit colour | `fmv001-frame-liberty.png`, `fmv001-frame-sewer.png` |
| Title join | "[parasite eve] / Press Start Button / © 1998 SQUARE" after the movie | `title-after-fmv001.png` (present 9300) |
| Menu | Start → "New Game / Tutorial" (no memory card) | `title-menu.png` |
| New Game | Up + Cross → choice 0xA0 → field; opcode 0x35 plays `FMV000.STR` (SQUARE logo, 150 frames) | `fmv000-square-logo.png`; `stream open lba=101705 … close frames=150 last=150` |
| Opening | Carnegie Hall arrival scene | `opening-scene-after-new-game.png` (present 4000) |
| Skip | Start during FMV001 → retail 60-frame wait → title | `stream close … frames=130` with `PE_PAD_SCRIPT=1000:FFF7,…` |

Commands:

```
DISC="rom/image/Parasite Eve (USA) (Disc 1)/Parasite Eve (USA) (Disc 1).bin"
CMAKE=$PWD/build/lanes/port/venv/bin/cmake scripts/build_pc_port.sh --build-dir build/pcbuild-fmv
# full movie + title, with XA WAV
PE_FMV_LOG=1 PE_AUDIO_WAV=full.wav ./build/pcbuild-fmv/parasite-eve-port --headless \
    --max-frames 9300 --screenshot full9300.ppm --disc-image "$DISC"
# skip movie, open menu, New Game (Start, Start, Up, Cross; masks are active-low raw)
PE_FMV_LOG=1 PE_PAD_SCRIPT="1000:FFF7,1010:FFFF,1300:FFF7,1310:FFFF,1450:FFEF,1460:FFFF,1500:BFFF,1510:FFFF" \
    ./build/pcbuild-fmv/parasite-eve-port --headless --max-frames 4000 --screenshot f4000.ppm --disc-image "$DISC"
```

## What changed

**HOST_ADAPTED stream producer: `pc_port/platform/pe_str_feed.{c,h}` (new).**
Production runs never enable the modeled CD register device, so nothing
delivered sectors into the libcd stream ring that `StGetNext`
(`func_8007C484`) polls. When `CdReadS` (`func_80081314`) opens a stream with
the device off, the feeder:

- reads raw sectors from the active disc at the 2× rate. Drive time is
  credited as 2.5 sectors per VBlank from *all* VBlanks, including the
  guest's own `VSync(4)`.
- assembles STR video chunks (`60 01 01 80` header). It writes each chunk's
  32-byte header into a `StSetRing` record and its 2016 data bytes into the
  data slot, then publishes the frame by writing status 2 on the first record
  last. Status 1 is the wrap marker. `func_8007C484`/`func_8007C394` are
  unchanged.
- advances VBlanks (`HostFB_VSync(0)`) only while the guest is waiting on the
  stream, and while a stream is open keeps one host present per VBlank, so the
  window pacer runs picture and audio in real time.
- passes XA audio sectors to the audio lane's `PE_Spu_CdSector`.

Hooks: `func_80081314` (open; no longer queues the four controller packets
that never drain), `func_8007C484` (pump), `func_8007A2A4` (close), and
`func_80080DC4` Pause/Stop (close + complete).

**Port bugs fixed against the retail overlay bytes** (PE.IMG overlay 0x03D2 at
0x8018EFF0, 0x039F module at 0x8010BCF8, 0x03C5 at 0x80120D00; disassembled
with `mips-linux-gnu-objdump`):

- `func_8010C89C` (DecDCTvlc): the long-code path must reload `at = t1 & 0xFF`
  from the second-table entry (`b` delay slot 0x8010CAD4). The port kept
  `at = 0`, so rare long VLC codes consumed no bits and frame 7 derailed.
- `func_80191B64`: `D_801D0DBD` also latches when `w >= record+8` (end frame).
  Only `w < end` skips the store at 0x80191C7C. Without this the movie never
  ends.
- `func_801924F8`: the give-up path waits for `func_8007F72C() == $s2 (1)`,
  not -1, and loops back to its own wait.
- `D_800ACDDC` → `D_8009CDDC` (retail `lui/lw %hi(D_8009CDDC)`) in
  924F8/92934/91DC8/909B4. It made the display bank byte 6, so the slice
  callback ended every frame after one slice.
- `func_80121C04`: the `"\FMV1"`/`"\FMV2"` prefix is `addiu a1, 0x80120FF4`,
  an address. The port loaded a word from it and crashed.
- `func_80192FE8`: y = `+0x20 + acc` and alpha = `256 − |acc<<4|`. The two
  formulas were swapped.
- `func_8018F468`: retranslated. The dirty rect is the union with last frame's
  box, the title background is restored from the overlay TIM into
  `DRAWENV+0x8080`, and every node's draw/update is called.

**Title (`func_801909B4`)**: the full retail main loop (0x801911C0), teardown
(0x80191410), menu dispatch (0xA0 → 1; card screen via `func_8005C498`;
0xB4 without a card / ≥0xB5 → 3), the 1000-frame attract replay, and the
0x8019172C epilogue. New `pc_port/game/boot/title_menu_callbacks_port.c`:
`func_8018F958` (type-6 wipe compositor), `8018FD04`, `8018FE1C` (menu
build), `80192F98`, `80193084`, `801930D8`, `8019316C`, `801931BC`,
`80193200`. The host pad fills `D_800BE9A2` before the title/movie
`func_8003EB04` reads; retail libpad fills it at VSync. `PE_Port_SetTitleLoopBudget`
is a host test knob and defaults to unlimited.

**Field movies**: `func_80017018` opcode 0x35 without skip-movie calls the
matched-decomp `func_80014E30`.

**MDEC**: a new decode command drops DecDCTvlc's padding tail. **Display**:
`HostFB_PresentDispEnv` decodes `DISPENV.isrgb24`.

**Tooling**: `PE_PAD_SCRIPT` (headless, non-route, keyed on present count),
`PE_FMV_LOG`/`PE_FMV_DEBUG`, and `PE_WATCHDOG_SEC` (backtrace on SIGALRM /
SIGABRT, for addr2line).

## Tests

- `test_native.c`: pad-seeded title test sets `PE_Port_SetTitleLoopBudget(0)`
  (the loop is now real); two seeds moved from `0x800ACDDC` to `0x8009CDDC`.
- `test_movie_updater.h`: device-disabled Pause now completes (HOST_ADAPTED)
  instead of the old `CD_command_wait` boundary.
- `test_movie_player.h` / `test_movie_autonomous.h`: the "\FMV1" prefix
  string is seeded AT 0x80120FF4/0x80120FFC (retail `addiu a1`), not as a
  pointer stored there.
- `SKIP2_script_movie_and_nop` keeps its contract: without an active disc or
  PE.IMG base (`D_800B0DD8 == 0`) opcode 0x35 stays the named
  `func_80014E30` cut (the real overlay read could never complete).
- The feeder only takes over when the ReadS location holds a real STR/XA
  sector (`PE_StrFeed_Probe`); fixture and failure-path ReadS tests keep the
  retail `func_8007F0C8` queue path. `PE_Port_RunControlReset` resets it.
- `test_native.c` main: `PE_WATCHDOG_SEC` backtrace aid (env-gated).

## Non-claims

- No hardware pixel or audio golden. The drive's seek/spin-up time, retry
  timing and the ATV volume matrix are not modeled with the device off. The
  matrix is straight stereo 0x80.
- `--route-pad` still forces `--skip-movie`: its pad schedule is keyed to
  present counts. FMV003 (`m0372i`) has not been driven headless end-to-end.
- CD-DA is not decoded.

## Verification (final binary, 2026-09-28 05:5x)

- Native suite, `./build/pcbuild-fmv/pe-native-tests` from the repo root:
  `Results: 1581 run, 1580 passed, 1 failed`. The one failure,
  `DAY1_retail_m0023i_pump`, comes from another lane's uncommitted
  `m0023i_effect_port.c` edit (05:39) and passed in the prior full run.
- Day-1 route regression:
  `pc_port/tools/day1_route_regress.sh --bin build/pcbuild-fmv/parasite-eve-port --until eve1_room`
  gives `REGRESS PASS until=eve1_room frame=42707` (baseline day1 r1: 42713).
  A longer run stops at `PE_M0023I_Flash` at frame 43861. That is the known
  pre-existing gap: day1 r1 hit it at 43864 without these changes.

## Movie-skip audio teardown (owner bug, 2026-09-28)

Cause: the skip path already did the full retail teardown (7A2A4 closes
the feeder, CdlPause), but the XA already queued in the SPU CD jitter buffer
(up to 1 s) kept playing. Fix: new `PE_Spu_CdInputStop()` in `pe_spu.c/h`
flushes the queue, fades the last frame out over 64 frames and resets the XA
history. `PE_StrFeed_Close`/`Open` call it on both natural end and skip.
Unit test: `AUDIO_cd_input_stop`.

WAV around the FMV001 skip (stream close at VBlank 1007 = 16.783 s; peak per
50 ms window, `build/lanes/fmv/wavwin.py`):

| | -100 ms | -50 ms | 0 ms | +50 ms | +100 ms | +150 ms | +200 ms |
| --- | --- | --- | --- | --- | --- | --- | --- |
| before | 4033 | 3904 | 3836 | 3911 | 3769 | 2324 | 0 |
| after | 4033 | 3904 | 1428 | 0 | 0 | 0 | 0 |

The rest of the run is unchanged: New Game, FMV000, then field audio
(AKAO KON=469) continuing past FMV000's natural end
(`skip-then-new-game.png`). Host device latency adds at most
`PE_AUDIO_LATENCY_MS` (100 ms) of already-rendered audio. The host fast-forward
fade is not wired in: `host_audio.c` is linked only into the executable, not
the runtime library.

## Wrong movie on New Game and at the opera stage (fmv2 lane, 2026-09-28)

Owner bug on c744fab6: New Game played a short movie with a loud burst and cut
early to the car-arrival scene. At the opera stage the same short movie played
again instead of the combustion movie.

**Cause.** The movie was always `FMV000.STR` (the 150-frame SQUARE logo, which
retail plays at boot). The feeder and the player read the file they were
given. The wrong id came from the field VM. `func_80017018_port.c` put its host
stand-in for the retail `sp+16` argument frame at `GA_VM_FRAME = 0x80120F80`.
That address lies inside the movie overlay's load window: section 1 of
`func_80014E30` (PE.IMG sectors 0x3C5..0x3C9, 0x2000 bytes) loads to
`D_8001160C[1] = 0x80120D00`. Opcode 0x35 (`func_80014E30`) loads the overlay
first and only then reads `**args` (`src/func_80014E30.c`:
`func_80121C04(**a0)`). By then `args[0]` holds overlay bytes (`";1\0\0"` =
0x0000313B) and the id reads as 0. In retail the frame is on the stack
(SYSTEM.CNF `STACK=801fff00`), so the load cannot reach it. Movie ids index the
overlay's 47-entry table at 0x80122438 (20 bytes each: name pointer, wide flag,
start frame, end frame, rectangle). Each name is appended to `"\FMV1"` or
`"\FMV2"`, searched with DsSearchFile (`func_80081414`), then read with
Setloc + ReadS. `PEDISC01.IDF` is not used for movies.

**Fix.** `GA_VM_FRAME` is now 0x801FF400 and `GA_375E0_LIST` is 0x801FF460.
Both are in the guest stack area, like retail's frame. They sit below every
port scratch window (0x801FF4F0 and up) and above the highest overlay end
(0x801ED7F8 + 0x4800 = 0x801F1FF8, `configs/USA/overlays/manifest.yaml`).
The port has no guest `$sp` (native C, no MIPS interpreter), so no guest
stack grows into that window. `PE_VMFRAME_AUDIT=1` checks after every dispatch
that the frame still holds the decoded pointers, and between dispatches that
nothing else wrote 0x801FF400..0x801FF4EF. It reported `bad=0` over 1.6 M
opcodes in the movie route run below.

**Harness aid.** With `PE_ROUTE_PLAY_MOVIES=1`, `--route-pad` still skips the
boot/title movies but lets opcode 0x35 play. It holds the route clock and sends
no pad input while `D_800B0DBA != 0` (a movie owns the display). With
`PE_FMV_LOG=1`, the log gives the movie id and name, the stream's start LBA,
the first frame's bitstream FNV hash, and a close line with frame and VBlank
counts. `PE_FMV_SHOT_DIR=<dir under build/>` saves the picture at published
frames 3, 60, 300 and 900.

**Per-movie evidence.** Port figures are from `PE_ROUTE_PLAY_MOVIES=1
PE_FMV_LOG=1 day1_route_regress.sh --until eve1_victory`
(log `build/lanes/fmv2/route-movies.log`). Retail figures come from the owner's
reference video (`build/reference/retail-video/`, ffmpeg `blackdetect`).
Screenshots match the retail frames at the same movie time; the side-by-side
images are `build/lanes/fmv2/cmp_fmv00{2,3,4}.png` (git-ignored).

| Point | id / file (LBA) | Port: frames, VBlanks | Stream header | Retail video |
| --- | --- | --- | --- | --- |
| Boot | 1 `FMV001.STR` (189742) (it opens with the same SQUARE logo as `FMV000.STR`; retail does not play FMV000 at boot) | earlier fmv lane: 2077/2077 | 2077 frames | starts 29.95 s (audio cross-correlation 0.997); skipped by the video's player at ≈160 s |
| New Game | 2 `FMV002.STR` (103206) | 1021/1021, 4089 VB = 68.15 s, 0 dropped | 1021 frames, record end 0x3FD | 181.05–249.1 s (audio cross-correlation 0.917) |
| Opera stage (combustion) | 3 `FMV003.STR` (113438) | 1151/1151, 4610 VB = 76.83 s, 0 dropped | 1151 frames, record end 0x47F | ≈436.4–512.7 s (black-frame detection) |
| Rat (backstage) | 4 `FMV004.STR` (125230) | 561/561, 2247 VB = 37.45 s, 0 dropped | 561 frames, record end 0x231 | ≈910.5–947.6 s (black-frame detection) |

Before the fix, the New Game run in the first table above opened
`lba=101705` (FMV000, 150 frames). With the fix, the same no-skip
`PE_PAD_SCRIPT` run logs `movie id=2 name=\FMV1\FMV002.STR;1` and
`stream open lba=103206`.

Non-claims: the play-movies route loses sync at M0022I (token A8001148, story
0x48, route frame ≈19300), before `eve1_room`. The route pads were recorded
without movies, and the post-movie restore frames still move the pad clock.
That is a harness limit; the standard skip-movie regression is the gate. Other
field stand-ins in the old overlay window (`GA_EA_SLOT` 0x80120FC0 in
`func_80015DAC_port.c`, `GA_DESC`/`GA_POLY` 0x80120F70/0x80120F30 in
`func_8001735C_port.c`) are used only within a single call and were not moved.

## Movie audio level, MDEC FIFO order, overlay-window stand-ins (fmv2 round 2)

Covers KNOWN_DIVERGENCES #6 and #7, plus the last three stand-in addresses
that sat in the old overlay window.

**CD audio volume (#7).** There are two volume stages, as on hardware:

- *Drive matrix (ATV0..3).* The game sets it through CdMix
  (`func_800870F0` → `func_8007A88C` → `func_8007B964`, and
  `func_80080AC4` for EA op 409). `pe_cdreg.c` already latched those writes
  with the ADPCTL bit-5 apply, whether or not the CD device is enabled. The
  feeder ignored them and mixed at a constant `0x80,0,0x80,0`. It now passes
  `PE_CdReg_GetAudioVolumes()` for each XA sector. Measured values: the title
  movie runs at `0x98` (`D_800B0DBE`, 119 %). The New Game movie runs at
  `0x80` and fades out over frames 1006–1019 through `func_80191B64`. That
  fade was lost before.
- *SPU CD input and main volume (0x1B0/0x1B2, 0x180/0x182, SPUCNT bit 0).*
  The sound init `func_80085290` applies a SpuCommonAttr block through
  `func_80085F74` (SpuSetCommonAttr, matched C). The block is mask 0x3FCF:
  main volume 0x3FFF, CD volume 0x7FFF, CD mix on. `pe_stream.c` had
  collapsed that call. It now runs through the SPU MMIO shadow once
  `D_8009B3FC == 0x1F801C00`. At movie open the registers read
  `180=3FFF 182=3FFF 1B0=7FFF 1B2=7FFF 1AA=0001`; before, they were unwritten
  and pe_spu applied its defaults. The values are unchanged, so the WAV level
  is identical. The later driver CD-volume path (`func_8008D7D0`, from
  `D_8009D2B4`) now writes onto real register state.

Level check against the owner's retail video. The capture's absolute gain is
unknown (YouTube normalisation), so the check compares the difference between
two movies in the same recording. Windows are 0.5–9.5 s of the boot SQUARE
logo (retail 29.95 s, port VBlank 486) and 5–60 s of FMV002 (retail
181.05 s, port VBlank 2384). Both were aligned by cross-correlating the audio
envelopes (`build/lanes/fmv2/level.py`, `align.py`).

| | logo RMS | FMV002 RMS | logo − FMV002 |
| --- | --- | --- | --- |
| retail video | −32.42 dBFS | −24.48 dBFS | **−7.94 dB** |
| port, fixed (drive ATV) | −24.54 dBFS | −16.93 dBFS | **−7.61 dB** |
| port, before (constant 0x80) | — | — | −9.10 dB (derived: logo × 0x80/0x98) |

The FMV002 fade-out tail, per 0.5 s window, relative to each body's level,
movie time 66.0 / 66.5 / 67.0 / 67.5 / 68.0 s: retail −13.96 / −16.29 /
−18.98 / −21.28 / −24.51 dB, port −13.09 / −15.92 / −19.15 / −21.32 / −24.54
dB. No port window clipped (peaks 15477 and 30256).

**MDEC (#6).** psx-spx (`ps1/cpu/mdec/macroblockdecodermdec`) says: a decode
command owns exactly its N parameter words; FE00h at a block start or after
EOB is ignored; elsewhere FE00h is EOB. The old code threw away any input the
previous command had not decoded when a new command began. Now that input is
decoded in order: its complete macroblocks queue in the data-out FIFO
(`g_stale`, `stale_output_bytes`) ahead of the new command's output, and the
next DMA1 reads them first. psx-spx does not document a trailing partial
macroblock. It produces no output here because only complete macroblocks
reach the FIFO. New unit case in `test_mdec_pixels.h` (stale block before the
new command's block, accounting). This path never occurs in the Day-1 movies.
A temporary counter found 0 occurrences over FMV001 (205 frames) and FMV002
(1021 frames), so pictures are unchanged.

**Stand-ins moved out of the overlay window.** `GA_POLY` (0x801FF480,
4 × 8 bytes) and `GA_DESC` (0x801FF4A0, 2 bytes, not kept by
`func_80035038`) are in `func_8001735C_port.c`. `GA_EA_SLOT` (0x801FF4B0,
slot word + four CD gain bytes) is in `func_80015DAC_port.c`. They were at
0x80120F30 / 0x80120F70 / 0x80120FC0, inside the 0x80120D00 overlay load
window. They now share the audited 0x801FF400..0x801FF4EF block with the VM
frame. `PE_VMFRAME_AUDIT=1` also checks that no one writes this whole block
between opcodes.

Verification (round 2, build `build/pcbuild-fmv2` from the shared tree regenerated at 21:5x):
native suite `Results: 1596 run, 1595 passed, 1 failed`. The failure is
`DAY1_model_fade`, which exercises `func_8003AF14`/`8003C638`/…. Another lane
is switching those to matched C; this round touched none of them.
`day1_route_regress.sh --until eve1_victory` with `PE_VMFRAME_AUDIT=1` gives
`REGRESS PASS frame=44352` (`eve1_room` at 42707, unchanged) and
`[VMFRAME] … bad=0`. The new MDEC case `DAY2_mdec_pixels` passes.
