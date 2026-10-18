# Ending credits as native code (2026-10-18)

Branch `claude/native-complete-20261017`. No game ran on the devbox; the
evidence is the retail bytes, the matched C, unit tests and the test-only
R3000A oracle (`pc_port/tests/pe_akao_interp_oracle.c`, never linked into the
port binary).

## What stopped

`func_8001220C` (main) handles the next-state word `0xAA108448`
(`src/func_8001220C.c`):

    case 0xAA108448u:
        if (!(D_800B0CD8.fF5 & 0x2)) { dispatch = 2; ... }
        else { func_8006F044(); func_801235DC(); D_8009D280 = 0xA80830C8u; ... }

`func_801235DC` is the credits main loop of ovl_0457
(`src/overlays/ovl_0457/func_801235DC.c`, LINK_EXACT). `func_8003EB04` sets
`0xAA108448` (with `D_8009D1A0 |= 0x12000`) after a nine-step pad sequence
from the table at `D_80092200`; bit 16 of `D_8009D1A0` makes
`func_801235DC` pick the second op table.

The port defined `func_801235DC` as a `static inline` in
`pc_port/include/psx_compat.h` that only called `Bootstrap_ReturnVoid`, so
this branch was a `CPU_BOUNDARY` STOP. Meanwhile all 27 ovl_0457 leaves are
generated TUs (`pc_port/game/decomp_ovl/ovl_0457/`) in the registry.

Inside the credits, the generated `func_8012403C` and `func_80125094`, the
background-picture loaders of the two op tables, reached three ovl_039F
libpress entries through `overlay:` boundaries:

| address | libpress | retail |
|---|---|---|
| `func_8010BF94` | DecDCTBufSize | `return *(u16 *)a0` (matched C) |
| `func_8010C86C` | DecDCTvlcSize | handwritten asm, see below |
| `func_8010C078` | DecDCToutSync | `a0 ? (func_8010C430() >> 24) & 1 : func_8010C39C()` (matched C) |

psyq_port_guard classifies ovl_039F as Psy-Q libpress, so these cannot be
`func_8010xxxx` definitions in the port.

## Retail facts used

- `func_8006F044` reloads two PE.IMG ranges before the call. The retail EXE
  holds the `u16` pairs at `0x8009315A..`: `0x37D, 0x38E, 0x39F, 0x3C5, 0x3C9,
  0x3D2, 0x457, 0x4FC, 0x516`. The pointers are `D_8001160C = 0x8010BCF8` and
  `D_80011610 = 0x80120D00`. So `reload_block(0x8009315E, 0x8001160C)` loads
  `[0x39F,0x3C5)` (ovl_039F) and `reload_block(0x80093166, 0x80011610)` loads
  `[0x457,0x4FC)` (ovl_0457). Those are the manifest windows of both overlays.
- ovl_039F module data: `D_8010DB50 = 0x1F801824` (MDEC1 status), `D_8010DB30 =
  0x1F801098` (DMA1 CHCR) and `D_8011EB8C = 0x00FFFFFF` (the VLC bound).
- `func_8010C86C`:

      lui t0,%hi(D_8011EB8C); addiu t0,%lo
      addi at,a0,-1; blez at,L; lw v0,0(t0)
      sll at,a0,1; jr ra; sw at,0(t0)
      L: lui/ori at,0xFFFFFF; jr ra; sw at,0(t0)

- `func_8010C448` (the timeout handler) does `printf("%s timeout:\n", ...)`,
  `MDEC1 = 0x80000000`, clears DMA0 and DMA1 CHCR, then `MDEC1 = 0x60000000`.
- The credits picture table `D_80125BA4` holds 31 BS pictures (header
  `0x3800xxxx`, version 2).

## Change

- `func_8001220C` calls `PE_GuestCall("func_8001220C", 0x801235DC, 0, ...)`
  after `func_8006F044`. The registry binds the address by the fingerprint of
  the bytes now there, which is the generated `ovl_0457__func_801235DC`. Any
  other bytes give a loud boundary. The `psx_compat.h` stub is gone.
- Host replacements in `pc_port/platform/pe_libpress.c`, marked
  `host-replacement:` in `pe_port_compat.h`:
  - `PE_DecDCTBufSize_Guest`.
  - `PE_DecDCTvlcSize_Guest`, which returns the old bound and sets `size << 1`,
    or `0xFFFFFF` for `size <= 1` (signed).
  - `PE_DecDCToutSync_Guest`. Mode != 0 returns MDEC1 status bit 24, which is
    CMD.26 (the "output signed" bit) of the model's last command. That is what
    retail reads, not a busy flag. Mode 0 services the model once, then takes
    retail's timeout path: log, reset, clear DMA, `0x60000000`, -1.
- `gen_overlay_ports.run_overlay` and the EXE E21 path in `gen_decomp_ports`
  now apply host replacements, as `run_rooms` already did. The parser moved
  to `gen_decomp_ports.host_replacements`.
- The same mechanism binds `func_80071A74` (printf, BIOS A(3Fh)) to
  `PE_Printf_Guest`, which prints to the host TTY through `PE_Bios_Printf`.
  room_m0318i `func_80192560` had stopped on its actor-not-found debug print.

PE_MDEC_Service runs on every `HostFB_VSync`, so the DMA1 output armed by
DecDCTout completes on the next VSync tick, as the hardware DMA does.

