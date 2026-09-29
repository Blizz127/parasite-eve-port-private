# Memory card: overwriting an existing save closed the game (r4b)

**Owner report on r4b (0999c817):** "it closes if i overwrite a save already
saved".

## Cause

The Legion logs (`build/lanes/legion/run-20260929T161953Z.log` and
`…152138Z.log`) both end with the same sequence. The first save (to an empty
slot) had worked:

    [MENU] Unported confirmation callback 80050544
    [STUB:BOOTSTRAP_RET] PE_MenuConfirmationCallback (first invocation)
    [HOST] stop_reason=unresolved-boundary

Saving over a used slot runs through the following steps. func_8004D6D4 and
everything after it are the matched C and hand ports named below:

1. func_8004D6D4 (hand port `absent_lo_large_port.c`, from
   `src/func_8004D6D4.c`) builds the 0x29 "overwrite?" confirm window: draw
   0x80044E14, update 0x80044E98, and `D_8009CFA8 = func_80050544`.
2. func_80044E98 handles the Yes/No answer. It calls
   `D_8009CFA8(window, confirmed)`.
3. r4b ran a hand port of func_80044E98 (`boot/func_8004F910_port.c`,
   removed in r5). It dispatched D_8009CFA8 through a fixed list: 45110,
   46574, 46DBC, 50580. For anything else, including 0x80050544, it called
   `PE_Port_RequestStop(PE_PORT_STOP_UNRESOLVED_BOUNDARY)`. The game closed
   on either answer.

## Fix (already in r5, bcb49b8b)

Switch-over batch M (batchKLMNO) retired that hand port. The generated
matched func_80044E98 calls the callback through
`PE_GuestCall("@80044E98:f", f, 2, window, confirmed)`. The guest-code
registry resolves 0x80050544 to its generated TU. The rest of the chain is
matched C or faithful hand ports:

- func_80050544(…, 1) → func_80062F3C(0x1F), func_8004D978(0x45),
  func_80042B50(func_800504F4)
- func_80042B6C runs the queued callback on its 4th count →
  func_800504F4 → func_80042020(D_8009CF44, D_8009CF48). For a used entry
  this sets next state 0xB.
- func_80041108 state 1 → 11: open the old name (mode 1), close it, and
  delete it with B(45h). → 4: create the new variant (FCREAT 0x10200, with
  the variant digit from D_800A1704) and write the 0x2000-byte image that
  func_80040B80 built.

So retail deletes and recreates the file under the alternate variant name,
following the matched C. It does not write in place. PE saves are one block
(0x2000 bytes), so there is no multi-block case.

## Regression tests (`pc_port/tests/test_memcard.h`)

- **MC_overwrite_existing_save:**
  1. A first save is made at variant 0.
  2. The generated func_80044E98 handles the confirm with row 0 (Yes). The
     port must not stop, and func_800504F4 must be queued.
  3. func_80042B6C ×3, then func_80042020 arms state 0xB.
  4. func_80041108 runs to state 3.
  5. Checks: the old variant is deleted, exactly one file remains, and the
     new variant is on the card. After reloading the card image from disk,
     its bytes equal the D_8009EED0 image, which differs from the first save.
- **MC_overwrite_declined:** row 1 (No). The port must not stop, nothing is
  queued, and the card driver stays idle. After reload only the first save
  exists, byte-identical.

| tree | result |
|---|---|
| HEAD-equivalent (13b1f9ab + A1, hand whitelist) | both tests FAIL with `[MENU] Unported confirmation callback 80050544` (1615 run, 1613 passed, 2 failed) |
| r5 stack (bcb49b8b) | 1618 run, 1618 passed, 0 skipped |

## In-game (headless) overwrite

Setup for all three runs:

- The card is a copy of a real save (`build/lanes/port/mc/s6/card.mcd`,
  BASLUS-006620000000A).
- `build/lanes/port3/mco/run.sh` drives the run: `--route-pad
  --hand-over-at 20580`, and `PE_DEBUG_SAVE_MENU_AT=20600` opens the save
  menu in the theater.
- The pad script uses X = BFFF and Left = FF7F.

| run | binary | input | result |
|---|---|---|---|
| headno | HEAD-equivalent (13b1f9ab + A1; hand func_80044E98) | X on the used file → "Overwrite data?" (cursor on No) → X | `[MENU] Unported confirmation callback 80050544`, `[STUB:BOOTSTRAP_RET] PE_MenuConfirmationCallback`, `stop_reason=unresolved-boundary`. **The owner's bug, reproduced.** |
| r5 | r5 stack | the same, repeatedly (10 × X) | the dialog is answered No each time; `stop_reason=frame-limit`, card unchanged (`r5/sheet.png`) |
| r5y | r5 stack | X → X → Left → X (Yes) → X | "Saving" → "File saved / OK" → file list. `[MEMCARD] deleted BASLUS-006620000000A`, `created … (1 block from 1)`, `closed … writing 8192/8192`, `stop_reason=frame-limit`. The save title changed from play time 00:00:00 to 00:11:36 (`r5y/sheet.png`) |

