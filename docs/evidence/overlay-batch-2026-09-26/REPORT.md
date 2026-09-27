# Overlay batch 2026-09-26

Twenty-five disc-1 overlay leaves matched `-O2 -G0` over the full declared span (`era_link_check.py --target`, `word mismatches=0`, `nonzero_pad=0`) and were included in a passing `scripts/exact_rebuild_overlay.sh` for their target. `disc1_preflight.py --target <id> --deep --only <func>` passed before each gate. Nothing was committed.

Profiles other than the default:

- `func_8010BE3C` (`ovl_039F`, `0x34`) uses `era_o2_g0_fill_epilogue_delay_slot`. Without `MASPSX_FILL_EPILOGUE_DELAY_SLOT` the `jr` and `addiu $sp` are swapped (2 words).
- `func_8019A5D4` (`room_m0273i`, `0x54`) uses `era_o2_g0_no_schedule_insns`. The product `(a1 + 1) * 6` is the shift-add `sll` / `addu` / `sll`, pinned to `$v0`. Plain `-O2` hoists that chain in front of the half stores (3 words).
- `func_8019959C` (`ovl_0700`, `0x20`) pins the scaled index to `$a1` and the cursor to `$v0`. An empty `asm` barrier keeps the first address in `$v0`.
- `func_80193114` (`room_m0418i`, `0x3C`) pins `-0x1000` / `0x1000` / `1` to `$v0` and `0x270` to `$v1`, with an empty `asm` barrier after the `0x270` so that constant is not sunk.

`func_8019537C` matches as a `switch` after clamping an unsigned index `>= 2` to 0. `func_80192C84` copies 4 bytes with `__builtin_memcpy` into a stack buffer (`lwl`/`lwr`/`swl`/`swr`) and calls `func_800D1AE0`. `func_801925AC` is a `switch` on an `unsigned int` (`sltiu`).

Gate lines are in `docs/ai_context/ACTIVE_HANDOFF.md` (top, 2026-09-26 overlay batch). Scratch logs: `ovl-gate-<id>.log`, `ovl-gate2-<id>.log`, `ovl-gate3-<id>.log`. Final images include `ovl_039F` `[10 c, 4 asm]` plan `c3760f0346c445ad` and `room_m0256i` `[54 c, 11 asm]` plan `ee40defbcb41d9d3`.

`func_8018FBE8` / `func_8018FC2C` (identical `0x78` inits) match with `$v0`/`$v1` reused for the constants `0x80`, `0x10`, `0xC8`, `0xFF`, `0x40`, `0x80`, `0x4B0` and the `func_800C4E50` pointer pinned to `$a0`. Gates: `room_m0075i` plan `a6f54082f927a0a4` sha1 `ca34d1321e158763e9ee8b3cc9df0b3d94d28978`; `room_m0419i` plan `f4f2bd922eb0de5c` sha1 `3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a`.

`func_801925B0` matches as a `switch`. `room_m0273i` plan `d8fe836d815ef00b` sha1 `692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a`.

`func_80199048` (`room_m0432i`) pins `D_8009D254` to `$a0` and `&D_801994F4` to `$a2`. Gate plan `1c904d496c26ebb5`, sha1 `e13a8b54116474ab22fa0ae909a264442944ae4f`, spans `[48 c, 11 asm, 1 rodata, 2 bin]`.

`func_80193400` (`room_m0418i`, `0x80`) writes the two `+0x50` updates at offset `0x10` then `0x14`; cc1 emits `0x14` then `0x10`. Gate plan `dfd9a295f85531b4`, sha1 `03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca`, spans `[92 c, 26 asm, 1 rodata, 2 bin]`.

`func_8010C448` (`ovl_039F`, `0x7C`) needs `-fno-schedule-insns -fno-delayed-branch` (overlay-local profile `era_o2_g0_no_schedule_insns_no_delayed_branch`). Gate plan `6b31e4b6057641d5`, sha1 `e7331867b4d1464da4d5f386d9137eaa1c98422b`, spans `[11 c, 4 asm, 0 rodata, 2 bin]`.

`func_80192704` (`room_m0418i`, `0x84`) keeps the `+0x28` store on `$a2` and copies that base into `$a0` in both arms of `v < 0x960`, which fills the `beqz` delay. Gate plan `b4bacce518c2613d`, sha1 `03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca`, spans `[93 c, 26 asm, 1 rodata, 2 bin]`.

`func_800D8978` was not integrated. The five `gte-inline` EXE spans stay asm.