## func_8004DCA4 with a nonzero mode

The field-VM handler `func_80016F10` calls `func_8004DCA4(script argument)`.
The hand port stopped on any nonzero mode (`func_8004DCA4_inventory_rename`).
It now follows `src/func_8004DCA4.c`:

- if `func_80054294()` (`D_8009D068`) is set, it calls `func_80048918(0, -2, -1)`;
- otherwise it calls `func_8005C488()` and returns.

The retail case generator never passed the mode:
`pc_port/tools/pe_name_frame_oracle.py` hard-coded `args=(0,)` for entry 0.
Eight cases are now appended: mode 1, `D_8009D068` = 0/1, and an open window
13/23/19 or none. Running retail code on the rebuilt retail EXE (SHA-1
`452fb033...`) gives `PASS: 155 original name frame cases`. The regenerated
header went through `tools/analysis/strip_retail_seeds.py`. Against the
committed header it only appends 434 patch rows and 8 case lines, and its
fixup table is unchanged.

The other cut in that file, `func_800339A0` with style >= 4, stays a loud
boundary. The matched C copies the four rows to a 16-byte stack buffer and
indexes it unchecked, so a larger style reads stack contents the host cannot
reproduce.

## Verification

Regeneration, Release build in /dev/shm and suite, 2026-10-18 01:18-01:46 UTC
(`regen_build_suite.sh`, isolated HOME, Disc 1 configured):

    Results: 1661 run, 1661 passed, 0 failed, 0 skipped
    TEST NAM9_retail_name_frame... PASS                       (155 retail cases)
    TEST credits_libpress_vlcsize_bufsize_outsync... PASS
    TEST credits_libpress_vlcsize_bufsize_match_retail... PASS   (76 sizes, 31 pictures, oracle)
    TEST credits_main_loop_resolves_native... PASS               (both op tables; bytes changed -> NULL)
    TEST credits_vlc_decode_bounded_resume_matches_retail...
         [credits] 31 pictures x 3 bounds, 558 resumed calls, retail == native PASS
    ctest park|gte-native|dev-menu: 100% tests passed out of 6

The decode test builds the VLC table with the retail `func_8010BD4C` in the
oracle. It then decodes every credits picture three ways:

- unbounded;
- with the bound `func_8012403C` sets (`BufSize >> 2`);
- with the bound `func_80125094` sets (`>> 4`).

Each decode runs once with the retail `func_8010C89C` bytes and once with the
hand port, resuming with `(0, 0, table)` until it returns 0. Every case
matches all of guest RAM and the return sequence, and the bounded output
equals the unbounded one. That is the first run of the hand port's resume
path: the movie path leaves the bound at `0xFFFFFF`.

`psyq_port_guard.py --binary parasite-eve-port --binary pe-native-tests`:
PASS relative to the allowlist (no new offender). `retail_data_guard.py
--strict --staged`: OK.

`gen_decomp_ports.py --e21 --verify` before regenerating listed exactly one
changed EXE TU, `func_800CF658`: RotMatrix is now `PE_RotMatrixYXZ_Guest`
instead of a boundary. That function has no reference in the EXE or in any
PE.IMG blob (no jal, no data word). In ovl_0457, `func_8012403C` and
`func_80125094` now call the three libpress replacements.

The printf replacement first dropped room_m0318i `func_80192560`. Its call
passes three arguments, and the lowering refused a host prototype with more
parameters. `guest_lower.DEAD_TRAILING_PARAMS` now marks printf's arguments
after the format as optional. The e21 harness compares only the format
pointer of a printf call (`TTY_ONLY_ARGS`): retail leaves an unrelated value
in `a3`, which the format never reads.

    e21_leaf.py room_m0318i func_80192560 --check-gte --coverage --seeds 64
    e21_leaf: ok func_80192560__da523018_port.c 9 seeds, retail coverage 40/48 words

The full build and suite with that TU regenerated runs in the next
regeneration.

## Erratum (2026-10-18, later the same day)

The statement above that the credits run native held for dispatch, but one
op ran wrong until f452c3594. Op table 1 entry 6 is the frame flip, ovl_0457
`func_801247F4`. It passes rows of `D_800BCE80[][20]` (DISPENV) and
`D_800BCDC8[][92]` (DRAWENV) to PutDispEnv (`func_800755F0`) and DrawOTagEnv
(`func_800754E4`). The verbatim lowering had flattened both two-dimensional
externs to one-dimensional byte arrays. The generated TU therefore passed the
byte at offset k (`movzbl` in the object code) where retail passes
0x800BCE80 + 20k and 0x800BCDC8 + 92k. Every frame of op table 1 read its
display and draw environment from the low-RAM vector area.

None of the credits tests here exercised op 6. They check the registry
binding, the libpress replacements and the picture decode. The bug was found
by reading the one ovl_0457 mismatch from `tools/analysis/e21_verbatim.py`.
Rule E23 (f452c3594) sends such a leaf to the E21 lowering. The fixed TU is
proven against retail:

    e21_diff.py --with-calls --check-gte --coverage --verbose --jobs 1 --seeds 32 --only func_801247F4_port.c
    ok func_801247F4_port.c: 32 seeds, retail coverage 42/42 words

No game ran, so the credits picture itself has not been checked visually.