## Audit: other hand ports that stop or skip on an unlisted guest callback

Starting point:
`grep -rn "Unported\|RequestStop(PE_PORT_STOP_UNRESOLVED\|not ported" pc_port --include=*.c`,
excluding the generated `game/decomp*/` directories and `tests/`. Each hit
was read in context. For every whitelist, the callback addresses that
callers can store were collected from the matched C (`src/`), the hand
ports and the retail data tables.

### Callback whitelists

| site | slot | list | fallback | addresses callers can store | status |
|---|---|---|---|---|---|
| func_80044E98 (r4b `func_8004F910_port.c`) | D_8009CFA8 | 45110, 46574, 46DBC, 50580 | none (stop) | + **0x80050544** (func_8004D6D4, overwrite confirm) | **owner bug**; fixed in r5 (generated TU, PE_GuestCall) |
| func_800638D8 `menu_draw_callback` (`func_800638D8_port.c:29-136`) | node+0x30 and constant draw args | 88 cases | PE_GuestCode_Resolve / PE_GuestCall, then stop | 42 literal stores found; all are in the list | OK |
| func_800634D4 cell predicate (`func_800638D8_port.c:174-183`) | node+0x8C | 4FC3C, 4FDA4, 4FE58 | none (stop) | src func_8004D18C/8004D084 → 4FDA4; func_8004D4C4 → 4FE58; func_80046ABC → 4FC3C | OK (every store listed), but no fallback |
| `contact_callback` (`actor_contact_port.c:173-179`) | actor+0x194 | 1D268 | none (stop) | literal 0x8001D268 (func_80029810_port.c:570, absent_lo_batch3_port.c:109); the type table D_800915DC[type][1] read from the retail EXE is **0** for types 0..12, which means no call | OK |
| func_80017018 VM dispatch (`func_80017018_port.c`) | D_800910A0[op] (static EXE table) | ~107 | registry, then stop | static table | OK for every resolvable opcode (as before) |
| func_800752AC OTC worker (`func_800752AC_port.c:185`) | libgpu jtb[11] (static, 0x80076354) | 76354 | none | static | OK |
| `sort_item` (`func_8005B500_port.c:10`) | D_8009D0B4 | 532B4, 5332C | none (stop) | 0x800532B4 (every port writer); b40 doc: 3 retail writers | OK |
| `sort_compare` (`func_8005B500_port.c:50`) | quicksort compare argument | 5AFFC, 5B124 | none (stop) | src func_8005B500/8005B71C → 5AFFC; func_8005B248 → 5B124 | OK |
| func_800C2414 weapon callbacks | effect record | case list | registry, then DAY1_SHIM skip (stop only with strict effect stack) | effect VM | known shim (DAY1_FIDELITY_GAPS) |
| func_8006914C module init (`func_80029810_port.c:1043`) | D_800942E0[code] descriptor word 0 | none | registry, then skip + log | room modules | known shim (logged "not ported; skipped") |
| CD VBlank (`cd_stream_port.c:188`) | D_800A36A0 | 7F7E8 | none (stop) | 0 or 0x8007F7E8 only (pe_libcd.c:162/172) | OK |
| CD ready/sync (`cd_stream_port.c:232-245`) | D_8009AFB8 / D_8009AFB4 | none | stop whenever set with the pending bit | 0x80080778 / 0x80080164 (libds stream) | Psy-Q libcd/libds path, left for PsyCross (KD #3); not hit on the route |

### Other stops that are not callback lists but can be reached in play

| site | trigger | callees now in the port? | status |
|---|---|---|---|
| func_80057834 (item "Use", `func_80044B0C_port.c:43-45`) | item subtype 2 → stop "func_8004F23C"; subtypes 4-6 and 12-14 → stop "func_80048918" (outside battle, D_8009D028 == 0) | yes: func_8004F23C (absent_lo_menu_port.c:151) and func_80048918 (port6_port.c) | **would stop**; follow-up: switch func_80057834 to `src/func_80057834.c` |
| func_8004DCA4 (`func_8005C498_port.c:22`) | a0 != 0 (field-VM op func_80016F10 with a nonzero argument) → stop "func_8004DCA4_inventory_rename" | yes: func_80054294, func_8005C488 (generated), func_80048918 (hand) | **would stop**; follow-up: port the a0 != 0 arm of `src/func_8004DCA4.c` |
| func_8004C608 help (`func_8004DF74_port.c:147`) | id < 61 not handled | every id 0..60 has a case | OK |

The rest are arithmetic traps, CD retry/timeouts, unmodelled hardware, or
test-only frontiers (`PE_Memcard_BoundaryMode`): func_80028574 damage
divide, func_800638D8 `list_divide`, func_801904B0, func_8018F05C GTE
overflow, movie_overlay/func_80192934 CD waits, pe_mdec, func_8010C0FC bad
mode, func_8006D2B8 unused state, func_80025EE8 target divide,
func_80042798 (test frontier only) and m0023i (strict mode only).
