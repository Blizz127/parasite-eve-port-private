# func_800D27FC — PARKED (14 words)

- VRAM 0x800D27FC, file offset 0xC2FFC, size 0x35C; on the Day 1 route (fan-in 25). Still `asm` in `configs/USA/disc1.yaml`.
- Best draft: `build/lanes/m348/drafts/func_800D27FC.c` (lane-local; inline-GTE macros + era `-O2 -G0`).
- Check command: ` python3 tools/analysis/era_link_check.py <draft> 0x800D27FC 0x35C 0x35C`

## Residual

Fourteen words, all in the DR_MODE block after the first AddPrim. Retail loads D_8009CDD8, then D_8009CDDC, forms both `&ordering[bank]` and `&packets[bank]` from the one `bank*4`, stores `D_8009CDD8 += 8`, then reloads otz and the two bases, and computes `mode = packets + old_cdd8` in the GetTPage delay slot. Our build loads the bank index after the D_8009CDD8 store. Everything before this block is exact (the TILE_1/TILE pair, the colours, the halo, the first AddPrim).

## Levers tried

- Moving the `sv` stores before the two packet allocations fixed the prologue (38 → 15).
- 3!/4!/5! statement-order sweeps of {old = cdd8, cdd8 = old + 8, bank = D_8009CDDC, ot = ..., mode = ...}: best 14.
- `D_8009CDD8 + (int)packets[...]` operand orders, and ot as an address expression inside addPrim: 15.
